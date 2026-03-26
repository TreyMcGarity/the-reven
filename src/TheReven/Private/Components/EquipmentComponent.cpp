// EquipmentComponent.cpp
// The Reven -- Equipment Component Implementation
//
// See EquipmentComponent.h for the full class overview, slot layout,
// and weapon swap logic.

#include "Components/EquipmentComponent.h"
#include "Components/InventoryComponent.h"
// TODO: Uncomment once ARevenCharacter is created
// #include "Characters/RevenCharacter.h"

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
UEquipmentComponent::UEquipmentComponent()
{
	// No tick needed. Equipment changes are event-driven (player action).
	PrimaryComponentTick.bCanEverTick = false;

	// LinkedInventory is set in BeginPlay, not here.
	LinkedInventory = nullptr;
}

// ---------------------------------------------------------------------------
// BeginPlay
// ---------------------------------------------------------------------------
void UEquipmentComponent::BeginPlay()
{
	Super::BeginPlay();

	// Automatically find the InventoryComponent on the same actor.
	// Both components should live on the player character (or any actor
	// that has both an inventory and equipment).
	AActor* Owner = GetOwner();
	if (Owner)
	{
		LinkedInventory = Owner->FindComponentByClass<UInventoryComponent>();

		if (!LinkedInventory)
		{
			UE_LOG(LogTemp, Error,
				TEXT("EquipmentComponent: Owner '%s' has no InventoryComponent! "
					 "Equipment operations will fail."),
				*Owner->GetName());
		}
	}
}

// ===========================================================================
// EQUIP / UNEQUIP
// ===========================================================================

bool UEquipmentComponent::EquipItem(FGuid ItemUniqueID)
{
	// -----------------------------------------------------------------------
	// Step 1: Validate -- make sure we have a linked inventory
	// -----------------------------------------------------------------------
	if (!LinkedInventory)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentComponent: Cannot equip -- no LinkedInventory."));
		return false;
	}

	// -----------------------------------------------------------------------
	// Step 2: Find the item in the inventory
	// -----------------------------------------------------------------------
	FInventoryItem* FoundItem = LinkedInventory->FindItem(ItemUniqueID);
	if (!FoundItem)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent: Item with ID '%s' not found in inventory."),
			*ItemUniqueID.ToString());
		return false;
	}

	// Cache a copy of the item data. We need this because the pointer will
	// be invalidated when we modify the inventory array.
	FInventoryItem ItemToEquip = *FoundItem;

	// -----------------------------------------------------------------------
	// Step 3: Determine the target equipment slot
	// -----------------------------------------------------------------------
	// The item's EquipSlot field tells us where it goes (Head, Body, MainHand, etc.).
	// If EquipSlot is "None" or invalid, the item can't be equipped.
	EEquipSlot TargetSlot = ItemToEquip.EquipSlot;

	// Validate that this is a valid slot. "None" means the item isn't equippable
	// (e.g., materials, consumables).
	if (TargetSlot == EEquipSlot::None)
	{
		UE_LOG(LogTemp, Warning, TEXT("EquipmentComponent: Item '%s' has no valid EquipSlot (None). Cannot equip."),
			*ItemToEquip.ItemDefID.ToString());
		return false;
	}

	// -----------------------------------------------------------------------
	// Step 4: If the target slot is occupied, unequip the old item first
	// -----------------------------------------------------------------------
	if (IsSlotOccupied(TargetSlot))
	{
		// We need to return the old item to inventory. If inventory is full
		// and can't accept it, the equip operation fails entirely.
		if (!LinkedInventory->CanAddItem())
		{
			UE_LOG(LogTemp, Warning,
				TEXT("EquipmentComponent: Cannot equip '%s' -- slot '%d' is occupied "
					 "and inventory is full (can't return the old item)."),
				*ItemToEquip.ItemDefID.ToString(), static_cast<int32>(TargetSlot));
			return false;
		}

		// Unequip the old item. This puts it back in inventory.
		// We already checked CanAddItem(), so this should succeed.
		if (!UnequipItem(TargetSlot))
		{
			UE_LOG(LogTemp, Error, TEXT("EquipmentComponent: Failed to unequip existing item from slot."));
			return false;
		}
	}

	// -----------------------------------------------------------------------
	// Step 5: Remove the item from inventory
	// -----------------------------------------------------------------------
	// For equipment, we always remove the full quantity (which should be 1
	// for weapons/armor since they don't stack).
	LinkedInventory->RemoveItem(ItemUniqueID, ItemToEquip.Quantity);

	// -----------------------------------------------------------------------
	// Step 6: Place the item into the equipment slot
	// -----------------------------------------------------------------------
	EquippedItems.Add(TargetSlot, ItemToEquip);

	UE_LOG(LogTemp, Log, TEXT("EquipmentComponent: Equipped '%s' into slot %d."),
		*ItemToEquip.ItemDefID.ToString(), static_cast<int32>(TargetSlot));

	// -----------------------------------------------------------------------
	// Step 7: Handle 2H weapon side effects
	// -----------------------------------------------------------------------
	// If we just equipped a 2H weapon in MainHand (e.g., Bow), the OffHand
	// must be cleared because you can't hold a shield while using a bow.
	if (TargetSlot == EEquipSlot::MainHand && ItemToEquip.WeaponType != EWeaponType::None)
	{
		// Check if this is a 2-handed weapon. Bows, greatswords, glaives,
		// and spears are all 2H but greatswords/glaives/spears go in Back.
		// A bow in MainHand is the main case here.
		bool bIsTwoHanded = (ItemToEquip.WeaponType == EWeaponType::Bow);

		if (bIsTwoHanded && IsSlotOccupied(EEquipSlot::OffHand))
		{
			UE_LOG(LogTemp, Log, TEXT("EquipmentComponent: 2H weapon equipped in MainHand -- auto-unequipping OffHand."));
			UnequipItem(EEquipSlot::OffHand);
		}
	}

	// -----------------------------------------------------------------------
	// Step 8: Update visuals and broadcast change
	// -----------------------------------------------------------------------
	NotifyEquipmentChanged();

	return true;
}

