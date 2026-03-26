// ItemPickup.h -- World-space item pickup actor for The Reven
//
// AItemPickup is an actor that represents a dropped/spawned item in the world.
// It implements IInteractable so the player can press E to pick it up.
//
// Lifecycle:
//   1. Something spawns an AItemPickup (loot drop, chest open, player drop).
//   2. The pickup loads its visual mesh from the item's DataTable definition.
//   3. It floats gently (bobbing animation) to draw the player's eye.
//   4. When the player walks close, the interaction sphere overlap fires and
//      the "Press E to pick up [ItemName]" prompt appears.
//   5. Player presses E -> Execute_Interact runs -> item goes into inventory
//      -> pickup destroys itself.
//
// Usage:
//   AItemPickup::SpawnPickup(GetWorld(), DropLocation, "IronSword", 1);

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Items/InteractableInterface.h"
#include "Data/RevenTypes.h"
#include "ItemPickup.generated.h"

// -----------------------------------------------------------------------
// Forward declarations -- avoids including heavy headers in this .h file.
// The .cpp will include the full headers where needed.
// -----------------------------------------------------------------------
class USphereComponent;
class UStaticMeshComponent;
class UWidgetComponent;
class ARevenCharacter;
class UDataTable;

/**
 * AItemPickup -- A world-space actor representing a dropped/spawnable item.
 *
 * Implements IInteractable so the player can pick it up with the interact key.
 * Spawns via the static SpawnPickup() factory or placed in the level by designers.
 */
UCLASS(BlueprintType, Blueprintable)
class AItemPickup : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	AItemPickup();

	// ===================================================================
	// IInteractable Interface
	// ===================================================================

	/**
	 * Called when the player presses the interact key (E) while overlapping
	 * this pickup's interaction sphere. Adds ItemData to the player's
	 * inventory and destroys this actor.
	 *
	 * @param Interactor  The player character who triggered the interaction.
	 */
	virtual void Interact_Implementation(ARevenCharacter* Interactor) override;

	/**
	 * Returns the text shown on the HUD prompt when the player is near
	 * this pickup. Format: "Pick up [DisplayName]".
	 */
	virtual FText GetInteractionText_Implementation() override;

	// ===================================================================
	// Initialization
	// ===================================================================

	/**
	 * Loads an item definition from the master item DataTable and configures
	 * this pickup's visual mesh, display name, and internal ItemData.
	 *
	 * If the item is gear (weapon or armor), random stat rolls are generated
	 * according to the rarity and material tier rules defined in the design doc.
	 *
	 * @param ItemDefID  The row name in the item DataTable (e.g., "IronSword").
	 * @param Quantity   How many of this item (stackable items only; gear is always 1).
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Pickup")
	void InitializeFromDefinition(FName ItemDefID, int32 Quantity = 1);

	// ===================================================================
	// Static Factory
	// ===================================================================

	/**
	 * Convenience factory to spawn a pickup in the world at a given location.
	 * Handles spawning the actor and calling InitializeFromDefinition.
	 *
	 * @param World       The world context (typically GetWorld()).
	 * @param Location    World-space position for the pickup.
	 * @param ItemDefID   Row name in the item DataTable.
	 * @param Quantity    Stack count (1 for gear).
	 * @return            The spawned AItemPickup, or nullptr on failure.
	 */
	UFUNCTION(BlueprintCallable, Category = "Item Pickup", meta = (WorldContext = "WorldContextObject"))
	static AItemPickup* SpawnPickup(UObject* WorldContextObject, FVector Location, FName ItemDefID, int32 Quantity = 1);

protected:

	// ===================================================================
	// AActor Overrides
	// ===================================================================

	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	// ===================================================================
	// Overlap Callbacks
	// ===================================================================

	/**
	 * Fires when an actor enters the interaction sphere.
	 * If the overlapping actor is the player, the interaction prompt is shown.
	 */
	UFUNCTION()
	void OnInteractionSphereBeginOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex,
		bool bFromSweep,
		const FHitResult& SweepResult
	);

	/**
	 * Fires when an actor leaves the interaction sphere.
	 * Hides the interaction prompt.
	 */
	UFUNCTION()
	void OnInteractionSphereEndOverlap(
		UPrimitiveComponent* OverlappedComponent,
		AActor* OtherActor,
		UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex
	);

	// ===================================================================
	// Properties -- Item Data
	// ===================================================================

	/** The item instance this pickup represents. Contains definition ID,
	 *  quantity, unique ID, random stat rolls, durability, etc. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Pickup")
	FInventoryItem ItemData;

	// ===================================================================
	// Properties -- Visuals & Interaction
	// ===================================================================

	/** The static mesh that visually represents the item in the world.
	 *  Set automatically from the item definition's WorldMesh field. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Pickup|Visuals")
	UStaticMeshComponent* MeshComponent;

	/** Sphere collision used to detect when the player is close enough to
	 *  interact. Radius is 150 units (~1.5 meters in UE5 default scale). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Pickup|Interaction")
	USphereComponent* InteractionSphere;

	/** Widget component that shows the "Press E to pick up [ItemName]" prompt.
	 *  Hidden by default; becomes visible when the player enters the sphere. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Item Pickup|Interaction")
	UWidgetComponent* InteractionPromptWidget;

	// ===================================================================
	// Properties -- Bobbing Animation
	// ===================================================================

	/** Speed of the sine-wave bobbing animation (radians per second).
	 *  Higher = faster bob. Default 2.0 gives a gentle, dreamy float. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Pickup|Animation")
	float BobSpeed = 2.0f;

	/** Maximum height offset of the bobbing animation in centimeters.
	 *  The pickup oscillates between -BobHeight and +BobHeight around its
	 *  spawn position. Default 10.0 cm = subtle movement. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Pickup|Animation")
	float BobHeight = 10.0f;

	/** Whether the player can currently pick this item up.
	 *  Can be toggled off to temporarily disable interaction (e.g., during
	 *  a loot drop animation before the item becomes grabbable). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Pickup|Interaction")
	bool bIsInteractable = true;

	// ===================================================================
	// Properties -- DataTable Reference
	// ===================================================================

	/** Reference to the master item DataTable. Set this in the Blueprint
	 *  defaults or assign at runtime. All item definitions are looked up here. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item Pickup|Data")
	UDataTable* ItemDataTable;

private:

	/** The world-space origin position of this actor, recorded at BeginPlay.
	 *  The bobbing animation oscillates around this point. */
	FVector OriginalLocation;

	/** Accumulated time for the bobbing sine wave. */
	float BobTimeAccumulator = 0.0f;

	/** Cached display name from the item definition, used for the prompt text. */
	FText CachedDisplayName;

	/**
	 * Generates random secondary stat rolls for gear items (weapons and armor)
	 * based on the item's rarity and material tier. Called during
	 * InitializeFromDefinition when the item is a weapon or armor piece.
	 *
	 * Roll rules (from inventory-equipment.md):
	 *   - Common:    primary stat only, 0 secondary stats
	 *   - Uncommon:  primary stat + 1 random secondary
	 *   - Rare:      primary stat + 2 random secondaries
	 *   - Epic:      primary stat + 3 random secondaries
	 *   - Legendary: hand-crafted, no random rolls (stats come from DataTable)
	 *
	 * @param Definition  The item definition containing rarity, tier, and primary stat info.
	 */
	void GenerateRandomStatRolls(const FItemDefinition& Definition);
};
