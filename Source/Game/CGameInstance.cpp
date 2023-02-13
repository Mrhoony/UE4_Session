#include "CGameInstance.h"
#include "Engine/Engine.h"
#include "Blueprint/UserWidget.h"

UCGameInstance::UCGameInstance(const FObjectInitializer& ObjectInitializer)
{
	GLog->Log("GameInstance Construction Called");

	ConstructorHelpers::FClassFinder<UUserWidget> mainMenuClass(TEXT("/Game/Widgets/WB_MainMenu"));

	if (mainMenuClass.Succeeded())
		MainMenuClass = mainMenuClass.Class;
}

void UCGameInstance::Init()
{
	GLog->Log("GameInstance Init Called");

	
}

void UCGameInstance::LoadMainMenu()
{
	if (MainMenuClass == nullptr) return;

	UUserWidget* mainMenu = CreateWidget<UUserWidget>(this, MainMenuClass);
	if (mainMenu == nullptr) return;
	mainMenu->AddToViewport();

	mainMenu->bIsFocusable = true;

	FInputModeUIOnly inputMode;
	inputMode.SetWidgetToFocus(mainMenu->TakeWidget());
	inputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	
	APlayerController* controller = GetFirstLocalPlayerController();
	if (controller == nullptr) return;
	controller->SetInputMode(inputMode);
	controller->bShowMouseCursor = true;
}

void UCGameInstance::Host()
{
	UEngine* engine = GetEngine();
	if (engine == nullptr) return;

	engine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, TEXT("Host"));

	UWorld* world = GetWorld();
	if (world == nullptr)return; // 트랜지션맵의 경우 월드가 리턴이 안됨

	world->ServerTravel("/Game/Maps/Play?listen");
}

void UCGameInstance::Join(const FString& InAddress)
{
	UEngine* engine = GetEngine();
	if (engine == nullptr) return;

	engine->AddOnScreenDebugMessage(0, 2.f, FColor::Green, FString::Printf(TEXT("Join to %s"), *InAddress));

	APlayerController* controller = GetFirstLocalPlayerController();
	if (controller == nullptr) return;

	// 스팀 OSS가 일종의 가상 공인아이피를 부여해준다
	// ETravelType 
	controller->ClientTravel(InAddress, ETravelType::TRAVEL_Absolute);
}