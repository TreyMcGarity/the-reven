// StaminaComponent.cpp -- Implementation of UStaminaComponent for The Reven
//
// See StaminaComponent.h for class-level documentation.

#include "Components/StaminaComponent.h"

// -----------------------------------------------------------------------
// Constructor
// -----------------------------------------------------------------------
UStaminaComponent::UStaminaComponent()
{
	// Enable ticking so we can handle passive stamina regeneration
	// and the exhaustion timer every frame.
	PrimaryComponentTick.bCanEverTick = true;

	// Start at full stamina.
	MaxStamina = 100.0f;
	CurrentStamina = MaxStamina;
}

// -----------------------------------------------------------------------
// BeginPlay
// -----------------------------------------------------------------------
void UStaminaComponent::BeginPlay()
{
	Super::BeginPlay();

	// Ensure we start at full stamina.
	CurrentStamina = MaxStamina;
}

// -----------------------------------------------------------------------
// TickComponent -- Stamina Regeneration & Exhaustion Timer
//
// Two responsibilities each frame:
//
// 1. Exhaustion timer: if the actor is exhausted (stamina break), count
//    down the exhaustion duration. During exhaustion, regen runs at HALF
//    rate (1.5% instead of 3%, per the design doc). When the timer
//    expires, exhaustion ends and normal regen resumes.
//
// 2. Stamina regeneration: after the regen delay (1 second since last
//    stamina use), regenerate stamina as a percentage of max per second,
//    scaled by the Haste stat.
//
// Regen formula (from progression-stats.md):
//   regenPerSecond = BaseRegenRate * MaxStamina * (1 + Haste / 200)
//
// During exhaustion, regen is halved:
//   regenPerSecond *= 0.5
//
// The design doc mentions regen ticks every 0.5 seconds, but per-frame
// ticking (scaled by DeltaTime) gives smoother visual feedback and is
// functionally equivalent.
// -----------------------------------------------------------------------
void UStaminaComponent::TickComponent(
	float DeltaTime, ELevelTick TickType,
	FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// --- Exhaustion Timer ---
	// If the actor is exhausted, tick down the exhaustion timer.
	// During exhaustion, regen still occurs but at half rate.
	if (bIsExhausted)
	{
		ExhaustionTimer += DeltaTime;

		// Check if exhaustion has expired.
		if (ExhaustionTimer >= ExhaustionDuration)
		{
			bIsExhausted = false;
			ExhaustionTimer = 0.0f;

			// Notify listeners that exhaustion has ended.
			// The animation system can stop the stagger animation,
			// the UI can clear the "cracked shield" VFX, and the
			// input system can re-enable combat actions.
			OnExhaustionEnd.Broadcast();

			UE_LOG(LogTemp, Log, TEXT("%s: Exhaustion ended."),
				*GetOwner()->GetName());
		}
	}

	// --- Track time since last stamina use ---
	TimeSinceLastUse += DeltaTime;

	// --- Stamina Regeneration ---
	// Only regenerate if:
	//   1. Enough time has passed since the last stamina-consuming action
	//      (RegenDelay, default 1 second).
	//   2. Stamina is not already at max.
	if (TimeSinceLastUse < RegenDelay)
	{
		return; // Still in the regen pause window.
	}

	if (CurrentStamina >= MaxStamina)
	{
		return; // Already full.
	}

	// Calculate regen for this frame.
	// Base formula: BaseRegenRate * MaxStamina * (1 + Haste / 200) * DeltaTime
	//
	// At default values (BaseRegenRate 0.03, MaxStamina 100, Haste 5):
	//   0.03 * 100 * (1 + 5/200) = 3.0 * 1.025 = 3.075 stamina/sec
	//
	// At Haste 50:
	//   0.03 * 100 * (1 + 50/200) = 3.0 * 1.25 = 3.75 stamina/sec
	//
	// At Haste 100:
	//   0.03 * 100 * (1 + 100/200) = 3.0 * 1.50 = 4.50 stamina/sec
	float RegenThisFrame = BaseRegenRate * MaxStamina * (1.0f + CachedHaste / 200.0f) * DeltaTime;

	// During exhaustion, regen is halved (from combat-system.md):
	// "Stamina regen resumes at half rate during exhaustion (1.5% max/sec instead of 3%)"
	if (bIsExhausted)
	{
		RegenThisFrame *= 0.5f;
	}

	// Apply regen and clamp to max.
	CurrentStamina = FMath::Min(CurrentStamina + RegenThisFrame, MaxStamina);

	// Notify listeners of the stamina change (UI stamina bar).
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);
}

