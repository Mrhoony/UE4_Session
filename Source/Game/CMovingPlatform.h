#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "CMovingPlatform.generated.h"

UCLASS()
class GAME_API ACMovingPlatform : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ACMovingPlatform();

public:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

public:
	void AddActiveTrigger();
	void RemoveActiveTrigger();

private:
	UPROPERTY(EditAnywhere)
		int32 ActiveTrigger = 1;

	UPROPERTY(EditAnywhere)
		float Speed = 20.f;
	
	UPROPERTY(EditAnywhere, meta = (MakeEditWidget))
		FVector TargetLocation;

private:
	FVector GlobalStartLocation;
	FVector GlobalTargetLocation;
	//bool Reverse;
};
