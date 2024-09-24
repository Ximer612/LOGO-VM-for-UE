// Copyright Epic Games, Inc. All Rights Reserved.

#include "Logo.h"
#include "Misc/FileHelper.h"
#include "AssetRegistry/AssetRegistryModule.h"
#if WITH_EDITOR
#include "Editor.h"
#endif
#include "Misc/OutputDeviceNull.h"
#include "ImageUtils.h"

#define LOCTEXT_NAMESPACE "FLogoModule"

FLogoVM::FLogoVM() : Position(0, 0), Direction(0, 1), Rotation(90), bIsPenUp(false), StageSize(20, 20)
{
	SetStageSize(StageSize);
	CurrentColor = FColor::Black;

	Colors.Add(FColor::Black);
	Colors.Add(FColor::White);
	Colors.Add(FColor::Red);
	Colors.Add(FColor::Blue);
	Colors.Add(FColor::Green);
	Colors.Add(FColor::Yellow);
	Colors.Add(FColor::Cyan);
	Colors.Add(FColor::Magenta);
	Colors.Add(FColor::Orange);

	Commands.Add("fd", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {

		if (TokensQueue.IsEmpty())
		{
			return false;
		}

		FString MyArg;
		TokensQueue.Dequeue(MyArg);

		if (!MyArg.IsNumeric())
		{
			return false;
		}

		const int32 MoveDistance = FCString::Atoi(*MyArg);

		LogoVM.Move(MoveDistance);

		return true;

		});

	Commands.Add("bk", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {

		if (TokensQueue.IsEmpty())
		{
			return false;
		}

		FString MyArg;
		TokensQueue.Dequeue(MyArg);

		if (!MyArg.IsNumeric())
		{
			return false;
		}

		const int32 MoveDistance = FCString::Atoi(*MyArg);

		LogoVM.Move(-MoveDistance);

		return true;

		});

	Commands.Add("pd", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {

		LogoVM.SetPenUp(false);

		return true;

		});

	Commands.Add("pu", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {

		LogoVM.SetPenUp(true);

		return true;

		});

	Commands.Add("pc", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {

		if (TokensQueue.IsEmpty())
		{
			return false;
		}

		FString MyArg;
		TokensQueue.Dequeue(MyArg);

		if (!MyArg.IsNumeric())
		{
			return false;
		}

		const int32 ColorIndex = FCString::Atoi(*MyArg);

		LogoVM.SetCurrentColor(ColorIndex);

		return true;

		});

	Commands.Add("cs", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {

		LogoVM.SetStageSize(LogoVM.GetStageSize());

		return true;

		});

	Commands.Add("ct", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {

		LogoVM.SetTurtlePosition(LogoVM.GetStartPosition());

		return true;

		});

	Commands.Add("rt", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {

		if (TokensQueue.IsEmpty())
		{
			return false;
		}

		FString MyArg;
		TokensQueue.Dequeue(MyArg);

		if (!MyArg.IsNumeric())
		{
			return false;
		}

		const int32 RotationAmount = FCString::Atoi(*MyArg);

		LogoVM.AddRotation(RotationAmount);

		return true;

		});

	Commands.Add("lt", [](FLogoVM& LogoVM, TQueue<FString>& TokensQueue) -> bool {

		if (TokensQueue.IsEmpty())
		{
			return false;
		}

		FString MyArg;
		TokensQueue.Dequeue(MyArg);

		if (!MyArg.IsNumeric())
		{
			return false;
		}

		const int32 RotationAmount = FCString::Atoi(*MyArg);

		LogoVM.AddRotation(-RotationAmount);

		return true;

		});
}

FLogoVM::~FLogoVM()
{
}

void FLogoVM::SetStageSize(const FIntVector2& InStageSize)
{
	StageCells.Empty();
	StageSize = InStageSize;
	StageCells.AddUninitialized(InStageSize.X * InStageSize.Y);

	for (int32 Index = 0; Index < StageCells.Num(); Index++)
	{
		StageCells[Index] = FColor::White;
	}

	StartPosition = FVector2D(StageSize.X * 0.5f, StageSize.Y * 0.5f);
	SetTurtlePosition(StartPosition);
}

