

#include "ItemBase.h"
#include <Components/StaticMeshComponent.h>
#include "public/GrabComp.h"
#include <Components/SphereComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>

AItemBase::AItemBase()
{
	PrimaryActorTick.bCanEverTick = true;

	defaultSceneRoot = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultRoot"));
	SetRootComponent(defaultSceneRoot);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComponent"));
	meshComp->SetupAttachment(RootComponent);

	niagaraComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraComponent"));
	niagaraComp->SetupAttachment(meshComp);

	hoverComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HoverComponent"));
	hoverComp->SetupAttachment(meshComp);
	hoverComp->SetRelativeScale3D(FVector(1.3f));
	hoverComp->SetCollisionProfileName(FName("NoCollision"));
	hoverComp->SetVisibility(false);
	
	detectionComp = CreateDefaultSubobject<USphereComponent>(TEXT("DetectionComponent"));
	detectionComp->SetupAttachment(meshComp);
	detectionComp->SetCollisionObjectType(ECC_WorldDynamic);
	detectionComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	detectionComp->SetCollisionResponseToChannel(ECC_GameTraceChannel8, ECR_Block);

	grabComp = CreateDefaultSubobject<UGrabComp>(TEXT("GrabComponent"));
	grabComp->SetupAttachment(meshComp);

}

void AItemBase::BeginPlay()
{
	Super::BeginPlay();
	
}

void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AItemBase::OnHovered()
{
	hoverComp->SetVisibility(true);
}

void AItemBase::OnUnhovered()
{
	hoverComp->SetVisibility(false);

}


