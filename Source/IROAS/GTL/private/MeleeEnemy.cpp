

#include "MeleeEnemy.h"
#include <Kismet/GameplayStatics.h>
#include "BattlePlayer.h"
#include <EngineUtils.h>
#include <AIModule/Classes/AIController.h>
#include <Components/TextRenderComponent.h>
#include <Components/SphereComponent.h>
#include <Components/CapsuleComponent.h>
#include "Components/BoxComponent.h"
#include "BaseEnemyAnim.h" 
#include "EnemySpawner.h" 
#include "GameFramework/FloatingPawnMovement.h"
#include "Kismet/KismetMathLibrary.h"
#include "EnergyBullet.h"
#include "Sword.h"
#include "SlashProjectile.h"
#include "Rock.h"




AMeleeEnemy::AMeleeEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	leftHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("LeftHand"));
	leftHandCollision->SetupAttachment(meshComp, FName("middle_01_l"));
	leftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	leftHandCollision->SetCollisionProfileName(TEXT("EnemyAttack"));
	rightHandCollision = CreateDefaultSubobject<USphereComponent>(TEXT("RightHand"));
	rightHandCollision->SetupAttachment(meshComp, FName("middle_01_r"));
	rightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	rightHandCollision->SetCollisionProfileName(TEXT("EnemyAttack"));


	// 헤드 Head

	spine_01Comp = CreateDefaultSubobject<UBoxComponent>(TEXT("spine_01"));
	spine_01Comp->SetupAttachment(meshComp, FName("spine_01"));
	spine_01Comp->ComponentTags.Add(FName("spine_01"));

	spine_01Comp->SetRelativeLocation(FVector(7.937439f, -0.996895f, 0.0f));
	spine_01Comp->SetRelativeRotation(FRotator(0.0f, 82.841473f, 0.0f));
	spine_01Comp->InitBoxExtent(FVector(25.0f, 20.0f, 20.0f));
	spine_01Comp->SetCollisionObjectType(ECC_GameTraceChannel3); //EnemyBody
	spine_01Comp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	spine_01Comp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////
	upperarm_rComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("upperarm_r"));
	upperarm_rComp->SetupAttachment(meshComp, FName("upperarm_r"));
	upperarm_rComp->ComponentTags.Add(FName("upperarm_r"));

	upperarm_rComp->SetRelativeLocation(FVector(-17.726883f, -2.259243f, -10.071114f));
	upperarm_rComp->SetRelativeRotation(FRotator(-77.843651f, -65.570553f, 60.044081f));
	upperarm_rComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	upperarm_rComp->InitCapsuleSize(20.0f, 20.0f);
	upperarm_rComp->SetCollisionObjectType(ECC_GameTraceChannel3); //EnemyBody
	upperarm_rComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	upperarm_rComp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////

	upperarm_lComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("upperarm_l"));
	upperarm_lComp->SetupAttachment(meshComp, FName("upperarm_l"));
	upperarm_lComp->ComponentTags.Add(FName("upperarm_l"));

	upperarm_lComp->SetRelativeLocation(FVector(13.749980f, 1.164164f, 2.554500f));
	upperarm_lComp->SetRelativeRotation(FRotator(-78.481850f, -73.424067f, 70.430619f));
	upperarm_lComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	upperarm_lComp->InitCapsuleSize(20.0f, 20.0f);
	upperarm_lComp->SetCollisionObjectType(ECC_GameTraceChannel3); //EnemyBody
	upperarm_lComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	upperarm_lComp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////

	thigh_rComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("thigh_r"));
	thigh_rComp->SetupAttachment(meshComp, FName("thigh_r"));
	thigh_rComp->ComponentTags.Add(FName("thigh_r"));

	thigh_rComp->SetRelativeLocation(FVector(18.704426f, -3.226856f, 4.326320f));
	thigh_rComp->SetRelativeRotation(FRotator(82.375114f, 180.0f, 193.381357f));
	thigh_rComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	thigh_rComp->InitCapsuleSize(20.0f, 30.0f);
	thigh_rComp->SetCollisionObjectType(ECC_GameTraceChannel3); //EnemyBody
	thigh_rComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	thigh_rComp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////

	thigh_lComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("thigh_l"));
	thigh_lComp->SetupAttachment(meshComp, FName("thigh_l"));
	thigh_lComp->ComponentTags.Add(FName("thigh_l"));

	thigh_lComp->SetRelativeLocation(FVector(-21.552837f, 6.525819f, -3.928487f));
	thigh_lComp->SetRelativeRotation(FRotator(82.375114f, 180.0f, 200.0f));
	thigh_lComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	thigh_lComp->InitCapsuleSize(20.0f, 30.0f);
	thigh_lComp->SetCollisionObjectType(ECC_GameTraceChannel3); //EnemyBody
	thigh_lComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	thigh_lComp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////
	foot_rComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("foot_r"));
	foot_rComp->SetupAttachment(meshComp, FName("foot_r"));
	foot_rComp->ComponentTags.Add(FName("foot_r"));

	foot_rComp->SetRelativeLocation(FVector(-26.704352f, -6.383555f, -3.169490f));
	foot_rComp->SetRelativeRotation(FRotator(88.157515f, 148.327238f, 139.300119f));
	foot_rComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	foot_rComp->InitCapsuleSize(20.0f, 30.0f);
	foot_rComp->SetCollisionObjectType(ECC_GameTraceChannel3); //EnemyBody
	foot_rComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	foot_rComp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////
	foot_lComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("foot_l"));
	foot_lComp->SetupAttachment(meshComp, FName("foot_l"));
	foot_lComp->ComponentTags.Add(FName("foot_l"));

	foot_lComp->SetRelativeLocation(FVector(26.201399f, 7.967633f, 1.668402f));
	foot_lComp->SetRelativeRotation(FRotator(-77.843651f, -65.570553f, 64.576384f));
	foot_lComp->SetRelativeScale3D(FVector(1.0f, 1.0f, 1.375075f));
	foot_lComp->InitCapsuleSize(20.0f, 30.0f);
	foot_lComp->SetCollisionObjectType(ECC_GameTraceChannel3); //EnemyBody
	foot_lComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	foot_lComp->SetCollisionProfileName(TEXT("EnemyBody"));


	/////////////////////////////////////////////////////////////////////////////
	// 몽타주 로드
	//static ConstructorHelpers::FObjectFinder<UAnimMontage> GetHitMontageObj(TEXT("/Script/Engine.AnimMontage'/Game/GTL/EnemyComtent/EnemyAnim/MeleeAnim/Melee_hit_Montage.Melee_hit_Montage'"));
	//if (GetHitMontageObj.Succeeded())
	//{
	//	HitFromTestRockMontage = GetHitMontageObj.Object;
	//	HitFromBulletMontage = GetHitMontageObj.Object;
	//	HitFromSwordMontage = GetHitMontageObj.Object;
	//	HitFromSlashMontage = GetHitMontageObj.Object;
	//}


	/////////////////////////////////////////////////////////////////////////////
	// 
	// 상속받은 속성
	hp = 100;
	attackDelay = 1.5f;
	attackRange = 300;
	attackPower = 10;
	IdleDelay = 1.0f;
	damageDelay = 1.0f;
	DownDelay = 2.5f;
	enemyType = EEnemyType::Melee;
	enemyState = EEnemyState::Idle;




}

