// RevenCharacter.cpp
// The Reven - Player Character Implementation
//
// This file contains the IMPLEMENTATION of everything declared in RevenCharacter.h.
// It sets up the character's components, handles input, and provides the
// interaction system.
//
// Remember: this class is a COORDINATOR. It creates components and routes
// input to them. The actual combat logic, stat calculations, inventory
// management, etc. all live in their respective components.

#include "Character/RevenCharacter.h"

// -- UE5 Engine Components --
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"

// -- Enhanced Input System --
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

// -- Our Custom Components --
// These headers don't exist yet -- they'll be created by other agents.
// Once they're created, uncomment these includes.
// #include "Components/InventoryComponent.h"
// #include "Components/EquipmentComponent.h"
// #include "Components/StatsComponent.h"
// #include "Components/HealthComponent.h"
// #include "Components/StaminaComponent.h"
// #include "Components/CombatComponent.h"

// -- Interaction Interface --
#include "Items/InteractableInterface.h"

// -- Debug drawing (for interaction line trace visualization during development) --
#include "DrawDebugHelpers.h"

// ============================================================================
// CONSTRUCTOR
// ============================================================================
// The constructor runs ONCE when the character is first created (in the editor
// or when spawned at runtime). This is where we:
//   1. Set up the capsule (collision shape)
//   2. Configure movement defaults
//   3. Create all visual components (camera, armor, weapons)
//   4. Create all gameplay components
//   5. Set default stat values
//
// IMPORTANT: The constructor CANNOT access the game world. No spawning actors,
// no finding other actors, no playing sounds. That goes in BeginPlay.
// ============================================================================
ARevenCharacter::ARevenCharacter()
{
	// ------------------------------------------------------------------
	// Tick Configuration
	// ------------------------------------------------------------------
	// We enable Tick but keep it LIGHTWEIGHT. Heavy work is delegated to
	// components, which have their own Tick functions.
	// ------------------------------------------------------------------
	PrimaryActorTick.bCanEverTick = true;

	// ------------------------------------------------------------------
	// Capsule Component (Collision)
	// ------------------------------------------------------------------
	// The capsule is the character's collision shape -- an invisible pill
	// shape that the physics engine uses for collisions. ACharacter creates
	// this automatically; we just set its size.
	//
	// HalfHeight = 96: total height ~192 units (~1.92 meters, average human)
	// Radius = 42: width of the capsule (~0.84 meters, wide enough for armor)
	// ------------------------------------------------------------------
	GetCapsuleComponent()->InitCapsuleSize(42.0f, 96.0f);

	// ------------------------------------------------------------------
	// Character Movement Configuration
	// ------------------------------------------------------------------
	// UCharacterMovementComponent is built into ACharacter. It handles
	// walking, falling, swimming, flying, etc. We configure it here.
	// ------------------------------------------------------------------
	UCharacterMovementComponent* MoveComp = GetCharacterMovement();

	// Orient the character to face the direction of movement
	// (not the camera direction). This feels natural for third-person games.
	MoveComp->bOrientRotationToMovement = true;

	// How quickly the character rotates to face the movement direction.
	// Higher values = snappier turning. (Yaw = left/right rotation)
	MoveComp->RotationRate = FRotator(0.0f, 540.0f, 0.0f);

	// Jump height -- how high the character can jump (in units).
	// 600 gives a satisfying jump without feeling floaty.
	MoveComp->JumpZVelocity = 600.0f;

	// Air control -- how much the player can steer while in the air.
	// 0.35 = moderate air control (0 = no control, 1 = full control).
	MoveComp->AirControl = 0.35f;

	// Default walk speed (will be changed dynamically based on state)
	MoveComp->MaxWalkSpeed = 600.0f;

	// Crouching is disabled for the vertical slice (not in scope).
	MoveComp->GetNavAgentPropertiesRef().bCanCrouch = false;

	// Don't rotate the character with the controller/camera.
	// The character faces movement direction, not camera direction.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// ------------------------------------------------------------------
	// Camera Boom (Spring Arm)
	// ------------------------------------------------------------------
	// CreateDefaultSubobject creates a component and attaches it to this actor.
	// The TEXT("name") is an internal name used by the engine -- it must be
	// unique within this actor. It's NOT the display name.
	// ------------------------------------------------------------------
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent); // Attach to the capsule
	CameraBoom->TargetArmLength = 300.0f;       // Camera distance behind player
	CameraBoom->bUsePawnControlRotation = true;  // Rotate boom with camera input
	CameraBoom->bEnableCameraLag = true;         // Smooth camera follow
	CameraBoom->CameraLagSpeed = 10.0f;          // How fast camera catches up (higher = less lag)

	// ------------------------------------------------------------------
	// Follow Camera
	// ------------------------------------------------------------------
	// Attached to the END of the spring arm. This is what the player sees through.
	// ------------------------------------------------------------------
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketEndName);
	FollowCamera->bUsePawnControlRotation = false; // Camera doesn't rotate independently; the boom handles rotation

	// ------------------------------------------------------------------
	// Armor Mesh Slots (Skeletal Meshes)
	// ------------------------------------------------------------------
	// Each armor piece is a separate skeletal mesh that follows the main
	// character skeleton. "SetLeaderPoseComponent" makes them copy the
	// bone transforms from the main mesh, so animations play on all pieces.
	//
	// We attach them to GetMesh() which is the character's primary
	// skeletal mesh (the body/skin).
	// ------------------------------------------------------------------

	HelmetMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HelmetMesh"));
	HelmetMesh->SetupAttachment(GetMesh());
	// LeaderPoseComponent is set in BeginPlay because GetMesh() may not
	// be fully initialized during construction for all use cases.

	BodyMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BodyMesh"));
	BodyMesh->SetupAttachment(GetMesh());

	GlovesMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GlovesMesh"));
	GlovesMesh->SetupAttachment(GetMesh());

	LegsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("LegsMesh"));
	LegsMesh->SetupAttachment(GetMesh());

	BootsMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BootsMesh"));
	BootsMesh->SetupAttachment(GetMesh());

	// ------------------------------------------------------------------
	// Weapon Mesh Slots (Static Meshes)
	// ------------------------------------------------------------------
	// Weapons are attached to named sockets on the skeleton. The socket
	// names (like "hand_r_socket") are defined in the UE5 skeleton editor.
	// The EquipmentComponent will call AttachToComponent with the
	// appropriate socket name when weapons are equipped.
	//
	// For now, we just attach them to the main mesh. The actual socket
	// attachment happens at runtime when a weapon is equipped.
	// ------------------------------------------------------------------

	MainHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MainHandMesh"));
	MainHandMesh->SetupAttachment(GetMesh(), FName("hand_r_socket"));
	MainHandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision); // Weapons don't collide; we use traces for hit detection

	OffHandMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("OffHandMesh"));
	OffHandMesh->SetupAttachment(GetMesh(), FName("hand_l_socket"));
	OffHandMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	BackWeaponMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BackWeaponMesh"));
	BackWeaponMesh->SetupAttachment(GetMesh(), FName("spine_03_socket"));
	BackWeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	// ------------------------------------------------------------------
	// Gameplay Components
	// ------------------------------------------------------------------
	// These are commented out until the component classes are created by
	// other agents. Once they exist, uncomment both the #include at the
	// top of this file AND the CreateDefaultSubobject calls below.
	//
	// CreateDefaultSubobject<T>(name) creates a component owned by this actor.
	// The component will be automatically ticked, replicated, and destroyed
	// with the actor.
	// ------------------------------------------------------------------

	// InventoryComponent = CreateDefaultSubobject<UInventoryComponent>(TEXT("InventoryComponent"));
	// EquipmentComponent = CreateDefaultSubobject<UEquipmentComponent>(TEXT("EquipmentComponent"));
	// StatsComponent = CreateDefaultSubobject<UStatsComponent>(TEXT("StatsComponent"));
	// HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));
	// StaminaComponent = CreateDefaultSubobject<UStaminaComponent>(TEXT("StaminaComponent"));
	// CombatComponent = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponent"));

	// Initialize the pointers to nullptr until components are created
	InventoryComponent = nullptr;
	EquipmentComponent = nullptr;
	StatsComponent = nullptr;
	HealthComponent = nullptr;
	StaminaComponent = nullptr;
	CombatComponent = nullptr;

	// ------------------------------------------------------------------
	// Default Stat Values
	// ------------------------------------------------------------------
	// These match the starting values from the design docs.
	// All base stats start at 5. The player levels up and allocates points.
	// ------------------------------------------------------------------
	CharacterLevel = 1;
	CurrentXP = 0;
	XPToNextLevel = 100; // First level-up requires 100 XP

	BaseMight = 5;
	BaseFortitude = 5;
	BaseHaste = 5;
	BaseAgility = 5;
	BaseSorcery = 5;

	// ------------------------------------------------------------------
	// Movement Speed Defaults
	// ------------------------------------------------------------------
	// From the design doc:
	//   Walk = 300, Run = 600, Sprint = 900
	// These are in Unreal units per second (~cm/s).
	// ------------------------------------------------------------------
	WalkSpeed = 300.0f;
	RunSpeed = 600.0f;
	SprintSpeed = 900.0f;
	bIsSprinting = false;

	// ------------------------------------------------------------------
	// Interaction System Defaults
	// ------------------------------------------------------------------
	InteractionTraceDistance = 300.0f; // 300 units forward (~3 meters)
}


