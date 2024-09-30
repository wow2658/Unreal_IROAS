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
	// �ݸ���
	SlashCollision = CreateDefaultSubobject<UCapsuleComponent>(TEXT("SlashCollision"));
	SetRootComponent(SlashCollision);
	SlashCollision->SetCapsuleHalfHeight(100.f);
	SlashCollision->SetCapsuleRadius(50.f);
	SlashCollision->SetCollisionProfileName(FName("EnemyAttack"));


	////����ƽ�޽� �Ҵ� (�ӽ�)
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

	// �θ��� speed ������
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

	// �θ� Tick ��� : ��� �����θ� ��

	// ���ư��� ���� Dir�� Boss���� Slash�� �ٶ󺸴� ������ �ν�, ����ȭ
	FVector Dir = (GetActorLocation() - Boss->GetActorLocation()).GetSafeNormal();
	FRotator NewRotation = UKismetMathLibrary::MakeRotFromXZ(Dir, GetActorUpVector());

	// Z�� ȸ���� ����, Pitch�� Roll�� ������ ȸ������ ����
	NewRotation.Pitch = GetActorRotation().Pitch;
	NewRotation.Roll = GetActorRotation().Roll;
	// �ν��ص״� Dir�� Slash�� ������ ��� ���� 
	SetActorRotation(NewRotation);
}

void ABossSlash::SetActive(bool isActive)
{
	//SlashMesh->SetVisibility(isActive);

	SetActorHiddenInGame(!isActive);

	if (isActive)
	{
		// Ȱ��ȭ������
		bIsActive = true;
		SlashCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	}
	else
	{
		// ��Ȱ��ȭ���� ��
		currentTime = lifeSpan;
		bIsActive = false;
		SlashCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		if (Boss != nullptr)
		{
			Boss->BossSlashPool.Push(this);

		}
	}
}