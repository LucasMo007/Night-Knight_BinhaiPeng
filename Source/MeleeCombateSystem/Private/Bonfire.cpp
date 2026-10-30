#include "Bonfire.h"
#include "MeleeGameMode.h"
#include "Components/ArrowComponent.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "GameFramework/Pawn.h"

ABonfire::ABonfire()
{
	PrimaryActorTick.bCanEverTick = false;

	RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));

	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(RootComponent);

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	Trigger->SetupAttachment(RootComponent);
	Trigger->SetSphereRadius(200.f);
	Trigger->SetCollisionProfileName(TEXT("Trigger"));

	FireLight = CreateDefaultSubobject<UPointLightComponent>(TEXT("FireLight"));
	FireLight->SetupAttachment(RootComponent);
	FireLight->SetRelativeLocation(FVector(0.f, 0.f, 80.f));
	FireLight->SetLightColor(FLinearColor(1.f, 0.45f, 0.1f));
	FireLight->SetIntensity(8000.f);
	FireLight->SetAttenuationRadius(800.f);
	FireLight->SetVisibility(false);

	RespawnPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("RespawnPoint"));
	RespawnPoint->SetupAttachment(RootComponent);
	RespawnPoint->SetRelativeLocation(FVector(150.f, 0.f, 100.f));
}

void ABonfire::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ABonfire::OnTriggerBegin);
}

void ABonfire::OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* Player = Cast<APawn>(OtherActor);
	if (!Player || !Player->IsPlayerControlled())
	{
		return;
	}

	if (!bLit)
	{
		bLit = true;
		FireLight->SetVisibility(true);
		OnLit(Player);
	}

	if (AMeleeGameMode* GameMode = GetWorld()->GetAuthGameMode<AMeleeGameMode>())
	{
		// Keep only yaw so the player spawns standing up
		const FRotator Facing(0.f, RespawnPoint->GetComponentRotation().Yaw, 0.f);
		GameMode->SetCheckpoint(FTransform(Facing, RespawnPoint->GetComponentLocation()));

		// Resting brings every enemy back
		GameMode->ResetEnemies();
	}

	OnRested(Player);
}