bool UEquipmentComponent::UnequipItem(EEquipSlot Slot)
{
	// -----------------------------------------------------------------------
	// Step 1: Check if anything is in this slot
	// -----------------------------------------------------------------------
	if (!IsSlotOccupied(Slot))
	{
		UE_LOG(LogTemp, Log, TEXT("EquipmentComponent: UnequipItem -- slot %d is already empty."),
			static_cast<int32>(Slot));
		return false;
	}

	// -----------------------------------------------------------------------
	// Step 2: Verify we can return the item to inventory
	// -----------------------------------------------------------------------
	if (!LinkedInventory)
	{
		UE_LOG(LogTemp, Error, TEXT("EquipmentComponent: Cannot unequip -- no LinkedInventory."));
		return false;
	}

	if (!LinkedInventory->CanAddItem())
	{
		UE_LOG(LogTemp, Warning,
			TEXT("EquipmentComponent: Cannot unequip slot %d -- inventory is full."),
			static_cast<int32>(Slot));
		return false;
	}

	// -----------------------------------------------------------------------
	// Step 3: Move item from equipment back to inventory
	// -----------------------------------------------------------------------
	FInventoryItem UnequippedItem = EquippedItems[Slot];

	// Remove from equipment map first, then add to inventory.
	EquippedItems.Remove(Slot);

	// Add the item back to inventory. This should succeed because we already
	// checked CanAddItem().
	bool bAdded = LinkedInventory->AddItem(UnequippedItem);
	if (!bAdded)
	{
		// This shouldn't happen since we checked CanAddItem(), but if it does,
		// re-equip the item to avoid data loss.
		UE_LOG(LogTemp, Error, TEXT("EquipmentComponent: Failed to return item to inventory! Re-equipping."));
		EquippedItems.Add(Slot, UnequippedItem);
		return false;
	}

	UE_LOG(LogTemp, Log, TEXT("EquipmentComponent: Unequipped '%s' from slot %d back to inventory."),
		*UnequippedItem.ItemDefID.ToString(), static_cast<int32>(Slot));

	// -----------------------------------------------------------------------
	// Step 4: Update visuals and broadcast change
	// -----------------------------------------------------------------------
	NotifyEquipmentChanged();

	return true;
}

