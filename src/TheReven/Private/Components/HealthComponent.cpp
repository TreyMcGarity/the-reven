// HealthComponent.cpp -- Implementation of UHealthComponent for The Reven
//
// See HealthComponent.h for class-level documentation.

#include "Components/HealthComponent.h"

// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
UHealthComponent::UHealthComponent()
{
	// Enable ticking so we can handle passive health regeneration.
	PrimaryComponentTick.bCanEverTick = true;

	// Default to level 1, Fortitude 5: MaxHealth = 100 + 10 + 10 = 120.
	MaxHealth = 120.0f;
	CurrentHealth = MaxHealth;
}

// -----------------------------------------------------------------------
// BeginPlay
// -----------------------------------------------------------------------
void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Ensure we start at full health.
	CurrentHealth = MaxHealth;
}

// -----------------------------------------------------------------------
// TickComponent -- Passive Health Regeneration
//
// Health regenerates continuously as a percentage of MaxHealth per second.
// The rate is boosted by Fortitude and further multiplied when out of combat.
//
// Regen formula (from progression-stats.md):
//   regenPerSecond = BaseRegenRate * MaxHealth * (1 + Fortitude / 200)
//
// Out-of-combat bonus (from progression-stats.md):
//   If TimeSinceLastDamage > OutOfCombatDelay (5 sec), multiply regen by 3x.
//
// Regen ticks every frame (DeltaTime) for smooth visual feedback.
// The design doc mentions 0.5s ticks, but per-frame is smoother and
// functionally equivalent -- we just scale by DeltaTime.
// -----------------------------------------------------------------------
void UHealthComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// Dead actors don't regenerate.
	if (bIsDead)
	{
		return;
	}

	// Track time since last damage for out-of-combat detection.
	TimeSinceLastDamage += DeltaTime;

	// Skip regen if already at full health.
	if (CurrentHealth >= MaxHealth)
	{
		return;
	}

	// Calculate the base regen amount for this frame.
	// BaseRegenRate (0.01) * MaxHealth gives 1% of max HP per second.
	// Fortitude scaling: multiply by (1 + Fortitude / 200).
	//   At Fort 0:   1.0x regen
	//   At Fort 50:  1.25x regen
	//   At Fort 100: 1.50x regen
	float RegenThisFrame = BaseRegenRate * MaxHealth * (1.0f + CachedFortitude / 200.0f) * DeltaTime;

	// Apply out-of-combat multiplier if enough time has passed without damage.
	// The design doc specifies 3x regen after 5 seconds of no damage taken
	// AND no damage dealt. For simplicity, we only track damage taken here.
	// Damage dealt tracking would require integration with the combat component.
	if (TimeSinceLastDamage >= OutOfCombatDelay)
	{
		RegenThisFrame *= OutOfCombatMultiplier;
	}

	// Apply the regen and clamp to max.
	CurrentHealth = FMath::Min(CurrentHealth + RegenThisFrame, MaxHealth);

	// Notify listeners that health has changed (UI health bar, etc.).
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

