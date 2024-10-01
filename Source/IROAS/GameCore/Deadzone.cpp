

#include "Deadzone.h"
#include <Components/BoxComponent.h>
#include "public/OnDamage_Interface.h"

ADeadzone::ADeadzone()
{
	PrimaryActorTick.bCanEverTick = true;

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	SetRootComponent(boxComp);

	boxComp->SetCollisionProfileName("PlayerAttack");
	boxComp->SetCanEverAffectNavigation(false);
}

void ADeadzone::BeginPlay()
{
	Super::BeginPlay();
	
}

void ADeadzone::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	boxComp->OnComponentBeginOverlap.AddDynamic(this, &ADeadzone::OnComponentBeginOverlap);

}

void ADeadzone::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IOnDamage_Interface* OnDamage = Cast<IOnDamage_Interface>(OtherActor);
	if (OnDamage != nullptr)
	{
		OnDamage->OnDamage_Interface(9999.f);

	}
}


