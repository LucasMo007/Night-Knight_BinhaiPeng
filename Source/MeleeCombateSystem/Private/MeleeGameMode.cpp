#include "MeleeGameMode.h"
#include "SoulDrop.h"
#include "Blueprint/UserWidget.h"
#include "EngineUtils.h"
#include "Engine/BlueprintGeneratedClass.h"
#include "GameFramework/Controller.h"
#include "GameFramework/Pawn.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/KismetSystemLibrary.h"

AMeleeGameMode::AMeleeGameMode()
{
	SoulDropClass = ASoulDrop::StaticClass();
}

namespace
{
	// Only the variables made in the enemy Blueprint that can be edited per instance (eye icon)
	bool IsInstanceSetting(const FProperty* Prop)
	{
		return Prop->HasAnyPropertyFlags(CPF_Edit)
			&& !Prop->HasAnyPropertyFlags(CPF_DisableEditOnInstance | CPF_EditConst | CPF_Transient
				| CPF_InstancedReference | CPF_ContainsInstancedReference)
			&& Cast<UBlueprintGeneratedClass>(Prop->GetOwnerClass()) != nullptr;
	}
}

void AMeleeGameMode::BeginPlay()
{
	Super::BeginPlay();

	if (!EnemyClass)
	{
		return;
	}

	for (TActorIterator<APawn> It(GetWorld(), EnemyClass); It; ++It)
	{
		APawn* Enemy = *It;

		FEnemySpawnRecord Record;
		Record.Class = Enemy->GetClass();
		Record.Transform = Enemy->GetActorTransform();

		for (TFieldIterator<FProperty> PropIt(Record.Class); PropIt; ++PropIt)
		{
			if (IsInstanceSetting(*PropIt))
			{
				FString Value;
				PropIt->ExportTextItem_InContainer(Value, Enemy, nullptr, Enemy, PPF_None);
				Record.Settings.Add(PropIt->GetFName(), Value);
			}
		}

		EnemyRecords.Add(MoveTemp(Record));
	}
}

void AMeleeGameMode::ResetEnemies()
{
	if (!EnemyClass)
	{
		return;
	}

	TArray<APawn*> Existing;
	for (TActorIterator<APawn> It(GetWorld(), EnemyClass); It; ++It)
	{
		Existing.Add(*It);
	}
	for (APawn* Enemy : Existing)
	{
		DestroyEnemy(Enemy);
	}

	for (const FEnemySpawnRecord& Record : EnemyRecords)
	{
		APawn* Enemy = GetWorld()->SpawnActorDeferred<APawn>(Record.Class, Record.Transform, nullptr, nullptr,
			ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn);
		if (!Enemy)
		{
			continue;
		}

		// Give the new enemy the same patrol points and settings as the one placed in the level
		for (const TPair<FName, FString>& Setting : Record.Settings)
		{
			if (FProperty* Prop = Record.Class->FindPropertyByName(Setting.Key))
			{
				Prop->ImportText_InContainer(*Setting.Value, Enemy, Enemy, PPF_None);
			}
		}

		Enemy->AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
		Enemy->FinishSpawning(Record.Transform);

		if (!Enemy->GetController())
		{
			Enemy->SpawnDefaultController();
		}
	}
}

void AMeleeGameMode::DestroyEnemy(APawn* Enemy)
{
	// Weapons the enemy spawned at runtime (its sword), attached or dropped
	TArray<AActor*> Extras;
	Enemy->GetAttachedActors(Extras, true, true);
	for (TFieldIterator<FObjectProperty> PropIt(Enemy->GetClass()); PropIt; ++PropIt)
	{
		AActor* Actor = Cast<AActor>(PropIt->GetObjectPropertyValue_InContainer(Enemy));
		if (Actor && !Actor->IsA<APawn>() && !Actor->IsA<AController>() && !Actor->IsNetStartupActor())
		{
			Extras.AddUnique(Actor);
		}
	}
	for (AActor* Extra : Extras)
	{
		if (IsValid(Extra))
		{
			Extra->Destroy();
		}
	}

	AController* Controller = Enemy->GetController();
	Enemy->Destroy();
	if (Controller && !Controller->IsPlayerController())
	{
		Controller->Destroy();
	}
}

void AMeleeGameMode::SetCheckpoint(const FTransform& NewTransform)
{
	CheckpointTransform = NewTransform;
	bHasCheckpoint = true;
}

void AMeleeGameMode::RestartPlayer(AController* NewPlayer)
{
	if (bHasCheckpoint && NewPlayer)
	{
		RestartPlayerAtTransform(NewPlayer, CheckpointTransform);

		// Like resting: dying sends every enemy back to its post
		ResetEnemies();
	}
	else
	{
		// No bonfire touched yet: use the PlayerStart
		Super::RestartPlayer(NewPlayer);
	}

	// Find out when this body dies so the souls can drop there
	if (NewPlayer && NewPlayer->GetPawn())
	{
		NewPlayer->GetPawn()->OnDestroyed.AddUniqueDynamic(this, &AMeleeGameMode::OnPlayerPawnDestroyed);
	}
}

void AMeleeGameMode::OnPlayerPawnDestroyed(AActor* DestroyedActor)
{
	UWorld* World = GetWorld();
	if (!World || World->bIsTearingDown || bMatchOver)
	{
		return;
	}

	// Died again before getting them back: those souls are gone for good
	if (ActiveSoulDrop.IsValid())
	{
		ActiveSoulDrop->Destroy();
	}
	ActiveSoulDrop = nullptr;

	if (SoulCount <= 0 || !SoulDropClass)
	{
		return;
	}

	FActorSpawnParameters Params;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	if (ASoulDrop* Drop = World->SpawnActor<ASoulDrop>(SoulDropClass, DestroyedActor->GetActorLocation(), FRotator::ZeroRotator, Params))
	{
		Drop->SetSouls(SoulCount);
		ActiveSoulDrop = Drop;
	}
	SoulCount = 0;
}

void AMeleeGameMode::OnSoulDropCollected(ASoulDrop* Drop)
{
	AddSouls(Drop->GetSouls());
	if (ActiveSoulDrop.Get() == Drop)
	{
		ActiveSoulDrop = nullptr;
	}
}

void AMeleeGameMode::AddSouls(int32 Amount)
{
	SoulCount = FMath::Max(0, SoulCount + Amount);
}

bool AMeleeGameMode::TryWin(APawn* Player)
{
	if (bMatchOver || !Player)
	{
		return false;
	}

	if (SoulCount < SoulsToWin)
	{
		OnNotEnoughSouls(SoulCount, SoulsToWin);
		return false;
	}

	bMatchOver = true;

	if (APlayerController* PC = Cast<APlayerController>(Player->GetController()))
	{
		if (VictoryWidgetClass)
		{
			if (UUserWidget* Widget = CreateWidget<UUserWidget>(PC, VictoryWidgetClass))
			{
				Widget->AddToViewport();
			}
		}
		Player->DisableInput(PC);
	}

	OnVictory();
	return true;
}

void AMeleeGameMode::OnNotEnoughSouls_Implementation(int32 Current, int32 Needed)
{
	const FString Message = FString::Printf(TEXT("Need %d souls to enter the church (%d / %d)"), Needed, Current, Needed);
	UKismetSystemLibrary::PrintString(this, Message, true, false, FLinearColor::Red, 3.f);
}