// ============================================================================
// BEGIN PLAY
// ============================================================================
// Called when the game starts (or when the actor is spawned during gameplay).
// This is the first function where you can safely access the game world.
//
// Use BeginPlay for:
//   - Setting up input mapping contexts
//   - Initializing references to other actors
//   - Setting leader pose on armor meshes
//   - Anything that requires the world to exist
// ============================================================================
void ARevenCharacter::BeginPlay()
{
	// Always call the parent's BeginPlay first -- it handles engine internals.
	Super::BeginPlay();

	// ------------------------------------------------------------------
	// Add the Input Mapping Context to the Player Controller
	// ------------------------------------------------------------------
	// The mapping context tells the Enhanced Input system which keys
	// correspond to which Input Actions. Without this, no input works.
	//
	// We add it with priority 0 (default). Higher priority contexts
	// override lower ones. Later, when we open a UI menu, we can add
	// a UI context with higher priority to disable combat inputs.
	// ------------------------------------------------------------------
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem =
			ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			if (DefaultMappingContext)
			{
				Subsystem->AddMappingContext(DefaultMappingContext, 0);
			}
		}
	}

	// ------------------------------------------------------------------
	// Set up Leader Pose for Armor Meshes
	// ------------------------------------------------------------------
	// LeaderPoseComponent tells each armor mesh to copy bone transforms
	// from the main character mesh. This means when the character plays
	// an animation, all armor pieces animate with it automatically.
	// ------------------------------------------------------------------
	if (GetMesh())
	{
		HelmetMesh->SetLeaderPoseComponent(GetMesh());
		BodyMesh->SetLeaderPoseComponent(GetMesh());
		GlovesMesh->SetLeaderPoseComponent(GetMesh());
		LegsMesh->SetLeaderPoseComponent(GetMesh());
		BootsMesh->SetLeaderPoseComponent(GetMesh());
	}

	// ------------------------------------------------------------------
	// Set initial movement speed to Run (default state)
	// ------------------------------------------------------------------
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::BeginPlay -- Character initialized. Level: %d"), CharacterLevel);
}