void AMeleeEnemy::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	enemyAnim = Cast<UBaseEnemyAnim>(meshComp->GetAnimInstance());
}

void AMeleeEnemy::BeginPlay()
{
	Super::BeginPlay();


	//enemyAnim = Cast<UBaseEnemyAnim>(meshComp->GetAnimInstance());
	leftHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);
	rightHandCollision->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);
	upperarm_rComp->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);
	upperarm_lComp->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);
	thigh_rComp->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);
	thigh_lComp->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);
	foot_rComp->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);
	foot_lComp->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);
	spine_01Comp->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);
	HeadComp->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);
	spine_01Comp->OnComponentBeginOverlap.AddDynamic(this, &AMeleeEnemy::OnAttackOverlap);

	SetMeleeMoveSpeed();
	SetActive(false);
}

void AMeleeEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (GetActorLocation().Z > 82.f)
	{
		SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, 82.f));
	}
}

void AMeleeEnemy::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void AMeleeEnemy::SetMeleeMoveSpeed()
{
	if (enemyAnim != nullptr)
	{
		enemyAnim->runTypeInt = FMath::RandRange(0, 4);
		FVector NewScale = GetActorScale3D();
		switch (enemyAnim->runTypeInt)
		{

		case 0:
			pawnMovementComp->MaxSpeed = speedHigh;
			break;
		case 1:
			pawnMovementComp->MaxSpeed = speedHigh;
			NewScale.Y = -NewScale.Y; // X 스케일 값을 반전
			SetActorScale3D(NewScale);
			break;
		case 2:
			pawnMovementComp->MaxSpeed = speedMiddle;
			break;
		case 3:
			pawnMovementComp->MaxSpeed = speedMiddle;
			NewScale.Y = -NewScale.Y; // X 스케일 값을 반전
			SetActorScale3D(NewScale);
			break;
		case 4:
			pawnMovementComp->MaxSpeed = speedLow;
			break;
		default:
			break;
		}

	}
}

