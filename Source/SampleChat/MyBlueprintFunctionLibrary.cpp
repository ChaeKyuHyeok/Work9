// Fill out your copyright notice in the Description page of Project Settings.


#include "MyBlueprintFunctionLibrary.h"

int32 UMyBlueprintFunctionLibrary::GetRandomNumber()
{
	int32 NumberFirst = 0;
	int32 NumberSecond = 0;
	int32 NumberThird = 0;

	NumberFirst = FMath::RandRange(1, 9);
	while (NumberSecond == 0)
	{
		NumberSecond = FMath::RandRange(1, 9);
		if (NumberSecond == NumberFirst) NumberSecond = 0;
	}
	while (NumberThird == 0)
	{
		NumberThird = FMath::RandRange(1, 9);
		if (NumberThird == NumberFirst || NumberThird == NumberSecond) NumberThird = 0;
	}

	return NumberFirst * 100 + NumberSecond * 10 + NumberThird;
}

int32 UMyBlueprintFunctionLibrary::ParsingPlayerAnswer(const FString& PlayerAnswer)
{
	// "/" 뒤의 숫자 문자열만 추출, 응답이 잘못된 경우 -1 반환
	FString NumberString = PlayerAnswer.RightChop(PlayerAnswer.Find(TEXT("/")) + 1);
	if (NumberString.Len() == 3)
	{
		return FCString::Atoi(*NumberString);
	}
	else
	{
		return -1;
	}
}

bool UMyBlueprintFunctionLibrary::IsCorrectNumber(int32 PlayersNumber, int32 CorrectNumber)
{
	if (PlayersNumber == CorrectNumber)
	{
		return true;
	}
	else
	{
		return false;
	}
}

FString UMyBlueprintFunctionLibrary::SetResultMessage(int32 PlayersNumber, int32 CorrectNumber)
{
	int32 PNumber1 = PlayersNumber / 100;
	int32 PNumber2 = PlayersNumber % 100 / 10;
	int32 PNumber3 = PlayersNumber % 10;
	int32 Number1 = CorrectNumber / 100;
	int32 Number2 = CorrectNumber % 100 / 10;
	int32 Number3 = CorrectNumber % 10;

	int32 Strike = 0;
	int32 Ball = 0;

	if (PNumber1 == Number1) Strike++;
	else if (PNumber1 == Number2 || PNumber1 == Number3) Ball++;

	if (PNumber2 == Number2) Strike++;
	else if (PNumber2 == Number1 || PNumber2 == Number3) Ball++;

	if (PNumber3 == Number3) Strike++;
	else if (PNumber3 == Number1 || PNumber3 == Number2) Ball++;
	
	if (Strike > 0 || Ball > 0)
	{
		return FString::Printf(TEXT("%dS%dB"), Strike, Ball);
	}
	else
	{
		return FString::Printf(TEXT("Out"));
	}
}
