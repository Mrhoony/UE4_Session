#include "CSpawnPoint.h"
#include "Global.h"

#include "Components/CapsuleComponent.h"

ACSpawnPoint::ACSpawnPoint()
{
	PrimaryActorTick.bCanEverTick = true;

	CHelpers::CreateSceneComponent<UCapsuleComponent>(this, &Capsule, "Capsule");
}

void ACSpawnPoint::OnConstruction(const FTransform& Transform)
{
	if (Team == ETeamTypes::Red)
		Capsule->ShapeColor = FColor::Red;
	else
		Capsule->ShapeColor = FColor::Blue;
}

void ACSpawnPoint::BeginPlay()
{
	Super::BeginPlay();

	OnActorBeginOverlap.AddDynamic(this, &ACSpawnPoint::OnActorBeginOverlap);
	OnActorBeginOverlap.AddDynamic(this, &ACSpawnPoint::OnActorEndOverlap);
}

void ACSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Capsule->UpdateOverlaps();
}

void ACSpawnPoint::OnActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (HasAuthority() == false) return;

	if(OverlappedActors.Find(OtherActor) < 0)
		OverlappedActors.Add(OtherActor);
}

void ACSpawnPoint::OnActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
}