// -----------------------------------------------------------------------
// ConsumeStamina
//
// Attempts to spend stamina for an action. Returns whether the action
// should proceed.
//
// Rules:
//   - If exhausted, always returns false (no actions during stamina break).
//   - If CurrentStamina < Amount, returns false (not enough stamina).
//   - Otherwise, deducts the cost and returns true.
//   - If the deduction brings stamina to zero, triggers exhaustion.
//
// The calling system (combat component, movement component) MUST check
// the return value. If false, the action must not execute.
//
// Example usage in the combat component:
//   if (StaminaComp->ConsumeStamina(10.0f))  // Light attack costs 10
//   {
//       PlayLightAttackMontage();
//   }
//   else
//   {
//       // Not enough stamina -- play a "can't do that" feedback sound.
//   }
// -----------------------------------------------------------------------
bool UStaminaComponent::ConsumeStamina(float Amount)
{
	// Cannot perform any stamina-consuming action while exhausted.
	// The player must wait for the exhaustion timer to expire.
	if (bIsExhausted)
	{
		UE_LOG(LogTemp, Verbose, TEXT("%s: Cannot consume stamina -- currently exhausted."),
			*GetOwner()->GetName());
		return false;
	}

	// Not enough stamina to perform the action.
	if (CurrentStamina < Amount)
	{
		UE_LOG(LogTemp, Verbose, TEXT("%s: Not enough stamina (have %.1f, need %.1f)."),
			*GetOwner()->GetName(), CurrentStamina, Amount);
		return false;
	}

	// Deduct the stamina cost.
	CurrentStamina = FMath::Max(CurrentStamina - Amount, 0.0f);

	// Reset the regen delay timer. Regen won't resume for RegenDelay seconds.
	TimeSinceLastUse = 0.0f;

	// Notify listeners of the stamina change.
	OnStaminaChanged.Broadcast(CurrentStamina, MaxStamina);

	// Check if stamina has hit zero -- trigger exhaustion (stamina break).
	// From combat-system.md:
	//   "When stamina reaches 0 from any cause:
	//    1. Exhaustion state triggers: 1.5 seconds of vulnerability
	//    2. During exhaustion: cannot attack, block, dodge, or sprint
	//    3. Player character plays a stagger/heavy breathing animation
	//    4. Stamina regen resumes at half rate during exhaustion
	//    5. After 1.5 seconds, exhaustion ends and normal regen resumes
	//    6. A visible cracked shield VFX and stamina bar flash communicate the state"
	if (CurrentStamina <= 0.0f)
	{
		bIsExhausted = true;
		ExhaustionTimer = 0.0f;

		// Notify listeners that exhaustion has started.
		// The animation system should play the stagger/heavy breathing animation,
		// the UI should show the "cracked shield" VFX and flash the stamina bar,
		// and the input system should disable combat actions.
		OnExhaustionStart.Broadcast();

		UE_LOG(LogTemp, Log, TEXT("%s: STAMINA BREAK! Exhausted for %.1f seconds."),
			*GetOwner()->GetName(), ExhaustionDuration);
	}

	// Return true -- the action that consumed this stamina should proceed.
	// Even if stamina hit zero, the triggering action still goes through
	// (e.g., the dodge that emptied the bar still executes, but the player
	// is exhausted afterward).
	return true;
}

// -----------------------------------------------------------------------
// GetStaminaPercent
// -----------------------------------------------------------------------
float UStaminaComponent::GetStaminaPercent() const
{
	if (MaxStamina <= 0.0f)
	{
		return 0.0f;
	}
	return CurrentStamina / MaxStamina;
}

// -----------------------------------------------------------------------
// IsExhausted
// -----------------------------------------------------------------------
bool UStaminaComponent::IsExhausted() const
{
	return bIsExhausted;
}

// -----------------------------------------------------------------------
// SetHaste
//
// Updates the cached Haste value used for regen scaling. Call this
// whenever the actor's effective Haste stat changes (level up, equip
// or unequip gear that provides Haste, buff/debuff application).
// -----------------------------------------------------------------------
void UStaminaComponent::SetHaste(float NewHaste)
{
	CachedHaste = NewHaste;
}
