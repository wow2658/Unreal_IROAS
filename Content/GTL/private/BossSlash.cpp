// Fill out your copyright notice in the Description page of Project Settings.


#include "BossSlash.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Boss.h"
#include "BossFSM.h"
#include "BossAnim.h"
#include "Kismet/KismetMathLibrary.h"

ABossSlash::ABossSlash()
{
	// 콜리전
	SlashCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SlashCollision"));
	SetRootComponent(SlashCollision);
	SlashCollision->SetCapsuleHalfHeight(100.f);
	SlashCollision->SetCapsuleRadius(50.f);
	SlashCollision->SetCollisionProfileName(FName("EnemyAttack"));


	////스태틱메시 할당 (임시)
	//SlashMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("SlashMesh"));
	//SlashMesh->SetupAttachment(SlashCollision);
	//SlashMesh->SetCollisionProfileName(FName("NoCollision"));
	//SlashMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//ConstructorHelpers::FObjectFinder<UStaticMesh> TempSlashMesh(TEXT("/Script/Engine.StaticMesh'/Game/EngineContents/FPWeapon/Mesh/FirstPersonProjectileMesh.FirstPersonProjectileMesh'"));
	//if (TempSlashMesh.Succeeded())
	//{
	//	SlashMesh->SetStaticMesh(TempSlashMesh.Object);
	//	SlashMesh->SetRelativeLocationAndRotation(FVector(0, 0, -100.f), FRotator(0, -90.f, 0));
	//	SlashMesh->SetRelativeScale3D(FVector(1.f));
	//}

	// 부모의 speed 재정의
	speed = slashSpeed;
	lifeSpan = slashLifeSpan;
	attackDamage = slashDamage;


}

void ABossSlash::BeginPlay()
{
	Super::BeginPlay();

	SlashCollision->OnComponentBeginOverlap.AddDynamic(this, &ABossSlash::OnComponentBeginOverlap);

	if (Boss != nullptr)
	{
		BossAnim = Cast<UBossAnim>(Boss->BossMesh->GetAnimInstance());
	}
	SetActorLocation(GetActorLocation()+FVector(1000,1000,-1000));
	SetActive(false);
	//SetLifeSpan(4.f);
}


void ABossSlash::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 부모 Tick 사용 : 기능 앞으로만 감

	// 날아가는 방향 Dir을 Boss에서 Slash를 바라보는 방향을 인식, 정규화
	FVector Dir = (GetActorLocation() - Boss->GetActorLocation()).GetSafeNormal();
	FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(Dir, GetActorUpVector());

	// Z축 회전만 고정, Pitch와 Roll은 현재의 회전값을 유지
	NewRotation.Pitch = GetActorRotation().Pitch;
	NewRotation.Roll = GetActorRotation().Roll;
	// 인식해뒀던 Dir로 Slash의 방향을 계속 조절 
	SetActorRotation(NewRotation);
}

void ABossSlash::SetActive(bool isActive)
{
	//SlashMesh->SetVisibility(isActive);

	SetActorHiddenInGame(!isActive);

	if (isActive)
	{
		// 활성화됬을때
		bIsActive = true;
		SlashCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		// 비활성화됬을 때
		currentTime = lifeSpan;
		bIsActive = false;
		SlashCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (Boss != nullptr)
		{
			Boss->BossSlashPool.Push(this);

		}
	}
}