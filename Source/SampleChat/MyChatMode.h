// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyChatMode.generated.h"

class AChatPlayerController;

UCLASS()
class SAMPLECHAT_API AMyChatMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMyChatMode();

	virtual void BeginPlay() override;
	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	TArray<AActor*> PlayerControllers;

	UFUNCTION()
	void NextTurn();
	UFUNCTION(Server, Reliable)
	void GotMessageFromClient(const FString& Msg, AChatPlayerController* CPC);

	int32 GetRandomNumberFromGameMode() const { return RandomNumber; }

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Turn System")
	int32 CurrentTurnIndex;
	
	void SetTurn(int32 NewTurnIndex);
private:
	int32 RandomNumber;

	void ResetGame(FString UserID);
};
