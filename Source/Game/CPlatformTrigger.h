#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CPlatformTrigger.generated.h"

UCLASS()
class GAME_API ACPlatformTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	ACPlatformTrigger();

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
		void OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
		void OnComponentEndOverlap(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	UPROPERTY(VisibleDefaultsOnly)
		class UBoxComponent* Box;

	UPROPERTY(EditAnywhere)
		TArray<class ACMovingPlatform*> PlatformsToTrigger;
};
