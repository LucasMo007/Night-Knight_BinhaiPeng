#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SoulDrop.generated.h"

class USphereComponent;
class UStaticMeshComponent;
class UPointLightComponent;

// The souls left where the player died; walk into it to get them back
UCLASS()
class MELEECOMBATESYSTEM_API ASoulDrop : public AActor
{
	GENERATED_BODY()

public:
	ASoulDrop();

	void SetSouls(int32 Amount) { Souls = Amount; }

	UFUNCTION(BlueprintPure, Category = "Souls")
	int32 GetSouls() const { return Souls; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Souls")
	TObjectPtr<USphereComponent> Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Souls")
	TObjectPtr<UStaticMeshComponent> Orb;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Souls")
	TObjectPtr<UPointLightComponent> Glow;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Souls")
	int32 Souls = 0;

private:
	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
