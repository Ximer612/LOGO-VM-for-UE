// Fill out your copyright notice in the Description page of Project Settings.


#include "LogoFactory.h"
#include "Logo.h"
#include "ImageUtils.h"

ULogoFactory::ULogoFactory()
{
	bEditorImport = true;
	SupportedClass = UTexture2D::StaticClass();
	Formats.Add(FString("logo;Logo file format"));
}

UObject* ULogoFactory::FactoryCreateFile(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, const FString& Filename, const TCHAR* Parms, FFeedbackContext* Warn, bool& bOutOperationCanceled)
{
	TArray<FColor> Result = ULogoVMBlueprintFunctionLibrary::LogoExecuteSimpleFromFile(Filename, 16, 16);

	FCreateTexture2DParameters Params;
	return FImageUtils::CreateTexture2D(16, 16, Result, InParent, InName.ToString(), Flags, Params);;
}
