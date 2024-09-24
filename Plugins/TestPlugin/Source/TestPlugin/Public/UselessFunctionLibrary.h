// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UselessFunctionLibrary.generated.h"

DECLARE_DYNAMIC_DELEGATE_OneParam(FAIVStringReady, const FString&, String);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAIVTextureReady, UTexture2D*, MandelbrotTexture);
DECLARE_DYNAMIC_DELEGATE_OneParam(FAIVNTPTime, const FString&, String);

/**
 * 
 */
UCLASS()
class TESTPLUGIN_API UUselessFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	static void AIVReportReflection(UObject* InObject);

	UFUNCTION(BlueprintCallable)
	static void DoSomethingReallySlow(const FAIVStringReady& FAIVStringReady);

	UFUNCTION(BlueprintCallable)
	static void MandelbrotGenerator(const FAIVTextureReady& OnTextureReady, const FVector2D Size, const int32 Iterations = 100);

	UFUNCTION(BlueprintCallable)
	static void GetTimeFromServer(const FString& Address, const int32 Port, const FAIVNTPTime& OnTimeQuery, const FString& FileName);
};