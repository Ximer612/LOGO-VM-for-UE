// Fill out your copyright notice in the Description page of Project Settings.


#include "LogoVMBlueprintFunctionLibrary.h"
#include "Logo.h"

TArray<FColor> ULogoVMBlueprintFunctionLibrary::LogoExecuteSimple(const TArray<FString>& Tokens, const int32 StageWidth, const int32 StageHeight)
{
	FLogoVM LogoVM;
	LogoVM.SetStageSize(FIntVector2(StageWidth,StageHeight));

	if (LogoVM.Execute(Tokens)) 
	{
		return LogoVM.GetStage();
	}

	return TArray<FColor>();
}

TArray<FColor> ULogoVMBlueprintFunctionLibrary::LogoExecuteSimpleFromFile(const FString& FilePath, const int32 StageWidth, const int32 StageHeight)
{
	FString Result;

	if (!FFileHelper::LoadFileToString(Result, *FilePath))
	{
		return TArray<FColor>();
	}

	TArray<FString> Tokens;
	const TCHAR* Separators[] = { TEXT(" "), TEXT("\t") ,TEXT("\n") ,TEXT("\r") };

	Result.ParseIntoArray(Tokens, Separators,4);

	//for (const FString& Token : Tokens)
	//{
	//	UE_LOG(LogTemp, Error, TEXT("Token: %s"), *Token);
	//}

	return LogoExecuteSimple(Tokens, StageWidth, StageHeight);
}
