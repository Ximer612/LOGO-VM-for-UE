// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "LogoVMBlueprintFunctionLibrary.generated.h"

/**
 * 
 */
UCLASS()
class LOGO_API ULogoVMBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable)
	static TArray<FColor> LogoExecuteSimple(const TArray<FString>& Tokens, const int32 StageWidth, const int32 StageHeight);

	UFUNCTION(BlueprintCallable)
	static TArray<FColor> LogoExecuteSimpleFromFile(const FString& FilePath, const int32 StageWidth, const int32 StageHeight);
};
