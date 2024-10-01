
#include "RangedEnemyProjectile.h"
#include <Components/SphereComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include "RangedGroundEnemy.h"
#include "OnDamage_Interface.h"
#include "BattlePlayer.h"

ARangedEnemyProjectile::ARangedEnemyProjectile()
{
	//PrimaryActorTick.bCanEverTick = true;

	magicCollision = CreateDefaultSubobject<USphereComponent>(TEXT("magicCollision"));
	SetRootComponent(magicCollision);
	magicFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("MagicEffect"));
	magicFX->SetupAttachment(RootComponent);
	/*magicFX->SetVisibility(false);
	magicFX->bAutoActivate = false;*/

	// 상속 받은 속성 세팅
	lifeSpan = magicLifeSpan;
	speed = magicSpeed;
}

void ARangedEnemyProjectile::BeginPlay()
{
	Super::BeginPlay();

	magicCollision->OnComponentBeginOverlap.AddDynamic(this, &ARangedEnemyProjectile::OnComponentBeginOverlap);
	//UE_LOG(LogTemp, Warning, TEXT("no"));

	if (me != nullptr)
	{
		attackDamage = me->attackPower;
	}
	
}

void ARangedEnemyProjectile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	/*FVector P0 = GetActorLocation();
	FVector dir = me->target->GetActorLocation() - GetActorLocation();
	dir.Normalize();
	FVector vt = dir * speed * DeltaTime;
	FVector P = P0 + vt;
	SetActorLocation(P);*/

	/*GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("%.2f"), DeltaTime));
	UE_LOG(LogTemp, Warning, TEXT("tickkkkkkkkkkkkkkkk"));*/
}


void ARangedEnemyProjectile::SetActive(bool isActive)
{
	if (magicFX)
		magicFX->SetVisibility(isActive);


	if (!isActive){
		me->magicBallPool.Add(this);
		currentTime = lifeSpan;
		magicCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		magicCollision->SetRelativeLocation(FVector(0,-1000.f,-1000.f));
	}
	else{
		bIsActive = true;
		magicCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		if (me != nullptr)
		{
			attackDamage = me->attackPower;
		}
		if (!me || !me->target)
		{
			return;
		}

		FVector TargetLocation = me->target->GetActorLocation();
		FVector MeLocation = me->GetActorLocation();

		FVector LookDirection = (TargetLocation - MeLocation);

		if (LookDirection.IsNearlyZero())
		{
			// me와 target의 위치가 거의 같으면
			return;
		}

		LookDirection.Normalize();

		FRotator LookAtRotation = LookDirection.Rotation();
		LookAtRotation.Pitch = 0.0f;
		LookAtRotation.Roll = 0.0f;
		SetActorRotation(LookAtRotation);
	}
}

void ARangedEnemyProjectile::GetOwningEnemy(class ARangedGroundEnemy* enemy)
{
	me = enemy;
}

void ARangedEnemyProjectile::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	ABattlePlayer* player = Cast<ABattlePlayer>(OtherActor);

	if (me->target == player) {
		me->target->HitbyRangedSound();
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("ranged Hit")));
	}
}
