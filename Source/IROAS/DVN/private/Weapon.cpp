

#include "Weapon.h"
#include <Components/StaticMeshComponent.h>
#include "GrabComp.h"
#include "IROASEnums.h"

AWeapon::AWeapon()
{
	PrimaryActorTick.bCanEverTick = true;

	defaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(defaultSceneRoot);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetupAttachment(RootComponent);

	grabComp = CreateDefaultSubobject<UGrabComp>(TEXT("GrabComp"));
	grabComp->SetupAttachment(meshComp);
	grabComp->SetGrabType(EGrabType::SnapObjToHand);
	Tags.Add(FName("Weapon"));
}

void AWeapon::BeginPlay()
{
	Super::BeginPlay();
	
}

void AWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

