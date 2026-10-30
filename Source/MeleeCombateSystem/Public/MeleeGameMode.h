#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MeleeGameMode.generated.h"

// Base GameMode: remembers the last bonfire and respawns the player there
UCLASS()
class MELEECOMBATESYSTEM_API AMeleeGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable, Category = "Checkpoint")
	void SetCheckpoint(const FTransform& NewTransform);

	UFUNCTION(BlueprintPure, Category = "Checkpoint")
	bool HasCheckpoint() const { return bHasCheckpoint; }

	virtual void RestartPlayer(AController* NewPlayer) override;

protected:
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Checkpoint")
	FTransform CheckpointTransform;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Checkpoint")
	bool bHasCheckpoint = false;
};
