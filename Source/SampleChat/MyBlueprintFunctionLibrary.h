// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MyBlueprintFunctionLibrary.generated.h"

UCLASS()
class SAMPLECHAT_API UMyBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "BlueprintFunctionLibrary")
	static int32 GetRandomNumber();
	UFUNCTION(BlueprintCallable, Category = "BlueprintFunctionLibrary")
	static int32 ParsingPlayerAnswer(const FString& PlayerAnswer);
	UFUNCTION(BlueprintCallable, Category = "BlueprintFunctionLibrary")
	static bool IsCorrectNumber(int32 PlayersNumber, int32 CorrectNumber);
	UFUNCTION(BlueprintCallable, Category = "BlueprintFunctionLibrary")
	static FString SetResultMessage(int32 PlayersNumber, int32 CorrectNumber);
};
