// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyStruct.generated.h"
/**
 * 
 */
USTRUCT(BlueprintType)
struct AIVLESSONPROJECT_API FMyStruct : public FTableRowBase
{
	GENERATED_BODY();

public:

	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Name;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	FString Description;
	UPROPERTY(BlueprintReadOnly, EditAnywhere)
	int32 MaxStack;
	FMyStruct();
	~FMyStruct();
};
