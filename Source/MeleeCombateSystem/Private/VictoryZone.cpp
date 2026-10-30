#include "VictoryZone.h"
#include "MeleeGameMode.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Pawn.h"

AVictoryZone::AVictoryZone()
{
	PrimaryActorTick.bCanEverTick = false;

	Trigger = CreateDefaultSubobject<UBoxComponent>(TEXT("Trigger"));
	RootComponent = Trigger;
	Trigger->SetBoxExtent(FVector(200.f, 200.f, 150.f));
	Trigger->SetCollisionProfileName(TEXT("Trigger"));
	Trigger->ShapeColor = FColor::Yellow;
}

void AVictoryZone::BeginPlay()
{
	Super::BeginPlay();
	Trigger->OnComponentBeginOverlap.AddDynamic(this, &AVictoryZone::OnTriggerBegin);
}

void AVictoryZone::OnTriggerBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APawn* Player = Cast<APawn>(OtherActor);
	if (!Player || !Player->IsPlayerControlled())
	{
		return;
	}

	if (AMeleeGameMode* GameMode = GetWorld()->GetAuthGameMode<AMeleeGameMode>())
	{
		GameMode->TryWin(Player);
	}
}
