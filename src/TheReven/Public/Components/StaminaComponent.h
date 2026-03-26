// StaminaComponent.h -- Stamina management component for The Reven
//
// UStaminaComponent manages the stamina resource pool, which governs the
// player's ability to attack, dodge, block, and sprint. Stamina is a FIXED
// pool of 100 that does NOT scale with level -- this is a deliberate design
// choice to keep stamina management a constant pressure throughout the game.
//
// Key design decisions (from progression-stats.md and combat-system.md):
//   - MaxStamina = 100 (fixed, never grows)
//   - Base regen = 3% of max per second (3 stamina/sec)
//   - Haste scaling: regen = base * (1 + Haste / 200)
//   - Regen pauses for 1 second after any stamina-consuming action
//   - Stamina break at 0: 1.5 sec exhaustion, cannot act, half regen
//
// Stamina costs (from combat-system.md):
//   - Light attack:  10 stamina
//   - Heavy attack:  25 stamina
//   - Dodge roll:    20 stamina
//   - Sprint:        5/sec
//   - Block per hit: variable (proportional to incoming damage)
//   - Parry:         0 (reward for skilled timing)
//
// Usage:
//   Attach to the player character. Before any stamina-consuming action,
//   call ConsumeStamina(cost). If it returns false, the action should NOT
//   execute (not enough stamina, or currently exhausted).

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StaminaComponent.generated.h"

// -----------------------------------------------------------------------
// Delegate declarations
// -----------------------------------------------------------------------

/** Broadcast when current or max stamina changes. Used by the stamina
 *  bar UI to update its display every frame during regen/consumption. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
	FOnStaminaChanged,
	float, CurrentStamina,
	float, MaxStamina
);

/** Broadcast when the actor enters the exhaustion state (stamina hit zero).
 *  Listeners can play stagger animations, show the "cracked shield" VFX,
 *  flash the stamina bar, and disable combat inputs. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExhaustionStart);

/** Broadcast when the exhaustion state ends (1.5 seconds elapsed).
 *  Listeners can re-enable combat inputs and clear the exhaustion VFX. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnExhaustionEnd);

/**
 * UStaminaComponent -- Manages stamina consumption, regeneration, and exhaustion.
 *
 * Attach to any actor that uses stamina (typically the player character).
 * Enemies generally don't use stamina, though boss enemies could attach
 * this component for stamina-break mechanics.
 */
UCLASS(ClassGroup = (TheReven), meta = (BlueprintSpawnableComponent))
class UStaminaComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UStaminaComponent();

	// ===================================================================
	// Public Functions
	// ===================================================================

	/**
	 * Attempt to consume stamina for an action (attack, dodge, sprint tick, etc.).
	 *
	 * Returns true if the stamina was successfully consumed, false if the
	 * action should be blocked. The action is blocked when:
	 *   - The actor is currently exhausted (stamina break state)
	 *   - CurrentStamina is too low for the requested amount
	 *
	 * IMPORTANT: If this returns false, the calling system must NOT execute
	 * the action. The player cannot attack, dodge, or sprint without stamina.
	 *
	 * If consumption brings stamina to zero, the exhaustion state triggers
	 * automatically. The last action that empties the bar still executes
	 * (e.g., a dodge that costs exactly the remaining stamina goes through,
	 * but the player is exhausted afterward).
	 *
	 * @param Amount  The stamina cost of the action.
	 * @return        True if the action can proceed, false if blocked.
	 */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	bool ConsumeStamina(float Amount);

	/**
	 * Returns current stamina as a 0-1 percentage for UI stamina bars.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Stamina")
	float GetStaminaPercent() const;

	/**
	 * Returns true if the actor is currently in the exhaustion state
	 * (stamina break). During exhaustion, the actor cannot attack, block,
	 * dodge, or sprint.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Stamina")
	bool IsExhausted() const;

	// ===================================================================
	// Delegates
	// ===================================================================

	/** Fired every time stamina changes (consumption, regeneration).
	 *  Parameters: CurrentStamina, MaxStamina. */
	UPROPERTY(BlueprintAssignable, Category = "Stamina|Events")
	FOnStaminaChanged OnStaminaChanged;

	/** Fired when stamina reaches zero and the exhaustion state begins. */
	UPROPERTY(BlueprintAssignable, Category = "Stamina|Events")
	FOnExhaustionStart OnExhaustionStart;

	/** Fired when the exhaustion state ends (after ExhaustionDuration). */
	UPROPERTY(BlueprintAssignable, Category = "Stamina|Events")
	FOnExhaustionEnd OnExhaustionEnd;

	// ===================================================================
	// Haste Stat Integration
	// ===================================================================

	/**
	 * Update the cached Haste value used for regen scaling.
	 * Call this whenever the actor's Haste stat changes (level up, gear swap).
	 *
	 * @param NewHaste  The actor's effective Haste stat (base + gear).
	 */
	UFUNCTION(BlueprintCallable, Category = "Stamina")
	void SetHaste(float NewHaste);

