#pragma once

#include "CoreMinimal.h"
#include "GameGameMode.h"
#include "CLobbyGameMode.generated.h"

UCLASS()
class GAME_API ACLobbyGameMode : public AGameGameMode
{
	GENERATED_BODY()
	
public:
	virtual void PostLogin(APlayerController* newPlayer) override;
	virtual void Logout(AController* Exiting) override;

private:
	void StartGame();

private:
	uint32 NumberOfPlayers = 0;
	FTimerHandle GameStartTimer;
};