// ============================================================================
// TICK
// ============================================================================
// Called every frame. We keep this LIGHTWEIGHT because it runs 60+ times
// per second. Heavy logic belongs in components.
//
// Currently, Tick only needs to exist for future use. Components handle
// their own per-frame logic (stamina regen, buff timers, etc.).
// ============================================================================
void ARevenCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// ------------------------------------------------------------------
	// Future: Interaction prompt check
	// ------------------------------------------------------------------
	// We could run a lightweight trace here to check if the player is
	// LOOKING at an interactable object (to show the "Press E to ..." prompt).
	// For now, this is left for the UI system to implement.
	// ------------------------------------------------------------------
}


// ============================================================================
// INPUT BINDING
// ============================================================================
// This function is called automatically by UE5 to set up input bindings.
// We use the Enhanced Input system (UE5's replacement for the old input system).
//
// Each binding connects:
//   Input Action (e.g., IA_Move)  -->  Event Type (Started/Triggered/Completed)  -->  Function (OnMove)
//
// Event types:
//   ETriggerEvent::Triggered  = fires every frame the input is active (for movement, look)
//   ETriggerEvent::Started    = fires once when the key is first pressed
//   ETriggerEvent::Completed  = fires once when the key is released
// ============================================================================
void ARevenCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Cast to Enhanced Input Component -- required for binding Input Actions.
	// The regular UInputComponent doesn't support Enhanced Input.
	UEnhancedInputComponent* EnhancedInput = CastChecked<UEnhancedInputComponent>(PlayerInputComponent);

	// ------------------------------------------------------------------
	// Movement Bindings
	// ------------------------------------------------------------------

	// Move -- fires every frame while WASD is held (Triggered = continuous)
	if (IA_Move)
	{
		EnhancedInput->BindAction(IA_Move, ETriggerEvent::Triggered, this, &ARevenCharacter::OnMove);
	}

	// Look -- fires every frame while the mouse moves
	if (IA_Look)
	{
		EnhancedInput->BindAction(IA_Look, ETriggerEvent::Triggered, this, &ARevenCharacter::OnLook);
	}

	// Jump -- fires once when spacebar is pressed
	if (IA_Jump)
	{
		EnhancedInput->BindAction(IA_Jump, ETriggerEvent::Started, this, &ARevenCharacter::OnJump);
	}

	// Sprint -- Started = begin sprinting, Completed = stop sprinting
	if (IA_Sprint)
	{
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Started, this, &ARevenCharacter::OnSprintStart);
		EnhancedInput->BindAction(IA_Sprint, ETriggerEvent::Completed, this, &ARevenCharacter::OnSprintStop);
	}

	// ------------------------------------------------------------------
	// Combat Bindings
	// ------------------------------------------------------------------

	if (IA_LightAttack)
	{
		EnhancedInput->BindAction(IA_LightAttack, ETriggerEvent::Started, this, &ARevenCharacter::OnLightAttack);
	}

	if (IA_HeavyAttack)
	{
		// Heavy attack triggers on release (after the hold threshold in the Input Action asset)
		EnhancedInput->BindAction(IA_HeavyAttack, ETriggerEvent::Triggered, this, &ARevenCharacter::OnHeavyAttack);
	}

	if (IA_Block)
	{
		EnhancedInput->BindAction(IA_Block, ETriggerEvent::Started, this, &ARevenCharacter::OnBlockStart);
		EnhancedInput->BindAction(IA_Block, ETriggerEvent::Completed, this, &ARevenCharacter::OnBlockStop);
	}

	if (IA_Dodge)
	{
		EnhancedInput->BindAction(IA_Dodge, ETriggerEvent::Started, this, &ARevenCharacter::OnDodge);
	}

	// ------------------------------------------------------------------
	// Interaction Binding
	// ------------------------------------------------------------------

	if (IA_Interact)
	{
		EnhancedInput->BindAction(IA_Interact, ETriggerEvent::Started, this, &ARevenCharacter::OnInteract);
	}

	// ------------------------------------------------------------------
	// Spell Slot Bindings (keys 1-5)
	// ------------------------------------------------------------------

	if (IA_Spell1)
	{
		EnhancedInput->BindAction(IA_Spell1, ETriggerEvent::Started, this, &ARevenCharacter::OnSpell1);
	}
	if (IA_Spell2)
	{
		EnhancedInput->BindAction(IA_Spell2, ETriggerEvent::Started, this, &ARevenCharacter::OnSpell2);
	}
	if (IA_Spell3)
	{
		EnhancedInput->BindAction(IA_Spell3, ETriggerEvent::Started, this, &ARevenCharacter::OnSpell3);
	}
	if (IA_Spell4)
	{
		EnhancedInput->BindAction(IA_Spell4, ETriggerEvent::Started, this, &ARevenCharacter::OnSpell4);
	}
	if (IA_Spell5)
	{
		EnhancedInput->BindAction(IA_Spell5, ETriggerEvent::Started, this, &ARevenCharacter::OnSpell5);
	}

	// ------------------------------------------------------------------
	// Item Slot Bindings (keys 6-0)
	// ------------------------------------------------------------------

	if (IA_Item1)
	{
		EnhancedInput->BindAction(IA_Item1, ETriggerEvent::Started, this, &ARevenCharacter::OnItem1);
	}
	if (IA_Item2)
	{
		EnhancedInput->BindAction(IA_Item2, ETriggerEvent::Started, this, &ARevenCharacter::OnItem2);
	}
	if (IA_Item3)
	{
		EnhancedInput->BindAction(IA_Item3, ETriggerEvent::Started, this, &ARevenCharacter::OnItem3);
	}
	if (IA_Item4)
	{
		EnhancedInput->BindAction(IA_Item4, ETriggerEvent::Started, this, &ARevenCharacter::OnItem4);
	}
	if (IA_Item5)
	{
		EnhancedInput->BindAction(IA_Item5, ETriggerEvent::Started, this, &ARevenCharacter::OnItem5);
	}
}


