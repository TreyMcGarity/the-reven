// InventoryComponent.cpp
// The Reven -- Inventory Component Implementation
//
// See InventoryComponent.h for the full class overview and function contracts.

#include "Components/InventoryComponent.h"
// AItemPickup is the world actor we spawn when the player drops an item.
// #include "Actors/ItemPickup.h"  // TODO: Uncomment once AItemPickup is created

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
UInventoryComponent::UInventoryComponent()
{
	// This component does not need to tick every frame. Inventory changes are
	// event-driven (player picks up item, opens shop, etc.).
	PrimaryComponentTick.bCanEverTick = false;

	// Default inventory size: 30 slots (6 columns x 5 rows).
	MaxSlots = 30;

	// Player starts with no gold.
	Gold = 0;
}

// ---------------------------------------------------------------------------
// BeginPlay
// ---------------------------------------------------------------------------
void UInventoryComponent::BeginPlay()
{
	Super::BeginPlay();

	// Reserve memory for the full grid to avoid repeated allocations as items
	// are added. This does NOT create 30 empty entries -- it just pre-allocates
	// the underlying array memory.
	Items.Reserve(MaxSlots);
}

// ===========================================================================
// ITEM MANAGEMENT
// ===========================================================================

bool UInventoryComponent::AddItem(FInventoryItem Item)
{
	// -----------------------------------------------------------------------
	// Step 1: Auto-stacking
	// -----------------------------------------------------------------------
	// If this item type supports stacking (materials, consumables), try to
	// merge it into an existing stack before using a new slot.
	if (IsStackable(Item.ItemType))
	{
		int32 Remainder = TryStackItem(Item);

		if (Remainder <= 0)
		{
			// The entire quantity was absorbed into existing stack(s).
			// Broadcast the change and return success.
			OnInventoryChanged.Broadcast();
			return true;
		}

		// Some quantity remains -- update the item's quantity to the leftover
		// and fall through to try adding it as a new slot.
		Item.Quantity = Remainder;
	}

	// -----------------------------------------------------------------------
	// Step 2: Check for available slot
	// -----------------------------------------------------------------------
	if (!CanAddItem())
	{
		// No room. The caller should display "Your inventory is full."
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: Cannot add item '%s' -- inventory is full (%d/%d slots)."),
			*Item.ItemDefID.ToString(), Items.Num(), MaxSlots);
		return false;
	}

	// -----------------------------------------------------------------------
	// Step 3: Add as a new slot
	// -----------------------------------------------------------------------
	// Ensure the item has a valid unique ID. If the caller didn't set one
	// (e.g., freshly created from a loot table), generate one now.
	if (!Item.UniqueID.IsValid())
	{
		Item.UniqueID = FGuid::NewGuid();
	}

	Items.Add(Item);

	// Let the UI know the inventory changed.
	OnInventoryChanged.Broadcast();

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent: Added item '%s' (x%d) to slot %d."),
		*Item.ItemDefID.ToString(), Item.Quantity, Items.Num() - 1);

	return true;
}

bool UInventoryComponent::RemoveItem(FGuid UniqueID, int32 Quantity)
{
	// Search for the item by its unique runtime ID.
	for (int32 i = 0; i < Items.Num(); ++i)
	{
		if (Items[i].UniqueID == UniqueID)
		{
			if (Quantity >= Items[i].Quantity)
			{
				// Remove the entire slot. RemoveAtSwap is O(1) because it
				// moves the last element into this slot. Order doesn't matter
				// since the UI can re-sort.
				UE_LOG(LogTemp, Log, TEXT("InventoryComponent: Removed item '%s' entirely from slot %d."),
					*Items[i].ItemDefID.ToString(), i);
				Items.RemoveAt(i);
			}
			else
			{
				// Decrement the stack. The slot stays.
				Items[i].Quantity -= Quantity;
				UE_LOG(LogTemp, Log, TEXT("InventoryComponent: Decremented item '%s' by %d. Remaining: %d."),
					*Items[i].ItemDefID.ToString(), Quantity, Items[i].Quantity);
			}

			OnInventoryChanged.Broadcast();
			return true;
		}
	}

	// Item not found.
	UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: RemoveItem failed -- no item with UniqueID '%s'."),
		*UniqueID.ToString());
	return false;
}

bool UInventoryComponent::HasItem(FName ItemDefID, int32 Quantity) const
{
	// Sum quantities across all stacks matching this definition.
	return GetItemCount(ItemDefID) >= Quantity;
}

int32 UInventoryComponent::GetItemCount(FName ItemDefID) const
{
	int32 Total = 0;

	for (const FInventoryItem& Item : Items)
	{
		if (Item.ItemDefID == ItemDefID)
		{
			Total += Item.Quantity;
		}
	}

	return Total;
}

FInventoryItem* UInventoryComponent::FindItem(FGuid UniqueID)
{
	for (FInventoryItem& Item : Items)
	{
		if (Item.UniqueID == UniqueID)
		{
			return &Item;
		}
	}

	return nullptr;
}

TArray<FInventoryItem>& UInventoryComponent::GetAllItems()
{
	return Items;
}

bool UInventoryComponent::CanAddItem() const
{
	return Items.Num() < MaxSlots;
}

// ===========================================================================
// GOLD MANAGEMENT
// ===========================================================================

