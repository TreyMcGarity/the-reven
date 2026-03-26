// EquipmentComponent.h
// The Reven -- Equipment Component
//
// This component manages what gear the player is currently wearing/wielding.
// It works hand-in-hand with UInventoryComponent:
//
//   INVENTORY  <-->  EQUIPMENT
//   (backpack)       (worn items)
//
// HOW IT WORKS:
//   1. The player opens the inventory UI and drags an item onto an equipment
//      slot (or right-clicks to auto-equip). This calls EquipItem().
//   2. EquipItem() removes the item from the InventoryComponent and places it
//      into the EquippedItems map under the correct EEquipSlot key.
//   3. If a slot is already occupied, the old item is returned to inventory
//      first (a swap).
//   4. Stats from all equipped items are summed via GetTotalStats() and
//      applied to the character.
//   5. Visual meshes on the character are updated through the owner character's
//      UpdateEquipmentVisuals() function.
//   6. OnEquipmentChanged fires so the UI can refresh the paperdoll display.
//
// WEAPON LOADOUT SWAPPING (X key):
//   The player has two weapon sets: Active (MainHand + OffHand) and Back.
//   Pressing X swaps them. See SwapWeaponLoadout() for details.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/RevenTypes.h"
#include "EquipmentComponent.generated.h"

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
class UInventoryComponent;
class ARevenCharacter; // The player character class that owns visual mesh updates

// ---------------------------------------------------------------------------
// Delegates
// ---------------------------------------------------------------------------

// Fires whenever any equipment slot changes (equip, unequip, swap).
// The UI's paperdoll widget binds to this to refresh slot icons and stats.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEquipmentChanged);

// ---------------------------------------------------------------------------
// UEquipmentComponent
// ---------------------------------------------------------------------------

UCLASS(ClassGroup = (TheReven), meta = (BlueprintSpawnableComponent))
class THEREVEN_API UEquipmentComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// -----------------------------------------------------------------------
	// Constructor
	// -----------------------------------------------------------------------
	UEquipmentComponent();

protected:
	// Called when the game starts. We use this to find the sibling
	// InventoryComponent on our owner actor.
	virtual void BeginPlay() override;

public:
	// =======================================================================
	// PROPERTIES
	// =======================================================================

	// Map of currently equipped items, keyed by equipment slot.
	// Only slots with items in them have entries. An absent key means
	// that slot is empty.
	//
	// Possible slots (from EEquipSlot):
	//   Head, Body, Gloves, Legs, Boots     -- armor
	//   MainHand, OffHand, Back             -- weapons
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Equipment")
	TMap<EEquipSlot, FInventoryItem> EquippedItems;

	// Pointer to the inventory component on the same actor. Set automatically
	// in BeginPlay(). Equipment operations (equip/unequip) move items between
	// this inventory and the EquippedItems map.
	UPROPERTY(BlueprintReadOnly, Category = "Equipment")
	UInventoryComponent* LinkedInventory;

	// =======================================================================
	// DELEGATES
	// =======================================================================

	// Broadcast whenever any equipment slot changes. The UI binds to this
	// to update the paperdoll, stat display, and weapon icons on the HUD.
	UPROPERTY(BlueprintAssignable, Category = "Equipment|Events")
	FOnEquipmentChanged OnEquipmentChanged;

	// =======================================================================
	// EQUIP / UNEQUIP (BlueprintCallable)
	// =======================================================================

	// Equip an item from the inventory into the appropriate equipment slot.
	//
	// FLOW:
	//   1. Find the item in LinkedInventory by its UniqueID.
	//   2. Determine which EEquipSlot the item belongs to (from item data).
	//   3. If that slot is already occupied, unequip the old item first
	//      (returns it to inventory).
	//   4. Remove the item from inventory and add it to EquippedItems.
	//   5. Update visuals on the owner character.
	//   6. Recalculate stats and broadcast OnEquipmentChanged.
	//
	// Returns TRUE on success. Returns FALSE if:
	//   - The item was not found in inventory.
	//   - The inventory is full and we can't return the currently equipped item.
	//   - The item type has no valid equipment slot.
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool EquipItem(FGuid ItemUniqueID);

	// Unequip whatever is in the given slot and return it to inventory.
	//
	// Returns TRUE if something was unequipped.
	// Returns FALSE if the slot was empty or if inventory is full (can't
	// return the item).
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	bool UnequipItem(EEquipSlot Slot);

	// Get the item currently in a given slot.
	// Returns a copy of the item. If the slot is empty, returns a default-
	// constructed FInventoryItem (check UniqueID.IsValid() to detect this).
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	FInventoryItem GetEquippedItem(EEquipSlot Slot) const;

	// Check whether a specific slot has an item equipped.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	bool IsSlotOccupied(EEquipSlot Slot) const;

	// =======================================================================
	// STAT AGGREGATION
	// =======================================================================

	// Calculate the total stats from ALL equipped items combined.
	//
	// This sums:
	//   - Each item's primary stat (the guaranteed stat from its slot)
	//   - Each item's randomly rolled secondary stats
	//   - Each item's base armor/damage values (stored in the stat block)
	//
	// The result is used by the character's stat system to determine actual
	// combat values (health from Fortitude, crit from Agility, etc.).
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Equipment")
	FStatBlock GetTotalStats() const;

	// =======================================================================
	// WEAPON LOADOUT SWAP
	// =======================================================================

	// Swap the Active weapon set (MainHand + OffHand) with the Back weapon.
	//
	// THE SWAP (triggered by pressing X):
	//   1. The MainHand and OffHand items move to a temporary holding area.
	//   2. The Back item moves to MainHand (it's always 2H, so OffHand
	//      becomes empty/disabled).
	//   3. The old MainHand goes to Back. The old OffHand goes back to
	//      inventory (since Back only holds one 2H weapon).
	//
	// SPECIAL CASES:
	//   - If Back is empty, do nothing (no weapon to swap to).
	//   - If MainHand is 2H (Bow), the swap is simpler: just exchange
	//     MainHand and Back.
	//   - After the swap, visuals and stats are updated.
	//
	// NOTE: The 0.5-second swap animation is handled by the animation system,
	// not by this component. This function handles the data side instantly.
	UFUNCTION(BlueprintCallable, Category = "Equipment")
	void SwapWeaponLoadout();

private:
	// -----------------------------------------------------------------------
	// Internal helpers
	// -----------------------------------------------------------------------

	// Notify the owner character to refresh its visual mesh components
	// (helmet mesh, body mesh, weapon mesh, etc.) based on current equipment.
	// This calls ARevenCharacter::UpdateEquipmentVisuals() if the owner is
	// a RevenCharacter.
	void UpdateOwnerVisuals();

	// Recalculate stats and broadcast the OnEquipmentChanged delegate.
	// Called after every equip/unequip/swap operation.
	void NotifyEquipmentChanged();
};
