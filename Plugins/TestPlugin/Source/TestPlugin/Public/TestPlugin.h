// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

class FAIVInfiniteLoop;

DECLARE_LOG_CATEGORY_EXTERN(LogAIV, Log, All); // in .h file

class FTestPluginModule : public IModuleInterface, public FSelfRegisteringExec
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;

private:
	FAIVInfiniteLoop* AIVRunnable = nullptr;
	FRunnableThread* NewAIVThread = nullptr;

	TFuture<FString> AIVFuture;
};