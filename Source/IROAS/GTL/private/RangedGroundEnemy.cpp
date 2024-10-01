// Fill out your copyright notice in the Description page of Project Settings.


#include "RangedGroundEnemy.h"
#include <GameFramework/CharacterMovementComponent.h>
#include "RangedEnemyProjectile.h"
#include "RangedEnemyAnim.h"
#include <AIModule/Classes/AIController.h>
#include "GameFramework/FloatingPawnMovement.h"
#include <Components/TextRenderComponent.h>
#include <Components/SphereComponent.h>
#include <Components/CapsuleComponent.h>
#include "Components/BoxComponent.h"
#include "EnemySpawner.h" 
#include "Rock.h"
#include "EnergyBullet.h"
#include "Sword.h"
#include "SlashProjectile.h"
#include "BattlePlayer.h"
#include "Components/SceneComponent.h"

ARangedGroundEnemy::ARangedGroundEnemy()
{
	PrimaryActorTick.bCanEverTick = true;



	// 헤드 Head는 Base에

	spine_01Comp = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_01"));
	spine_01Comp->SetupAttachment(meshComp, FName("spine_01"));
	spine_01Comp->ComponentTags.Add(FName("spine_01"));

	spine_01Comp->SetRelativeLocation(FVector(7.937439f, -0.996895f, 0.0f));
	spine_01Comp->SetRelativeRotation(FRotator(0.0f, 82.841473f, 0.0f));
	spine_01Comp->InitBoxExtent(FVector(25.0f, 20.0f, 20.0f));

	spine_01Comp->SetCollisionProfileName(TEXT("EnemyBody"));



	/////////////////////////////////////////////////////////////////////////////

	upperarm_rComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("upperarm_r"));
	upperarm_rComp->SetupAttachment(meshComp, FName("upperarm_r"));
	upperarm_rComp->ComponentTags.Add(FName("upperarm_r"));

	upperarm_rComp->SetRelativeLocation(FVector(-17.726883f, -2.259243f, -10.071114f));
	upperarm_rComp->SetRelativeRotation(FRotator(-77.843651f, -65.570553f, 60.044081f));
	upperarm_rComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	upperarm_rComp->InitCapsuleSize(20.0f, 20.0f);

	upperarm_rComp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////

	upperarm_lComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("upperarm_l"));
	upperarm_lComp->SetupAttachment(meshComp, FName("upperarm_l"));
	upperarm_lComp->ComponentTags.Add(FName("upperarm_l"));

	upperarm_lComp->SetRelativeLocation(FVector(13.749980f, 1.164164f, 2.554500f));
	upperarm_lComp->SetRelativeRotation(FRotator(-78.481850f, -73.424067f, 70.430619f));
	upperarm_lComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	upperarm_lComp->InitCapsuleSize(20.0f, 20.0f);

	upperarm_lComp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////

	thigh_rComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("thigh_r"));
	thigh_rComp->SetupAttachment(meshComp, FName("thigh_r"));
	thigh_rComp->ComponentTags.Add(FName("thigh_r"));

	thigh_rComp->SetRelativeLocation(FVector(18.704426f, -3.226856f, 4.326320f));
	thigh_rComp->SetRelativeRotation(FRotator(82.375114f, 180.0f, 193.381357f));
	thigh_rComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	thigh_rComp->InitCapsuleSize(20.0f, 30.0f);

	thigh_rComp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////

	thigh_lComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("thigh_l"));
	thigh_lComp->SetupAttachment(meshComp, FName("thigh_l"));
	thigh_lComp->ComponentTags.Add(FName("thigh_l"));

	thigh_lComp->SetRelativeLocation(FVector(-21.552837f, 6.525819f, -3.928487f));
	thigh_lComp->SetRelativeRotation(FRotator(82.375114f, 180.0f, 200.0f));
	thigh_lComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	thigh_lComp->InitCapsuleSize(20.0f, 30.0f);

	thigh_lComp->SetCollisionProfileName(TEXT("EnemyBody"));



	/////////////////////////////////////////////////////////////////////////////

	foot_rComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("foot_r"));
	foot_rComp->SetupAttachment(meshComp, FName("foot_r"));
	foot_rComp->ComponentTags.Add(FName("foot_r"));

	foot_rComp->SetRelativeLocation(FVector(-26.704352f, -6.383555f, -3.169490f));
	foot_rComp->SetRelativeRotation(FRotator(88.157515f, 148.327238f, 139.300119f));
	foot_rComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	foot_rComp->InitCapsuleSize(20.0f, 30.0f);

	foot_rComp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////

	foot_lComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("foot_l"));
	foot_lComp->SetupAttachment(meshComp, FName("foot_l"));
	foot_lComp->ComponentTags.Add(FName("foot_l"));

	foot_lComp->SetRelativeLocation(FVector(26.201399f, 7.967633f, 1.668402f));
	foot_lComp->SetRelativeRotation(FRotator(-77.843651f, -65.570553f, 64.576384f));
	foot_lComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	foot_lComp->InitCapsuleSize(20.0f, 30.0f);

	foot_lComp->SetCollisionProfileName(TEXT("EnemyBody"));



	/////////////////////////////////////////////////////////////////////////////

	// Spawner에서 hp 재할당
	hp = 100;
	attackDelay = 3.0f;
	attackRange = 1200;
	attackPower = 10;
	IdleDelay = 1.0f;
	damageDelay = 1.0f;
	DownDelay = 2.5f;
	enemyType = EEnemyType::Ranged_Ground;
	enemyState = EEnemyState::Idle;

	// 몽타주 로드
	static ConstructorHelpers::FObjectFinder<UAnimMontage> GetHitMontageObj(TEXT("/Script/Engine.AnimMontage'/Game/YSL/Animation/AM_RangedGroundDamage.AM_RangedGroundDamage'"));
	if (GetHitMontageObj.Succeeded())
	{
		HitFromTestRockMontage = GetHitMontageObj.Object;
		HitFromBulletMontage = GetHitMontageObj.Object;
		HitFromSwordMontage = GetHitMontageObj.Object;
		HitFromSlashMontage = GetHitMontageObj.Object;
	}

}

