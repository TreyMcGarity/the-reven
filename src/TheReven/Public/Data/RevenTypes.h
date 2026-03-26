// RevenTypes.h -- Core data types for The Reven
// Every system includes this file. All enums and structs live here.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RevenTypes.generated.h"

// ---------------------------------------------------------------------------
// Enums
// ---------------------------------------------------------------------------

UENUM(BlueprintType)
enum class EStatType : uint8
{
	Might       UMETA(DisplayName = "Might"),
	Fortitude   UMETA(DisplayName = "Fortitude"),
	Haste       UMETA(DisplayName = "Haste"),
	Agility     UMETA(DisplayName = "Agility"),
	Sorcery     UMETA(DisplayName = "Sorcery")
};

UENUM(BlueprintType)
enum class ERarity : uint8
{
	Common      UMETA(DisplayName = "Common"),
	Uncommon    UMETA(DisplayName = "Uncommon"),
	Rare        UMETA(DisplayName = "Rare"),
	Epic        UMETA(DisplayName = "Epic"),
	Legendary   UMETA(DisplayName = "Legendary")
};

UENUM(BlueprintType)
enum class EEquipSlot : uint8
{
	Head        UMETA(DisplayName = "Head"),
	Body        UMETA(DisplayName = "Body"),
	Gloves      UMETA(DisplayName = "Gloves"),
	Legs        UMETA(DisplayName = "Legs"),
	Boots       UMETA(DisplayName = "Boots"),
	MainHand    UMETA(DisplayName = "Main Hand"),
	OffHand     UMETA(DisplayName = "Off Hand"),
	Back        UMETA(DisplayName = "Back")
};

UENUM(BlueprintType)
enum class EItemType : uint8
{
	Weapon      UMETA(DisplayName = "Weapon"),
	Armor       UMETA(DisplayName = "Armor"),
	Consumable  UMETA(DisplayName = "Consumable"),
	Material    UMETA(DisplayName = "Material"),
	Artifact    UMETA(DisplayName = "Artifact"),
	SpellTome   UMETA(DisplayName = "Spell Tome")
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Sword       UMETA(DisplayName = "Sword"),
	Axe         UMETA(DisplayName = "Axe"),
	Mace        UMETA(DisplayName = "Mace"),
	Shield      UMETA(DisplayName = "Shield"),
	Greatsword  UMETA(DisplayName = "Greatsword"),
	Glaive      UMETA(DisplayName = "Glaive"),
	Bow         UMETA(DisplayName = "Bow")
};

UENUM(BlueprintType)
enum class EDamageType : uint8
{
	Physical    UMETA(DisplayName = "Physical"),
	Fire        UMETA(DisplayName = "Fire"),
	Frost       UMETA(DisplayName = "Frost"),
	Nature      UMETA(DisplayName = "Nature"),
	Dark        UMETA(DisplayName = "Dark")
};

UENUM(BlueprintType)
enum class EMagicSchool : uint8
{
	Flora         UMETA(DisplayName = "Flora"),
	Earth         UMETA(DisplayName = "Earth"),
	Fire          UMETA(DisplayName = "Fire"),
	Water         UMETA(DisplayName = "Water"),
	Wild          UMETA(DisplayName = "Wild"),
	Restoration   UMETA(DisplayName = "Restoration"),
	Temporal      UMETA(DisplayName = "Temporal"),
	Soul          UMETA(DisplayName = "Soul"),
	Necromancy    UMETA(DisplayName = "Necromancy"),
	Conjuration   UMETA(DisplayName = "Conjuration"),
	Blood         UMETA(DisplayName = "Blood")
};

UENUM(BlueprintType)
enum class EMagicTier : uint8
{
	Terrestrial UMETA(DisplayName = "Terrestrial"),
	Astral      UMETA(DisplayName = "Astral"),
	Baleful     UMETA(DisplayName = "Baleful")
};

UENUM(BlueprintType)
enum class ESpellTargetType : uint8
{
	Self        UMETA(DisplayName = "Self"),
	Projectile  UMETA(DisplayName = "Projectile"),
	AoEDecal    UMETA(DisplayName = "AoE Decal"),
	Channeled   UMETA(DisplayName = "Channeled")
};

// ---------------------------------------------------------------------------
// Structs
// ---------------------------------------------------------------------------

/**
 * FStatBlock -- maps each EStatType to a float value.
 * Used for both base character stats and per-item random rolls.
 */