// ============================================================================
// INPUT HANDLER IMPLEMENTATIONS
// ============================================================================
// Each function below handles ONE player input. The pattern is:
//   1. Extract the value from FInputActionValue
//   2. Route to the appropriate component (or handle directly if simple)
//   3. Log for debugging (remove in shipping builds)
// ============================================================================

// ---------------------------------------------------------------------------
// MOVEMENT
// ---------------------------------------------------------------------------

void ARevenCharacter::OnMove(const FInputActionValue& Value)
{
	// Get the 2D movement vector from the input.
	// X = right/left (-1 to 1), Y = forward/backward (-1 to 1).
	const FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// Get the direction the controller (camera) is facing.
		// We want to move RELATIVE to the camera, not the character.
		// So pressing W always moves "forward on screen" regardless of
		// which way the character is facing.
		const FRotator Rotation = Controller->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0); // Only use yaw (horizontal rotation)

		// Calculate forward and right direction vectors from the camera's yaw.
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// Apply movement. AddMovementInput feeds into the CharacterMovementComponent
		// which handles acceleration, deceleration, and collision automatically.
		AddMovementInput(ForwardDirection, MovementVector.Y); // W/S
		AddMovementInput(RightDirection, MovementVector.X);   // A/D
	}
}

void ARevenCharacter::OnLook(const FInputActionValue& Value)
{
	// Get the 2D look vector (mouse delta or stick input).
	const FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// AddControllerYawInput/PitchInput rotate the camera through the
		// controller. The spring arm's bUsePawnControlRotation makes the
		// camera boom follow the controller rotation.
		AddControllerYawInput(LookAxisVector.X);   // Horizontal look (left/right)
		AddControllerPitchInput(LookAxisVector.Y); // Vertical look (up/down)
	}
}

