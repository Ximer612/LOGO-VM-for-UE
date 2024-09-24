// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "LogoVMBlueprintFunctionLibrary.h"

/*
fd (Forward)
Example: fd 10 will move the turtle forward ten steps.

bk (Backward)
Example: bk 5 will move the turtle backwards five steps.

rt (Right turn)
Example: rt 90 will turn the turtle 90o to the right.

lt (Left turn)
Example: lt 45 will turn the turtle 45o to the left.

ct (Center turtle)
cs (Clear screen)

pu (Pen up)
The turtle can move without drawing a line.

pd (Pen down)

pc (Pen colour)
Example: pc 2
0 = black, 1 = blue, 2 = red etc
*/

class FLogoVM
{
public:
	FLogoVM();
	~FLogoVM();

	FLogoVM(FLogoVM&& Other) = delete;
	FLogoVM(const FLogoVM& Other) = delete;
	FLogoVM& operator=(FLogoVM& Other) = delete;

	void SetStageSize(const FIntVector2& InStageSize);

	bool Execute(const TArray<FString>& Tokens);

	void SetPenUp(const bool bInIsPenDown)
	{
		bIsPenUp = bInIsPenDown;
	}

	void Move(const int32& MoveAmount);

	//void SetRotation(const float RotationAmount);

	void AddRotation(const int RotationAmount);
	
	const TArray<FColor>& GetStage() const
	{
		return StageCells;
	}

	const FIntVector2& GetStageSize() const
	{
		return StageSize;
	}

	void SetCurrentColor(const int32 ColorIndex)
	{
		if (Colors.IsValidIndex(ColorIndex))
		{
			CurrentColor = Colors[ColorIndex];
		}

	}

	void SetTurtlePosition(const FVector2D InPosition)
	{
		Position = InPosition;
	}

	const FVector2D& GetStartPosition() const
	{
		return StartPosition;
	}

protected:
	FVector2D StartPosition;
	FVector2D Position;
	FVector2D Direction;
	int32 Rotation;
	bool bIsPenUp;
	FIntVector2 StageSize;
	TArray<FColor> StageCells;
	FColor CurrentColor;
	TMap<FString, TFunction<bool(FLogoVM&, TQueue<FString>&)>> Commands;
	TArray<FColor> Colors;
};

class FLogoModule : public IModuleInterface, public FSelfRegisteringExec
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

	virtual bool Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar) override;
};