USTRUCT(BlueprintType)
struct FStatBlock
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	TMap<EStatType, float> Stats;

	/** Get the value for a stat. Returns 0 if absent. */
	float GetStat(EStatType Stat) const
	{
		const float* Found = Stats.Find(Stat);
		return Found ? *Found : 0.f;
	}

	/** Set a stat to an absolute value. */
	void SetStat(EStatType Stat, float Value)
	{
		Stats.FindOrAdd(Stat) = Value;
	}

	/** Add a delta to a stat (creates entry at 0 if absent). */
	void AddStat(EStatType Stat, float Delta)
	{
		Stats.FindOrAdd(Stat) += Delta;
	}
};

/**
 * FItemDefinition -- one row in the master item DataTable.
 * Defines the immutable template for an item. Instances carry per-drop rolls
 * in FInventoryItem.
 */
USTRUCT(BlueprintType)
struct FItemDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FName ItemID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classification")
	EItemType ItemType = EItemType::Material;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classification")
	EEquipSlot EquipSlot = EEquipSlot::MainHand;

	/** Only relevant when ItemType == Weapon. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classification", meta = (EditCondition = "ItemType == EItemType::Weapon"))
	EWeaponType WeaponType = EWeaponType::Sword;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classification")
	ERarity Rarity = ERarity::Common;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirements")
	int32 RequiredLevel = 1;

	/** Base damage for weapons. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseDamage = 0.f;

	/** Base armor value for armor pieces. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseArmor = 0.f;

	/** The guaranteed primary stat type for this item's equip slot. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	EStatType PrimaryStatType = EStatType::Might;

	/** Guaranteed primary stat value (before random roll variance). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float PrimaryStatValue = 0.f;

	/** Material tier (1-4 in levels 1-10). Drives stat roll ranges. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Crafting")
	int32 MaterialTier = 1;

	/** Max stack size for this item. Weapons/armor = 1. Materials up to 99. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 StackMaxSize = 1;

	/** Static mesh shown when the item is in the world. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	TSoftObjectPtr<UStaticMesh> WorldMesh;

	/** Icon texture for UI display. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	TSoftObjectPtr<UTexture2D> Icon;
};

/**
 * FInventoryItem -- a specific instance of an item in the player's inventory.
 * Points back to an FItemDefinition row via ItemDefinitionID and carries
 * per-instance data such as random stat rolls, durability, and enchantments.
 */
USTRUCT(BlueprintType)
struct FInventoryItem
{
	GENERATED_BODY()

	/** Row name in the item DataTable that defines this item's template. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FName ItemDefinitionID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Quantity = 1;

	/** Globally unique ID for this stack/instance. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FGuid UniqueID;

	/** Random secondary stat rolls generated at drop/craft time. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	FStatBlock StatRolls;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Durability")
	float CurrentDurability = 100.f;

	/** FName IDs referencing enchantment definitions. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Enchantments")
	TArray<FName> EnchantmentIDs;
};

/**
 * FSpellDefinition -- one row in the spell DataTable.
 */
USTRUCT(BlueprintType)
struct FSpellDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FName SpellID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classification")
	EMagicSchool School = EMagicSchool::Fire;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classification")
	EMagicTier Tier = EMagicTier::Terrestrial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Classification")
	ESpellTargetType TargetType = ESpellTargetType::Projectile;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost")
	float ManaCost = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cost")
	float Cooldown = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseDamage = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float CastTime = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Requirements")
	int32 RequiredLevel = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText Description;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visuals")
	TSoftObjectPtr<UTexture2D> Icon;
};

/**
 * FEnemyDefinition -- one row in the enemy DataTable.
 */
USTRUCT(BlueprintType)
struct FEnemyDefinition : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FName EnemyID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Identity")
	FText DisplayName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float MaxHealth = 100.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Combat")
	float BaseDamage = 10.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float DetectionRange = 1000.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 200.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float MoveSpeed = 400.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
	int32 XPReward = 10;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Rewards")
	FName LootTableID;
};

/**
 * FLootEntry -- a single possible drop within a loot table.
 */
USTRUCT(BlueprintType)
struct FLootEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FName ItemDefinitionID;

	/** Probability of this entry dropping (0.0 to 1.0). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot", meta = (ClampMin = "0.0", ClampMax = "1.0"))
	float DropChance = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot", meta = (ClampMin = "1"))
	int32 MinQuantity = 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot", meta = (ClampMin = "1"))
	int32 MaxQuantity = 1;
};

/**
 * FLootTable -- one row in the loot table DataTable.
 * Referenced by FEnemyDefinition::LootTableID.
 */
USTRUCT(BlueprintType)
struct FLootTable : public FTableRowBase
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	FName LootTableID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	TArray<FLootEntry> Entries;

	/** Minimum rarity guaranteed from this table (at least one drop will meet this). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Loot")
	ERarity GuaranteedRarityMin = ERarity::Common;
};
