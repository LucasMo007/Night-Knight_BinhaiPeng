#include "MeleeGameMode.h"

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
		return;
	}

	// No bonfire touched yet: use the PlayerStart
	Super::RestartPlayer(NewPlayer);
}
