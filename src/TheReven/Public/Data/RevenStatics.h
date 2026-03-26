// RevenStatics.h -- Static utility functions for The Reven
// Pure math helpers that any system can call without coupling to actors.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Data/RevenTypes.h"
#include "RevenStatics.generated.h"

/**
 * URevenStatics -- project-wide static helpers.
 *
 * All functions are BlueprintCallable so designers can use them in BP graphs
 * as well as C++ systems.
 */
UCLASS()
class URevenStatics : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Generate random secondary stat rolls for a freshly dropped or crafted item.
	 *
	 * Rarity determines how many secondary stats are rolled:
	 *   Common     = 0 secondaries
	 *   Uncommon   = 1
	 *   Rare       = 2
	 *   Epic       = 3
	 *   Legendary  = 4 (hand-crafted in practice, but supported here)
	 *
	 * The primary stat for the slot is NOT included -- it is set separately from
	 * the item definition.  Secondary stats are chosen at random (uniform) from
	 * the remaining four stat types, with no duplicates.
	 *
	 * Value range per secondary scales with rarity:
	 *   Common     = 0
	 *   Uncommon   = [1, 2]
	 *   Rare       = [1, 3]
	 *   Epic       = [2, 5]
	 *   Legendary  = [3, 6]
	 */
	UFUNCTION(BlueprintCallable, Category = "Reven|Utility")
	static FStatBlock GenerateRandomStatRolls(ERarity Rarity, EEquipSlot Slot);

	/**
	 * Melee / ranged damage formula from the combat spec:
	 *   final = BaseDamage * (1 + Might / 100) * ComboMultiplier
	 *
	 * Critical multiplier is handled separately at the call site.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Reven|Combat")
	static float CalculateDamage(float BaseDamage, float Might, float ComboMultiplier);

	/**
	 * Spell damage formula from the combat spec:
	 *   final = BaseDamage * (1 + Sorcery / 100)
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Reven|Combat")
	static float CalculateSpellDamage(float BaseDamage, float Sorcery);

	/**
	 * XP required to reach a given level (quadratic curve from the progression spec):
	 *   xp_needed = 100 * Level^1.5
	 *
	 * Returns 0 for Level <= 0.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Reven|Progression")
	static int32 GetXPForLevel(int32 Level);

	/**
	 * Generic regeneration rate helper (health, stamina, mana).
	 * From the progression spec the pattern is:
	 *   effective_rate = BaseRate * (1 + StatValue / 200)
	 *
	 * Where StatValue is Fortitude for HP regen, Haste for stamina regen,
	 * or Sorcery for mana regen.
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Reven|Stats")
	static float GetRegenRate(float BaseRate, float StatValue);
};