void ARevenCharacter::OnJump(const FInputActionValue& Value)
{
	// ACharacter has built-in Jump() and StopJumping() functions.
	// Jump() checks CanJump() internally (grounded, not crouching, etc.).
	Jump();
}

void ARevenCharacter::OnSprintStart(const FInputActionValue& Value)
{
	// Start sprinting: increase speed, set flag for animation blueprint.
	// The StaminaComponent will handle the 5/sec stamina drain.
	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed = SprintSpeed;

	// TODO: Tell StaminaComponent to begin sprint drain
	// if (StaminaComponent) { StaminaComponent->StartSprintDrain(); }
}

void ARevenCharacter::OnSprintStop(const FInputActionValue& Value)
{
	// Stop sprinting: return to run speed.
	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed = RunSpeed;

	// TODO: Tell StaminaComponent to stop sprint drain
	// if (StaminaComponent) { StaminaComponent->StopSprintDrain(); }
}

// ---------------------------------------------------------------------------
// COMBAT
// ---------------------------------------------------------------------------
// All combat handlers delegate to the CombatComponent. The character class
// does NOT contain combat logic -- it just passes the message along.
// This keeps combat code isolated and easy to modify.
// ---------------------------------------------------------------------------

void ARevenCharacter::OnLightAttack(const FInputActionValue& Value)
{
	// Route to CombatComponent -- it handles combo chains, stamina costs,
	// animation montage playback, and hit detection.
	// if (CombatComponent) { CombatComponent->RequestLightAttack(); }

	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnLightAttack -- Light attack requested"));
}

