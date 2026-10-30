#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bonfire.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UPointLightComponent;
class UArrowComponent;

// Walk into it to light it and make it your respawn point
UCLASS()
class MELEECOMBATESYSTEM_API ABonfire : public AActor
{
	GENERATED_BODY()

public:
	ABonfire();

	UFUNCTION(BlueprintPure, Category = "Bonfire")
	bool IsLit() const { return bLit; }

protected:
	virtual void BeginPlay() override;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire")
	TObjectPtr<UStaticMeshComponent> Mesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire")
	TObjectPtr<USphereComponent> Trigger;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire")
	TObjectPtr<UPointLightComponent> FireLight;

	// Where the player respawns; the arrow is the facing direction
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Bonfire")
	TObjectPtr<UArrowComponent> RespawnPoint;

	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "Bonfire")
	bool bLit = false;

	// First time the player reaches this bonfire
	UFUNCTION(BlueprintImplementableEvent, Category = "Bonfire")
	void OnLit(APawn* Player);

	// Every time the player reaches it (heal here in Blueprint)
	UFUNCTION(BlueprintImplementableEvent, Category = "Bonfire")
	void OnRested(APawn* Player);

private:
	UFUNCTION()
	void OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
};
