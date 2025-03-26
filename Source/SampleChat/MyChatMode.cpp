// Fill out your copyright notice in the Description page of Project Settings.


#include "MyChatMode.h"
#include "ChatPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "MyBlueprintFunctionLibrary.h"
#include "Widget_ChatWindow.h"
#include "Net/UnrealNetwork.h"

AMyChatMode::AMyChatMode()
{
	PrimaryActorTick.bCanEverTick = false;
	CurrentTurnIndex = 0;
	MaxPlayerNumber = 2;
}

void AMyChatMode::BeginPlay()
{
	Super::BeginPlay();

	RandomNumber = UMyBlueprintFunctionLibrary::GetRandomNumber();
	UE_LOG(LogTemp, Warning, TEXT("생성된 숫자 : %d"), RandomNumber);
}

void AMyChatMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);

	PlayerControllers.Add(NewPlayer);
	
	if (PlayerControllers.Num() == MaxPlayerNumber)
	{
		SetTurn(0);
	}
}

void AMyChatMode::NextTurn()
{
	if (PlayerControllers.Num() == 0)
		return;

	CurrentTurnIndex = (CurrentTurnIndex + 1) % PlayerControllers.Num();
	SetTurn(CurrentTurnIndex);
}

void AMyChatMode::HandlePlayerTimeout(AChatPlayerController* TimeoutPlayer)
{
	if (TimeoutPlayer)
	{
		for (AActor* PC : PlayerControllers)
		{
			if (AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(PC))
			{
				ChatPlayerController->ClientGotBroadcast(TimeoutPlayer->GetUserID() + FString::Printf(TEXT("의 시간이 초과 되었다.")));
			}
		}

		TimeoutPlayer->SetTryCount(TimeoutPlayer->GetTryCount() + 1);

		if (TimeoutPlayer->GetTryCount() >= 3)
		{
			for (AActor* PC : PlayerControllers)
			{
				if (AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(PC))
				{
					ChatPlayerController->ClientGotBroadcast(
						TimeoutPlayer->GetUserID() + FString::Printf(TEXT(" 모든 도전 횟수 사용으로 다른 플레이어가 승리한다")));
				}
			}

			FString OtherPlayerID;
			for (AActor* PC : PlayerControllers)
			{
				if (AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(PC))
				{
					if (ChatPlayerController != TimeoutPlayer)
					{
						OtherPlayerID = ChatPlayerController->GetUserID();
						break;
					}
				}
			}
			ResetGame(OtherPlayerID);
		}
		else
		{
			NextTurn();
		}
	}
}

void AMyChatMode::SetTurn(int32 NewTurnIndex)
{
	for (int32 i = 0; i < PlayerControllers.Num(); i++)
	{
		AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(PlayerControllers[i]);
		if (ChatPlayerController)
		{
			bool bIsCurrentTurn = (i == CurrentTurnIndex);
			
			if (bIsCurrentTurn)
			{
				ChatPlayerController->SetMyTurn(bIsCurrentTurn);
			}
			else
			{
				ChatPlayerController->StopTurnTimer(10.0f);
			}
		}
	}
}

void AMyChatMode::ResetGame(FString UserID)
{
	CurrentTurnIndex = 0;
	SetTurn(0);
	RandomNumber = UMyBlueprintFunctionLibrary::GetRandomNumber();
	UE_LOG(LogTemp, Warning, TEXT("생성된 숫자 : %d"), RandomNumber);
	for (AActor* PC : PlayerControllers)
	{
		AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(PC);
		if (ChatPlayerController)
		{
			ChatPlayerController->SetTryCount(0);
			ChatPlayerController->ClientGotBroadcast(UserID + FString::Printf(TEXT(" Won!! 다시 게임이 시작됐다.")));
		}
	}
}

void AMyChatMode::ServerGotMessageFromClient_Implementation(const FString& Msg, AChatPlayerController* CPC)
{
	int32 CPCTryCount;
	int32 PlayerAnswerNumber;
	FString PlayerID;
	FString Result;
	FString OtherPlayerID;
	bool bIsCorrect = false;
	bool bDoReset = false;

	if (Msg.Contains("/"))
	{
		
		//시도횟수를 가져와서 ++ 해줌
		CPCTryCount = CPC->GetTryCount() + 1;
		CPC->SetTryCount(CPCTryCount);
		//플레이어가 제출한 정답에서 숫자 추출
		PlayerAnswerNumber = UMyBlueprintFunctionLibrary::ParsingPlayerAnswer(Msg);
		//입력한 플레이어의 ID 추출
		PlayerID = Msg.Left(Msg.Find(" :"));
		//응답에 따라 결과 문자열 생성
		Result = UMyBlueprintFunctionLibrary::SetResultMessage(PlayerAnswerNumber, RandomNumber);
		//정답인지 아닌지 판별하는 bool
		bIsCorrect = UMyBlueprintFunctionLibrary::IsCorrectNumber(PlayerAnswerNumber, RandomNumber);
	}

	for (AActor* Player : PlayerControllers)
	{
		AChatPlayerController* ChatPC = Cast<AChatPlayerController>(Player);
		if (ChatPC == CPC) continue;

		if (OtherPlayerID.IsEmpty())
		{
			OtherPlayerID.Append(ChatPC->GetUserID());
		}
	}

	bool bShouldChangeTurn = false;
	for (AActor* PC : PlayerControllers)
	{
		AChatPlayerController* ChatPlayerController = Cast<AChatPlayerController>(PC);
		if (ChatPlayerController)
		{
			//응답일 경우
			if (Msg.Contains("/"))
			{
				if (bIsCorrect)
				{
					ChatPlayerController->ClientGotBroadcast(
						PlayerID + FString::Printf(TEXT("가 제출한 정답 : %d"), PlayerAnswerNumber));
					bDoReset = true;
				}
				else
				{
					ChatPlayerController->ClientGotBroadcast(
						PlayerID + FString::Printf(TEXT(" : 시도 횟수 : %d"), CPCTryCount));
					ChatPlayerController->ClientGotBroadcast(
						PlayerID + FString::Printf(TEXT("가 제출한 정답 : %d"), PlayerAnswerNumber));
					ChatPlayerController->ClientGotBroadcast(Result);
					if (CPCTryCount == 3)
					{
						ChatPlayerController->ClientGotBroadcast(
							PlayerID + FString::Printf(TEXT(" 모든 도전 횟수 사용으로 다른 플레이어가 승리한다")));
						bDoReset = true;
					}
				}
				if (ChatPlayerController == CPC && ChatPlayerController->GetMyTurn())
				{
					bShouldChangeTurn = true;
				}
			}
			//응답이 아닐 경우 일반 대화 로그로 출력
			else
			{
				ChatPlayerController->ClientGotBroadcast(Msg);
			}
		}
	}

	if (bIsCorrect)
	{
		if (CPC)
		{
			CPC->ClientUpdateScore();
		}
	}
	if (bShouldChangeTurn)
	{
		NextTurn();
	}
	if (bDoReset)
	{
		bIsCorrect ? ResetGame(PlayerID) : ResetGame(OtherPlayerID);
	}
}
