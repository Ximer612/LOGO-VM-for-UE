// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestPlugin.h"
#include "HAL/ThreadManager.h"

DEFINE_LOG_CATEGORY(LogAIV); // in .cpp file

#define LOCTEXT_NAMESPACE "FTestPluginModule"

class FAIVInfiniteLoop : public FRunnable
{
public:
	bool Init() override
	{
		UE_LOG(LogTemp, Error, TEXT("FAIVInfiniteLoop Init() [%s]"), *FThreadManager::Get().GetThreadName(FPlatformTLS::GetCurrentThreadId()));
		bRunning = true;

		return true;
	}

	uint32 Run() override
	{
		while (bRunning)
		{
			UE_LOG(LogTemp, Error, TEXT("Hello from FAIVInfiniteLoop! [%s]"), *FThreadManager::Get().GetThreadName(FPlatformTLS::GetCurrentThreadId()));
			FPlatformProcess::Sleep(3);
		}

		UE_LOG(LogTemp, Error, TEXT("FAIVInfiniteLoop Run() complete [%s]"), *FThreadManager::Get().GetThreadName(FPlatformTLS::GetCurrentThreadId()));

		return 0;
	}

	void Stop() override
	{
		UE_LOG(LogTemp, Error, TEXT("FAIVInfiniteLoop Stop() [%s]"), *FThreadManager::Get().GetThreadName(FPlatformTLS::GetCurrentThreadId()));
		bRunning = false;
	}

protected:
	bool bRunning;
};

void FTestPluginModule::StartupModule()
{
	FQueuedThreadPool* ThreadPool = FQueuedThreadPool::Allocate();
	ThreadPool->Create(10, 64 * 1024, EThreadPriority::TPri_Normal, TEXT("AIVThreadPool"));
}

void FTestPluginModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
	UE_LOG(LogTemp, Error, TEXT("FUselessPluginModule::ShutdownModule() [%s]"), *FThreadManager::Get().GetThreadName(FPlatformTLS::GetCurrentThreadId()));
	if (NewAIVThread && AIVRunnable)
	{
		NewAIVThread->Kill(true);

		delete NewAIVThread;
		delete AIVRunnable;
		AIVRunnable = nullptr;
		NewAIVThread = nullptr;
	}
}

bool FTestPluginModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FParse::Command(&Cmd, TEXT("killaiv")))
	{
		if (NewAIVThread && AIVRunnable)
		{
			NewAIVThread->Kill(true);

			delete NewAIVThread;
			delete AIVRunnable;
			AIVRunnable = nullptr;
			NewAIVThread = nullptr;
		}
		return true;
	}
	else if (FParse::Command(&Cmd, TEXT("spawnaiv")))
	{
		if (!NewAIVThread && !AIVRunnable)
		{
			UE_LOG(LogTemp, Error, TEXT("FUselessPluginModule::StartupModule() [%s]"), *FThreadManager::Get().GetThreadName(FPlatformTLS::GetCurrentThreadId()));
			AIVRunnable = new FAIVInfiniteLoop();
			NewAIVThread = FRunnableThread::Create(AIVRunnable, TEXT("AIVInfiniteLoop"));
		}
	}
	else if (FParse::Command(&Cmd, TEXT("aivtask")))
	{
		/*FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([]()
			{
				FPlatformProcess::Sleep(3);
				UE_LOG(LogTemp, Error, TEXT("Hello running on %s"), *FThreadManager::Get().GetThreadName(FPlatformTLS::GetCurrentThreadId()));
			}, TStatId(), nullptr, ENamedThreads::AnyThread);
			*/

			/*AsyncTask(ENamedThreads::AnyThread, []()
				{
					FPlatformProcess::Sleep(3);
					UE_LOG(LogTemp, Error, TEXT("Hello running on %s"), *FThreadManager::Get().GetThreadName(FPlatformTLS::GetCurrentThreadId()));
				});*/

		AIVFuture = AsyncPool(*GThreadPool, []() -> FString
			{
				FPlatformProcess::Sleep(3);
				return "Hello World";
			});


		UE_LOG(LogTemp, Error, TEXT("Task enqueued"));
		//Task->Wait();
		return true;
	}
	else if (FParse::Command(&Cmd, TEXT("aivfuture")))
	{
		if (AIVFuture.IsReady())
		{
			UE_LOG(LogTemp, Warning, TEXT("Future is ready: %s"), *AIVFuture.Get());
		}
		else
		{
			UE_LOG(LogTemp, Warning, TEXT("Future is not ready"));
		}
		return true;
	}
	return false;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTestPluginModule, TestPlugin)