#include "FP_FirstPersonCharacter.h"
#include "Global.h"

#include "CBullet.h"
#include "CPlayerState.h"
#include "FP_FirstPersonGameMode.h"

#include "Animation/AnimInstance.h"
#include "Components/CapsuleComponent.h"
#include "Components/InputComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerController.h"
#include "Net/UnrealNetwork.h"
#include "Particles/ParticleSystemComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

#define COLLISION_WEAPON		ECC_GameTraceChannel1

DEFINE_LOG_CATEGORY_STATIC(LogFPChar, Warning, All);

AFP_FirstPersonCharacter::AFP_FirstPersonCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);

	// Set our turn rates for input
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	// Create a CameraComponent	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	Camera->SetupAttachment(GetCapsuleComponent());
	Camera->SetRelativeLocation(FVector(0, 0, 64.f)); // Position the camera
	Camera->bUsePawnControlRotation = true;
	
	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FP_Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	FP_Mesh->SetOnlyOwnerSee(true);				// Set so only owner can see mesh
	FP_Mesh->SetupAttachment(Camera);	// Attach mesh to Camera
	FP_Mesh->bCastDynamicShadow = false;			// Disallow mesh to cast dynamic shadows
	FP_Mesh->CastShadow = false;				// Disallow mesh to cast other shadows

	// Create a gun mesh component
	FP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	FP_Gun->SetOnlyOwnerSee(true);			// Only the owning player will see this mesh
	FP_Gun->bCastDynamicShadow = false;		// Disallow mesh to cast dynamic shadows
	FP_Gun->CastShadow = false;			// Disallow mesh to cast other shadows
	FP_Gun->SetupAttachment(FP_Mesh, TEXT("GripPoint"));

	// Set weapon damage and range
	WeaponRange = 5000.0f;
	WeaponDamage = 7.0f;

	// Default offset from the character location for projectiles to spawn
	GunOffset = FVector(100.0f, 30.0f, 10.0f);

	// Note: The ProjectileClass and the skeletal mesh/anim blueprints for FP_Mesh are set in the
	// derived blueprint asset named MyCharacter (to avoid direct content references in C++)

	GetMesh()->SetOwnerNoSee(true);
	TP_Gun = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("TP_Gun"));
	TP_Gun->SetupAttachment(GetMesh(), TEXT("GripPoint"));
	TP_Gun->SetOwnerNoSee(true);

	FP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("FP_GunShotParticle"));
	FP_GunShotParticle->bAutoActivate = false;
	FP_GunShotParticle->SetupAttachment(FP_Gun, "Muzzle");
	FP_GunShotParticle->SetOnlyOwnerSee(true);

	TP_GunShotParticle = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TP_GunShotParticle"));
	TP_GunShotParticle->bAutoActivate = false;
	TP_GunShotParticle->SetupAttachment(TP_Gun, "Muzzle");
	TP_GunShotParticle->SetOwnerNoSee(true);
}

void AFP_FirstPersonCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(AFP_FirstPersonCharacter, CurrentTeam);
}

void AFP_FirstPersonCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	SelfPlayerState = Cast<ACPlayerState>(GetPlayerState());

	if(GetLocalRole() == ENetRole::ROLE_Authority && SelfPlayerState != nullptr)
	SelfPlayerState->Health = 100.f;
}

void AFP_FirstPersonCharacter::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority() == false) SetTeamColor(CurrentTeam);
}

void AFP_FirstPersonCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);
	
	// Set up gameplay key bindings

	// Bind jump events
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Jump", IE_Released, this, &ACharacter::StopJumping);
	
	// Bind fire event
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFP_FirstPersonCharacter::OnFire);
	
	// Bind movement events
	PlayerInputComponent->BindAxis("MoveForward", this, &AFP_FirstPersonCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFP_FirstPersonCharacter::MoveRight);
	
	// We have 2 versions of the rotation bindings to handle different kinds of devices differently
	// "turn" handles devices that provide an absolute delta, such as a mouse.
	// "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AFP_FirstPersonCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookUpRate", this, &AFP_FirstPersonCharacter::LookUpAtRate);
}

void AFP_FirstPersonCharacter::Respawn()
{
	CheckFalse(HasAuthority());

	SelfPlayerState->Health = 100.f;
	Cast<AFP_FirstPersonGameMode>(GetWorld()->GetAuthGameMode())->Respawn(this);

	Destroy(true);
}

void AFP_FirstPersonCharacter::OnFire()
{
	CheckTrue(GetSelfPlayerState()->Health <= 0);

	// Try and play a firing animation if specified
	if (FireAnimation != NULL)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = FP_Mesh->GetAnimInstance();
		if (AnimInstance != NULL)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}

	if (FP_GunShotParticle != nullptr)
		FP_GunShotParticle->Activate(true);

	// Now send a trace from the end of our gun to see if we should hit anything
	APlayerController* PlayerController = Cast<APlayerController>(GetController());
	
	FVector ShootDir = FVector::ZeroVector;
	FVector StartTrace = FVector::ZeroVector;

	if (PlayerController)
	{
		// Calculate the direction of fire and the start location for trace
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(StartTrace, CamRot);
		ShootDir = CamRot.Vector();

		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		StartTrace = StartTrace + ShootDir * ((GetActorLocation() - StartTrace) | ShootDir);
	}

	// Calculate endpoint of trace
	const FVector EndTrace = StartTrace + ShootDir * WeaponRange;

	OnServerFire(StartTrace, EndTrace);
}

