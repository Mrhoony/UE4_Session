#include "FP_FirstPersonGameMode.h"

#include "CGameState.h"
#include "CPlayerState.h"
#include "FP_FirstPersonHUD.h"
#include "FP_FirstPersonCharacter.h"
#include "CSpawnPoint.h"

#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"

AFP_FirstPersonGameMode::AFP_FirstPersonGameMode()
{
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));

	DefaultPawnClass = PlayerPawnClassFinder.Class;
	HUDClass = AFP_FirstPersonHUD::StaticClass();
	GameStateClass = ACGameState::StaticClass();
	PlayerStateClass = ACPlayerState::StaticClass();
}

void AFP_FirstPersonGameMode::BeginPlay()
{
	for (TActorIterator<ACSpawnPoint> iter(GetWorld()); iter; ++iter)
	{
		//TODO:
	}
}

void AFP_FirstPersonGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AFP_FirstPersonCharacter* player = Cast<AFP_FirstPersonCharacter>(NewPlayer->GetPawn());
	ACPlayerState* playerState = Cast<ACPlayerState>(NewPlayer->PlayerState);

	if (player != nullptr && playerState != nullptr)
	{
		player->SetPlayerState(playerState);
		player->CurrentTeam = playerState->Team;
		player->SetTeamColor(playerState->Team);
	}
}
