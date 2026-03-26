// ItemPickup.cpp -- Implementation of AItemPickup for The Reven
//
// See ItemPickup.h for class-level documentation.

#include "Items/ItemPickup.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "GameFramework/Character.h"
// NOTE: Include your actual character header once it exists:
// #include "Character/RevenCharacter.h"

// -----------------------------------------------------------------------
// Constructor -- sets up the component hierarchy
// -----------------------------------------------------------------------
AItemPickup::AItemPickup()
{
	// Enable Tick so we can run the bobbing animation every frame.
	PrimaryActorTick.bCanEverTick = true;

	// ---------------------------------------------------------------
	// Mesh Component -- the visible item in the world.
	// This is the root component so that the whole actor moves as one unit.
	// ---------------------------------------------------------------
	MeshComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	// Items should be visible but not block the player's movement.
	// We use "OverlapAll" so the item doesn't have solid collision
	// but can still be detected by traces and overlaps.
	MeshComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComponent->SetGenerateOverlapEvents(false);

	// ---------------------------------------------------------------
	// Interaction Sphere -- detects when the player is close enough.
	// Radius of 150 units means the player must be within ~1.5 meters
	// (at UE5 default scale of 1 unit = 1 cm) to see the prompt.
	// ---------------------------------------------------------------
	InteractionSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractionSphere"));
	InteractionSphere->SetupAttachment(RootComponent);
	InteractionSphere->SetSphereRadius(150.0f);

	// The sphere only generates overlap events -- it never blocks anything.
	InteractionSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	InteractionSphere->SetCollisionResponseToAllChannels(ECR_Ignore);
	InteractionSphere->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	InteractionSphere->SetGenerateOverlapEvents(true);

	// ---------------------------------------------------------------
	// Widget Component -- shows "Press E to pick up [ItemName]".
	// Positioned above the item mesh. Hidden until the player enters
	// the interaction sphere.
	// ---------------------------------------------------------------
	InteractionPromptWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("InteractionPromptWidget"));
	InteractionPromptWidget->SetupAttachment(RootComponent);
	InteractionPromptWidget->SetRelativeLocation(FVector(0.f, 0.f, 60.f)); // Float above the item
	InteractionPromptWidget->SetWidgetSpace(EWidgetSpace::Screen);          // Always faces camera
	InteractionPromptWidget->SetDrawAtDesiredSize(true);
	InteractionPromptWidget->SetVisibility(false); // Hidden until player approaches
}

// -----------------------------------------------------------------------
// BeginPlay
// -----------------------------------------------------------------------
void AItemPickup::BeginPlay()
{
	Super::BeginPlay();

	// Store the spawn location so the bobbing animation oscillates around it.
	OriginalLocation = GetActorLocation();

	// Bind overlap events to our callback functions.
	// OnComponentBeginOverlap fires when any actor enters the sphere.
	// OnComponentEndOverlap fires when any actor leaves the sphere.
	InteractionSphere->OnComponentBeginOverlap.AddDynamic(
		this, &AItemPickup::OnInteractionSphereBeginOverlap);
	InteractionSphere->OnComponentEndOverlap.AddDynamic(
		this, &AItemPickup::OnInteractionSphereEndOverlap);
}

// -----------------------------------------------------------------------
// Tick -- Bobbing animation
//
// Creates a gentle floating effect using a sine wave on the Z axis.
// The item oscillates between OriginalLocation.Z - BobHeight and
// OriginalLocation.Z + BobHeight at a rate controlled by BobSpeed.
// -----------------------------------------------------------------------
void AItemPickup::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// Accumulate time for the sine wave. Using an accumulator instead of
	// GetGameTimeSinceCreation() lets us control BobSpeed independently
	// and avoids precision issues with very large time values.
	BobTimeAccumulator += DeltaTime * BobSpeed;

	// Calculate the Z offset using a sine wave.
	// FMath::Sin returns [-1, 1], so the offset range is [-BobHeight, +BobHeight].
	const float ZOffset = FMath::Sin(BobTimeAccumulator) * BobHeight;

	// Apply the offset to the original spawn location.
	FVector NewLocation = OriginalLocation;
	NewLocation.Z += ZOffset;
	SetActorLocation(NewLocation);
}

