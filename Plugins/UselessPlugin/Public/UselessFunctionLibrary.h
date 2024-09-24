// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UselessFunctionLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FAIVStringReady, const FString&, String);

/**
 * 
 */
UCLASS()
class USELESSPLUGIN_API UUselessFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void AIVReportReflection(UObject* InObject);
	

	UFUNCTION(BlueprintCallable)
	static void DoSomethingReallySlow(const FAIVStringReady& FAIVStringReady);
};
