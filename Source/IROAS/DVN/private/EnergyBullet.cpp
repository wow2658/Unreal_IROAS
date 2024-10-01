

#include "EnergyBullet.h"
#include <Components/SphereComponent.h>
#include <Components/StaticMeshComponent.h>
#include "BaseEnemy.h"
#include "Gun.h"
#include "Particles/ParticleSystemComponent.h"
#include "Particles/ParticleSystem.h"
#include <Kismet/GameplayStatics.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include "Boss.h"

AEnergyBullet::AEnergyBullet()
{
	PrimaryActorTick.bCanEverTick = true;
	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	sphereComp->SetCollisionProfileName(FName("PlayerAttack"));
	SetRootComponent(sphereComp);

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	meshComp->SetCollisionProfileName(FName("NoCollision"));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetHiddenInGame(true);

	bulletTrail = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("BulletTrail"));
	bulletTrail->SetupAttachment(RootComponent);
	ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystemAsset(TEXT("/Script/Engine.ParticleSystem'/Game/Assets/ParagonRevenant/FX/Particles/Revenant/Abilities/Primary/FX/P_Revenant_Primary_Trail_LastShot.P_Revenant_Primary_Trail_LastShot'"));
	if (ParticleSystemAsset.Succeeded())
	{
		bulletTrail->SetTemplate(ParticleSystemAsset.Object);
	}

	ConstructorHelpers::FObjectFinder<UParticleSystem> hitEffectAsset(TEXT("/Script/Engine.ParticleSystem'/Game/Assets/ParagonRevenant/FX/Particles/Revenant/Abilities/Primary/FX/P_Revenant_Primary_HitCharacter.P_Revenant_Primary_HitCharacter'"));
	if (hitEffectAsset.Succeeded()) {
		enemyHitFX1 = hitEffectAsset.Object;
	}

	lifeSpan = bulletLifeSpan;
	//speed = bulletSpeed;
}

void AEnergyBullet::BeginPlay()
{
	Super::BeginPlay();
	sphereComp->OnComponentBeginOverlap.AddDynamic(this, &AEnergyBullet::OnComponentBeginOverlap);

	attackDamage = bulletDamage;
	currentTime = lifeSpan;
	//bIsActive = false;
	//SetActorEnableCollision(false);
	
	if (!bIsDebugging) {
		sphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
	
	bulletTrail->SetAutoActivate(true);
	//bulletTrail->SetAutoActivate(bIsDebugging);

	SetActorHiddenInGame(!bIsDebugging);
	SetActorTickEnabled(!bIsDebugging);
	Super::SetActorTickEnabled(!bIsDebugging);
	//SetActive(false);
}

void AEnergyBullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (!bIsActive && firingGun != nullptr) {
		SetActorLocation(firingGun->firePos->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	}

}



void AEnergyBullet::SetActive(bool isActive)
{
	if(bIsDebugging) return;
	
	bulletTrail->SetVisibility(isActive);

	if(isActive)	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, FString::Printf(TEXT("BulletCollision: %s\nCollisionEnabled: %s"), *sphereComp->GetCollisionProfileName().ToString(), sphereComp->GetCollisionEnabled() == ECollisionEnabled::NoCollision ? *FString::Printf(TEXT("FALSE")) : *FString::Printf(TEXT("TRUE"))));
		sphereComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);

	}
	else {
		sphereComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		currentTime = lifeSpan;
		//SetActorLocation(FVector(0.f, 0.f, -5000.f), false, nullptr, ETeleportType::TeleportPhysics);
		if(firingGun != nullptr)firingGun->bulletPool.Enqueue(this);
	}
	bIsActive = isActive;
	SetActorHiddenInGame(!isActive);
	
}

void AEnergyBullet::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("hit")));
	
	ABaseEnemy* enemy = Cast<ABaseEnemy>(OtherActor);


	if (enemy != nullptr) {
		if (enemyHitFX1 != nullptr) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), enemyHitFX1, OtherComp->GetComponentLocation());
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("hit effect hit")));
		}
		if (enemyHitFX2 != nullptr) {
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), enemyHitFX2, OtherComp->GetComponentLocation());
		}
		if (enemyHitSound != nullptr)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), enemyHitSound, SweepResult.ImpactPoint);
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("%f \n %f \n %f"), SweepResult.ImpactPoint.X, SweepResult.ImpactPoint.Y, SweepResult.ImpactPoint.Z));
		
		// ³ª¹«
		if (OtherActor->ActorHasTag("Tree")) {
			if (woodHitFX != nullptr) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), woodHitFX, OverlappedComponent->GetComponentLocation());
				//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("treee")));
			}
			if (woodHitSound != nullptr)
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), woodHitSound, OverlappedComponent->GetComponentLocation());
			
			return;
		}

		// ¹ÙÀ§. ¼®»ó
		else if (OtherActor->ActorHasTag("Rock") || OtherActor->GetName().Contains("Spawner")) {
			if (rockHitFX != nullptr)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), rockHitFX, OverlappedComponent->GetComponentLocation());
			if (rockHitSound != nullptr)
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), rockHitSound, OverlappedComponent->GetComponentLocation());
			return;
		}

		// ¶¥, Èë
		else if (OtherActor->ActorHasTag("Ground")) {
			if (groundHitFX != nullptr)
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), groundHitFX, OverlappedComponent->GetComponentLocation());
			if (groundHitSound != nullptr)
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), groundHitSound, OverlappedComponent->GetComponentLocation());
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("ground")));
		}

		// º¸½º
		else {
			ABoss* boss = Cast<ABoss>(OtherActor);
			if (boss == nullptr) return;

			if (enemyHitFX1 != nullptr) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), enemyHitFX1, SweepResult.ImpactPoint);
				//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("hit effect hit")));
			}
			if (enemyHitFX2 != nullptr) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), enemyHitFX2, SweepResult.ImpactPoint);
			}
			if (enemyHitSound != nullptr)
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), enemyHitSound, SweepResult.ImpactPoint);
		}
	}
}