// -----------------------------------------------------------------------
// Interact_Implementation -- IInteractable
//
// Called when the player presses E while overlapping the interaction sphere.
// Attempts to add the item to the player's inventory. On success, this
// actor destroys itself. On failure (e.g., full inventory), the pickup
// remains in the world.
// -----------------------------------------------------------------------
void AItemPickup::Interact_Implementation(ARevenCharacter* Interactor)
{
	// Guard: only allow interaction if the pickup is in an interactable state.
	if (!bIsInteractable || !Interactor)
	{
		return;
	}

	// TODO: Once the inventory system is implemented on ARevenCharacter,
	// call the player's AddToInventory function here. Example:
	//
	//   bool bSuccess = Interactor->GetInventoryComponent()->AddItem(ItemData);
	//   if (!bSuccess)
	//   {
	//       // Inventory full -- show "Inventory Full" message to the player.
	//       UE_LOG(LogTemp, Warning, TEXT("Inventory full -- cannot pick up %s"), *CachedDisplayName.ToString());
	//       return;
	//   }
	//
	// For now, log the pickup and destroy.

	UE_LOG(LogTemp, Log, TEXT("Player picked up: %s (x%d)"),
		*CachedDisplayName.ToString(), ItemData.Quantity);

	// Destroy this actor now that the item has been collected.
	Destroy();
}

// -----------------------------------------------------------------------
// GetInteractionText_Implementation -- IInteractable
//
// Returns a user-friendly prompt string. The UI system reads this to
// populate the interaction widget.
// -----------------------------------------------------------------------
FText AItemPickup::GetInteractionText_Implementation()
{
	return FText::Format(
		NSLOCTEXT("ItemPickup", "PickUpPrompt", "Pick up {0}"),
		CachedDisplayName
	);
}

// -----------------------------------------------------------------------
// InitializeFromDefinition
//
// Loads an item definition from the DataTable and sets up this pickup's
// visuals and item data. Should be called immediately after spawning.
//
// Steps:
//   1. Look up the row in the item DataTable.
//   2. Copy relevant fields into our FInventoryItem (ItemData).
//   3. Generate a unique ID for this item instance.
//   4. If it's gear (weapon/armor), roll random secondary stats.
//   5. Set the mesh component's static mesh from the definition.
// -----------------------------------------------------------------------
void AItemPickup::InitializeFromDefinition(FName ItemDefID, int32 Quantity)
{
	// Guard: we need a valid DataTable reference to look up item definitions.
	if (!ItemDataTable)
	{
		UE_LOG(LogTemp, Error,
			TEXT("AItemPickup::InitializeFromDefinition -- ItemDataTable is null! "
			     "Set it in the Blueprint defaults or assign it at runtime."));
		return;
	}

	// Look up the row in the DataTable. FindRow returns a pointer to the
	// struct, or nullptr if the row doesn't exist.
	static const FString ContextString(TEXT("AItemPickup::InitializeFromDefinition"));
	const FItemDefinition* Definition = ItemDataTable->FindRow<FItemDefinition>(
		ItemDefID, ContextString);

	if (!Definition)
	{
		UE_LOG(LogTemp, Error,
			TEXT("AItemPickup::InitializeFromDefinition -- "
			     "Could not find item definition '%s' in DataTable."),
			*ItemDefID.ToString());
		return;
	}

	// --- Populate ItemData from the definition ---

	ItemData.ItemDefinitionID = ItemDefID;
	ItemData.Quantity = Quantity;
	ItemData.UniqueID = FGuid::NewGuid();  // Every item instance gets a globally unique ID.
	ItemData.CurrentDurability = 100.0f;   // Fresh items start at full durability.

	// Cache the display name for prompt text.
	CachedDisplayName = Definition->DisplayName;

	// --- Set the primary stat on the item's stat rolls ---
	// The primary stat is always present on gear. Its type is determined by
	// the equip slot (Head -> Might, Body -> Fortitude, etc.).
	if (Definition->ItemType == EItemType::Weapon || Definition->ItemType == EItemType::Armor)
	{
		ItemData.StatRolls.SetStat(Definition->PrimaryStatType, Definition->PrimaryStatValue);

		// Generate random secondary stat rolls based on rarity.
		// Legendary items have hand-crafted stats and skip random rolls.
		if (Definition->Rarity != ERarity::Legendary)
		{
			GenerateRandomStatRolls(*Definition);
		}
	}

	// --- Set the visual mesh ---
	// WorldMesh is a TSoftObjectPtr, so we need to load it synchronously.
	// For a production game, you'd want to use async loading, but for the
	// vertical slice synchronous loading is fine for pickups.
	if (!Definition->WorldMesh.IsNull())
	{
		UStaticMesh* LoadedMesh = Definition->WorldMesh.LoadSynchronous();
		if (LoadedMesh)
		{
			MeshComponent->SetStaticMesh(LoadedMesh);
		}
	}
}

