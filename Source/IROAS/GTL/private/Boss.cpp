// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss.h"
#include <GameFramework/CharacterMovementComponent.h>
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "BattlePlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "BossSlash.h"
#include "Rock.h"
#include "BaseEnemy.h"
#include "BossAnim.h"
#include "Materials/MaterialInstanceConstant.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "GameFramework/SpringArmComponent.h"
#include "Camera/CameraComponent.h"
#include "../DVN/public/SlashProjectile.h"


ABoss::ABoss()
{
	PrimaryActorTick.bCanEverTick = true;

	//콜리전
	CapsuleCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleCollision"));
	SetRootComponent(CapsuleCollision);
	CapsuleCollision->SetCapsuleHalfHeight(100.f * BossSize);
	CapsuleCollision->SetCapsuleRadius(50.f * BossSize);


	//스켈레탈메시 할당
	BossMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("BossMesh"));
	BossMesh->SetupAttachment(CapsuleCollision);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempBossMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/GTL/BossContent/BossAssets/BossMesh/bossmesh.bossmesh'"));
	if (TempBossMesh.Succeeded())
	{
		BossMesh->SetSkeletalMesh(TempBossMesh.Object);
		BossMesh->SetRelativeLocationAndRotation(FVector(0, 0, 0.f) * BossSize, FRotator(0, -90.f, 0));
		//BossMesh->SetRelativeScale3D(FVector(3.0f));
	}
	BossMesh->SetRelativeScale3D(FVector(BossSize));

	//애니메이션 할당
	ConstructorHelpers::FClassFinder<UBossAnim> TemBosspAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/GTL/BossContent/ABP_Boss.ABP_Boss_C'"));
	if (TemBosspAnim.Succeeded())
	{
		BossMesh->SetAnimInstanceClass(TemBosspAnim.Class);
	}

	// FSM 인식
	BossFSM = CreateDefaultSubobject<UBossFSM>(TEXT("BossFSM"));

	// PlacedInWorld Or Spawned 설정
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
	
	// Muzzle
	SlashPoint = CreateDefaultSubobject<USceneComponent>(TEXT("SlashPosition"));
	SlashPoint->SetupAttachment(BossMesh);
	SlashPoint->SetRelativeLocation(FVector(0,140.f,70.f));

	RockPoint= CreateDefaultSubobject<USceneComponent>(TEXT("RockPosition"));
	RockPoint->SetupAttachment(BossMesh);
	RockPoint->SetRelativeLocation(FVector(0,0,350.f));


	// 블링크 깜빡 나이아가라 VFX
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> MyNiagaraSystem(TEXT("/Script/Niagara.NiagaraSystem'/Game/GTL/SkillContent/NiagaraUltimateHits/Fx/Hit/NS_Hit9_01.NS_Hit9_01'"));
	BlinkVFX = MyNiagaraSystem.Object;

	// 블링크의 리본 나이아가라 VFX 5포인트 
	static const FString NiagaraSystemObjPath = TEXT("/Script/Niagara.NiagaraSystem'/Game/GTL/SkillContent/Ribbon/FX/Projectile/NS_ProjA_09.NS_ProjA_09'");

		// 블링크의 리본 나이아가라 VFX //////////////////////////////////////////////01
	NiagaraRibbonComponent01 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraRibbon01"));
	NiagaraRibbonComponent01->SetupAttachment(BossMesh);
	NiagaraRibbonComponent01->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemObj01(*NiagaraSystemObjPath);
	if (NiagaraSystemObj01.Succeeded())
	{
		// 컴포넌트에 Niagara 시스템 설정
		NiagaraRibbonComponent01->SetAsset(NiagaraSystemObj01.Object);
		NiagaraRibbonComponent01->SetRelativeLocation(FVector(0, 0, 40.f));
	}
	
	////////////////////////////////////////////////////////////////////////////////


		// 블링크의 리본 나이아가라 VFX //////////////////////////////////////////////02
	NiagaraRibbonComponent02 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraRibbon02"));
	NiagaraRibbonComponent02->SetupAttachment(BossMesh);
	NiagaraRibbonComponent02->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemObj02(*NiagaraSystemObjPath);
	if (NiagaraSystemObj02.Succeeded())
	{
		// 컴포넌트에 Niagara 시스템 설정
		NiagaraRibbonComponent02->SetAsset(NiagaraSystemObj02.Object);
		NiagaraRibbonComponent02->SetRelativeLocation(FVector(0, 0, 60.f));
	}
	
	////////////////////////////////////////////////////////////////////////////////

		// 블링크의 리본 나이아가라 VFX //////////////////////////////////////////////03
	NiagaraRibbonComponent03 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraRibbon03"));
	NiagaraRibbonComponent03->SetupAttachment(BossMesh);
	NiagaraRibbonComponent03->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemObj03(*NiagaraSystemObjPath);
	if (NiagaraSystemObj03.Succeeded())
	{
		// 컴포넌트에 Niagara 시스템 설정
		NiagaraRibbonComponent03->SetAsset(NiagaraSystemObj03.Object);
		NiagaraRibbonComponent03->SetRelativeLocation(FVector(0, 0, 80.f));
	}
	
	////////////////////////////////////////////////////////////////////////////////

		// 블링크의 리본 나이아가라 VFX //////////////////////////////////////////////04
	NiagaraRibbonComponent04 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraRibbon04"));
	NiagaraRibbonComponent04->SetupAttachment(BossMesh);
	NiagaraRibbonComponent04->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemObj04(*NiagaraSystemObjPath);
	if (NiagaraSystemObj04.Succeeded())
	{
		// 컴포넌트에 Niagara 시스템 설정
		NiagaraRibbonComponent04->SetAsset(NiagaraSystemObj04.Object);
		NiagaraRibbonComponent04->SetRelativeLocation(FVector(0, 0, 100.f));
	}

	////////////////////////////////////////////////////////////////////////////////

		// 블링크의 리본 나이아가라 VFX //////////////////////////////////////////////05
	NiagaraRibbonComponent05 = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraRibbon05"));
	NiagaraRibbonComponent05->SetupAttachment(BossMesh);
	NiagaraRibbonComponent05->SetAutoActivate(false);

	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemObj05(*NiagaraSystemObjPath);
	if (NiagaraSystemObj05.Succeeded())
	{
		// 컴포넌트에 Niagara 시스템 설정
		NiagaraRibbonComponent05->SetAsset(NiagaraSystemObj05.Object);
		NiagaraRibbonComponent05->SetRelativeLocation(FVector(0, 0, 120.f));
	}
	
	////////////////////////////////////////////////////////////////////////////////

	BossSpringArm1 = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm1"));
	BossSpringArm1->TargetArmLength = 600.0f;
	BossSpringArm1->bDoCollisionTest = false;
	BossSpringArm1->SetRelativeLocationAndRotation(FVector(-0.0f, 0.0f, 0.0f), FRotator(12.0f, -34.0f, 0.0f));
	//BossSpringArm1->SetRelativeLocationAndRotation(FVector(-0.0f, 0.0f, 95.0f), FRotator(12.0f, -34.0f, 0.0f));
	BossSpringArm1->SetupAttachment(BossMesh);

	BossCamera1 = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera1"));
	BossCamera1->SetupAttachment(BossSpringArm1);
	BossCamera1->SetActive(true);

	BossSpringArm2 = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm2"));
	BossSpringArm2->TargetArmLength = 1000.0f;
	BossSpringArm2->bDoCollisionTest = false;
	BossSpringArm2->SetRelativeLocationAndRotation(FVector(-90.0f, -25.0f, -35.0f), FRotator(0.0f, -125.0f, 0.0f));
	//BossSpringArm2->SetRelativeLocationAndRotation(FVector(-90.0f, -25.0f, 35.0f), FRotator(0.0f, -125.0f, 0.0f));
	BossSpringArm2->SetupAttachment(BossMesh);

	BossCamera2 = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera2"));
	BossCamera2->SetupAttachment(BossSpringArm2);
	BossCamera2->SetRelativeRotation(FRotator(0.0f, 0.0f, 0.0f));
	BossCamera2->SetActive(false);





	//////

	NiagaraSmoke = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraSmoke"));
	NiagaraSmoke->SetupAttachment(BossMesh);
	static ConstructorHelpers::FObjectFinder<UNiagaraSystem> NiagaraSystemAssetFinder(TEXT("/Script/Niagara.NiagaraSystem'/Game/GTL/BossDieSmoke.BossDieSmoke'"));
	if (NiagaraSystemAssetFinder.Succeeded())
	{
		UNiagaraSystem* NiagaraSystem = NiagaraSystemAssetFinder.Object;
		NiagaraSmoke->SetAsset(NiagaraSystem);
		NiagaraSmoke->SetRelativeLocation(FVector(0, 115.0f, 25.0f));
		NiagaraSmoke->SetRelativeRotation(FRotator(0.0f, 90.0f, 0.0f));
		NiagaraSmoke->SetRelativeScale3D(FVector(3.0f));
		NiagaraSmoke->SetAutoActivate(false);
	}
}