FInventoryItem UEquipmentComponent::GetEquippedItem(EEquipSlot Slot) const
{
	// Look up the slot in the map. If found, return a copy.
	// If not found, return a default FInventoryItem (caller can check
	// UniqueID.IsValid() == false to know it's empty).
	const FInventoryItem* Item = EquippedItems.Find(Slot);
	if (Item)
	{
		return *Item;
	}

	// Return an empty/default item to indicate nothing is equipped.
	return FInventoryItem();
}

bool UEquipmentComponent::IsSlotOccupied(EEquipSlot Slot) const
{
	return EquippedItems.Contains(Slot);
}

// ===========================================================================
// STAT AGGREGATION
// ===========================================================================

FStatBlock UEquipmentComponent::GetTotalStats() const
{
	// Start with a zeroed-out stat block and accumulate from all equipped items.
	FStatBlock TotalStats;

	// Walk through every occupied equipment slot and sum up its stats.
	for (const auto& Pair : EquippedItems)
	{
		const FInventoryItem& Item = Pair.Value;

		// Add the item's stat block (primary stat + any random secondary rolls).
		// The FStatBlock contains fields like Might, Fortitude, Agility,
		// Sorcery, Haste, BaseArmor, BaseDamage, etc.
		TotalStats.Might += Item.Stats.Might;
		TotalStats.Fortitude += Item.Stats.Fortitude;
		TotalStats.Agility += Item.Stats.Agility;
		TotalStats.Sorcery += Item.Stats.Sorcery;
		TotalStats.Haste += Item.Stats.Haste;
		TotalStats.BaseArmor += Item.Stats.BaseArmor;
		TotalStats.BaseDamage += Item.Stats.BaseDamage;
	}

	return TotalStats;
}

// ===========================================================================
// WEAPON LOADOUT SWAP
// ===========================================================================

