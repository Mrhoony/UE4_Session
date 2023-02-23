#include "FP_FirstPersonGameMode.h"
#include "Global.h"

#include "CGameState.h"
#include "CPlayerState.h"
#include "FP_FirstPersonHUD.h"
#include "FP_FirstPersonCharacter.h"
#include "CSpawnPoint.h"

#include "UObject/ConstructorHelpers.h"
#include "EngineUtils.h"

AFP_FirstPersonGameMode::AFP_FirstPersonGameMode()
{
	PrimaryActorTick.bCanEverTick = true;

	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));

	DefaultPawnClass = PlayerPawnClassFinder.Class;
	HUDClass = AFP_FirstPersonHUD::StaticClass();
	GameStateClass = ACGameState::StaticClass();
	PlayerStateClass = ACPlayerState::StaticClass();
}

void AFP_FirstPersonGameMode::BeginPlay()
{
	Super::BeginPlay();

	for (TActorIterator<ACSpawnPoint> iter(GetWorld()); iter; ++iter)
	{
		if (iter->GetTeam() == ETeamTypes::Red)
			RedTeamSpawners.Add(*iter);
		else
			BlueTeamSpawners.Add(*iter);
	}

	//APlayerController* controller = GetWorld()->GetFirstPlayerController(); // 호스트, 게스트는 PostLogin에서 호출
	//if (controller != nullptr)
	//{
	//	AFP_FirstPersonCharacter* player = Cast<AFP_FirstPersonCharacter>(controller->GetPawn());
	//	if (player != nullptr)
	//	{
	//		player->SetTeamColor(ETeamTypes::Red);
	//		RedTeamCharacters.Add(player);

	//		Spawn(player);
	//	}
	//}
}

void AFP_FirstPersonGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	AFP_FirstPersonCharacter* player = Cast<AFP_FirstPersonCharacter>(NewPlayer->GetPawn());
	ACPlayerState* playerState = Cast<ACPlayerState>(NewPlayer->PlayerState);

	if (player != nullptr && playerState != nullptr)
	{
		player->SetPlayerState(playerState);

		if (BlueTeamCharacters.Num() >= RedTeamCharacters.Num())
		{
			RedTeamCharacters.Add(player);
			playerState->Team = ETeamTypes::Red;
		}
		else
		{
			BlueTeamCharacters.Add(player);
			playerState->Team = ETeamTypes::Blue;
		}

		player->CurrentTeam = playerState->Team;
		player->SetTeamColor(playerState->Team);

		Spawn(player);
	}
}

void AFP_FirstPersonGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (ToBeSpawns.Num() > 0)
	{
		for (AFP_FirstPersonCharacter* player : ToBeSpawns)
			Spawn(player);
	}
}

void AFP_FirstPersonGameMode::Spawn(class AFP_FirstPersonCharacter* InPlayer)
{
	TArray<ACSpawnPoint*>* targetTeam = nullptr;

	if (InPlayer->CurrentTeam == ETeamTypes::Red)
		targetTeam = &RedTeamSpawners;
	else
		targetTeam = &BlueTeamSpawners;

	for (ACSpawnPoint* point : *targetTeam)
	{
		if (point->IsBlocked() == false)
		{
			InPlayer->SetActorLocation(point->GetActorLocation());
			InPlayer->SetActorRotation(point->GetActorRotation());
			point->UpdateOverlaps();

			if (ToBeSpawns.Find(InPlayer) >= 0)
				ToBeSpawns.Remove(InPlayer);
	
			return;
		}
	}

	if(ToBeSpawns.Find(InPlayer) < 0)
		ToBeSpawns.Add(InPlayer);
}

void AFP_FirstPersonGameMode::Respawn(AFP_FirstPersonCharacter* InPlayer)
{
	AController* controller = InPlayer->GetController();
	InPlayer->DetachFromControllerPendingDestroy();

	//InPlayer->Destroy(true);

	AFP_FirstPersonCharacter* player = Cast<AFP_FirstPersonCharacter>(GetWorld()->SpawnActor(DefaultPawnClass));
	if (player != nullptr)
	{
		controller->Possess(player);
		ACPlayerState* playerState = Cast<ACPlayerState>(player->GetController()->PlayerState);
		player->CurrentTeam = playerState->Team;
		player->SetSelfPlayerState(playerState);

		Spawn(player);
		player->SetTeamColor(playerState->Team);
	}
}
