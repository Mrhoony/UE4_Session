#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "CGameState.h"
#include "CPlayerState.generated.h"

UCLASS()
class GAME_API ACPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	ACPlayerState(const FObjectInitializer& ObjectInitializer);

public:
	UPROPERTY(Replicated)
		float Health;

	UPROPERTY(Replicated)
		int32 Death;

	UPROPERTY(Replicated)
		ETeamTypes Team;
};