bool FLogoVM::Execute(const TArray<FString>& Tokens)
{
	TQueue<FString> TokensQueue;

	for (const FString& Token : Tokens)
	{
		TokensQueue.Enqueue(Token);
	}

	while (!TokensQueue.IsEmpty())
	{
		FString PoppedToken;
		TokensQueue.Dequeue(PoppedToken);

		if (Commands.Contains(PoppedToken))
		{
			if (!Commands[PoppedToken](*this, TokensQueue))
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}

	return true;
}

void FLogoVM::Move(const int32& MoveAmount)
{
	if (!bIsPenUp)
	{
		FIntVector2 MyPos = FIntVector2(Position.X, Position.Y);

		for (int32 Step = 0; Step < FMath::Abs(MoveAmount); ++Step)
		{
			const int32 Index = Position.X + Position.Y * StageSize.X;
			if (StageCells.IsValidIndex(Index))
			{
				StageCells[Index] = CurrentColor;
			}
			Position += Direction * FMath::Sign(MoveAmount);
			Position = Position.RoundToVector();

			//UE_LOG(LogTemp, Error, TEXT("Turtle pos: %s"), *Position.ToString());
		}
	}
	else
	{
		Position += Direction * MoveAmount;
		Position = Position.RoundToVector();
	}	
}


void FLogoVM::AddRotation(const int RotationAmount)
{
	Rotation = (Rotation + RotationAmount) % 360;

	UE_LOG(LogTemp, Warning, TEXT("My new Rotation: %d, RotationAmount %d"), Rotation, RotationAmount);

	float RadiantRotation = FMath::DegreesToRadians(Rotation);

	Direction.X = FMath::Cos(RadiantRotation);
	Direction.Y = FMath::Sin(RadiantRotation);
}

void FLogoModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void FLogoModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.
}

bool FLogoModule::Exec(UWorld* InWorld, const TCHAR* Cmd, FOutputDevice& Ar)
{
	if (FParse::Command(&Cmd, TEXT("Logo")))
	{
		FString InputFileName = FParse::Token(Cmd, false);
		if (!InputFileName.IsEmpty())
		{
			UE_LOG(LogTemp, Error, TEXT("FileName= %s"), *InputFileName);

			TArray<FColor> Result = ULogoVMBlueprintFunctionLibrary::LogoExecuteSimpleFromFile(InputFileName, 16, 16);

			UWorld* EditorWorld = GEditor->GetEditorWorldContext().World();

			UBlueprint* CubeObject = LoadObject<UBlueprint>(nullptr, TEXT("/Logo/BP_CubeStage.BP_CubeStage"));
			UClass* CubeClass = CubeObject->GeneratedClass;
			UE_LOG(LogTemp, Error, TEXT("CubeObject ptr = %p"), CubeObject);


			if (CubeClass->IsChildOf<AActor>())
			{
				UE_LOG(LogTemp, Error, TEXT("Obj = %p"), CubeObject);

				for (int32 Index = 0; Index < Result.Num(); Index++)
				{
					FTransform Transform;
					Transform.SetLocation(FVector((Index % 16) * 100, (Index / 16) * 100, 1));
					AActor* Cube = EditorWorld->SpawnActor<AActor>(CubeClass,Transform);
					FOutputDeviceNull OutputDeviceNull;

					const FString Arg = FString::Printf(TEXT("SetColor %s"), *(Result[Index].ToString()));
					Cube->CallFunctionByNameWithArguments(*Arg, OutputDeviceNull, nullptr,true);

					//if (Result[Index].R == 0)
					//{
					//	FString Arg = FString::Printf(TEXT("SetColor(R=%u,G=%u,B=%u,A=%u)"), Result[Index].R, Result[Index].G, Result[Index].B);
					//	Cube->CallFunctionByNameWithArguments(*Arg, OutputDeviceNull, nullptr, true);
					//}

					//UE_LOG(LogTemp, Error, TEXT("Color = %s"), *Color.ToString());
				}
			}

			UPackage* NewPackage = CreatePackage(TEXT("/Game/Logo/Textures/LogoPackage"));
			FString OutputFileName = FPackageName::LongPackageNameToFilename(NewPackage->GetPathName(), FPackageName::GetAssetPackageExtension());
			FCreateTexture2DParameters Params;
			UTexture2D* MyTexture = FImageUtils::CreateTexture2D(16, 16, Result, NewPackage, TEXT("LOGO Package"), RF_Public | RF_Standalone, Params);
			UPackage::SavePackage(NewPackage, MyTexture, RF_Public | RF_Standalone, *OutputFileName);
			FAssetRegistryModule::AssetCreated(MyTexture);

			return true;
		}
	}

	return false;
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FLogoModule, Logo)
