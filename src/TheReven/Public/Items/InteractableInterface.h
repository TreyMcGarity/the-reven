// InteractableInterface.h
// The Reven - Interactable Interface
//
// This is a UE5 "Interface" -- it defines a contract that any actor can implement.
// Think of it like a promise: "If you implement IInteractable, you MUST provide
// these functions." This lets the player interact with doors, NPCs, chests,
// item pickups, etc. without the player needing to know what the object IS --
// only that it CAN be interacted with.
//
// How UE5 interfaces work:
//   1. UInteractableInterface (the U-class) is an empty shell that UE5 needs
//      for its reflection system. You never touch it.
//   2. IInteractable (the I-class) is where you declare the actual functions.
//   3. Any actor that wants to be interactable inherits from IInteractable
//      and implements the functions.
//   4. To check if an actor is interactable: actor->Implements<UInteractableInterface>()
//   5. To call the function: IInteractable::Execute_Interact(actor, Interactor)

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "InteractableInterface.generated.h"

// -----------------------------------------------------------------------
// Forward declaration -- we reference the player character but don't need
// the full header here. The .cpp files that USE this interface will
// include RevenCharacter.h themselves.
// -----------------------------------------------------------------------
class ARevenCharacter;

// -----------------------------------------------------------------------
// The U-class: UE5 boilerplate. Do not add anything here.
// The UINTERFACE macro tells Unreal's reflection system that this is
// an interface. MinimalAPI means we don't export symbols (keeps compile fast).
// -----------------------------------------------------------------------
UINTERFACE(MinimalAPI, Blueprintable)
class UInteractableInterface : public UInterface
{
	GENERATED_BODY()
};

// -----------------------------------------------------------------------
// The I-class: This is where you define what interactable objects must do.
// Any class that inherits from IInteractable must implement these functions.
// -----------------------------------------------------------------------
class IInteractable
{
	GENERATED_BODY()

public:

	// ---------------------------------------------------------------
	// Interact
	// Called when the player presses E (or whatever the interact key is)
	// and the line trace hits this actor.
	//
	// @param Interactor - The player character who is interacting.
	//                     This lets the interacted object know WHO
	//                     is interacting (e.g., to give items to the
	//                     correct player in co-op).
	//
	// BlueprintNativeEvent means:
	//   - C++ classes implement this by overriding Execute_Interact
	//   - Blueprint classes can also override it in the event graph
	//   - You call it via: IInteractable::Execute_Interact(TargetActor, Interactor)
	// ---------------------------------------------------------------
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(ARevenCharacter* Interactor);

	// ---------------------------------------------------------------
	// GetInteractionText
	// Returns the text shown on the HUD when the player looks at this
	// object. For example: "Pick up Iron Sword", "Talk to Renna",
	// "Open Chest".
	//
	// @return FText - The localization-friendly text to display.
	//                 We use FText (not FString) because FText supports
	//                 UE5's localization system for translations.
	// ---------------------------------------------------------------
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	FText GetInteractionText();
};
