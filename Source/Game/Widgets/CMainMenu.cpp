#include "CMainMenu.h"

#include "CServerRow.h"

#include "Components/Button.h"
#include "Components/WidgetSwitcher.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"

UCMainMenu::UCMainMenu(const FObjectInitializer& ObjectInitializer)
{
	ConstructorHelpers::FClassFinder<UUserWidget> serverRowClass(TEXT("/Game/Widgets/WB_ServerRow"));
	if (serverRowClass.Succeeded())
		ServerRowClass = serverRowClass.Class;
}

bool UCMainMenu::Initialize()
{
	bool success = Super::Initialize();
	if (success == false) return false;
	
	if (HostButton == nullptr) return false;
	HostButton->OnClicked.AddDynamic(this, &UCMainMenu::HostServer);

	if (JoinButton == nullptr) return false;
	JoinButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenJoinMenu);

	if (CancelJoinMenuButton == nullptr) return false;
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenMainMenu);

	if (ConfirmJoinMenuButton == nullptr) return false;
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::JoinServer);

	if (QuitButton == nullptr) return false;
	QuitButton->OnClicked.AddDynamic(this, &UCMainMenu::QuitPressed);

	return true;
}

void UCMainMenu::SetServerList(TArray<FString> InServerNames)
{
	ServerList->ClearChildren();

	for (FString& serverName : InServerNames)
	{
		UCServerRow* row = CreateWidget<UCServerRow>(this, ServerRowClass);
		if (row == nullptr)return;

		row->ServerName->SetText(FText::FromString(serverName));
		row->Setup(this, ServerList->GetAllChildren().Num());

		ServerList->AddChild(row);
	}
}

void UCMainMenu::SetSelectedIndex(uint32 Index)
{
	SelectedIndex = Index;
}

void UCMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
		MenuInterface->Host();
}

void UCMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet())
	{
		UE_LOG(LogTemp, Display, TEXT("Selected Index : %d"), SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Selected Index is not set"));
	}

	if (MenuInterface != nullptr)
	{
		/*if (IPAddressField == nullptr) return;
		const FString& address = IPAddressField->GetText().ToString();*/

		MenuInterface->Join("");
	}
}

void UCMainMenu::OpenJoinMenu()
{
	if (MenuSwitcher == nullptr) return;
	if (JoinMenu == nullptr) return;

	if (MenuInterface != nullptr)
	MenuInterface->RefreshServerList();

	MenuSwitcher->SetActiveWidget(JoinMenu);
}

void UCMainMenu::OpenMainMenu()
{
	if (MenuSwitcher == nullptr) return;
	if (MainMenu == nullptr) return;

	MenuSwitcher->SetActiveWidget(MainMenu);
}

void UCMainMenu::QuitPressed()
{
	UWorld* world = GetWorld();
	if (world == nullptr) return;

	APlayerController* controller = world->GetFirstPlayerController();
	if (controller == nullptr) return;

	controller->ConsoleCommand("Quit");
}