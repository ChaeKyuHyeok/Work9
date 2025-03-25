// Fill out your copyright notice in the Description page of Project Settings.


#include "Widget_ChatWindow.h"

#include "ChatPlayerController.h"
#include "Components/EditableTextBox.h"
#include "Kismet/GameplayStatics.h"

void UWidget_ChatWindow::NativeConstruct()
{
	Super::NativeConstruct();

	if (APlayerController* PlayerController = GetWorld()->GetFirstPlayerController())
	{
		if (AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(PlayerController))
		{
			ChatPlayerController->OnUserLogin.AddDynamic(this, &UWidget_ChatWindow::OnLoginEvent);
		}
	}
	
	if (EditableTextBox->IsValidLowLevel())
	{
		EditableTextBox->OnTextCommitted.AddDynamic(this, &UWidget_ChatWindow::HandleTextCommitted);
	}
}

void UWidget_ChatWindow::OnLoginEvent(const FString& UserID)
{
	if (EditableTextBox)
	{
		EditableTextBox->SetText(FText::FromString(UserID));
	}
}

void UWidget_ChatWindow::HandleTextCommitted(const FText& Text, ETextCommit::Type Commit)
{
	if (Commit == ETextCommit::OnEnter)
	{
		if (!Text.IsEmpty())
		{
			AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
			if (Text.ToString().Contains("/") && !ChatPlayerController->GetMyTurn())
			{
				GEngine->AddOnScreenDebugMessage(
					-1,
					5.f,
					FColor::Red,
					FString::Printf(TEXT("내 턴 아님!!! 답 맞추기 불가능")));
			}
			else
			{
				SetMsgToUserController.Broadcast(Text.ToString());
			}
		}

		if (EditableTextBox->IsValidLowLevel())
		{
			EditableTextBox->SetText(FText::GetEmpty());
		}
	}
}