void AFP_FirstPersonCharacter::OnServerFire_Implementation(const FVector& LineStart, const FVector& LineEnd)
{
	const FHitResult Impact = WeaponTrace(LineStart, LineEnd);
	MulticastFireEffect();
}
void AFP_FirstPersonCharacter::MulticastFireEffect_Implementation()
{
	if (TP_FireAnimation != nullptr)
	{
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (animInstance != nullptr)
		{
			animInstance->Montage_Play(TP_FireAnimation, 1.f);
		}
	}

	if (TP_GunShotParticle != nullptr)
		TP_GunShotParticle->Activate(true);

	if (FireSound != nullptr) 
	{
		// 멀티플레이의 사운드 재생 검색필요
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	GetWorld()->SpawnActor<ACBullet>(ACBullet::StaticClass(), FP_Gun->GetSocketLocation("Muzzle"), GetControlRotation());
}
void AFP_FirstPersonCharacter::SetTeamColor_Implementation(ETeamTypes InTeamType)
{
	FLinearColor color;
	if (InTeamType == ETeamTypes::Blue)
		color = FLinearColor(0.0f, 0.0f, 0.5f);
	else
		color = FLinearColor(0.5f, 0.0f, 0.0f);

	if (DynamicMaterial == nullptr)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(GetMesh()->GetMaterial(0), nullptr);
		DynamicMaterial->SetVectorParameterValue("BodyColor", color);
		GetMesh()->SetMaterial(0, DynamicMaterial);
		FP_Mesh->SetMaterial(0, DynamicMaterial);
	}
}

ACPlayerState* AFP_FirstPersonCharacter::GetSelfPlayerState()
{
	if(SelfPlayerState == nullptr)
		SelfPlayerState = Cast<ACPlayerState>(GetPlayerState());

	return SelfPlayerState;
}

void AFP_FirstPersonCharacter::SetSelfPlayerState(ACPlayerState* NewPlayerState)
{
	CheckFalse(HasAuthority());

	SetPlayerState(NewPlayerState);
	SelfPlayerState = NewPlayerState;
}

void AFP_FirstPersonCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// Add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}
void AFP_FirstPersonCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// Add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}
void AFP_FirstPersonCharacter::TurnAtRate(float Rate)
{
	// Calculate delta for this frame from the rate information
	AddControllerYawInput(Rate * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}
void AFP_FirstPersonCharacter::LookUpAtRate(float Rate)
{
	// Calculate delta for this frame from the rate information
	AddControllerPitchInput(Rate * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}

FHitResult AFP_FirstPersonCharacter::WeaponTrace(const FVector& StartTrace, const FVector& EndTrace)
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, StartTrace, EndTrace, COLLISION_WEAPON, TraceParams);

	CheckFalseResult(Hit.IsValidBlockingHit(), Hit);
	AFP_FirstPersonCharacter* other = Cast<AFP_FirstPersonCharacter>(Hit.GetActor());
	if (other != nullptr
		&& other->GetSelfPlayerState()->Team != GetSelfPlayerState()->Team
		&& other->GetSelfPlayerState()->Health > 0)
	{
		FDamageEvent e;
		other->TakeDamage(WeaponDamage, e, GetController(), this);
	}

	return Hit;
}

float AFP_FirstPersonCharacter::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	
	CheckTrueResult(DamageCauser == this, DamageAmount);
	SelfPlayerState->Health -= DamageAmount;

	if (SelfPlayerState->Health <= 0)
	{
		PlayDead();

		SelfPlayerState->Death++;
		AFP_FirstPersonCharacter* other = Cast<AFP_FirstPersonCharacter>(DamageCauser);
		if (other != nullptr)
			other->SelfPlayerState->Score += 1.0f;

		FTimerHandle handle;
		GetWorldTimerManager().SetTimer(handle, this, &AFP_FirstPersonCharacter::Respawn, 3.f, false);

		return DamageAmount;
	}
	
	PlayDamage();

	return DamageAmount;
}

//void AActor::GetLifetimeReplicatedProps( TArray< FLifetimeProperty > & OutLifetimeProps ) const // 이 함수를 이용해야함
//{
//    DOREPLIFETIME( AActor, Owner ); // 등록을 해줘야한다 서버용 가비지컬렉터, 없을경우 복제하지 않아도 되므로
//}

void AFP_FirstPersonCharacter::PlayDamage_Implementation()
{
	if (TP_HitAnimation != nullptr)
	{
		UAnimInstance* animInstance = GetMesh()->GetAnimInstance();
		if (animInstance != nullptr)
		{
			animInstance->Montage_Play(TP_HitAnimation, 1.5f);
		}
	}
}

void AFP_FirstPersonCharacter::PlayDead_Implementation()
{
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionProfileName("Ragdoll");
	GetMesh()->SetPhysicsBlendWeight(1.f);
	GetMesh()->SetSimulatePhysics(true);
}