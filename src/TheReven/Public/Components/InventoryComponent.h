// InventoryComponent.h
// The Reven -- Inventory Component
//
// This component manages the player's (or any actor's) inventory: a flat grid
// of up to MaxSlots items plus a Gold currency counter.
//
// HOW IT WORKS (high-level):
//   1. Attach this component to any actor that should own an inventory.
//   2. Call AddItem() to insert items. Stackable items (materials, consumables)
//      auto-stack up to their StackMaxSize before occupying a new slot.
//   3. Call RemoveItem() by the item's UniqueID to remove a specific stack entry.
//   4. EquipmentComponent reads from this component when equipping / unequipping.
//   5. Two delegates -- OnInventoryChanged and OnGoldChanged -- fire whenever
//      the inventory mutates so that UI widgets can bind and refresh.
//
// The Items array acts as the "grid." Each element is one slot. An empty slot
// is simply the absence of an element (the array only holds occupied slots).
// When Items.Num() >= MaxSlots, the inventory is full.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Data/RevenTypes.h"
#include "InventoryComponent.generated.h"

// ---------------------------------------------------------------------------
// Forward declarations
// ---------------------------------------------------------------------------
class AItemPickup; // World-spawned pickup actor (used by DropItem)

// ---------------------------------------------------------------------------
// Delegates
// ---------------------------------------------------------------------------

// Fires whenever any item is added or removed from the inventory.
// UI widgets bind to this to refresh the inventory grid display.
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnInventoryChanged);

// Fires whenever the Gold amount changes (earned, spent, picked up, etc.).
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnGoldChanged, int32, NewGold);

// ---------------------------------------------------------------------------
// UInventoryComponent
// ---------------------------------------------------------------------------

UCLASS(ClassGroup = (TheReven), meta = (BlueprintSpawnableComponent))
class THEREVEN_API UInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// -----------------------------------------------------------------------
	// Constructor
	// -----------------------------------------------------------------------
	UInventoryComponent();

protected:
	// Called when the game starts or when the component is spawned at runtime.
	virtual void BeginPlay() override;

public:
	// =======================================================================
	// PROPERTIES
	// =======================================================================

	// The flat list of inventory items. Each entry represents one occupied
	// slot in the inventory grid. Maximum size is MaxSlots.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<FInventoryItem> Items;

	// How many slots the inventory has. Defaults to 30 (6 columns x 5 rows).
	// Can be increased at runtime (e.g., Traveler's Pack upgrade).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 MaxSlots;

	// Currency counter. Gold is NOT stored as an inventory item -- it is a
	// simple integer displayed on the HUD.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Inventory")
	int32 Gold;

	// =======================================================================
	// DELEGATES (BlueprintAssignable -- UI can bind in Blueprint)
	// =======================================================================

	// Broadcast whenever the Items array changes (add, remove, stack update).
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnInventoryChanged OnInventoryChanged;

	// Broadcast whenever Gold is added or removed. Passes the new total.
	UPROPERTY(BlueprintAssignable, Category = "Inventory|Events")
	FOnGoldChanged OnGoldChanged;

	// =======================================================================
	// ITEM MANAGEMENT (BlueprintCallable)
	// =======================================================================

	// Attempt to add an item to the inventory.
	//
	// AUTO-STACKING LOGIC:
	//   If the item's type is Material or Consumable (i.e., stackable), we
	//   first look for an existing stack of the same ItemDefID that hasn't
	//   reached StackMaxSize. If found, we merge as much quantity as possible
	//   into that stack. Any remainder creates a new slot.
	//
	// Returns TRUE if the item was added (fully or partially stacked).
	// Returns FALSE if the inventory is completely full and nothing could be
	// added -- the caller should show a "Your inventory is full" message.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool AddItem(FInventoryItem Item);

	// Remove a specific quantity from the stack identified by UniqueID.
	//   - If Quantity >= the stack's current quantity, the entire slot is removed.
	//   - If Quantity < the stack's current quantity, the stack shrinks.
	// Returns TRUE if the item was found and removed (or decremented).
	// Returns FALSE if no item with that UniqueID exists.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool RemoveItem(FGuid UniqueID, int32 Quantity = 1);

	// Check whether the inventory contains at least 'Quantity' of the item
	// identified by ItemDefID (the data table row name, e.g., "MAT_IronOre").
	// Searches across ALL stacks of that item and sums their quantities.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	bool HasItem(FName ItemDefID, int32 Quantity = 1) const;

	// Count the total quantity of a given item definition across all stacks.
	// For example, if two slots each hold 50 Iron Ore, this returns 100.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	int32 GetItemCount(FName ItemDefID) const;

	// Find a specific inventory item by its runtime-unique ID (FGuid).
	// Returns a pointer to the item in the Items array, or nullptr if not found.
	//
	// NOTE: The returned pointer is INVALIDATED if the Items array is modified
	// (add/remove). Cache the data you need, don't hold the pointer long-term.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	FInventoryItem* FindItem(FGuid UniqueID);

	// Get a reference to the full Items array. Useful for UI iteration.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	TArray<FInventoryItem>& GetAllItems();

	// Returns TRUE if at least one slot is available (Items.Num() < MaxSlots).
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory")
	bool CanAddItem() const;

	// =======================================================================
	// GOLD MANAGEMENT (BlueprintCallable)
	// =======================================================================

	// Add gold. Amount must be positive. Broadcasts OnGoldChanged.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Gold")
	void AddGold(int32 Amount);

	// Attempt to spend gold. Returns FALSE if the player doesn't have enough.
	// On success, subtracts the amount and broadcasts OnGoldChanged.
	UFUNCTION(BlueprintCallable, Category = "Inventory|Gold")
	bool RemoveGold(int32 Amount);

	// Get the current gold total.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Inventory|Gold")
	int32 GetGold() const;

	// =======================================================================
	// WORLD INTERACTION
	// =======================================================================

	// Drop an item into the world. This:
	//   1. Finds the item in the inventory by UniqueID.
	//   2. Removes it from the Items array.
	//   3. Spawns an AItemPickup actor slightly in front of the owning actor
	//      so the player can see it land.
	// Does nothing if the item is not found or is a quest item (non-droppable).
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void DropItem(FGuid UniqueID);

private:
	// -----------------------------------------------------------------------
	// Internal helpers
	// -----------------------------------------------------------------------

	// Returns true if the given item type supports stacking (materials,
	// consumables). Weapons and armor never stack.
	bool IsStackable(EItemType ItemType) const;

	// Try to merge 'Item' into an existing stack of the same definition.
	// Returns the quantity that could NOT be absorbed (remainder).
	// If the full quantity was absorbed, returns 0.
	int32 TryStackItem(FInventoryItem& Item);
};
