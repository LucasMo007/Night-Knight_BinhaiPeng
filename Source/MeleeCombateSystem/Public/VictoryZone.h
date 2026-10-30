#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VictoryZone.generated.h"

class UBoxComponent;

// Put this at the church; entering it with enough souls wins the game
UCLASS()
class MELEECOMBATESYSTEM_API AVictoryZone : public AActor
{
	GENERATED_BODY()

public:
	AVictoryZone();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Victory")
	TObjectPtr<UBoxComponent> Trigger;

private:
	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
