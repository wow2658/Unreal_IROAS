

#include "SlashProjectile.h"
#include <Components/StaticMeshComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "Sword.h"
#include "OnDamage_Interface.h"
#include "Kismet/GameplayStatics.h"
#include "NiagaraFunctionLibrary.h"

ASlashProjectile::ASlashProjectile()
{
	collisionMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CollisionMesh"));
	SetRootComponent(collisionMesh);
	collisionMesh->SetCollisionProfileName(FName("PlayerAttack"));
	collisionMesh->bMultiBodyOverlap = true;

	slashVFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SlashVFX"));
	slashVFX->SetupAttachment(RootComponent);
	slashVFX->SetRelativeLocation(FVector(-1.f, 0.f,-149.f));
	slashVFX->SetAutoActivate(false);
	
	attackDamage = slashDamage;
	lifeSpan = slashLifeSpan;
}

void ASlashProjectile::BeginPlay()
{
	Super::BeginPlay();
	collisionMesh->OnComponentBeginOverlap.AddDynamic(this, &ASlashProjectile::OnEnemyHit);

	currentTime = lifeSpan;
	bIsActive = false;
	SetActorEnableCollision(false);
	SetActorHiddenInGame(false);

}

void ASlashProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ASlashProjectile::SetActive(bool isActive)
{
	slashVFX->SetActive(isActive);
	if (isActive) {
		//collisionMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		//collisionMesh->SetCollisionProfileName(FName("PlayerAttack"));
	}
	else {
		currentTime = lifeSpan;
		SetActorLocation(FVector(0.f, 0.f, -5000.f), false, nullptr, ETeleportType::TeleportPhysics);
		if (owningSword != nullptr) owningSword->slashPool.Enqueue(this);
	}
	bIsActive = isActive;
	SetActorEnableCollision(isActive);
	SetActorHiddenInGame(!isActive);
}

void ASlashProjectile::OnEnemyHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IOnDamage_Interface* OnDamage = Cast<IOnDamage_Interface>(OtherActor);
	if (OnDamage != nullptr)
	{
		if (hitEffect != nullptr) 
		{
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(),	hitEffect, SweepResult.ImpactPoint);
		}

		if (OtherActor->ActorHasTag(FName("Boss")))
		{
			OnDamage->OnDamage_Interface(attackDamage*0.25); //bossslashdamage
		}
		else
		{
			OnDamage->OnDamage_Interface(attackDamage);
		}
		
	}
}
