// Fill out your copyright notice in the Description page of Project Settings.


#include "UselessFunctionLibrary.h"

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