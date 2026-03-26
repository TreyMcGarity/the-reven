// RevenStatics.cpp -- Implementation of static utility functions for The Reven

#include "Data/RevenStatics.h"

// ---------------------------------------------------------------------------
// Primary stat associated with each equipment slot (from the design doc):
//   Head -> Might, Body -> Fortitude, Gloves -> Agility,
//   Legs -> Sorcery, Boots -> Haste
// Weapon slots have no fixed primary stat -- we default to Might.
// ---------------------------------------------------------------------------
static EStatType GetPrimaryStatForSlot(EEquipSlot Slot)
{
	switch (Slot)
	{
	case EEquipSlot::Head:     return EStatType::Might;
	case EEquipSlot::Body:     return EStatType::Fortitude;
	case EEquipSlot::Gloves:   return EStatType::Agility;
	case EEquipSlot::Legs:     return EStatType::Sorcery;
	case EEquipSlot::Boots:    return EStatType::Haste;
	default:                   return EStatType::Might;
	}
}

FStatBlock URevenStatics::GenerateRandomStatRolls(ERarity Rarity, EEquipSlot Slot)
{
	FStatBlock Block;

	// Number of secondary stats and value ranges per rarity.
	int32 NumSecondaries = 0;
	int32 MinValue = 0;
	int32 MaxValue = 0;

	switch (Rarity)
	{
	case ERarity::Common:
		NumSecondaries = 0;
		break;
	case ERarity::Uncommon:
		NumSecondaries = 1;
		MinValue = 1;
		MaxValue = 2;
		break;
	case ERarity::Rare:
		NumSecondaries = 2;
		MinValue = 1;
		MaxValue = 3;
		break;
	case ERarity::Epic:
		NumSecondaries = 3;
		MinValue = 2;
		MaxValue = 5;
		break;
	case ERarity::Legendary:
		NumSecondaries = 4;
		MinValue = 3;
		MaxValue = 6;
		break;
	}

	if (NumSecondaries <= 0)
	{
		return Block;
	}

	// Build pool of candidate stats, excluding the slot's primary stat.
	const EStatType PrimaryStat = GetPrimaryStatForSlot(Slot);

	TArray<EStatType> Pool;
	Pool.Reserve(4);

	const EStatType AllStats[] = {
		EStatType::Might,
		EStatType::Fortitude,
		EStatType::Haste,
		EStatType::Agility,
		EStatType::Sorcery
	};

	for (EStatType Stat : AllStats)
	{
		if (Stat != PrimaryStat)
		{
			Pool.Add(Stat);
		}
	}

	// Clamp to available pool size (should be 4, but safety first).
	NumSecondaries = FMath::Min(NumSecondaries, Pool.Num());

	// Pick stats at random without replacement (Fisher-Yates partial shuffle).
	for (int32 i = 0; i < NumSecondaries; ++i)
	{
		const int32 SwapIndex = FMath::RandRange(i, Pool.Num() - 1);
		Pool.Swap(i, SwapIndex);

		const float RolledValue = static_cast<float>(FMath::RandRange(MinValue, MaxValue));
		Block.SetStat(Pool[i], RolledValue);
	}

	return Block;
}

float URevenStatics::CalculateDamage(float BaseDamage, float Might, float ComboMultiplier)
{
	// Combat spec: final_damage = base_weapon_damage * (1 + Might / 100) * combo_multiplier
	return BaseDamage * (1.f + Might / 100.f) * ComboMultiplier;
}

float URevenStatics::CalculateSpellDamage(float BaseDamage, float Sorcery)
{
	// Combat spec: final_damage = base_spell_damage * (1 + Sorcery / 100)
	return BaseDamage * (1.f + Sorcery / 100.f);
}

int32 URevenStatics::GetXPForLevel(int32 Level)
{
	// Progression spec: xp_needed = 100 * level^1.5
	if (Level <= 0)
	{
		return 0;
	}

	return FMath::RoundToInt32(100.f * FMath::Pow(static_cast<float>(Level), 1.5f));
}

float URevenStatics::GetRegenRate(float BaseRate, float StatValue)
{
	// Progression spec: rate = BaseRate * (1 + StatValue / 200)
	return BaseRate * (1.f + StatValue / 200.f);
}
