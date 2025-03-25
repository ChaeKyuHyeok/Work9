// Fill out your copyright notice in the Description page of Project Settings.


#include "ChatPlayerController.h"

#include "MyChatMode.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Widget_ChatWindow.h"
#include "Net/UnrealNetwork.h"

AChatPlayerController::AChatPlayerController()
{
	Widget = nullptr;
	bReplicates = true;
	RandomNumberFromServer = 0;
	TryCount = 0;
	bMyTurn = false;
}

void AChatPlayerController::ServerSetUserID_Implementation(const FString& ID)
{
	UserID = ID;
}

void AChatPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (AMyChatMode* GameMode = Cast<AMyChatMode>(GetWorld()->GetAuthGameMode()))
		{
			//GameMode->UpdatePCs();
			RandomNumberFromServer = GameMode->GetRandomNumberFromGameMode();
			MulticastUpdateRandomNumber(RandomNumberFromServer);
		}
	}

	if (IsLocalPlayerController())
	{
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(
			TimerHandle,
			this,
			&AChatPlayerController::FindAndBindWidget,
			0.5f,
			false);
	}
}

void AChatPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AChatPlayerController, RandomNumberFromServer);
	DOREPLIFETIME(AChatPlayerController, bMyTurn);
}

void AChatPlayerController::ClientUpdateTurnState_Implementation(bool bIsMyTurn)
{
	bMyTurn = bIsMyTurn;
}

void AChatPlayerController::MulticastUpdateRandomNumber_Implementation(int32 NewNumber)
{
	RandomNumberFromServer = NewNumber;
}

void AChatPlayerController::FindAndBindWidget()
{
	TArray<UUserWidget*> FoundWidgets;
	UWidgetBlueprintLibrary::GetAllWidgetsOfClass(this, FoundWidgets, UWidget_ChatWindow::StaticClass(), true);

	if (FoundWidgets.Num() > 0)
	{
		Widget = FoundWidgets[0];
		UWidget_ChatWindow* ChatWidget = Cast<UWidget_ChatWindow>(Widget);
		if (ChatWidget)
		{
			ChatWidget->SetMsgToUserController.AddDynamic(this, &AChatPlayerController::OnSendMessageToServer);
		}
	}
}

void AChatPlayerController::OnLoginWithID_Implementation(const FString& ID)
{
	this->UserID = ID;
	if (!HasAuthority())
	{
		ServerSetUserID(ID);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *ID);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("User logged in: %s"), *ID));
}

void AChatPlayerController::GotBroadcast_Implementation(const FString& Msg)
{
	FColor MsgColor;
	if (Msg.Contains("가 제출한 정답"))
	{
		MsgColor = FColor::Orange;
	}
	else if (Msg.Contains("Won!!"))
	{
		MsgColor = FColor::Cyan;
	}
	else
	{
		MsgColor = FColor::White;
	}

	GEngine->AddOnScreenDebugMessage(-1, 5.f, MsgColor, *Msg);
}

void AChatPlayerController::OnSendMessageToServer_Implementation(const FString& Msg)
{
	UE_LOG(LogTemp, Warning, TEXT("User ID : %s, Msg : %s"), *UserID, *Msg);
	AMyChatMode* MyChatMode = Cast<AMyChatMode>(GetWorld()->GetAuthGameMode());
	if (MyChatMode)
	{
		MyChatMode->GotMessageFromClient(FString::Printf(TEXT("%s : %s"), *UserID, *Msg), this);
	}
}
