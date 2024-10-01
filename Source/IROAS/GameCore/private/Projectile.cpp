

#include "Projectile.h"
#include "Components/BoxComponent.h"
#include "OnDamage_Interface.h"
#include "Sword.h"
#include "Components/SkeletalMeshComponent.h"


AProjectile::AProjectile()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AProjectile::BeginPlay()
{
	Super::BeginPlay();
}

void AProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bIsActive && currentTime > 0.0f) {
		FVector P0 = GetActorLocation();
		FVector vt = GetActorForwardVector() * speed* DeltaTime;
		FVector P = P0 + vt;
		SetActorLocation(P, true);
		currentTime -= DeltaTime;
		if (currentTime <= 0.0f) {
			bIsActive = false;
			SetActive(false);
		}
	}
}


void AProjectile::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (Cast<ASword>(OtherComp->GetOwner()) != nullptr) return;
	IOnDamage_Interface* OnDamage = Cast<IOnDamage_Interface>(OtherActor);
	if (OnDamage != nullptr)
	{

		//if (Cast<ASword>(OtherComp->GetOwner()) != nullptr) return;
		OnDamage->OnDamage_Interface(attackDamage);
		SetActive(false);
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("%s"), *OverlappedComponent->GetName())); // BoxCollision (���� ������?)
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("%s"), *OtherActor->GetActorLabel())); // BP_MeleeEnemy_C_5 (������ ������ �¾ҳ�?)
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("%s"), *OtherComp->GetName())); // head �߿��Ѻκ� (������ ��� ������?)
	

	}
	else SetActive(false);

}