void ABoss::BeginPlay()
{
	Super::BeginPlay();

	// Anim 인식
	BossAnim = Cast<UBossAnim>(BossMesh->GetAnimInstance());

	// 플레이어들 찾기
	FindTarget();

	// BossSlash 오브젝트풀 생성
	CreateSlashPool();


	// 기본 재질 저장  (피격 아웃라인)
	OriginalMaterial = BossMesh->GetMaterial(0);

	UMaterialInstanceConstant* HitOverlayMaterialInstanceConstant = LoadObject<UMaterialInstanceConstant>(nullptr, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/GTL/HitsContent/Hit_Outline/M_Overlay_Inst_Damage.M_Overlay_Inst_Damage'"));
	// 오버레이 재질 인스턴스 생성 (피격 아웃라인 Hit)
	if (HitOverlayMaterialInstanceConstant != nullptr)
	{
		HitOverlayMaterialInstance = UMaterialInstanceDynamic::Create(HitOverlayMaterialInstanceConstant, this);
	}

	UMaterialInstanceConstant* DieOverlayMaterialInstanceConstant = LoadObject<UMaterialInstanceConstant>(nullptr, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/GTL/HitsContent/Hit_Outline/M_Overlay_Inst_Die.M_Overlay_Inst_Die'"));
	// 오버레이 재질 인스턴스 생성 (피격 아웃라인 Hit)
	if (DieOverlayMaterialInstanceConstant != nullptr)
	{
		DieOverlayMaterialInstance = UMaterialInstanceDynamic::Create(DieOverlayMaterialInstanceConstant, this);
	}

	CameraBlendAlpha = 0.0f;
}

void ABoss::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// FindTarget()으로 인식된 플레이어가 0보다 많으면
	if (Players.Num() > 0 && BossFSM->mBossState != EEnemyState::Dead) //죽었을때도 플레이어 방향으로 회전하는것 방지
	{
		FVector Direction = (Players[index_Player]->GetActorLocation() - GetActorLocation()).GetSafeNormal();
		FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(Direction, GetActorUpVector());

		// Z축 회전만 고정, Pitch와 Roll은 현재의 회전값을 유지
		NewRotation.Pitch = GetActorRotation().Pitch;
		NewRotation.Roll = GetActorRotation().Roll;

		SetActorRotation(NewRotation);
	}

	if (bIsCameraBlending)
	{
		if (!bIsCameraBlending) return; 
		
		CameraBlendAlpha += GetWorld()->DeltaTimeSeconds / CameraBlendDuration; // 1초동안 카메라 전환 (멀미주의)
		CameraBlendAlpha = FMath::Clamp(CameraBlendAlpha, 0.0f, 1.0f);

		// 스프링암 조절
		BossSpringArm1->TargetArmLength = FMath::Lerp(600.f, 1000.f, CameraBlendAlpha);

		//FVector NewSpringArm1Location = FMath::Lerp(FVector(0,0,95.f), FVector(-90.f,-25.f,35.f), CameraBlendAlpha);
		FVector NewSpringArm1Location = FMath::Lerp(FVector(0,0,0.f), FVector(-90.f,-25.f,-35.f), CameraBlendAlpha);
		BossSpringArm1->SetRelativeLocation(NewSpringArm1Location);
		
		FRotator NewSpringArm1Rotation = FMath::Lerp(FRotator(12.f,-34.f,0.f), FRotator(0.f, -125.f, 0.f), CameraBlendAlpha);
		BossSpringArm1->SetRelativeRotation(NewSpringArm1Rotation);

		if (CameraBlendAlpha>=1.0f)
		{
			ReturncameraDelay +=DeltaTime;
			if (ReturncameraDelay>2.f)
			{	
				APlayerController* PlayerController = UGameplayStatics::GetPlayerController(this, 0);
				if (PlayerController)
				{
					APawn* PlayerPawn = PlayerController->GetPawn();
					if (PlayerPawn)
					{
						PlayerController->SetViewTargetWithBlend(PlayerPawn, 4.0f, EViewTargetBlendFunction::VTBlend_EaseInOut, 2.0f);
					}
				}
				bIsCameraBlending = false;
			}
			

		}
		//카메라 조절
		// 
		// 
		//UGameplayStatics::OpenLevel(GetWorld(), FName("YourMainMenuLevel"));
		// 죽은 상태에서 일정시간DieDelayTime 5초 지났을때 사용하고 싶은 로직
		//BossCamera1->SetWorldLocationAndRotation(NewLocation, NewRotation);
	}
}

/************************************************************************************************
보스기능들: 활용은 BossFSM에서 활용

*************************************************************************************************/
void ABoss::FindTarget() // IdleState에서 활용
{

			// 월드에 존재하는 모든 배틀플레이어를 Get하여 Players 액터배열에 추가
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABattlePlayer::StaticClass(), Players);
	if (Players.Num() > 0)
	{		// 모든 플레이어를 찍고 돌아서
		for (int32 Index = 0; Index < Players.Num(); Index++)
		{
			target = Cast<ABattlePlayer>(Players[0]);
			// 플레이어들을 잘 인식했는지 Index와 Name 출력
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Player %d: %s"), Index, *Players[Index]->GetName()));
		}
	}
}
			// 타겟팅 바꾸는 용도
