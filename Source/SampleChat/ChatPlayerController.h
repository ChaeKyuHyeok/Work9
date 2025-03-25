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

	UUserWidget* Widget;

	UFUNCTION(Client, Reliable)
	void ClientUpdateTurnState(bool bIsMyTurn);
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Chat")
	void OnLoginWithID(const FString& ID);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Chat")
	void ServerSetUserID(const FString& ID);
	UFUNCTION(Client, Reliable, BlueprintCallable, Category = "Chat")
	void GotBroadcast(const FString& Msg);
	UFUNCTION(Server, Reliable, BlueprintCallable, Category = "Chat")
	void OnSendMessageToServer(const FString& Msg);

	FString GetUserID() const { return UserID; }
	int32 GetTryCount() const { return TryCount; }
	bool GetMyTurn() const { return bMyTurn; }
	void SetTryCount(int32 NewNumber) { TryCount = NewNumber; }
	void SetMyTurn(bool bNewBool) { bMyTurn = bNewBool; ClientUpdateTurnState(bNewBool); }

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