// -----------------------------------------------------------------------
// SpawnPickup (Static Factory)
//
// Convenience function to spawn an AItemPickup in the world and initialize
// it from a DataTable definition in one call. This is the preferred way
// to create pickups from gameplay code (loot drops, chest contents, etc.).
// -----------------------------------------------------------------------
AItemPickup* AItemPickup::SpawnPickup(
	UObject* WorldContextObject, FVector Location, FName ItemDefID, int32 Quantity)
{
	if (!WorldContextObject)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemPickup::SpawnPickup -- WorldContextObject is null."));
		return nullptr;
	}

	UWorld* World = WorldContextObject->GetWorld();
	if (!World)
	{
		UE_LOG(LogTemp, Error, TEXT("AItemPickup::SpawnPickup -- Could not get World."));
		return nullptr;
	}

	// Set up spawn parameters. We use SpawnActor with a transform.
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride =
		ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

	// Spawn the pickup actor at the desired location with default rotation.
	AItemPickup* Pickup = World->SpawnActor<AItemPickup>(
		AItemPickup::StaticClass(),
		FTransform(FRotator::ZeroRotator, Location),
		SpawnParams
	);

	if (Pickup)
	{
		// Initialize the pickup with the item definition.
		// NOTE: The ItemDataTable must be set on the CDO (Class Default Object)
		// or the spawned instance must have it assigned. In practice, you'd
		// either set it in a Blueprint child class or pass it as a parameter.
		Pickup->InitializeFromDefinition(ItemDefID, Quantity);
	}

	return Pickup;
}

// -----------------------------------------------------------------------
// Overlap Callbacks
// -----------------------------------------------------------------------

void AItemPickup::OnInteractionSphereBeginOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex,
	bool bFromSweep,
	const FHitResult& SweepResult)
{
	// Only show the prompt for the player character, not for other actors
	// (enemies, projectiles, other pickups, etc.).
	// We check if the overlapping actor is a character (Pawn) as a simple
	// filter. In a full implementation, you'd cast to ARevenCharacter.
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()) && bIsInteractable)
	{
		// Make the interaction prompt visible.
		if (InteractionPromptWidget)
		{
			InteractionPromptWidget->SetVisibility(true);
		}
	}
}

void AItemPickup::OnInteractionSphereEndOverlap(
	UPrimitiveComponent* OverlappedComponent,
	AActor* OtherActor,
	UPrimitiveComponent* OtherComp,
	int32 OtherBodyIndex)
{
	// Hide the prompt when the player leaves the interaction radius.
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		if (InteractionPromptWidget)
		{
			InteractionPromptWidget->SetVisibility(false);
		}
	}
}

