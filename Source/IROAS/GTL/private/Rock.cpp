// Fill out your copyright notice in the Description page of Project Settings.


#include "Rock.h"
#include "Kismet/GameplayStatics.h"
#include "Boss.h"
#include "BossFSM.h"
#include "BossAnim.h"
#include "BattlePlayer.h"
#include "RageTriggerBox.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"

ARock::ARock()
{
	// 콜리전
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	RootComponent = BoxCollision;
	BoxCollision->SetBoxExtent(FVector(90.f,100.f,80.f));


	// 스태틱메시 할당 (임시)
	RockMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("RockMesh"));
	RockMesh->SetupAttachment(BoxCollision);
	ConstructorHelpers::FObjectFinder<UStaticMesh> TempRockMesh(TEXT("/Script/Engine.StaticMesh'/Game/GTL/SkillContent/Rock_megaScanAssets/S_Mossy_Rock_tjjubfufa_lod0.S_Mossy_Rock_tjjubfufa_lod0'"));
	if (TempRockMesh.Succeeded())
	{
		RockMesh->SetStaticMesh(TempRockMesh.Object);
		RockMesh->SetRelativeLocation(FVector(-15.f, 25.5f, -45.f));
		RockMesh->SetRelativeRotation(FRotator(5.5f, 50.f, -35.f));
		RockMesh->SetRelativeScale3D(FVector(0.8f));
	}

	// speed 재정의
	speed = 5000;
	attackDamage = 10.0f;
}

void ARock::BeginPlay()
{
	Super::BeginPlay();

	// 보스 인식
	Boss = Cast<ABoss>(UGameplayStatics::GetActorOfClass(GetWorld(), ABoss::StaticClass()));
	if (Boss != nullptr)
	{
		BossAnim = Cast<UBossAnim>(Boss->BossMesh->GetAnimInstance());
	}


	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ARock::OnComponentBeginOverlap);

	if (Boss != nullptr && Boss->BossFSM != nullptr)
	{
		Boss->BossFSM->currentTime = 0;
	}

	SetLifeSpan(6.f);


	rageTriggerBox = Cast<ARageTriggerBox>(UGameplayStatics::GetActorOfClass(GetWorld(), ARageTriggerBox::StaticClass()));
	if (rageTriggerBox != nullptr)
	{
		attackDamage = rageTriggerBox->rockDamage;
		speed = rageTriggerBox->rockSpeed;
	}
}



void ARock::Tick(float DeltaTime)
{
	// 부모Tick 거부

	// speed에 가속을 적용
	currentTime += DeltaTime;
	float SizeDelay = 1.5f;
	if (currentTime<= SizeDelay)
	{
		FVector BeforeSize = FVector(5.f);
		FVector AfterSize = FVector(1.f);
		float RockSizeAlpha = 0;
		RockSizeAlpha += currentTime / SizeDelay;

		FVector NewRockSize = FMath::Lerp(BeforeSize, AfterSize, RockSizeAlpha);

		SetActorRelativeScale3D(NewRockSize);
		SetActorRotation(Boss->GetActorRotation());
	}
	speed += 1000 * GetWorld()->DeltaTimeSeconds;
	if (BossAnim != nullptr && Boss!=nullptr && Boss->inAction && !BossAnim->inAction_Anim)
	{

		// 애니메이션 발동
		BossAnim->inAction_Anim = true;
		Boss->inAction = false;

	}

	//if (Boss->BossFSM->currentTime > RockDelayTime)
	if (BossAnim != nullptr && BossAnim->CanGoRock)
	{
		FVector P0 = GetActorLocation();

		if (Boss!=nullptr)
		{
			float DistanceBossAndTarget = FVector::DistXY(Boss->GetActorLocation(), Boss->Players[0]->GetActorLocation());
			float RockPositionZ = Boss->RockPoint->GetComponentLocation().Z - Boss->target->camera->GetComponentLocation().Z; //261.251269f? 플레이어 VR Root 눈높이었던것으로 기억

			// DistanceBossAndTarget:RockPositionZ = 1:x 
			float targetDistanceRatio = RockPositionZ / DistanceBossAndTarget;


			// 대각선 아래 방향으로 speed의 속도로 날아가게 구현
			FVector vt = (GetActorForwardVector() + GetActorUpVector() * -targetDistanceRatio) * speed * DeltaTime;
			FVector P = P0 + vt;
			SetActorLocation(P);
		}


	}

}




// Rock이 파괴됐을 경우 IdleState로 전환
void ARock::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	if (EndPlayReason == EEndPlayReason::Destroyed)
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Blue, FString::Printf(TEXT("Destroyed Rock")));
		if (Boss != nullptr)
		{
			if (Boss->BossFSM->mBossState != EEnemyState::Dead)
			{
				Boss->BossFSM->mBossState = EEnemyState::Idle;
				BossAnim->BossAnimState = EEnemyState::Idle;
				Boss->BossFSM->currentTime = 0;
				BossAnim->inAction_Anim = false;
				BossAnim->CanGoRock = false;

			}
			
		}
	}
}


void ARock::OnComponentBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	Super::OnComponentBeginOverlap(OverlappedComponent, OtherActor, OtherComp, OtherBodyIndex, bFromSweep, SweepResult);

	Boss->target->HitbyBossSound();
}
