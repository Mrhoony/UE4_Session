#include "CGameInstance.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/CMainMenu.h"

#include "OnlineSessionSettings.h"

const static FName SESSION_NAME = TEXT("GameSession");

UCGameInstance::UCGameInstance(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> mainMenuClass(TEXT("/Game/Widgets/WB_MainMenu"));
	if (mainMenuClass.Succeeded())
		MainMenuClass = mainMenuClass.Class;

	ConstructorHelpers::FClassFinder<UUserWidget> inGameMenuClass(TEXT("/Game/Widgets/WB_InGameMenu"));
	if (inGameMenuClass.Succeeded())
		InGameMenuClass = inGameMenuClass.Class;
}

void UCGameInstance::Init()
{
	IOnlineSubsystem* oss = IOnlineSubsystem::Get();
	if (oss != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("OSS Pointer Found. Name : %s"), *oss->GetSubsystemName().ToString());
		SessionInterface = oss->GetSessionInterface();

		if (SessionInterface.IsValid())
		{
			SessionInterface->OnCreateSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnCreateSessionComplete);
			SessionInterface->OnDestroySessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnDestroySessionComplete);
			SessionInterface->OnFindSessionsCompleteDelegates.AddUObject(this, &UCGameInstance::OnFindSessionsComplete);
			SessionInterface->OnJoinSessionCompleteDelegates.AddUObject(this, &UCGameInstance::OnJoinSessionsComplete);
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("Not Found OSS"));
	}
}

void UCGameInstance::LoadMainMenu()
{
	if (MainMenuClass == nullptr) return;

	MainMenu = CreateWidget<UCMainMenu>(this, MainMenuClass);
	if (MainMenu == nullptr) return;
	MainMenu->Setup();
	MainMenu->SetMenuInterface(this);
}

void UCGameInstance::LoadInGameMenu()
{
	if (InGameMenuClass == nullptr) return;

	UCMenuWidget* inGameMenu = CreateWidget<UCMenuWidget>(this, InGameMenuClass);
	if (inGameMenu == nullptr) return;
	inGameMenu->SetMenuInterface(this);
	inGameMenu->Setup();
}

void UCGameInstance::Host()
{
	if (SessionInterface.IsValid())
	{
		auto existingSession = SessionInterface->GetNamedSession(SESSION_NAME);

		if (existingSession != nullptr)
		{
			SessionInterface->DestroySession(SESSION_NAME);
		}
		else
		{
			CreateSession();
		}
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("SessionInterface Is Not Found"));
	}
}

void UCGameInstance::CreateSession()
{
	if (SessionInterface.IsValid())
	{
		FOnlineSessionSettings sessionSettings;

		if (IOnlineSubsystem::Get()->GetSubsystemName() == "NULL")
		{
			sessionSettings.bIsLANMatch = true;
		}
		else
		{
			sessionSettings.bIsLANMatch = false;
		}
		sessionSettings.NumPublicConnections = 500;
		sessionSettings.bShouldAdvertise = true;
		sessionSettings.bUsesPresence = true;
		SessionInterface->CreateSession(0, SESSION_NAME, sessionSettings);
	}
}

void UCGameInstance::Join(const uint32& Index)
{
	if (SessionInterface.IsValid() == false) return;
	if (SessionSearch.IsValid() == false) return;

	if (MainMenu != nullptr)
		MainMenu->Teardown();

	SessionInterface->JoinSession(0, SESSION_NAME, SessionSearch->SearchResults[Index]);
}

void UCGameInstance::LoadMainMenuLevel()
{
	APlayerController* controller = GetFirstLocalPlayerController();
	if (controller == nullptr) return;
	controller->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}

void UCGameInstance::RefreshServerList()
{
	SessionSearch = MakeShareable(new FOnlineSessionSearch());
	if (SessionSearch.IsValid())
	{
		//SessionSearch->bIsLanQuery = true;
		SessionSearch->MaxSearchResults = 100; // 기본값 1, Equals 여부를 검사하게되면 검색이 안되기 때문에 큰 숫자 100을 넣음
		SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, true, EOnlineComparisonOp::Equals);
		UE_LOG(LogTemp, Error, TEXT("Start Find Sessions"));
		SessionInterface->FindSessions(0, SessionSearch.ToSharedRef());
	}
}

void UCGameInstance::OnCreateSessionComplete(FName InSessionName, bool InSuccess)
{
	if (InSuccess == false)
	{
		UE_LOG(LogTemp, Error, TEXT("COULD NOT CREATE SESSION!"));
		return;
	}

	UE_LOG(LogTemp, Error, TEXT("Session Name : %s"), *InSessionName.ToString());

	if (MainMenu != nullptr)
		MainMenu->Teardown();

	UEngine* engine = GetEngine();
	if (engine == nullptr) return;

	engine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, TEXT("Host"));

	UWorld* world = GetWorld();
	if (world == nullptr)return; // 트랜지션맵의 경우 월드가 리턴이 안됨

	world->ServerTravel("/Game/Maps/Play?listen");
}

void UCGameInstance::OnDestroySessionComplete(FName InSessionName, bool InSuccess)
{
	if (InSuccess == true)
		CreateSession();
}

void UCGameInstance::OnFindSessionsComplete(bool InSuccess)
{
	if (InSuccess == true && SessionSearch.IsValid() && MainMenu != nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("Finished Find Sessions"));

		TArray<FServerData> serverDatas;
		for (const FOnlineSessionSearchResult& searchResult : SessionSearch->SearchResults)
		{
			UE_LOG(LogTemp, Warning, TEXT("Found Session ID : %s"), *searchResult.GetSessionIdStr());
			UE_LOG(LogTemp, Warning, TEXT("Ping : %d"), searchResult.PingInMs);

			FServerData data;
			data.Name = searchResult.GetSessionIdStr();
			data.MaxPlayers = searchResult.Session.SessionSettings.NumPublicConnections;
			data.CurrentPlayers = data.MaxPlayers - searchResult.Session.NumOpenPublicConnections;
			data.HostUserName = searchResult.Session.OwningUserName;
			serverDatas.Add(data);
		}

		MainMenu->SetServerList(serverDatas);
	}
}

void UCGameInstance::OnJoinSessionsComplete(FName InSessionName, EOnJoinSessionCompleteResult::Type InResult)
{
	if (SessionInterface.IsValid() == false) return;

	FString address;
	if (SessionInterface->GetResolvedConnectString(InSessionName, address) == false)
	{
		UE_LOG(LogTemp, Warning, TEXT("Could Not Get IP Address"));
		return;
	}

	UEngine* engine = GetEngine();
	if (engine == nullptr) return;
	engine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, FString::Printf(TEXT("Join to %s"), *address));

	APlayerController* controller = GetFirstLocalPlayerController();
	if (controller == nullptr) return;

	// 스팀 OSS가 일종의 가상 공인아이피를 부여해준다
	// ETravelType
	controller->ClientTravel(address, ETravelType::TRAVEL_Absolute);
}
