// HealthComponent.h -- Health management component for The Reven
//
// UHealthComponent is an actor component that manages health, damage intake,
// healing, death, and passive health regeneration. It is designed to be
// attached to any actor that has a health bar -- the player character,
// enemies, destructible objects, etc.
//
// Key design decisions (from progression-stats.md and combat-system.md):
//   - Max health = 100 + (Level * 10) + (Fortitude * 2)
//   - Health regen = 1% of max HP per second, scaled by Fortitude
//   - Regen formula: rate = 0.01 * MaxHealth * (1 + Fortitude / 200)
//   - Out-of-combat regen: 3x multiplier after 5 seconds of no damage
//   - Damage reduction = Fortitude / (Fortitude + 100)
//
// Usage:
//   Attach to any actor in the editor or via C++. Call RecalculateMaxHealth
//   whenever the actor's level or Fortitude stat changes. Call TakeDamage
//   when the actor receives damage. The component handles regen via Tick.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/RevenTypes.h"
#include "HealthComponent.generated.h"

// -----------------------------------------------------------------------
// Delegate declarations
//
// These are multicast delegates -- multiple listeners can bind to them.
// The UI system, audio system, and AI can all respond to health changes
// without the HealthComponent knowing about any of them.
// -----------------------------------------------------------------------

/** Broadcast whenever CurrentHealth or MaxHealth changes. Used by
 *  the health bar UI to update its display. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnHealthChanged,
	float, CurrentHealth,
	float, MaxHealth
);

/** Broadcast once when the actor dies (health reaches zero).
 *  Listeners can play death animations, drop loot, update quest state, etc. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnDeath);

/**
 * UHealthComponent -- Manages health, damage, healing, regen, and death.
 *
 * Attach this component to any actor that needs a health pool. Works for
 * both player characters and enemies, though enemies may use simpler
 * setups (no regen, fixed max health from their DataTable definition).
 */
UCLASS(ClassGroup = (TheReven), meta = (BlueprintSpawnableComponent))
class UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	// ===================================================================
	// Public Functions
	// ===================================================================

	/**
	 * Apply damage to this actor. Calculates actual damage after applying
	 * the Fortitude-based damage reduction formula.
	 *
	 * Damage reduction formula (from combat-system.md):
	 *   damage_taken = RawDamage * (1 - Fortitude / (Fortitude + 100))
	 *
	 * This gives diminishing returns: at Fort 10, 9.1% reduction; at Fort 50,
	 * 33.3%; it never reaches 100%.
	 *
	 * @param RawDamage   The incoming damage before resistances.
	 * @param DamageType  The type of damage (Physical, Fire, Frost, etc.).
	 *                    Currently only Physical is reduced by Fortitude.
	 *                    Elemental resistances are a future system.
	 * @return            The actual damage dealt after reductions.
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	float TakeDamage(float RawDamage, EDamageType DamageType);

	/**
	 * Restore health by the given amount. Clamps to MaxHealth.
	 * Broadcasts OnHealthChanged after healing.
	 *
	 * @param Amount  How much health to restore (positive value).
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void Heal(float Amount);

	/**
	 * Recalculate MaxHealth based on the actor's level and Fortitude stat.
	 *
	 * Formula (from progression-stats.md):
	 *   MaxHealth = 100 + (Level * 10) + (Fortitude * 2)
	 *
	 * Call this whenever the actor levels up or their Fortitude changes
	 * (e.g., equipping/unequipping gear). CurrentHealth is scaled
	 * proportionally so the health percentage stays the same.
	 *
	 * @param Level      The actor's current level.
	 * @param Fortitude  The actor's effective Fortitude stat (base + gear).
	 */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void RecalculateMaxHealth(int32 Level, float Fortitude);

	/**
	 * Returns current health as a 0-1 percentage for UI health bars.
	 * Returns 0 if MaxHealth is zero (prevents division by zero).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
	float GetHealthPercent() const;

	/**
	 * Returns true if the actor is still alive (CurrentHealth > 0).
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Health")
	bool IsAlive() const;

	// ===================================================================
	// Delegates
	// ===================================================================

	/** Fired every time health changes (damage, healing, regen).
	 *  Parameters: CurrentHealth, MaxHealth. */
	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnHealthChanged OnHealthChanged;

	/** Fired once when health reaches zero. Will not fire again until
	 *  the actor is revived (health set above zero). */
	UPROPERTY(BlueprintAssignable, Category = "Health|Events")
	FOnDeath OnDeath;

protected:

	// ===================================================================
	// UActorComponent Overrides
	// ===================================================================

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// ===================================================================
	// Properties -- Health Pool
	// ===================================================================

	/** Maximum health. Calculated from level and Fortitude via
	 *  RecalculateMaxHealth(). Do not set directly in most cases. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float MaxHealth = 120.0f;

	/** Current health. Clamped to [0, MaxHealth]. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health")
	float CurrentHealth = 120.0f;

	// ===================================================================
	// Properties -- Regeneration
	// ===================================================================

	/** Base HP per level used in the max health formula.
	 *  Formula: MaxHealth = 100 + (Level * BaseHealthPerLevel) + (Fortitude * 2)
	 *  Default 10.0 matches the design doc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health|Regen")
	float BaseHealthPerLevel = 10.0f;

	/** Base health regeneration rate as a fraction of MaxHealth per second.
	 *  Default 0.01 = 1% per second, matching the design doc.
	 *  Actual rate is: BaseRegenRate * MaxHealth * (1 + Fortitude/200). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health|Regen")
	float BaseRegenRate = 0.01f;

	/** Multiplier applied to regen when the actor is out of combat.
	 *  Default 3.0 = triple regen speed out of combat. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health|Regen")
	float OutOfCombatMultiplier = 3.0f;

	/** How many seconds without taking damage before out-of-combat regen
	 *  kicks in. Default 5.0 seconds from the design doc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Health|Regen")
	float OutOfCombatDelay = 5.0f;

	/** Tracks how long since this actor last took damage. When this
	 *  exceeds OutOfCombatDelay, the enhanced regen multiplier applies. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health|Regen")
	float TimeSinceLastDamage = 0.0f;

	/** True when CurrentHealth has reached zero. Set by TakeDamage,
	 *  cleared if the actor is healed back above zero. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Health")
	bool bIsDead = false;

private:

	/** Cached Fortitude value used for regen calculations in Tick.
	 *  Updated whenever RecalculateMaxHealth is called. */
	float CachedFortitude = 5.0f;
};
