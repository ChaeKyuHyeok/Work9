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

void AChatPlayerController::ClientUpdateTurnTimeRemaining_Implementation(float TimeRemaining)
{
	TurnTimeRemaining = TimeRemaining;

	if (UWidget_ChatWindow* ChatWidget = Cast<UWidget_ChatWindow>(Widget))
	{
		ChatWidget->UpdateTimerText(FString::FromInt(TurnTimeRemaining));
	}
}

void AChatPlayerController::ClientUpdateScore_Implementation()
{
	if (Widget)
	{
		if (UWidget_ChatWindow* ChatWidget = Cast<UWidget_ChatWindow>(Widget))
		{
			ChatWidget->UpdateScoreTextBlock();
		}
	}
}

void AChatPlayerController::StartTurnTimer()
{
	TurnTimeRemaining = 10.0f;
	GetWorldTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&AChatPlayerController::UpdateTurnTimer,
		1.0f,
		true);
	ClientUpdateTurnTimeRemaining(TurnTimeRemaining);
}

void AChatPlayerController::StopTurnTimer(float TimeDefault)
{
	ClientUpdateTurnTimeRemaining(TimeDefault);
	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
}

void AChatPlayerController::UpdateTurnTimer()
{
	TurnTimeRemaining -= 1.0f;

	ClientUpdateTurnTimeRemaining(TurnTimeRemaining);

	if (TurnTimeRemaining <= 0.0f)
	{
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		OnTurnTimeExpired();
	}
}

void AChatPlayerController::OnTurnTimeExpired()
{
	if (HasAuthority())
	{
		if (AMyChatMode* GameMode = Cast<AMyChatMode>(GetWorld()->GetAuthGameMode()))
		{
			GameMode->HandlePlayerTimeout(this);
		}
	}
}

void AChatPlayerController::SetMyTurn(bool bNewTurn)
{
	bMyTurn = bNewTurn;

	if (bMyTurn)
	{
		StartTurnTimer();
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		if (UWidget_ChatWindow* ChatWidget = Cast<UWidget_ChatWindow>(Widget))
		{
			ChatWidget->UpdateTimerText(FString::FromInt(10.0f));
		}
	}
	
	ClientUpdateTurnState(bNewTurn); 
}

void AChatPlayerController::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		if (AMyChatMode* GameMode = Cast<AMyChatMode>(GetWorld()->GetAuthGameMode()))
		{
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
	DOREPLIFETIME(AChatPlayerController, TurnTimeRemaining);
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
			ChatWidget->SetMsgToUserController.AddDynamic(this, &AChatPlayerController::ServerOnSendMessageToServer);
		}
	}
}

void AChatPlayerController::ClientOnLoginWithID_Implementation(const FString& ID)
{
	this->UserID = ID;
	if (!HasAuthority())
	{
		ServerSetUserID(ID);
	}

	UE_LOG(LogTemp, Warning, TEXT("%s"), *ID);

	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, FString::Printf(TEXT("%s 로 로그인"), *ID));
}

void AChatPlayerController::ClientGotBroadcast_Implementation(const FString& Msg)
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

void AChatPlayerController::ServerOnSendMessageToServer_Implementation(const FString& Msg)
{
	AMyChatMode* MyChatMode = Cast<AMyChatMode>(GetWorld()->GetAuthGameMode());
	if (MyChatMode)
	{
		MyChatMode->ServerGotMessageFromClient(FString::Printf(TEXT("%s : %s"), *UserID, *Msg), this);
	}
}
