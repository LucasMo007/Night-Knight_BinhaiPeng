#include "SoulDrop.h"
#include "MeleeGameMode.h"
#include "Components/PointLightComponent.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"

ASoulDrop::ASoulDrop()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<USphereComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->SetSphereRadius(120.f);
	Trigger->SetCollisionProfileName(TEXT("Trigger"));

	// A small glowing ball so the player can find it again
	Orb = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Orb"));
	Orb->SetupAttachment(RootComponent);
	Orb->SetRelativeScale3D(FVector(0.3f));
	Orb->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Orb->SetCastShadow(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		Orb->SetStaticMesh(SphereMesh.Object);
	}

	Glow = CreateDefaultSubobject<UPointLightComponent>(TEXT("Glow"));
	Glow->SetupAttachment(RootComponent);
	Glow->SetLightColor(FLinearColor(0.2f, 0.9f, 1.f));
	Glow->SetIntensity(6000.f);
	Glow->SetAttenuationRadius(500.f);
	Glow->SetCastShadows(false);
}

void ASoulDrop::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &ASoulDrop::OnTriggerBegin);
}

void ASoulDrop::OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* Player = Cast<APawn>(OtherActor);
	if (!Player || !Player->IsPlayerControlled())
	{
		return;
	}

	if (AMeleeGameMode* GameMode = GetWorld()->GetAuthGameMode<AMeleeGameMode>())
	{
		GameMode->OnSoulDropCollected(this);
	}
	Destroy();
}
