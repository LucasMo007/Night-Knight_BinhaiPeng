#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MeleeGameMode.generated.h"

class UUserWidget;
class ASoulDrop;

// Base GameMode: bonfire checkpoints, the souls count and the win condition
UCLASS()
class MELEECOMBATESYSTEM_API AMeleeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMeleeGameMode();

	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SetCheckpoint(const FTransform& NewTransform);

	UFUNCTION(BlueprintPure, Category = "Checkpoint")
	bool HasCheckpoint() const { return bHasCheckpoint; }

	virtual void RestartPlayer(AController* NewPlayer) override;

	// Souls-like rest: every enemy goes back to where it started, alive and at full health
	UFUNCTION(BlueprintCallable, Category = "Enemies")
	void ResetEnemies();

	UFUNCTION(BlueprintCallable, Category = "Souls")
	void AddSouls(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Souls")
	int32 GetSoulCount() const { return SoulCount; }

	// The player walked back into the souls they dropped
	void OnSoulDropCollected(ASoulDrop* Drop);

	// Called by the victory zone; wins only with enough souls
	UFUNCTION(BlueprintCallable, Category = "Match")
	bool TryWin(APawn* Player);

	UFUNCTION(BlueprintPure, Category = "Match")
	bool IsMatchOver() const { return bMatchOver; }

protected:
	virtual void BeginPlay() override;

	// Enemies of this class are recorded at start and respawned by ResetEnemies (set to BP_Skeleton)
	UPROPERTY(EditDefaultsOnly, Category = "Enemies")
	TSubclassOf<APawn> EnemyClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Checkpoint")
	FTransform CheckpointTransform;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Checkpoint")
	bool bHasCheckpoint = false;

	// Lives in the GameMode so it survives the player dying
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Souls")
	int32 SoulCount = 0;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Match")
	int32 SoulsToWin = 800;

	// Spawned where the player dies, holding all their souls (can be a Blueprint child for looks)
	UPROPERTY(EditDefaultsOnly, Category = "Souls")
	TSubclassOf<ASoulDrop> SoulDropClass;

	// Shown when the player wins (set to UI_PlayerVictoryHUD)
	UPROPERTY(EditDefaultsOnly, Category = "Match")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Match")
	bool bMatchOver = false;

	// Player reached the goal without enough souls; override in Blueprint for a nicer message
	UFUNCTION(BlueprintNativeEvent, Category = "Match")
	void OnNotEnoughSouls(int32 Current, int32 Needed);

	UFUNCTION(BlueprintImplementableEvent, Category = "Match")
	void OnVictory();

private:
	// Where an enemy started and the per-instance settings it had (patrol points etc.)
	struct FEnemySpawnRecord
	{
		TSubclassOf<APawn> Class;
		FTransform Transform;
		TMap<FName, FString> Settings;
	};

	TArray<FEnemySpawnRecord> EnemyRecords;

	// Only one drop at a time: dying again before picking it up loses it
	TWeakObjectPtr<ASoulDrop> ActiveSoulDrop;

	UFUNCTION()
	void OnPlayerPawnDestroyed(AActor* DestroyedActor);

	void DestroyEnemy(APawn* Enemy);
};