// -----------------------------------------------------------------------
// TakeDamage
//
// Applies incoming damage after Fortitude-based damage reduction.
//
// Damage reduction formula (from combat-system.md):
//   DamageReduction% = Fortitude / (Fortitude + 100)
//   ActualDamage = RawDamage * (1 - DamageReduction%)
//
// Examples:
//   Fortitude  0: 0/(0+100)     = 0.0%  reduction -> full damage
//   Fortitude 10: 10/(10+100)   = 9.1%  reduction
//   Fortitude 50: 50/(50+100)   = 33.3% reduction
//   Fortitude 100: 100/(100+100) = 50.0% reduction
//
// The formula has diminishing returns and never reaches 100%.
// -----------------------------------------------------------------------
float UHealthComponent::TakeDamage(float RawDamage, EDamageType DamageType)
{
	// Dead actors cannot take more damage.
	if (bIsDead)
	{
		return 0.0f;
	}

	// Calculate damage reduction from Fortitude.
	// Currently only Physical damage is reduced by Fortitude.
	// Elemental damage types (Fire, Frost, Nature, Dark) will use separate
	// resistance values in a future system. For now, they deal full damage.
	float DamageReductionPercent = 0.0f;
	if (DamageType == EDamageType::Physical)
	{
		DamageReductionPercent = CachedFortitude / (CachedFortitude + 100.0f);
	}

	// Calculate the actual damage after reduction.
	const float ActualDamage = RawDamage * (1.0f - DamageReductionPercent);

	// Apply the damage.
	CurrentHealth = FMath::Max(CurrentHealth - ActualDamage, 0.0f);

	// Reset the out-of-combat timer. The actor just took damage, so the
	// enhanced regen multiplier won't apply until 5 more seconds pass.
	TimeSinceLastDamage = 0.0f;

	// Notify listeners of the health change.
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);

	// Check for death.
	if (CurrentHealth <= 0.0f && !bIsDead)
	{
		bIsDead = true;
		OnDeath.Broadcast();

		UE_LOG(LogTemp, Log, TEXT("%s has died."),
			*GetOwner()->GetName());
	}

	return ActualDamage;
}

// -----------------------------------------------------------------------
// Heal
//
// Restores health by the given amount. Clamps to MaxHealth so we
// never exceed the maximum. A dead actor can be revived by healing
// (bIsDead is cleared when health goes above zero).
// -----------------------------------------------------------------------
void UHealthComponent::Heal(float Amount)
{
	if (Amount <= 0.0f)
	{
		return; // No negative healing (that would be damage -- use TakeDamage).
	}

	// If the actor was dead and is being healed, revive them.
	if (bIsDead && Amount > 0.0f)
	{
		bIsDead = false;
	}

	// Apply healing, clamped to MaxHealth.
	CurrentHealth = FMath::Min(CurrentHealth + Amount, MaxHealth);

	// Notify listeners.
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

// -----------------------------------------------------------------------
// RecalculateMaxHealth
//
// Updates MaxHealth based on the actor's current level and Fortitude.
// Preserves the current health *percentage* so that leveling up doesn't
// suddenly fill the health bar (or drop it if max decreased somehow).
//
// Formula (from progression-stats.md):
//   MaxHealth = 100 + (Level * BaseHealthPerLevel) + (Fortitude * 2)
//
// Examples:
//   Level 1,  Fort 5:   100 + 10 + 10  = 120 HP
//   Level 10, Fort 12:  100 + 100 + 24 = 224 HP
//   Level 50, Fort 60:  100 + 500 + 120 = 720 HP
//   Level 100, Fort 100: 100 + 1000 + 200 = 1300 HP
// -----------------------------------------------------------------------
void UHealthComponent::RecalculateMaxHealth(int32 Level, float Fortitude)
{
	// Cache Fortitude for use in Tick (regen scaling) and TakeDamage (DR).
	CachedFortitude = Fortitude;

	// Remember the current health percentage so we can preserve it.
	const float HealthPercent = GetHealthPercent();

	// Apply the formula.
	MaxHealth = 100.0f + (static_cast<float>(Level) * BaseHealthPerLevel) + (Fortitude * 2.0f);

	// Restore health to the same percentage of the new max.
	// This prevents the "free heal on level up" exploit while also
	// preventing a sudden health drop if Fortitude gear is unequipped.
	CurrentHealth = MaxHealth * HealthPercent;

	// Notify listeners of the change.
	OnHealthChanged.Broadcast(CurrentHealth, MaxHealth);
}

// -----------------------------------------------------------------------
// GetHealthPercent
// -----------------------------------------------------------------------
float UHealthComponent::GetHealthPercent() const
{
	// Guard against division by zero (shouldn't happen, but be safe).
	if (MaxHealth <= 0.0f)
	{
		return 0.0f;
	}
	return CurrentHealth / MaxHealth;
}

// -----------------------------------------------------------------------
// IsAlive
// -----------------------------------------------------------------------
bool UHealthComponent::IsAlive() const
{
	return !bIsDead;
}