protected:

	// ===================================================================
	// UActorComponent Overrides
	// ===================================================================

	virtual void BeginPlay() override;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
		FActorComponentTickFunction* ThisTickFunction) override;

	// ===================================================================
	// Properties -- Stamina Pool
	// ===================================================================

	/** Maximum stamina. Fixed at 100 -- does NOT scale with level.
	 *  This is an intentional design choice: stamina management remains
	 *  a constant pressure. What improves is regen rate (via Haste)
	 *  and efficiency (Agility for dodge cost, Fortitude for block cost). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float MaxStamina = 100.0f;

	/** Current stamina. Clamped to [0, MaxStamina]. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina")
	float CurrentStamina = 100.0f;

	// ===================================================================
	// Properties -- Regeneration
	// ===================================================================

	/** Base stamina regeneration rate as a fraction of MaxStamina per second.
	 *  Default 0.03 = 3% per second = 3 stamina/sec at 100 max.
	 *
	 *  Note: The design docs have a conflict between 3/sec and 10/sec.
	 *  The progression-stats doc recommends deferring to the vertical slice
	 *  value of 10/sec. To use 10/sec, change this to 0.10. The current
	 *  value (0.03) matches the "punishing, Dark Souls-like" option.
	 *  Adjust based on playtesting. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina|Regen")
	float BaseRegenRate = 0.03f;

	/** How long (in seconds) after the last stamina-consuming action before
	 *  regeneration resumes. Default 1.0 second from the design doc.
	 *  This prevents regen from counteracting active stamina use. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina|Regen")
	float RegenDelay = 1.0f;

	/** Tracks how long since the last stamina-consuming action.
	 *  Regen only occurs when this exceeds RegenDelay. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina|Regen")
	float TimeSinceLastUse = 0.0f;

	// ===================================================================
	// Properties -- Exhaustion (Stamina Break)
	// ===================================================================

	/** True when the actor is in the exhaustion state (stamina break).
	 *  During exhaustion: cannot attack, block, dodge, or sprint.
	 *  The character plays a stagger/heavy breathing animation and a
	 *  "cracked shield" VFX appears. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina|Exhaustion")
	bool bIsExhausted = false;

	/** How long the exhaustion state lasts in seconds.
	 *  Default 1.5 seconds from the design doc. During this window,
	 *  the player is completely vulnerable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stamina|Exhaustion")
	float ExhaustionDuration = 1.5f;

	/** Timer tracking how long the current exhaustion state has lasted.
	 *  When this reaches ExhaustionDuration, exhaustion ends. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stamina|Exhaustion")
	float ExhaustionTimer = 0.0f;

private:

	/** Cached Haste stat value used for regen scaling in Tick.
	 *  Updated via SetHaste(). Default 5 (starting Haste at level 1). */
	float CachedHaste = 5.0f;
};