void ARevenCharacter::OnHeavyAttack(const FInputActionValue& Value)
{
	// Heavy attack triggers after a hold threshold (0.4s+).
	// CombatComponent will check if we're in a combo and play the appropriate montage.
	// if (CombatComponent) { CombatComponent->RequestHeavyAttack(); }

	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnHeavyAttack -- Heavy attack requested"));
}

void ARevenCharacter::OnBlockStart(const FInputActionValue& Value)
{
	// Start blocking. The CombatComponent starts a 200ms parry window
	// on block start (per combat design doc).
	// if (CombatComponent) { CombatComponent->StartBlock(); }

	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnBlockStart -- Block started (parry window open)"));
}

void ARevenCharacter::OnBlockStop(const FInputActionValue& Value)
{
	// Stop blocking. Return to normal state.
	// if (CombatComponent) { CombatComponent->StopBlock(); }

	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnBlockStop -- Block stopped"));
}

void ARevenCharacter::OnDodge(const FInputActionValue& Value)
{
	// Dodge roll. Costs 20 stamina. Has i-frames (duration based on Agility).
	// if (CombatComponent) { CombatComponent->RequestDodge(); }

	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnDodge -- Dodge requested"));
}

// ---------------------------------------------------------------------------
// INTERACTION
// ---------------------------------------------------------------------------

void ARevenCharacter::OnInteract(const FInputActionValue& Value)
{
	// Perform a line trace forward from the camera to find interactable objects.
	AActor* HitActor = PerformInteractionTrace();

	if (HitActor)
	{
		// Check if the hit actor implements the IInteractable interface.
		// UE5 provides Implements<>() to check at runtime.
		if (HitActor->Implements<UInteractableInterface>())
		{
			// Call the interface function using UE5's Execute_ pattern.
			// This works for both C++ and Blueprint implementations of the interface.
			IInteractable::Execute_Interact(HitActor, this);

			UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnInteract -- Interacted with: %s"), *HitActor->GetName());
		}
	}
}

