// RevenCharacter.h
// The Reven - Player Character Class
//
// This is the MAIN player character class. It acts as a COORDINATOR:
//   - It OWNS components (inventory, combat, stats, etc.)
//   - It RECEIVES input from the player
//   - It ROUTES that input to the correct component
//   - It does NOT contain game logic itself
//
// Why this design?
//   - Each component handles one responsibility (Single Responsibility Principle)
//   - You can work on combat without touching inventory code
//   - Components can be reused on NPCs or enemies if needed
//   - It's easier to debug: if damage is wrong, look at StatsComponent, not this file
//
// This class handles:
//   1. Visual setup (camera, mesh, armor slots, weapon slots)
//   2. Input binding (keyboard/mouse -> function calls)
//   3. Movement speeds (walk, run, sprint)
//   4. Interaction system (line trace to find interactable objects)

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"  // Needed for FInputActionValue in input handlers
#include "RevenCharacter.generated.h"

// ============================================================================
// FORWARD DECLARATIONS
// ============================================================================
// Forward declarations tell the compiler "this class exists, I'll include its
// header later." This speeds up compilation because the compiler doesn't need
// to read those headers just to know the class name exists.
//
// We forward-declare components here because this header only uses POINTERS
// to them (e.g., UInventoryComponent*). Pointers are always the same size,
// so the compiler doesn't need the full class definition -- just the name.
// The .cpp file will #include the actual headers when it needs to call
// functions on these components.
// ============================================================================

// -- UE5 built-in components --
class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UStaticMeshComponent;

// -- Enhanced Input System classes --
class UInputMappingContext;
class UInputAction;

// -- Our custom gameplay components (created by other agents) --
// These don't exist yet! That's fine. Forward declarations let us compile
// this file without the actual component code. When those components are
// created later, everything will link together.
class UInventoryComponent;
class UEquipmentComponent;
class UStatsComponent;
class UHealthComponent;
class UStaminaComponent;
class UCombatComponent;

// ============================================================================
// ARevenCharacter
// ============================================================================
// The "A" prefix means this is an Actor class (UE5 naming convention).
// ACharacter gives us: a capsule collider, skeletal mesh, movement component,
// and built-in support for walking, jumping, crouching, etc.
// ============================================================================
UCLASS(config=Game)
class THEREVEN_API ARevenCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Constructor -- sets up components and default values
	ARevenCharacter();

