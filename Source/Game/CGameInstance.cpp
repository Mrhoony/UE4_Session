#include "CGameInstance.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"
#include "Widgets/CMainMenu.h"

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
	if(MainMenu != nullptr)
		MainMenu->Teardown();

	UEngine* engine = GetEngine();
	if (engine == nullptr) return;

	engine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, TEXT("Host"));

	UWorld* world = GetWorld();
	if (world == nullptr)return; // 트랜지션맵의 경우 월드가 리턴이 안됨

	world->ServerTravel("/Game/Maps/Play?listen");
}

void UCGameInstance::Join(const FString& InAddress)
{
	if (MainMenu != nullptr)
		MainMenu->Teardown();

	UEngine* engine = GetEngine();
	if (engine == nullptr) return;
	engine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, FString::Printf(TEXT("Join to %s"), *InAddress));

	APlayerController* controller = GetFirstLocalPlayerController();
	if (controller == nullptr) return;

	// 스팀 OSS가 일종의 가상 공인아이피를 부여해준다
	// ETravelType 
	controller->ClientTravel(InAddress, ETravelType::TRAVEL_Absolute);
}

void UCGameInstance::LoadMainMenuLevel()
{
	APlayerController* controller = GetFirstLocalPlayerController();
	if (controller == nullptr) return;
	controller->ClientTravel("/Game/Maps/MainMenu", ETravelType::TRAVEL_Absolute);
}