void UInventoryComponent::AddGold(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: AddGold called with non-positive amount (%d). Ignoring."), Amount);
		return;
	}

	Gold += Amount;
	OnGoldChanged.Broadcast(Gold);

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent: Added %d gold. New total: %d."), Amount, Gold);
}

bool UInventoryComponent::RemoveGold(int32 Amount)
{
	if (Amount <= 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: RemoveGold called with non-positive amount (%d). Ignoring."), Amount);
		return false;
	}

	if (Gold < Amount)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: Cannot remove %d gold -- only have %d."), Amount, Gold);
		return false;
	}

	Gold -= Amount;
	OnGoldChanged.Broadcast(Gold);

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent: Removed %d gold. New total: %d."), Amount, Gold);
	return true;
}

int32 UInventoryComponent::GetGold() const
{
	return Gold;
}

// ===========================================================================
// WORLD INTERACTION
// ===========================================================================

void UInventoryComponent::DropItem(FGuid UniqueID)
{
	// -----------------------------------------------------------------------
	// Step 1: Find the item in our inventory
	// -----------------------------------------------------------------------
	FInventoryItem* ItemPtr = FindItem(UniqueID);
	if (!ItemPtr)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: DropItem failed -- item not found."));
		return;
	}

	// Quest items / key items cannot be dropped. They have a lock icon in the
	// UI and should never reach this code path, but we guard against it anyway.
	if (ItemPtr->ItemType == EItemType::Artifact)
	{
		UE_LOG(LogTemp, Warning, TEXT("InventoryComponent: Cannot drop quest/artifact item '%s'."),
			*ItemPtr->ItemDefID.ToString());
		return;
	}

	// -----------------------------------------------------------------------
	// Step 2: Cache item data before removing (pointer will be invalidated)
	// -----------------------------------------------------------------------
	FInventoryItem DroppedItem = *ItemPtr;

	// -----------------------------------------------------------------------
	// Step 3: Remove from inventory
	// -----------------------------------------------------------------------
	RemoveItem(UniqueID, DroppedItem.Quantity);

	// -----------------------------------------------------------------------
	// Step 4: Spawn a pickup actor in the world
	// -----------------------------------------------------------------------
	AActor* Owner = GetOwner();
	if (!Owner)
	{
		UE_LOG(LogTemp, Error, TEXT("InventoryComponent: DropItem -- no owning actor!"));
		return;
	}

	// Calculate spawn location: slightly in front of the owner so the player
	// can see the dropped item. 100 units forward, 50 units down (to hit ground).
	FVector SpawnLocation = Owner->GetActorLocation()
		+ Owner->GetActorForwardVector() * 100.0f
		+ FVector(0.0f, 0.0f, -50.0f);

	FRotator SpawnRotation = FRotator::ZeroRotator;

	// TODO: Uncomment once AItemPickup class is implemented.
	// FActorSpawnParameters SpawnParams;
	// SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	//
	// AItemPickup* Pickup = GetWorld()->SpawnActor<AItemPickup>(
	//     AItemPickup::StaticClass(),
	//     SpawnLocation,
	//     SpawnRotation,
	//     SpawnParams
	// );
	//
	// if (Pickup)
	// {
	//     Pickup->InitializeFromItem(DroppedItem);
	// }

	UE_LOG(LogTemp, Log, TEXT("InventoryComponent: Dropped item '%s' at %s."),
		*DroppedItem.ItemDefID.ToString(), *SpawnLocation.ToString());
}

// ===========================================================================
// INTERNAL HELPERS
// ===========================================================================

bool UInventoryComponent::IsStackable(EItemType ItemType) const
{
	// Only materials and consumables stack. Weapons, armor, and artifacts
	// always occupy one slot per unit.
	return (ItemType == EItemType::Material || ItemType == EItemType::Consumable);
}

int32 UInventoryComponent::TryStackItem(FInventoryItem& Item)
{
	// Walk through all existing slots looking for stacks of the same item
	// definition that have room to absorb more.
	int32 Remaining = Item.Quantity;

	for (FInventoryItem& Existing : Items)
	{
		if (Remaining <= 0)
		{
			break;
		}

		// Must be the same item definition to stack.
		if (Existing.ItemDefID != Item.ItemDefID)
		{
			continue;
		}

		// Calculate how many more units this stack can hold.
		// StackMaxSize comes from the item definition (e.g., 99 for materials,
		// 20 for consumables). If it's 0 or unset, default to 1 (no stacking).
		int32 MaxStack = Existing.StackMaxSize > 0 ? Existing.StackMaxSize : 1;
		int32 AvailableSpace = MaxStack - Existing.Quantity;

		if (AvailableSpace <= 0)
		{
			// This stack is already full, try the next one.
			continue;
		}

		// Merge as much as we can into this stack.
		int32 ToAdd = FMath::Min(Remaining, AvailableSpace);
		Existing.Quantity += ToAdd;
		Remaining -= ToAdd;

		UE_LOG(LogTemp, Log, TEXT("InventoryComponent: Stacked %d of '%s' into existing slot (now %d/%d)."),
			ToAdd, *Item.ItemDefID.ToString(), Existing.Quantity, MaxStack);
	}

	// Return whatever quantity couldn't be absorbed. The caller will add a
	// new slot for this remainder (if space allows).
	return Remaining;
}