void ABoss::SwapTarget()
{
	if (Players.Num() > 0 && Players.Num() == 2) Players.Swap(0, 1);
}

/************************************************************************************************
ThrowRock

*************************************************************************************************/
void ABoss::ThrowRock()
{
		// 언리얼 에디터에서 BP_Boss안에 추가한 RockArray가 0개가 아니라면 (바위컨셉일때 여러 바위를 던지려고 써뒀던 로직)
	if (RockArray.Num() > 0)
	{
		// 추가 클래스 중에 0번 인덱스 가져오기
		TSubclassOf<class ARock> BossRockClass = RockArray[0];

		// 가져와서 Boss의 머리위에 Boss가 바라보는 방향으로 향하게 스폰하기
		if (BossRockClass != nullptr)
		{
			ARock* NewRock = GetWorld()->SpawnActor<ARock>(BossRockClass, RockPoint->GetComponentLocation(), GetActorRotation());
			if (NewRock != nullptr)
			{	
				// 보스 기본데미지의 두배로 RockDamage설정
				//NewRock->attackDamage = defaultAttackPower * 2;
			}
		}
	}
}
/************************************************************************************************
BossSlash

*************************************************************************************************/
void ABoss::BossSlash()
{
	// Pool 비어있으면 스킵. SetActive는 BossAnim에서
	if (BossSlashPool.Num() < 1) return;
}

