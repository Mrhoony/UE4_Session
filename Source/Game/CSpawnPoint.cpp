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

	OnActorBeginOverlap.AddDynamic(this, &ACSpawnPoint::ActorBeginOverlap);
	OnActorEndOverlap.AddDynamic(this, &ACSpawnPoint::ActorEndOverlap);
}

void ACSpawnPoint::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	Capsule->UpdateOverlaps();
}

void ACSpawnPoint::ActorBeginOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (HasAuthority() == false) return;

	if(OverlappedActors.Find(OtherActor) < 0)
		OverlappedActors.Add(OtherActor);
}

void ACSpawnPoint::ActorEndOverlap(AActor* OverlappedActor, AActor* OtherActor)
{
	if (HasAuthority() == false) return;

	if (OverlappedActors.Find(OtherActor) >= 0)
		OverlappedActors.Remove(OtherActor);
}