void ARangedGroundEnemy::BeginPlay()
{
	Super::BeginPlay();

	enemyAnim = Cast<URangedEnemyAnim>(meshComp->GetAnimInstance());

	upperarm_rComp->OnComponentBeginOverlap.AddDynamic(this, &ARangedGroundEnemy::OnAttackOverlap);
	upperarm_lComp->OnComponentBeginOverlap.AddDynamic(this, &ARangedGroundEnemy::OnAttackOverlap);
	thigh_rComp->OnComponentBeginOverlap.AddDynamic(this, &ARangedGroundEnemy::OnAttackOverlap);
	thigh_lComp->OnComponentBeginOverlap.AddDynamic(this, &ARangedGroundEnemy::OnAttackOverlap);
	foot_rComp->OnComponentBeginOverlap.AddDynamic(this, &ARangedGroundEnemy::OnAttackOverlap);
	foot_lComp->OnComponentBeginOverlap.AddDynamic(this, &ARangedGroundEnemy::OnAttackOverlap);
	HeadComp->OnComponentBeginOverlap.AddDynamic(this, &ARangedGroundEnemy::OnAttackOverlap);
	spine_01Comp->OnComponentBeginOverlap.AddDynamic(this, &ARangedGroundEnemy::OnAttackOverlap);


	for (int i = 0; i < magicBallPoolSize; i++)
	{
		FActorSpawnParameters param;
		param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform SpawnTransform;
		SpawnTransform.SetLocation(FVector(0.f, 0.f, -1000.f));
		magicBall = GetWorld()->SpawnActor<ARangedEnemyProjectile>(magicBallFactory, SpawnTransform, param);


		magicBallPool.Add(magicBall);
		magicBall->GetOwningEnemy(this);
		magicBall->SetActive(false);
		magicBall->magicDamage = attackPower;
	}

	SetRangedMoveSpeed();
	SetActive(false);
}

void ARangedGroundEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetActorLocation().Z > 82.f)
	{
		SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 82.f));
	}
}

void ARangedGroundEnemy::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}


void ARangedGroundEnemy::SetRangedMoveSpeed()
{
	if (enemyAnim != nullptr)
	{
		enemyAnim->runTypeInt = FMath::RandRange(0, 3);
		FVector NewScale = GetActorScale3D();
		switch (enemyAnim->runTypeInt)
		{

		case 0:
			pawnMovementComp->MaxSpeed = speedHigh;
			break;
		case 1:
			pawnMovementComp->MaxSpeed = speedMiddle;
			NewScale.Y = -NewScale.Y; // X 스케일 값을 반전
			SetActorScale3D(NewScale);
			break;
		case 2:
			pawnMovementComp->MaxSpeed = speedMiddle;
			break;
		case 3:
			pawnMovementComp->MaxSpeed = speedLow;
			NewScale.Y = -NewScale.Y; // X 스케일 값을 반전
			SetActorScale3D(NewScale);
			break;
		default:
			break;

		}

	}
}