void ABoss::CreateSlashPool()
{
	for (uint8 i = 0; i < BossSlashPoolPoolSize; i++)
	{
		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ABossSlash* slash = GetWorld()->SpawnActor<ABossSlash>(BP_BossSlash, Param);
		slash->Boss = this;
		slash->slashDamage = defaultAttackPower;
		
		BossSlashPool.Push(slash);
		


	}
}

void ABoss::PutObjectPool()
{
	FVector Loc = SlashPoint->GetComponentLocation();

	if (BossSlashPool.Num() <= 1) return;
	ABossSlash* spawnedSlash = BossSlashPool[PoolIndex++];
	if (spawnedSlash != nullptr)
	{	
		BossSlashPool.Pop(0);
		spawnedSlash->SetActive(true);
		spawnedSlash->SetActorLocation(Loc);
		PoolIndex %= BossSlashPool.Num();
	}

}

void ABoss::LionRoar()
{

}

void ABoss::Telekinesis()
{

}


void ABoss::ApplyDamage()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("ApplyDamage")));

}


void ABoss::OnTakeDamage()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("OnTakeDamage")));

}

void ABoss::Die()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("Die")));
}

// 보스가 구독한 인터페이스 내용 Interface
void ABoss::OnDamage_Interface(float DamageAmount)
{
	if (BossFSM->mBossState == EEnemyState::Idle)
	{
		//hp -= DamageAmount; Idle 상태에서만 맞게하려다가 너무 안죽어서 주석처리
	}


	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("%f"), hp));

	hp -= DamageAmount;

	if (hp >= 1 && BossFSM != nullptr && BossAnim != nullptr && BossFSM->mBossState != EEnemyState::Move && BossFSM->mBossState != EEnemyState::Blink && BossFSM->mBossState != EEnemyState::Dead)
	{
		BossFSM->hitcnt = (BossFSM->hitcnt + 1) % 3;
		BossAnim->bhitcnt = BossFSM->hitcnt;
		if (BossFSM->mBossState != EEnemyState::Attack)
		{
			BossFSM->mBossState = EEnemyState::Damage;
			BossAnim->BossAnimState = EEnemyState::Damage;
		}

		OnHitOverlayMaterial();
	}
	if (hp <1 && BossFSM != nullptr && BossFSM->mBossState != EEnemyState::Dead)
	{
		//hp -= DamageAmount;
		//OnDieOverlayMaterial();
		BossFSM->mBossState = EEnemyState::Dead;
		BossAnim->BossAnimState = EEnemyState::Dead;;
		hp = 0; // 나중에 문제될 수 있으니 조심


		/*TArray<AActor*> FoundEnemy;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), FoundEnemy);
		for (AActor* Actor : FoundEnemy)
		{
			ABaseEnemy* Enemy = Cast<ABaseEnemy>(Actor);
			if (Enemy)
			{
				Enemy->Destroy();

			}
		}*/

		//게임끝내는 로직 게임 인스턴스에서 에너미 0명 되면
	}
	//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("%f"), hp));
}