void UEquipmentComponent::SwapWeaponLoadout()
{
	// -----------------------------------------------------------------------
	// WEAPON SWAP LOGIC (the X-key swap):
	//
	// The player has two loadouts:
	//   ACTIVE  = MainHand + OffHand   (what's in their hands right now)
	//   BACK    = Back slot            (2H weapon shown on character's back)
	//
	// Pressing X swaps them:
	//   - Back weapon goes to MainHand (it's 2H, so OffHand is disabled)
	//   - Old MainHand goes to Back (if it was 1H, old OffHand goes to inventory)
	//   - If MainHand was 2H (Bow), it simply swaps with Back
	// -----------------------------------------------------------------------

	// If there's nothing on the back, do nothing. The player has no second
	// loadout to swap to.
	if (!IsSlotOccupied(EEquipSlot::Back))
	{
		UE_LOG(LogTemp, Log, TEXT("EquipmentComponent: SwapWeaponLoadout -- Back slot is empty. Nothing to swap."));
		return;
	}

	// Cache all current weapon data before modifying slots.
	FInventoryItem BackItem = EquippedItems[EEquipSlot::Back];

	// Check what's currently in the active hands.
	bool bHasMainHand = IsSlotOccupied(EEquipSlot::MainHand);
	bool bHasOffHand = IsSlotOccupied(EEquipSlot::OffHand);

	FInventoryItem OldMainHand;
	FInventoryItem OldOffHand;

	if (bHasMainHand)
	{
		OldMainHand = EquippedItems[EEquipSlot::MainHand];
	}
	if (bHasOffHand)
	{
		OldOffHand = EquippedItems[EEquipSlot::OffHand];
	}

	// -----------------------------------------------------------------------
	// Perform the swap
	// -----------------------------------------------------------------------

	// Clear all three weapon slots first to avoid conflicts.
	EquippedItems.Remove(EEquipSlot::MainHand);
	EquippedItems.Remove(EEquipSlot::OffHand);
	EquippedItems.Remove(EEquipSlot::Back);

	// The Back weapon (always 2H) becomes the active MainHand weapon.
	EquippedItems.Add(EEquipSlot::MainHand, BackItem);
	// OffHand stays empty -- 2H weapon disables it.

	// The old MainHand goes to Back (if it existed).
	if (bHasMainHand)
	{
		EquippedItems.Add(EEquipSlot::Back, OldMainHand);
	}

	// The old OffHand (shield, torch) goes back to inventory since the Back
	// slot only holds one 2H weapon and can't accommodate an off-hand item.
	if (bHasOffHand && LinkedInventory)
	{
		UE_LOG(LogTemp, Log, TEXT("EquipmentComponent: Returning OffHand item '%s' to inventory during swap."),
			*OldOffHand.ItemDefID.ToString());

		bool bAdded = LinkedInventory->AddItem(OldOffHand);
		if (!bAdded)
		{
			// Inventory is full -- drop the off-hand item on the ground.
			UE_LOG(LogTemp, Warning,
				TEXT("EquipmentComponent: Inventory full during weapon swap! "
					 "Off-hand item '%s' could not be returned. Dropping it."),
				*OldOffHand.ItemDefID.ToString());

			if (LinkedInventory)
			{
				// We can't use DropItem since it's not in inventory.
				// For now, log a warning. The AItemPickup spawn would go here.
				// TODO: Spawn AItemPickup directly for this edge case.
			}
		}
	}

	UE_LOG(LogTemp, Log, TEXT("EquipmentComponent: Weapon loadout swapped. "
		"New MainHand: '%s', New Back: '%s'."),
		*BackItem.ItemDefID.ToString(),
		bHasMainHand ? *OldMainHand.ItemDefID.ToString() : TEXT("(empty)"));

	// -----------------------------------------------------------------------
	// Update visuals and notify listeners
	// -----------------------------------------------------------------------
	NotifyEquipmentChanged();
}

// ===========================================================================
// INTERNAL HELPERS
// ===========================================================================

void UEquipmentComponent::UpdateOwnerVisuals()
{
	// Tell the owner character to refresh its visual mesh components based
	// on the current equipment state. This updates:
	//   - Helmet mesh (Head slot)
	//   - Body/chest mesh (Body slot)
	//   - Glove mesh (Gloves slot)
	//   - Leg armor mesh (Legs slot)
	//   - Boot mesh (Boots slot)
	//   - Weapon mesh in hand (MainHand slot)
	//   - Shield/torch mesh (OffHand slot)
	//   - Back weapon mesh (Back slot -- visible on character's back)

	AActor* Owner = GetOwner();
	if (!Owner)
	{
		return;
	}

	// TODO: Uncomment once ARevenCharacter is implemented.
	// ARevenCharacter* Character = Cast<ARevenCharacter>(Owner);
	// if (Character)
	// {
	//     Character->UpdateEquipmentVisuals(EquippedItems);
	// }

	UE_LOG(LogTemp, Log, TEXT("EquipmentComponent: Visual update requested on owner '%s'."),
		*Owner->GetName());
}

void UEquipmentComponent::NotifyEquipmentChanged()
{
	// Update the character's visible meshes to reflect the new equipment.
	UpdateOwnerVisuals();

	// Broadcast the delegate so that any listening UI widgets (paperdoll,
	// stat panel, HUD weapon icons) can refresh themselves.
	OnEquipmentChanged.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("EquipmentComponent: OnEquipmentChanged broadcast. "
		"Total equipped slots: %d."), EquippedItems.Num());
}