protected:
	// Called when the game starts or when the actor is spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind input actions to functions
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ====================================================================
	// SECTION 1: CAMERA COMPONENTS
	// ====================================================================
	// The camera system uses a "spring arm" (boom) attached to the character.
	// The camera sits at the end of the boom, giving us third-person view.
	// The spring arm handles collision: if a wall is behind the player, the
	// arm shortens so the camera doesn't clip through geometry.
	// ====================================================================

	/** Camera boom -- positions the camera behind the character.
	 *  Arm length 300 = camera is 300 units behind the player.
	 *  Camera lag = smooth follow (camera glides instead of snapping). */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USpringArmComponent> CameraBoom;

	/** Follow camera -- the actual camera that renders the player's view. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Camera", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCameraComponent> FollowCamera;

	// ====================================================================
	// SECTION 2: ARMOR VISUAL MESH SLOTS
	// ====================================================================
	// These are additional skeletal meshes attached to the character's main
	// skeleton. When the player equips armor, the EquipmentComponent sets
	// the skeletal mesh on the appropriate slot.
	//
	// Why SkeletalMeshComponent (not StaticMesh)?
	//   Armor needs to deform WITH the character's skeleton. If the player
	//   raises their arm, the glove mesh needs to move with it. Skeletal
	//   meshes follow the bone transforms of their parent skeleton.
	//
	// "Leader Pose Component" is set in the constructor so these meshes
	// automatically follow the main body's skeleton animations.
	// ====================================================================

	/** Helmet armor slot -- attached to the head bone area */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Armor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> HelmetMesh;

	/** Body armor slot -- chest, torso, shoulders */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Armor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> BodyMesh;

	/** Gloves armor slot -- hands and forearms */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Armor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> GlovesMesh;

	/** Leg armor slot -- thighs, knees, greaves */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Armor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> LegsMesh;

	/** Boot armor slot -- feet and ankles */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Armor", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USkeletalMeshComponent> BootsMesh;

	// ====================================================================
	// SECTION 3: WEAPON VISUAL MESH SLOTS
	// ====================================================================
	// Weapons use StaticMeshComponent (not Skeletal) because weapons are
	// rigid objects -- a sword doesn't bend. They're attached to sockets
	// on the character's skeleton (e.g., "hand_r_socket" for main hand).
	//
	// Socket names are defined on the skeleton in the UE5 editor. The
	// EquipmentComponent tells these meshes what to display and which
	// socket to attach to when weapons are equipped/swapped.
	//
	// Three slots support the game's weapon system:
	//   - MainHand: sword, axe, mace (1H) or greatsword/glaive (2H)
	//   - OffHand: shield, torch, or empty
	//   - BackWeapon: 2H weapon stored on back when using 1H setup
	// ====================================================================

	/** Main hand weapon visual -- attached to right hand socket */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Weapons", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MainHandMesh;

	/** Off hand weapon/shield visual -- attached to left hand socket */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Weapons", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> OffHandMesh;

	/** Back weapon visual -- for stowed 2H weapon on the character's back */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Equipment|Weapons", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> BackWeaponMesh;

	// ====================================================================
	// SECTION 4: GAMEPLAY COMPONENTS
	// ====================================================================
	// These components contain the ACTUAL game logic. The character class
	// just owns them and routes input to them.
	//
	// Each component is created in the constructor with CreateDefaultSubobject.
	// They are automatically replicated, saved, and managed by UE5.
	//
	// These will be implemented by other agents -- we just declare pointers
	// here so the character knows they exist.
	// ====================================================================

	/** Manages the player's inventory (items, stacking, weight, etc.) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInventoryComponent> InventoryComponent;

	/** Manages equipped gear (armor slots, weapon slots, stat bonuses from gear) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UEquipmentComponent> EquipmentComponent;

	/** Tracks character stats: Might, Fortitude, Haste, Agility, Sorcery */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStatsComponent> StatsComponent;

	/** Manages health: current HP, max HP, taking damage, healing, death */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UHealthComponent> HealthComponent;

	/** Manages stamina: current stamina, costs, regen, exhaustion state */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaminaComponent> StaminaComponent;

	/** Handles combat logic: attacks, combos, hit detection, damage application */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCombatComponent> CombatComponent;

	// ====================================================================
	// SECTION 5: CHARACTER STATS AND PROGRESSION
	// ====================================================================
	// Base stats for the player character. These are the CHARACTER's stats,
	// not the component stats. The StatsComponent will READ these and combine
	// them with gear bonuses, buffs, etc. to calculate final values.
	// ====================================================================

	/** Current character level (starts at 1, vertical slice goes to 10) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Progression")
	int32 CharacterLevel;

	/** Current experience points -- when this reaches XPToNextLevel, the character levels up */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Progression")
	int32 CurrentXP;

	/** XP needed to reach the next level -- scales with level */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Progression")
	int32 XPToNextLevel;

	/** Base Might stat -- increases melee and ranged damage.
	 *  Formula: damage * (1 + Might / 100) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Base")
	int32 BaseMight;

	/** Base Fortitude stat -- damage reduction and max HP bonus.
	 *  DR = Fortitude / (Fortitude + 100); HP bonus = Fortitude * 2 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Base")
	int32 BaseFortitude;

	/** Base Haste stat -- movement speed and stamina regen.
	 *  Move speed = base * (1 + Haste / 200); Stamina regen boost */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Base")
	int32 BaseHaste;

	/** Base Agility stat -- attack speed and dodge i-frame duration.
	 *  Montage play rate = 1.0 + Agility / 200 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Base")
	int32 BaseAgility;

	/** Base Sorcery stat -- spell damage, mana regen, cooldown reduction.
	 *  Spell damage * (1 + Sorcery / 100) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats|Base")
	int32 BaseSorcery;

	// ====================================================================
	// SECTION 6: MOVEMENT SPEEDS
	// ====================================================================
	// Three movement modes with different speeds. The character movement
	// component's MaxWalkSpeed is changed based on the current mode.
	//
	// Design rationale from combat doc:
	//   - Sprinting into combat means arriving with reduced stamina
	//   - Walk speed during blocking is 50% of WalkSpeed (set in combat logic)
	// ====================================================================

	/** Walking speed -- default movement, 300 units/sec */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float WalkSpeed;

	/** Running speed -- when moving with intent, 600 units/sec */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float RunSpeed;

	/** Sprint speed -- holding sprint key, drains 5 stamina/sec, 900 units/sec */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float SprintSpeed;

	/** Whether the character is currently sprinting (used by anim blueprint) */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	bool bIsSprinting;

	// ====================================================================
	// SECTION 7: ENHANCED INPUT -- INPUT ACTIONS
	// ====================================================================
	// UE5's Enhanced Input System uses "Input Actions" (UInputAction) to
	// represent player intentions. Each action is an asset created in the
	// editor (right-click -> Input -> Input Action).
	//
	// These are set in the Blueprint derived from this class, NOT hardcoded.
	// This means designers can remap keys without touching C++ code.
	//
	// The InputMappingContext defines which keys trigger which actions.
	// It's added to the player controller in BeginPlay.
	// ====================================================================

	/** Input mapping context -- defines key bindings for all actions.
	 *  Created in UE5 editor: right-click Content Browser -> Input -> Input Mapping Context */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// -- Core Movement Inputs --

	/** Move action -- WASD/left stick. Value type: Axis2D (X = right/left, Y = forward/back) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Move;

	/** Look action -- mouse delta/right stick. Value type: Axis2D */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Look;

	/** Jump action -- spacebar. Value type: Bool (pressed/released) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Jump;

	/** Sprint action -- shift (hold). Value type: Bool (started/completed) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Movement", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Sprint;

	// -- Combat Inputs --

	/** Light attack -- left mouse click. Tap for combo chain. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_LightAttack;

	/** Heavy attack -- left mouse hold (0.4s+), triggers on release. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_HeavyAttack;

	/** Block -- right mouse hold. First 200ms is parry window. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Block;

	/** Dodge -- shift tap (or shift in combat). Costs 20 stamina. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Combat", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Dodge;

	// -- Interaction Input --

	/** Interact -- E key. Talks to NPCs, picks up items, opens doors. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Interaction", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Interact;

	// -- Spell Slot Inputs (keys 1-5) --

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Spells", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Spell1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Spells", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Spell2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Spells", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Spell3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Spells", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Spell4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Spells", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Spell5;

	// -- Item Slot Inputs (keys 6-0) --

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Items", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Item1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Items", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Item2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Items", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Item3;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Items", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Item4;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input|Items", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInputAction> IA_Item5;

	// ====================================================================
	// SECTION 8: INTERACTION SYSTEM
	// ====================================================================

	/** How far the interaction line trace reaches (in Unreal units, ~cm).
	 *  300 units = about 3 meters -- close enough to feel natural. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
	float InteractionTraceDistance;

	// ====================================================================
	// SECTION 9: PUBLIC ACCESSOR FUNCTIONS
	// ====================================================================
	// UE5 convention: provide getter functions for components so other
	// classes can safely access them without direct pointer access.
	// FORCEINLINE = compiler hint to inline these tiny functions for speed.
	// ====================================================================

	/** Returns the camera boom subobject */
	FORCEINLINE USpringArmComponent* GetCameraBoom() const { return CameraBoom; }

	/** Returns the follow camera subobject */
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }

protected:

	// ====================================================================
	// SECTION 10: INPUT HANDLER FUNCTIONS
	// ====================================================================
	// These functions are called when the player presses keys/buttons.
	// They are bound in SetupPlayerInputComponent.
	//
	// Each handler receives an FInputActionValue which contains the input
	// data (bool for buttons, Vector2D for movement/look, etc.).
	//
	// The naming convention "On[Action]" makes it clear these are responses
	// to player input events.
	// ====================================================================

	// -- Movement --

	/** Handle WASD/stick movement. Calculates direction and moves the character. */
	void OnMove(const FInputActionValue& Value);

	/** Handle mouse/stick camera rotation. */
	void OnLook(const FInputActionValue& Value);

	/** Handle jump input. Uses ACharacter's built-in jump. */
	void OnJump(const FInputActionValue& Value);

	/** Sprint started -- increase movement speed, begin stamina drain. */
	void OnSprintStart(const FInputActionValue& Value);

	/** Sprint stopped -- return to normal movement speed. */
	void OnSprintStop(const FInputActionValue& Value);

	// -- Combat --

	/** Light attack pressed -- route to CombatComponent for combo handling. */
	void OnLightAttack(const FInputActionValue& Value);

	/** Heavy attack triggered (hold + release) -- route to CombatComponent. */
	void OnHeavyAttack(const FInputActionValue& Value);

	/** Block started (RMB pressed) -- route to CombatComponent. Also starts parry window. */
	void OnBlockStart(const FInputActionValue& Value);

	/** Block stopped (RMB released) -- route to CombatComponent. */
	void OnBlockStop(const FInputActionValue& Value);

	/** Dodge pressed -- route to CombatComponent. Costs 20 stamina. */
	void OnDodge(const FInputActionValue& Value);

	// -- Interaction --

	/** Interact pressed (E key) -- performs line trace and interacts with hit actor. */
	void OnInteract(const FInputActionValue& Value);

	// -- Spell Slots --

	/** Cast spell in slot 1 (key 1) */
	void OnSpell1(const FInputActionValue& Value);

	/** Cast spell in slot 2 (key 2) */
	void OnSpell2(const FInputActionValue& Value);

	/** Cast spell in slot 3 (key 3) */
	void OnSpell3(const FInputActionValue& Value);

	/** Cast spell in slot 4 (key 4) */
	void OnSpell4(const FInputActionValue& Value);

	/** Cast spell in slot 5 (key 5) */
	void OnSpell5(const FInputActionValue& Value);

	// -- Item Slots --

	/** Use item in slot 1 (key 6) */
	void OnItem1(const FInputActionValue& Value);

	/** Use item in slot 2 (key 7) */
	void OnItem2(const FInputActionValue& Value);

	/** Use item in slot 3 (key 8) */
	void OnItem3(const FInputActionValue& Value);

	/** Use item in slot 4 (key 9) */
	void OnItem4(const FInputActionValue& Value);

	/** Use item in slot 5 (key 0) */
	void OnItem5(const FInputActionValue& Value);

private:

	// ====================================================================
	// SECTION 11: INTERNAL HELPERS
	// ====================================================================

	/** Performs the interaction line trace and returns the hit actor (if any).
	 *  This is separated from OnInteract so it can also be used by the UI
	 *  to show interaction prompts before the player presses E. */
	AActor* PerformInteractionTrace();
};
