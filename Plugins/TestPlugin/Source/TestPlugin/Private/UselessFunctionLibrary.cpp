// Fill out your copyright notice in the Description page of Project Settings.


#include "UselessFunctionLibrary.h"
#include <ImageUtils.h>
#include <Interfaces/IPv4/IPv4Endpoint.h>
#include <Common/UdpSocketBuilder.h>
#include <Sockets.h>
#include <Serialization/ArrayWriter.h>
#include <Serialization/ArrayReader.h>
#include "Misc/ByteSwap.h"

struct FAIVSocketBuilder
{
	FAIVSocketBuilder(const FString& SocketDescription)
	{
		Socket = FUdpSocketBuilder(SocketDescription);
	}

	~FAIVSocketBuilder()
	{
		Socket->Close();
	}

	FAIVSocketBuilder() = delete;
	FAIVSocketBuilder(const FAIVSocketBuilder&) = delete;
	FAIVSocketBuilder& operator=(const FAIVSocketBuilder&) = delete;

	FSocket* Socket;
};

void UUselessFunctionLibrary::AIVReportReflection(UObject* InObject)
{
	if (!InObject)
	{
		return;
	}

	UE_LOG(LogTemp, Warning, TEXT("Object Name: %s"), *InObject->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Object Path: %s"), *InObject->GetPathName());
	UE_LOG(LogTemp, Warning, TEXT("Object FullName: %s"), *InObject->GetFullName());

	UClass* ObjectClass = InObject->GetClass();

	UE_LOG(LogTemp, Warning, TEXT("Class Name: %s"), *ObjectClass->GetName());
	UE_LOG(LogTemp, Warning, TEXT("Class Path: %s"), *ObjectClass->GetPathName());
	UE_LOG(LogTemp, Warning, TEXT("Class FullName: %s"), *ObjectClass->GetFullName());

	for (TFieldIterator<FProperty> It(ObjectClass); It; ++It)
	{
		FProperty* Property = *It;

		UE_LOG(LogTemp, Warning, TEXT("Property %s@%d (%s)"), *Property->GetName(), Property->GetOffset_ForInternal(), *Property->GetCPPType());

		FStrProperty* StrProperty = CastField<FStrProperty>(Property);
		if (StrProperty)
		{
			FString* StringPtr = StrProperty->GetPropertyValuePtr_InContainer(InObject);
			if (StringPtr)
			{
				UE_LOG(LogTemp, Warning, TEXT("\"%s\""), *(*StringPtr));
			}
		}

	}

	FProperty* Property = ObjectClass->FindPropertyByName(TEXT("DummyMessage"));
	if (!Property)
	{
		return;
	}

	FStrProperty* StrProperty = CastField<FStrProperty>(Property);
	if (StrProperty)
	{
		StrProperty->SetPropertyValue_InContainer(InObject, "Hello Reflection");
	}


}

void UUselessFunctionLibrary::DoSomethingReallySlow(const FAIVStringReady& FAIVStringReady)
{
	AsyncPool(*GThreadPool, [FAIVStringReady]()
		{
			FPlatformProcess::Sleep(3);
			FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([FAIVStringReady]()
				{
					FAIVStringReady.ExecuteIfBound("Hello World");
				}, TStatId(), nullptr, ENamedThreads::GameThread);
		});
}

void UUselessFunctionLibrary::MandelbrotGenerator(const FAIVTextureReady& OnTextureReady, const FVector2D Size, const int32 Iterations)
{
	if (Size.X <= 0 || Size.Y <= 0 || Iterations <= 0)
	{
		OnTextureReady.ExecuteIfBound(nullptr);
		return;
	}

	//copy because size & iterations will be destroyed after the scope
	AsyncThread([Size, Iterations, OnTextureReady]()
	{
		double LoadingStartTime = FPlatformTime::Seconds();

		TArray<FColor> Colors;
		Colors.AddUninitialized(Size.X * Size.Y);

		auto SquareImaginary = [](const FVector2D Number) -> FVector2D
			{
				return FVector2D(
					FMath::Pow(Number.X, 2) - FMath::Pow(Number.Y, 2),
					2 * Number.X * Number.Y);
			};

		for (int32 PixelIndex = 0; PixelIndex < (Size.X * Size.Y); PixelIndex++)
		//ParallelFor(Size.X * Size.Y, [&](const int32 PixelIndex)
			{
				const int32 X = PixelIndex % static_cast<int32>(Size.X);
				const int32 Y = PixelIndex / static_cast<int32>(Size.X);

				const FVector2D Coord = FVector2D((1.0 / Size.X) * X, (1.0 / Size.Y) * Y);
				const FVector2D FixedCoord = (Coord - 0.5) * 2 - FVector2D(1, 0);
				FVector2D Z = FVector2D(0, 0);
				bool bFound = false;
				for (int32 Iteration = 0; Iteration < Iterations; Iteration++)
				{
					Z = SquareImaginary(Z) + FixedCoord;
					if (Z.Length() > 2)
					{
						const float Value = static_cast<float>(Iteration) / Iterations;
						Colors[Y * Size.X + X] = FColor(Value * 255, 0, 0, 255);
						bFound = true;
						break;
					}
				}

				if (!bFound)
				{
					Colors[Y * Size.X + X] = FColor(255, 255, 255, 255);
				}

			}
		//);

		UE_LOG(LogTemp, Error, TEXT("Mandelbrot generated in %f seconds - you have %d cores"), FPlatformTime::Seconds() - LoadingStartTime, FPlatformMisc::NumberOfCores());

		FGraphEventRef Task = FFunctionGraphTask::CreateAndDispatchWhenReady([OnTextureReady, Size, &Colors]()
			{
				UTexture2D* Texture = FImageUtils::CreateTexture2D(Size.X, Size.Y, Colors, GetTransientPackage(),
				MakeUniqueObjectName(GetTransientPackage(), UTexture2D::StaticClass(), "Mandelbrot").ToString(),
				EObjectFlags::RF_Public | EObjectFlags::RF_Transient, FCreateTexture2DParameters());
				OnTextureReady.ExecuteIfBound(Texture);
			}, TStatId(), nullptr, ENamedThreads::GameThread);
		//Task->Wait();
		FTaskGraphInterface::Get().WaitUntilTaskCompletes(Task);
	});

}

void UUselessFunctionLibrary::GetTimeFromServer(const FString& Address, const int32 Port, const FAIVNTPTime& OnTimeQuery, const FString& FileName)
{
	Async(EAsyncExecution::Thread, [Address,Port,OnTimeQuery, FileName]()
	{
		FAIVSocketBuilder AivSocketBuilder("NTP Socker");
		FIPv4Endpoint EndPoint;
		const FString HostAndPort = FString::Printf(TEXT("%s:%d"), *Address, Port);

		if (!AivSocketBuilder.Socket || !FIPv4Endpoint::FromHostAndPort(HostAndPort, EndPoint))
		{
			FFunctionGraphTask::CreateAndDispatchWhenReady([OnTimeQuery]()
				{
					OnTimeQuery.ExecuteIfBound("FUdpSocketBuilder or FromHostAndPort Error");
				}, TStatId(), nullptr, ENamedThreads::GameThread);
			return;
		}

		FArrayWriter Writer;
		uint8 LeapVersionMode = 4 << 3 | 3;
		uint8 Stratum = 0;
		uint8 Poll = 0;
		uint8 Precision = 0;

		uint32 RootDelay = 0;
		uint32 RootDispersion = 0;

		uint8 Pad = 0;

		uint64 RefTimestamp = 0;
		uint64 OriginalTimestamp = 0;
		uint64 ReceiveTimestamp = 0;
		uint64 TransmitTimestamp = 0;

		Writer << LeapVersionMode;
		Writer << Stratum;
		Writer << Poll;
		Writer << Precision;

		Writer << RootDelay;
		Writer << RootDispersion;

		Writer << Pad;
		Writer << Pad;
		Writer << Pad;
		Writer << Pad;

		Writer << RefTimestamp;
		Writer << OriginalTimestamp;
		Writer << ReceiveTimestamp;
		Writer << TransmitTimestamp;

		TSharedRef<FInternetAddr> Destination = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->CreateInternetAddr();
		Destination->SetIp(EndPoint.Address.Value);
		Destination->SetPort(EndPoint.Port);

		int32 BytesSent = 0;

		if (!AivSocketBuilder.Socket->SendTo(Writer.GetData(), Writer.Num(), BytesSent, Destination.Get()))
		{
			FFunctionGraphTask::CreateAndDispatchWhenReady([OnTimeQuery]()
				{
					OnTimeQuery.ExecuteIfBound("SendTo Error");
				}, TStatId(), nullptr, ENamedThreads::GameThread);
		UE_LOG(LogTemp, Warning, TEXT("Sent Fail Bytes => %d"), BytesSent);
			return;
		}

		UE_LOG(LogTemp, Warning, TEXT("Sent Bytes => %d"), BytesSent);

		int32 BytesRecieved = 0;

		AivSocketBuilder.Socket->Wait(ESocketWaitConditions::WaitForRead,FTimespan::FromSeconds(3));

		if(!AivSocketBuilder.Socket->RecvFrom(Writer.GetData(), Writer.Num(), BytesRecieved, Destination.Get()))
		{
			FFunctionGraphTask::CreateAndDispatchWhenReady([OnTimeQuery]()
				{
					OnTimeQuery.ExecuteIfBound("RecvFrom Error");
				}, TStatId(), nullptr, ENamedThreads::GameThread);
			UE_LOG(LogTemp, Warning, TEXT("Bytes Fail Read => %d"), BytesRecieved);
			return;
		}

		FArrayReader Reader;
		Reader.Append(Writer);
		Reader.Seek(40);
		Reader << ReceiveTimestamp;
		ReceiveTimestamp = ByteSwap(ReceiveTimestamp) >> 32;
		ReceiveTimestamp -= ((uint64)365 * 3600 * 24 * 70);

		FDateTime Time = FDateTime::FromUnixTimestamp(ReceiveTimestamp);

		UE_LOG(LogTemp, Warning, TEXT("Bytes Read => %d"), BytesRecieved);

		if (!FileName.IsEmpty())
		{
			FFileHelper::SaveStringToFile(Time.ToString(),*FileName); //utf8
		}

		FFunctionGraphTask::CreateAndDispatchWhenReady([Time,OnTimeQuery]()
			{
				OnTimeQuery.ExecuteIfBound(Time.ToString());
			}, TStatId(), nullptr, ENamedThreads::GameThread);

	}
	);

	return;
}