void ABoss::OnHitOverlayMaterial() //tick이 아니라 한번만 발동되어야 해서 넉백 플래그 안에서 호출
{
	if (HitOverlayMaterialInstance != nullptr)
	{

		// 오버레이 재질 적용
		BossMesh->SetRenderCustomDepth(true);
		BossMesh->SetOverlayMaterial(HitOverlayMaterialInstance);
		// 원래 재질로 복원하기 위해 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ResetMaterial_FromHit, this, &ABoss::ResetMaterial, 0.5f, false);

	}

}

void ABoss::OnDieOverlayMaterial() //tick이 아니라 한번만 발동되어야 해서 넉백 플래그 안에서 호출
{
	if (DieOverlayMaterialInstance != nullptr)
	{
		
		// 오버레이 재질 적용
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ResetMaterial_FromHit);
		BossMesh->SetRenderCustomDepth(true);
		BossMesh->SetOverlayMaterial(DieOverlayMaterialInstance);

	}

}

void ABoss::ResetMaterial()
{
	// 원래 재질로 복원
	BossMesh->SetOverlayMaterial(OriginalMaterial);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_StopAnim);
}

void ABoss::SpawnBlinkVFX()
{
	if (BlinkVFX)
	{
		UNiagaraComponent* NiagaraComponent = UNiagaraFunctionLibrary::SpawnSystemAtLocation(
			GetWorld(),
			BlinkVFX,
			GetActorLocation()+GetActorForwardVector()*100 + GetActorUpVector()* NiagaraRibbonComponent03->GetComponentLocation().Z,
			GetActorRotation()
		);

		//if (NiagaraComponent)
		//{
		//	NiagaraComponent->SetAsset(MySystem);
		//	NiagaraComponent->SetRelativeScale3D(FVector(1.0f)); // 스케일 설정
		//	NiagaraComponent->SetPaused(false); // 일시 정지 상태 해제
		//	NiagaraComponent->Activate(true); // 활성화
		//}
	}

}

void ABoss::OnAttackOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)

{
	if (OtherActor->IsA(ASlashProjectile::StaticClass()))
	{
		SetActorEnableCollision(false);

		FTimerHandle TimerSlashBugfix;
		GetWorld()->GetTimerManager().SetTimer(TimerSlashBugfix, [this, OtherActor]()
			{
				if (IsValid(OtherActor))
				{

					SetActorEnableCollision(true);

				}

			}, 1.0f, false);

	}
	

}