void AMeleeEnemy::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (RagdollFlag) return;
	//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("BP_SlashProjectile Hit %s"), *OverlappedComponent->GetName())); // BoxCollision (나는 누군가?)
	//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("BP_SlashProjectile = %s"), *OtherActor->GetActorLabel())); // BP_MeleeEnemy_C_5 (월드의 누구가 때렸나?)
	//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("BP_SlashProjectile = %s"), *OtherComp->GetName())); // head 중요한부분 (누구의 어디에 맞았나?)

	if (!OtherActor->IsA(ABaseEnemy::StaticClass()) && !OtherActor->IsA(ABattlePlayer::StaticClass()))
	{
		
		enemyAnim->DieRandomInt = FMath::RandRange(0,1);
		if (hp<1) // Die
		{
		/*	OnRagdoll();
			meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
			SetActorEnableCollision(true);
			meshComp->AddImpulse(FVector(0, 500000, 500000));*/
			if (OverlappedComponent->GetName().Contains(TEXT("head")))
			{

				if (OtherActor->IsA(ASword::StaticClass()))
				{
					// 헤드 칼 데미지
					enemyAnim->DamageSectionInt = 0;
					enemyAnim->DieSectionInt = 0;

				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					// 헤드 총 데미지
					enemyAnim->DamageSectionInt = 0;
					enemyAnim->DieSectionInt = 0;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					// 헤드 참격  데미지
					enemyAnim->DamageSectionInt = 0;
					enemyAnim->DieSectionInt = 0;

				}
		
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("spine")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					// 헤드 칼 데미지
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 3;

				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{

					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 3;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{

					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 3;

				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("foot_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 6;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 6;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 6;
				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("foot_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 7;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 7;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 7;
				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("upperarm_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 1;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 1;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 1;
				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("upperarm_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 2;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 2;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 2;
				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("thigh_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 4;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 4;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 4;
				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("thigh_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 5;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 5;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 5;
				}
				
			}
		}
		else if (hp>=1) // hit 결국 이게 다 처리해줌
		{
			
			
			if (OverlappedComponent->GetName().Contains(TEXT("head")))
			{

				if (OtherActor->IsA(ASword::StaticClass()))
				{
					// 헤드 칼 데미지
					enemyAnim->DamageSectionInt = 0;
					enemyAnim->DieSectionInt = 0;

				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					// 헤드 총 데미지
					enemyAnim->DamageSectionInt = 0;
					enemyAnim->DieSectionInt = 0;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					// 헤드 참격  데미지
					enemyAnim->DamageSectionInt = 0;
					enemyAnim->DieSectionInt = 0;
				}
			
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("spine")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					// 헤드 칼 데미지
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 3;

				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{

					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 3;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{

					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 3;

				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("foot_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 6;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 6;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 6;
				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("foot_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 7;

				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 7;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 7;
				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("upperarm_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 1;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 1;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 1;
				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("upperarm_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 2;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 2;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 2;
				}
				
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("thigh_l")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 4;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 4;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 4;
				}
			
			}
			else if (OverlappedComponent->GetName().Contains(TEXT("thigh_r")))
			{
				if (OtherActor->IsA(ASword::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 5;
				}
				else if (OtherActor->IsA(AEnergyBullet::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 5;
				}
				else if (OtherActor->IsA(ASlashProjectile::StaticClass()))
				{
					enemyAnim->DamageSectionInt = FMath::RandRange(0, 3);
					enemyAnim->DieSectionInt = 5;
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

						//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("x = %f, y = %f, z= %f"), RagdollPower.X, RagdollPower.Y, RagdollPower.Z));
					}

				}, GetWorld()->GetDeltaSeconds(), false);

		}
		else if (OtherActor->IsA(ASword::StaticClass()) || OtherActor->IsA(AEnergyBullet::StaticClass())&& hp<1 )
		{
			SetActorEnableCollision(false); //
		}
	}

	//내가 플레이어에게 데미지를 주는 파트 여기에서 Attack 몽타주를 넣어야할듯
	if (target == OtherActor)
	{
		
		IOnDamage_Interface* OnDamage = Cast<IOnDamage_Interface>(target);
		if (OnDamage != nullptr)
		{
			OnDamage->OnDamage_Interface(attackPower);
			target->HitbyMeleeSound();
						
		}

	}

}

void AMeleeEnemy::SetCollision(bool isActive)
{

	if (isActive)
	{
		leftHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		rightHandCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	
	}
	else
	{
		leftHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		rightHandCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	
	}
}

void AMeleeEnemy::Move()
{
	Super::Move();
	SetCollision(false);

	if (Tags.Contains("Rage")) 
	{

		curTime += GetWorld()->DeltaTimeSeconds;
		if (curTime > 4.f)
		{
				if (!RageTypeIntFlag && enemyAnim!=nullptr)
				{
					enemyAnim->RageTypeInt = FMath::RandRange(0,7);
					RageTypeIntFlag = true;
				}
				enemyState = EEnemyState::Rage;
				enemyAnim->animState = EEnemyState::Rage;
				curTime=0;
			
		}

	}
}

void AMeleeEnemy::ApplyDamage()
{
	Super::ApplyDamage();
	//if (enemyAnim != nullptr && enemyAnim->bAttackPlay) {
	//	SetCollision(true);
	//}
}

void AMeleeEnemy::OnTakeDamage()
{
	Super::OnTakeDamage();

}

void AMeleeEnemy::DieDown()
{
	Super::DieDown();

	//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("Down Curtime: %f"), curTime));
	if (curTime > DownDelay)
	{
		// 아래로 이동
		FVector P = GetActorLocation() + FVector::DownVector * 100 * GetWorld()->DeltaTimeSeconds;
		if (P.Z < -300)
		{
			PutObjectPool();

		}
		else if (curTime> DownDelay + 5.f)
		{
			PutObjectPool();
		}

	}

}

void AMeleeEnemy::PutObjectPool()
{
	Super::PutObjectPool();
	if (spawner != nullptr)
	{
		OffRagdoll(); //Down에서 호출되는데 거기에서 Idle로 전환됨
		spawner->Melee1Pool.Enqueue(this);
		//spawner->spawnedMelee1Count--;
		SetActive(false);
		IamInvisible = true;
		hp=spawner->enemyHP;
		curTime=0;
		
		GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Blue, FString::Printf(TEXT("AMeleeEnemy %d"), myCycle++));
	}

}

void AMeleeEnemy::OnRagdoll()
{
	if (meshComp != nullptr && pawnMovementComp != nullptr)
	{
		meshComp->bPauseAnims = true;
		pawnMovementComp->MaxSpeed = 0;
		meshComp->SetSimulatePhysics(true);
		enemyState = EEnemyState::Dead;
		//RagdollFlag = true; // 중복으로 맞는것 방지용. false일때만 감지됩니다. OffRagdoll에서 다음 오브젝트풀 사이클때를 위한 false 초기화

		/*FTimerHandle TimerHandle_OffRagdoll;
			GetWorld()->GetTimerManager().SetTimer(TimerHandle_OffRagdoll, this, &AMeleeEnemy::OffRagdoll, 3.0f, false);*/
		Tags.Empty();
	}
}

void AMeleeEnemy::OffRagdoll()
{
	if (meshComp != nullptr && pawnMovementComp != nullptr)
	{
		meshComp->SetSimulatePhysics(false);
		meshComp->bPauseAnims = false;
		SetMeleeMoveSpeed();
		meshComp->AttachToComponent(capsuleComp, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
		RagdollFlag = false;
		if (meshComp->GetRelativeScale3D().Y<0)
		{
			meshComp->SetRelativeLocationAndRotation(FVector(0, 0, -150.f), FRotator(0, 90.f, 0));
		}
		else
		{
			meshComp->SetRelativeLocationAndRotation(FVector(0, 0, -150.f), FRotator(0, -90.f, 0));
		}
	}
}

void AMeleeEnemy::MeleeSpawnSound()
{
	
}