// -----------------------------------------------------------------------
// GenerateRandomStatRolls (Private)
//
// Rolls random secondary stats for gear items according to the rarity
// and material tier rules from the inventory-equipment design doc.
//
// Rarity determines how many secondary stats are rolled:
//   - Common:   0 secondaries (primary only)
//   - Uncommon: 1 secondary
//   - Rare:     2 secondaries
//   - Epic:     3 secondaries
//
// Secondary stats are chosen randomly from the 4 stats that are NOT
// the item's primary stat, with equal weighting (25% each). No
// duplicates -- each secondary stat can only appear once on an item.
//
// Stat value ranges are driven by the material tier. For the vertical
// slice (levels 1-10), the tiers and ranges are:
//
//   T1 Leather:         Uncommon 1, Rare 1, Epic 1-2
//   T2 Iron:            Uncommon 1-2, Rare 1-2, Epic 1-3
//   T3 Reinforced Iron: Uncommon 1-3, Rare 2-3, Epic 2-4
//   T4 Dark Iron:       Uncommon 2-4, Rare 3-5, Epic 3-6
// -----------------------------------------------------------------------
void AItemPickup::GenerateRandomStatRolls(const FItemDefinition& Definition)
{
	// Determine how many secondary stats to roll based on rarity.
	int32 NumSecondaryStats = 0;
	switch (Definition.Rarity)
	{
		case ERarity::Common:
			NumSecondaryStats = 0; // Common items have no secondary stats.
			break;
		case ERarity::Uncommon:
			NumSecondaryStats = 1;
			break;
		case ERarity::Rare:
			NumSecondaryStats = 2;
			break;
		case ERarity::Epic:
			NumSecondaryStats = 3;
			break;
		default:
			// Legendary items should not reach this function.
			NumSecondaryStats = 0;
			break;
	}

	if (NumSecondaryStats == 0)
	{
		return; // Nothing to roll.
	}

	// Build the pool of available secondary stats by excluding the primary stat.
	// There are 5 stats total; the primary is guaranteed, so 4 remain.
	TArray<EStatType> StatPool;
	StatPool.Reserve(4);

	const TArray<EStatType> AllStats = {
		EStatType::Might,
		EStatType::Fortitude,
		EStatType::Haste,
		EStatType::Agility,
		EStatType::Sorcery
	};

	for (EStatType Stat : AllStats)
	{
		if (Stat != Definition.PrimaryStatType)
		{
			StatPool.Add(Stat);
		}
	}

	// Determine the min/max value range for secondary stats based on material
	// tier and rarity. These values come from the design doc tables.
	int32 MinValue = 1;
	int32 MaxValue = 1;

	switch (Definition.MaterialTier)
	{
		case 1: // Leather
			switch (Definition.Rarity)
			{
				case ERarity::Uncommon: MinValue = 1; MaxValue = 1; break;
				case ERarity::Rare:     MinValue = 1; MaxValue = 1; break;
				case ERarity::Epic:     MinValue = 1; MaxValue = 2; break;
				default: break;
			}
			break;

		case 2: // Iron
			switch (Definition.Rarity)
			{
				case ERarity::Uncommon: MinValue = 1; MaxValue = 2; break;
				case ERarity::Rare:     MinValue = 1; MaxValue = 2; break;
				case ERarity::Epic:     MinValue = 1; MaxValue = 3; break;
				default: break;
			}
			break;

		case 3: // Reinforced Iron
			switch (Definition.Rarity)
			{
				case ERarity::Uncommon: MinValue = 1; MaxValue = 3; break;
				case ERarity::Rare:     MinValue = 2; MaxValue = 3; break;
				case ERarity::Epic:     MinValue = 2; MaxValue = 4; break;
				default: break;
			}
			break;

		case 4: // Dark Iron
			switch (Definition.Rarity)
			{
				case ERarity::Uncommon: MinValue = 2; MaxValue = 4; break;
				case ERarity::Rare:     MinValue = 3; MaxValue = 5; break;
				case ERarity::Epic:     MinValue = 3; MaxValue = 6; break;
				default: break;
			}
			break;

		default:
			UE_LOG(LogTemp, Warning,
				TEXT("AItemPickup::GenerateRandomStatRolls -- Unknown material tier %d. "
				     "Using default range [1, 1]."),
				Definition.MaterialTier);
			break;
	}

	// Roll each secondary stat: pick a random stat from the pool, roll a
	// value in [MinValue, MaxValue], assign it, then remove from pool to
	// prevent duplicate stats on the same item.
	for (int32 i = 0; i < NumSecondaryStats && StatPool.Num() > 0; ++i)
	{
		// Pick a random index from the remaining pool (equal 25% weighting).
		const int32 RandomIndex = FMath::RandRange(0, StatPool.Num() - 1);
		const EStatType ChosenStat = StatPool[RandomIndex];

		// Roll the stat value.
		const float RolledValue = static_cast<float>(FMath::RandRange(MinValue, MaxValue));

		// Assign the rolled value to the item's stat block.
		ItemData.StatRolls.SetStat(ChosenStat, RolledValue);

		// Remove the chosen stat from the pool so it can't be rolled again.
		StatPool.RemoveAt(RandomIndex);
	}
}
