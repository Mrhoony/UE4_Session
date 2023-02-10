#include "CMovingPlatform.h"
#include "Materials/MaterialInstanceConstant.h"

ACMovingPlatform::ACMovingPlatform()
{
	PrimaryActorTick.bCanEverTick = true;

	SetMobility(EComponentMobility::Movable);

	ConstructorHelpers::FObjectFinder<UStaticMesh> meshAsset(TEXT("StaticMesh'/Game/Geometry/Meshes/1M_Cube.1M_Cube'"));
	if(meshAsset.Succeeded())
		GetStaticMeshComponent()->SetStaticMesh(meshAsset.Object);

	ConstructorHelpers::FObjectFinder<UMaterialInstance> materialAsset(TEXT("MaterialInstanceConstant'/Game/Materials/M_Platform_Red.M_Platform_Red'"));
	if (materialAsset.Succeeded())
		GetStaticMeshComponent()->SetMaterial(0, materialAsset.Object);
}

void ACMovingPlatform::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		SetReplicates(true);
		SetReplicateMovement(true);
	}

	GlobalStartLocation = GetActorLocation();
	GlobalTargetLocation = GetTransform().TransformPosition(TargetLocation);
	//Reverse = false;
}

void ACMovingPlatform::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ActiveTrigger < 1) return;

	/*if (HasAuthority())
	{
		FVector location = GetActorLocation();
		FVector direction;

		if (Reverse)		{			direction = (GlobalStartLocation - location).GetSafeNormal();		}
		else		{			direction = (GlobalTargetLocation - location).GetSafeNormal();		}
		
		location += Speed * direction * DeltaTime;
		SetActorLocation(location);

		if (Reverse == false && location.Size() < GlobalTargetLocation.Size()) Reverse = true;
		if (Reverse == true && location.Size() > GlobalStartLocation.Size()) Reverse = false;
	}*/
	if (HasAuthority())
	{
		FVector location = GetActorLocation();		

		float totalDistance = (GlobalStartLocation - GlobalTargetLocation).Size();
		float currentDistance = (GlobalStartLocation - location).Size();

		if (currentDistance > totalDistance)
		{
			FVector temp = GlobalStartLocation;
			GlobalStartLocation = GlobalTargetLocation;
			GlobalTargetLocation = temp;
		}

		FVector direction = (GlobalTargetLocation - GlobalStartLocation).GetSafeNormal();
		location += Speed * direction * DeltaTime;
		SetActorLocation(location);
	}
}

void ACMovingPlatform::AddActiveTrigger()
{
	ActiveTrigger++;
}

void ACMovingPlatform::RemoveActiveTrigger()
{
	if(ActiveTrigger > 0)
		ActiveTrigger--;
}
