// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ChatPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnUserLoginDelegate, const FString&, ID);

UCLASS()
class SAMPLECHAT_API AChatPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AChatPlayerController();
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UPROPERTY()
	FOnUserLoginDelegate OnUserLogin;
	UPROPERTY(Replicated)
	bool bMyTurn;
	UPROPERTY(Replicated)
	float TurnTimeRemaining;

	UPROPERTY()
	FTimerHandle TurnTimerHandle;

	UUserWidget* Widget;
	
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Chat")
	void ClientOnLoginWithID(const FString& ID);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Chat")
	void ServerSetUserID(const FString& ID);
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Chat")
	void ClientGotBroadcast(const FString& Msg);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Chat")
	void ServerOnSendMessageToServer(const FString& Msg);
	UFUNCTION(Client, Reliable)
	void ClientUpdateScore();
	UFUNCTION(Client, Reliable)
	void ClientUpdateTurnState(bool bIsMyTurn);
	UFUNCTION(Client, Reliable)
	void ClientUpdateTurnTimeRemaining(float TimeRemaining);
	UFUNCTION()
	void StartTurnTimer();
	UFUNCTION()
	void StopTurnTimer(float TimeDefault);
	UFUNCTION()
	void UpdateTurnTimer();
	UFUNCTION()
	void OnTurnTimeExpired();

	FString GetUserID() const { return UserID; }
	UUserWidget* GetWidget() const { return Widget; }
	int32 GetTryCount() const { return TryCount; }
	bool GetMyTurn() const { return bMyTurn; }
	void SetTryCount(int32 NewNumber) { TryCount = NewNumber; }
	void SetCorrectCount(int32 NewNumber);
	void SetMyTurn(bool bNewTurn);

protected:
	virtual void BeginPlay() override;

	UPROPERTY(Replicated)
	int32 RandomNumberFromServer;

	int32 TryCount;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastUpdateRandomNumber(int32 NewNumber);

private:
	FString UserID;

	void FindAndBindWidget();
};
