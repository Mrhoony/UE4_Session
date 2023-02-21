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
	HostButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenHostMenu);
	if (JoinButton == nullptr) return false;
	JoinButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenJoinMenu);

	if (CancelJoinMenuButton == nullptr) return false;
	CancelJoinMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenMainMenu);
	if (CancelHostMenuButton == nullptr) return false;
	CancelHostMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::OpenMainMenu);

	if (ConfirmJoinMenuButton == nullptr) return false;
	ConfirmJoinMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::JoinServer);
	if (ConfirmHostMenuButton == nullptr) return false;
	ConfirmHostMenuButton->OnClicked.AddDynamic(this, &UCMainMenu::HostServer);

	if (QuitButton == nullptr) return false;
	QuitButton->OnClicked.AddDynamic(this, &UCMainMenu::QuitPressed);

	return true;
}

void UCMainMenu::SetServerList(TArray<FServerData> InServerNames)
{
	ServerList->ClearChildren();

	for (FServerData& serverData : InServerNames)
	{
		UCServerRow* row = CreateWidget<UCServerRow>(this, ServerRowClass);
		if (row == nullptr)return;

		row->ServerName->SetText(FText::FromString(serverData.Name));
		row->HostUser->SetText(FText::FromString(serverData.HostUserName));
		FString fractionText = FString::Printf(TEXT("%d/%d"), serverData.CurrentPlayers, serverData.MaxPlayers);
		row->ConnectionFraction->SetText(FText::FromString(fractionText));
		row->Setup(this, ServerList->GetAllChildren().Num());

		ServerList->AddChild(row);
	}
}

void UCMainMenu::SetSelectedIndex(uint32 Index)
{
	SelectedIndex = Index;
	UpdateChildren();
}

void UCMainMenu::HostServer()
{
	if (MenuInterface != nullptr)
	{
		FString serverName = ServerHostName->Text.ToString();
		MenuInterface->Host(serverName);
	}
}

void UCMainMenu::JoinServer()
{
	if (SelectedIndex.IsSet() && MenuInterface != nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("Selected Index : %d"), SelectedIndex.GetValue());
		MenuInterface->Join(SelectedIndex.GetValue());
	}
	else
	{
		UE_LOG(LogTemp, Display, TEXT("Selected Index is not set"));
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

void UCMainMenu::OpenHostMenu()
{
	if (MenuSwitcher == nullptr) return;
	if (HostMenu == nullptr) return;

	MenuSwitcher->SetActiveWidget(HostMenu);
}

void UCMainMenu::UpdateChildren()
{
	for (int32 i = 0; i < ServerList->GetChildrenCount(); i++)
	{
		UCServerRow* serverRow = Cast<UCServerRow>(ServerList->GetChildAt(i));
		if (serverRow != nullptr)
		{
			serverRow->bSelected = (SelectedIndex.IsSet()) && (SelectedIndex.GetValue() == i);
		}
	}
}