void ARangedGroundEnemy::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RagdollFlag) return;

	if (!OtherActor->IsA(ABaseEnemy::StaticClass()) && !OtherActor->IsA(ABattlePlayer::StaticClass()))
	{
		enemyAnim->DieRandomInt = FMath::RandRange(0, 1);
		if (hp < 1) // Die
		{
			if (OverlappedComponent->GetName().Contains(TEXT("head")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					// 헤드 칼 데미지
					enemyAnim->DieSectionInt_RangedG = 0;

				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					// 헤드 총 데미지
					enemyAnim->DieSectionInt_RangedG = 0;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					// 헤드 참격  데미지
					enemyAnim->DieSectionInt_RangedG = 0;

				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("spine")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					// 헤드 칼 데미지
					enemyAnim->DieSectionInt_RangedG = 3;
					

				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					// 헤드 총 데미지
					enemyAnim->DieSectionInt_RangedG = 3;

				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					// 헤드 참격  데미지
					enemyAnim->DieSectionInt_RangedG = 3;


				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("foot_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 6;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 6;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 6;
				}

			}
			else if (OverlappedComponent->GetName().Contains(TEXT("foot_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 7;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 7;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 7;
				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("upperarm_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 1;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 1;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 1;
				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("upperarm_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 2;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 2;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 2;
				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("thigh_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 4;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 4;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 4;
				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("thigh_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 5;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 5;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DieSectionInt_RangedG = 5;
				}
			}
		}
		else if (hp >= 1) // hit 결국 이게 다 처리해줌
		{
			if (OverlappedComponent->GetName().Contains(TEXT("head")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					// 헤드 칼 데미지
					enemyAnim->DamageSectionInt = 0;
					enemyAnim->DieSectionInt_RangedG = 0;

				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					// 헤드 총 데미지
					enemyAnim->DamageSectionInt = 0;
					enemyAnim->DieSectionInt_RangedG = 0;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					// 헤드 참격  데미지
					enemyAnim->DamageSectionInt = 0;
					enemyAnim->DieSectionInt_RangedG = 0;

				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("spine")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					// 헤드 칼 데미지
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 3;


				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{

					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 3;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{

					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 3;

				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("foot_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 6;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 6;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 6;
				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("foot_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 7;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 7;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 7;
				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("upperarm_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 1;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 1;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 1;
				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("upperarm_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 2;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 2;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 2;
				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("thigh_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 4;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 4;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 4;
				}
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("thigh_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 5;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 5;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt_RangedG = 5;
				}
			}
		}

		if (!RagdollFlag && OtherActor->IsA(ASlashProjectile::StaticClass()))
		{
			RagdollFlag = true;

			FVector CurrentLocation = OtherActor->GetActorLocation();

			FTimerHandle TimerHandle_RagdollActivation;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_RagdollActivation, [this, OtherActor, CurrentLocation]()
				{
					if (IsValid(OtherActor))
					{
						FVector Velocity = CalculateVelocity(OtherActor, CurrentLocation);

						FVector RagdollPower = Velocity.GetSafeNormal() * RagdollBackPower;
						RagdollPower.Z = RagdollUpPower;

						OnRagdoll();
						meshComp->AddImpulse(RagdollPower);
						
						//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("x = %f, y = %f, z= %f"), RagdollPower.X, RagdollPower.Y, RagdollPower.Z));
					}

				}, GetWorld()->GetDeltaSeconds(), false);

		}
		else if (OtherActor->IsA(ASword::StaticClass()) || OtherActor->IsA(AEnergyBullet::StaticClass()) && hp < 1)
		{
			SetActorEnableCollision(false); // Fiber Mode
		}
	}

	//melee에서는 Attack구현된 부분
}

void ARangedGroundEnemy::FindTarget()
{
	Super::FindTarget();
}

void ARangedGroundEnemy::Move()
{
	Super::Move();
}

void ARangedGroundEnemy::ApplyDamage()
{
	Super::ApplyDamage();
	//if(!meshComp->IsVisible()) return;
}

void ARangedGroundEnemy::OnTakeDamage()
{
	Super::OnTakeDamage();

}

void ARangedGroundEnemy::DieDown()
{
	Super::DieDown();
	//
	if (curTime > DownDelay)
	{
		// 아래로 이동
		FVector P = GetActorLocation() + FVector::DownVector * 100 * GetWorld()->DeltaTimeSeconds;
		if (P.Z < -300)
		{
			PutObjectPool();
		}
		else if (curTime > DownDelay + 5.f)
		{
			PutObjectPool();
		}

	}
}

void ARangedGroundEnemy::PutObjectPool()
{
	Super::PutObjectPool();
	if (spawner != nullptr)
	{
		OffRagdoll();
		spawner->RangedGround1Pool.Enqueue(this);
		//spawner->spawnedRangedGround1Count--;
		SetActive(false);
		IamInvisible = true;
		hp = spawner->enemyHP;
		curTime=0;
	
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("ARangedGroundEnemy %d"), myCycle++));
	}

}


void ARangedGroundEnemy::OnRagdoll()
{
	if (meshComp != nullptr && pawnMovementComp != nullptr)
	{
		meshComp->bPauseAnims = true;
		pawnMovementComp->MaxSpeed = 0;
		meshComp->SetSimulatePhysics(true);
		enemyState = EEnemyState::Dead;
		RagdollFlag = true; // 중복으로 맞는것 방지용. false일때만 감지됩니다. OffRagdoll에서 다음 오브젝트풀 사이클때를 위한 false 초기화
	}
}

void ARangedGroundEnemy::OffRagdoll()
{
	if (meshComp != nullptr && pawnMovementComp != nullptr)
	{
		meshComp->SetSimulatePhysics(false);
		meshComp->bPauseAnims = false;
		SetRangedMoveSpeed();
		meshComp->AttachToComponent(capsuleComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		RagdollFlag = false;


		if (meshComp->GetRelativeScale3D().Y < 0)
		{
			meshComp->SetRelativeLocationAndRotation(FVector(0, 0, -150.f), FRotator(0, 90.f, 0));
		}
		else
		{
			meshComp->SetRelativeLocationAndRotation(FVector(0, 0, -150.f), FRotator(0, -90.f, 0));
		}
	}
}

