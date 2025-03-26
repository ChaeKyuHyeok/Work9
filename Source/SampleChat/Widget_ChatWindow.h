// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/EditableTextBox.h"
#include "Widget_ChatWindow.generated.h"

class UCanvasPanel;
class UEditableTextBox;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FSetMsgToUserControllerDelegate, const FString&, Msg);

UCLASS()
class SAMPLECHAT_API UWidget_ChatWindow : public UUserWidget
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chat")
	FSetMsgToUserControllerDelegate SetMsgToUserController;
	
	UFUNCTION()
	void OnLoginEvent(const FString& UserID);
	UFUNCTION()
	void HandleTextCommitted(const FText& Text, ETextCommit::Type Commit);
	UFUNCTION()
	void UpdateTimerText(const FString& NewTime);
	UFUNCTION()
	void UpdateScoreTextBlock();
	
protected:
	virtual void NativeConstruct() override;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> RootCanvas;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> EditableTextBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> ScoreTextBlock;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerTextBlock;

	int32 CorrectCount;
};
