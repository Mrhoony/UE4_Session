#include "CPlatformTrigger.h"
#include "CMovingPlatform.h"

#include "Components/BoxComponent.h"

ACPlatformTrigger::ACPlatformTrigger()
{
	Box = CreateDefaultSubobject<UBoxComponent>(FName("Box"));
	if (Box == nullptr) return;

	RootComponent = Box;
}

// Called when the game starts or when spawned
void ACPlatformTrigger::BeginPlay()
{
	Super::BeginPlay();

	Box->OnComponentBeginOverlap.AddDynamic(this, &ACPlatformTrigger::OnComponentBeginOverlap);
	Box->OnComponentEndOverlap.AddDynamic(this, &ACPlatformTrigger::OnComponentEndOverlap);
}

void ACPlatformTrigger::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	for (ACMovingPlatform* platform : PlatformsToTrigger)
		platform->AddActiveTrigger();
}

void ACPlatformTrigger::OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	for (ACMovingPlatform* platform : PlatformsToTrigger)
		platform->RemoveActiveTrigger();
}
