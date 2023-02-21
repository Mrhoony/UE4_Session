#include "CLobbyGameMode.h"
#include "CGameInstance.h"

void ACLobbyGameMode::PostLogin(APlayerController* newPlayer)
{
	Super::PostLogin(newPlayer);

	++NumberOfPlayers;

	if (NumberOfPlayers >= 3)
	{
		UE_LOG(LogTemp, Error, TEXT("Reached 3 Players!"));

		GetWorldTimerManager().SetTimer(GameStartTimer, this, &ACLobbyGameMode::StartGame, 10.f, false);
	}
}

void ACLobbyGameMode::Logout(AController* Exiting)
{
	Super::Logout(Exiting);

	--NumberOfPlayers;
}

void ACLobbyGameMode::StartGame()
{
	UCGameInstance* gameInstance = Cast<UCGameInstance>(GetGameInstance());
	if (gameInstance == nullptr) return;

	gameInstance->StartSession();

	UWorld* world = GetWorld();
	if (world == nullptr)return;

	bUseSeamlessTravel = true;
	world->ServerTravel("/Game/Maps/Play?listen");
}