AActor* ARevenCharacter::PerformInteractionTrace()
{
	// ------------------------------------------------------------------
	// Line Trace: shoots an invisible ray forward from the camera and
	// checks what it hits. This is how we detect interactable objects.
	//
	// We trace from the CAMERA (not the character) because the player
	// is looking at the screen center. The camera's forward direction
	// matches what the player sees.
	// ------------------------------------------------------------------

	if (!FollowCamera)
	{
		return nullptr;
	}

	// Start point: camera location
	const FVector TraceStart = FollowCamera->GetComponentLocation();

	// End point: camera location + camera forward direction * trace distance
	const FVector TraceEnd = TraceStart + (FollowCamera->GetForwardVector() * InteractionTraceDistance);

	// Set up trace parameters
	FHitResult HitResult;
	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this); // Don't hit ourselves

	// Perform the line trace. ECC_Visibility is a standard collision channel
	// that most interactable objects respond to.
	bool bHit = GetWorld()->LineTraceSingleByChannel(
		HitResult,
		TraceStart,
		TraceEnd,
		ECC_Visibility,
		QueryParams
	);

	// Debug: draw the trace line so we can see it during development.
	// Green = hit something, Red = hit nothing.
	// Remove or wrap in #if WITH_EDITOR before shipping.
#if WITH_EDITOR
	DrawDebugLine(
		GetWorld(),
		TraceStart,
		TraceEnd,
		bHit ? FColor::Green : FColor::Red,
		false,    // persistent (false = disappears after duration)
		1.0f,     // duration in seconds
		0,        // depth priority
		1.0f      // thickness
	);
#endif

	if (bHit)
	{
		return HitResult.GetActor();
	}

	return nullptr;
}

// ---------------------------------------------------------------------------
// SPELL SLOTS (keys 1-5)
// ---------------------------------------------------------------------------
// Each spell slot routes to the CombatComponent (or a future SpellComponent)
// which handles mana costs, cooldowns, and spell effects.
// ---------------------------------------------------------------------------

void ARevenCharacter::OnSpell1(const FInputActionValue& Value)
{
	// if (CombatComponent) { CombatComponent->CastSpell(0); }
	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnSpell1 -- Spell slot 1 activated"));
}

void ARevenCharacter::OnSpell2(const FInputActionValue& Value)
{
	// if (CombatComponent) { CombatComponent->CastSpell(1); }
	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnSpell2 -- Spell slot 2 activated"));
}

void ARevenCharacter::OnSpell3(const FInputActionValue& Value)
{
	// if (CombatComponent) { CombatComponent->CastSpell(2); }
	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnSpell3 -- Spell slot 3 activated"));
}

void ARevenCharacter::OnSpell4(const FInputActionValue& Value)
{
	// if (CombatComponent) { CombatComponent->CastSpell(3); }
	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnSpell4 -- Spell slot 4 activated"));
}

void ARevenCharacter::OnSpell5(const FInputActionValue& Value)
{
	// if (CombatComponent) { CombatComponent->CastSpell(4); }
	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnSpell5 -- Spell slot 5 activated"));
}

// ---------------------------------------------------------------------------
// ITEM SLOTS (keys 6-0)
// ---------------------------------------------------------------------------
// Each item slot routes to the InventoryComponent which handles item usage,
// cooldowns, and effects (potions, food, consumables).
// ---------------------------------------------------------------------------

void ARevenCharacter::OnItem1(const FInputActionValue& Value)
{
	// if (InventoryComponent) { InventoryComponent->UseItemSlot(0); }
	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnItem1 -- Item slot 1 activated"));
}

void ARevenCharacter::OnItem2(const FInputActionValue& Value)
{
	// if (InventoryComponent) { InventoryComponent->UseItemSlot(1); }
	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnItem2 -- Item slot 2 activated"));
}

void ARevenCharacter::OnItem3(const FInputActionValue& Value)
{
	// if (InventoryComponent) { InventoryComponent->UseItemSlot(2); }
	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnItem3 -- Item slot 3 activated"));
}

void ARevenCharacter::OnItem4(const FInputActionValue& Value)
{
	// if (InventoryComponent) { InventoryComponent->UseItemSlot(3); }
	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnItem4 -- Item slot 4 activated"));
}

void ARevenCharacter::OnItem5(const FInputActionValue& Value)
{
	// if (InventoryComponent) { InventoryComponent->UseItemSlot(4); }
	UE_LOG(LogTemp, Log, TEXT("ARevenCharacter::OnItem5 -- Item slot 5 activated"));
}
