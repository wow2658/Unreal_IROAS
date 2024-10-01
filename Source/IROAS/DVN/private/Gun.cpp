

#include "Gun.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "BattlePlayer.h"
#include "EnergyBullet.h"
#include "GrabComp.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include <Components/BoxComponent.h>
#include <Animation/AnimationAsset.h>
#include <Components/TextRenderComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include <GameFramework/PlayerController.h>


AGun::AGun()
{
	PrimaryActorTick.bCanEverTick = true;
	
	weaponType = EWeaponType::Gun;

	gunMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMesh"));
	gunMesh->SetupAttachment(RootComponent);
	firePos = CreateDefaultSubobject<USceneComponent>(TEXT("FirePosition"));
	firePos->SetupAttachment(gunMesh);
	reloadPos = CreateDefaultSubobject<UBoxComponent>(TEXT("ReloadPosition"));
	reloadPos->SetupAttachment(gunMesh);
	bulletGaugeBar1 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletGaugeBar1"));
	bulletGaugeBar2 = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletGaugeBar2"));
	bulletGaugeNum = CreateDefaultSubobject<UTextRenderComponent>(TEXT("BulletGaugeNum"));
	bulletReloadArrowRight = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletReloadArrowRight"));
	bulletReloadArrowLeft = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BulletReloadArrowLeft"));


	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Game/YSL/Assets/Gun_1"));
	if (tempMesh.Succeeded()) {
		gunMesh->SetSkeletalMesh(tempMesh.Object);
		gunMesh->SetRelativeRotation(FRotator(0.f, -90.f, 0.f));
		grabComp->SetupAttachment(gunMesh);
		grabComp->SetRelativeLocationAndRotation(FVector(-0.f, -9.f, -0.7f), FRotator(0.f, 90.f, 0.f));
		firePos->SetRelativeLocationAndRotation(FVector(0.f, 43.f, 9.f), FRotator(0.f, 90.f,  0.f));
		bulletGaugeBar1->SetupAttachment(gunMesh);
		bulletGaugeBar1->SetRelativeLocationAndRotation(FVector(5.3f, -3.3f, 8.0f), FRotator(0.f, 90.f,  -90.f));
		bulletGaugeBar2->SetupAttachment(gunMesh);
		bulletGaugeBar2->SetRelativeLocationAndRotation(FVector(-5.3f, -3.3f, 8.0f), FRotator(0.f, 90.f,  90.f));
		bulletGaugeNum->SetupAttachment(gunMesh);
		bulletGaugeNum->SetRelativeLocationAndRotation(FVector(0.f, 0.f, 18.0f), FRotator(0.f, -90.f,  0.f));
		bulletGaugeNum->SetRelativeScale3D(FVector(0.3f));
		bulletReloadArrowRight->SetupAttachment(gunMesh);
		bulletReloadArrowRight->SetRelativeLocationAndRotation(FVector(-10.f, 0.f, 8.f), FRotator(-90, 0, -90));
		bulletReloadArrowLeft->SetupAttachment(gunMesh);
		bulletReloadArrowLeft->SetRelativeLocationAndRotation(FVector(10.f, 0.f, 8.f), FRotator(90, 0, 90));
	}
	ConstructorHelpers::FObjectFinder<UParticleSystem> ParticleSystemAsset(TEXT("/Script/Engine.ParticleSystem'/Game/Assets/ParagonRevenant/FX/Particles/Revenant/Abilities/Primary/FX/P_Revenant_Primary_MuzzleFlash.P_Revenant_Primary_MuzzleFlash'"));
	if (ParticleSystemAsset.Succeeded())
	{
		muzzleFX_1 = ParticleSystemAsset.Object;
	}

	attackEnergy = 2.0f;
	Tags.Add(FName("Gun"));
}

void AGun::BeginPlay()
{
	Super::BeginPlay();
	SetBulletCount(maxBulletNum);

	// bulletPool
	for (int32 i = 0; i < bulletPoolSize; i++) {
		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform spawnTransform;
		spawnTransform.SetLocation(FVector(0.f, 0.f, 1000.f));
		AEnergyBullet* bullet = GetWorld()->SpawnActorDeferred<AEnergyBullet>(energyBullet_bp, spawnTransform);
		if (bullet != nullptr) {
			bullet->firingGun = this;
			bullet->bulletDamage = gunDamage;
			bullet->speed = bulletSpeed;
			bulletPool.Enqueue(bullet);
		}
		bullet->FinishSpawning(spawnTransform);
	}

	bulletGaugeBar1->AttachToComponent(gunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("bulletGaugeBar1"));
	bulletGaugeBar2->AttachToComponent(gunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("bulletGaugeBar2"));
	bulletGaugeNum->AttachToComponent(gunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("bulletGaugeNum"));
	bulletGaugeNum->SetText(FText::AsNumber(maxBulletNum));
	bulletReloadArrowRight->AttachToComponent(gunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("bulletArrowRight"));
	bulletReloadArrowRight->SetVisibility(false);
	bulletReloadArrowRight->bHiddenInGame = true;
	bulletReloadArrowLeft->AttachToComponent(gunMesh, FAttachmentTransformRules::SnapToTargetNotIncludingScale, FName("bulletArrowLeft"));
	bulletReloadArrowLeft->SetVisibility(false);
	bulletReloadArrowLeft->bHiddenInGame = true;

	bIsActive = false;
	SetActorHiddenInGame(true);
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	gunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	gun_ABP = Cast<UAnimInstance>(gunMesh->GetAnimInstance());
}

void AGun::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	if (!bIsActive) return;

	if (bulletCount < maxBulletNum && !bInReload)
		CheckSpeed(DeltaTime);

	// reload 블링크
	if (bulletCount == 0 && blink) {
		blink = false;

		/*if (bulletReloadArrowLeft->bHiddenInGame)
			bulletReloadArrowLeft->bHiddenInGame = false;
		if (bulletReloadArrowRight->bHiddenInGame)
			bulletReloadArrowRight->bHiddenInGame = false;*/
		
		FTimerHandle flashHandle;
		GetWorldTimerManager().SetTimer(flashHandle, FTimerDelegate::CreateLambda([&]() {
			/*if (bisRightHand)
				bulletReloadArrowRight->ToggleVisibility();
			else
				bulletReloadArrowLeft->ToggleVisibility();*/
			if (bulletGaugeNum != nullptr) 
				bulletGaugeNum->ToggleVisibility();
			//GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Blue, FString::Printf(TEXT("5555 blink")));
			blink = true;
			}), 0.3f, false);
	}

	// Toggle Visibility 오류를 잡기 위한 코드
	if (!bulletGaugeNum->IsVisible() && bulletCount != 0)
		bulletGaugeNum->SetVisibility(true);
	
	//GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Green, FString::Printf(TEXT("gunDamage : %.2f"), gunDamage));
}

void AGun::Attack()
{
	//GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Red, FString::Printf(TEXT("bInFire: %s"), bInFire ? TEXT("True") : TEXT("False")));
	if (!bIsActive) return;
	//if (GetWorldTimerManager().IsTimerActive(fireDelayTimer)) return;
	if (bInFire || bInDelay || bulletPool.IsEmpty()) return;
	
	if (owningPlayer != nullptr && bulletCount > 0) {
		if (bisRightHand) {
			owningPlayer->pc->PlayHapticEffect(fireHaptic, EControllerHand::Right, 1.f, false);
		}
		else if (!bisRightHand) {
			owningPlayer->pc->PlayHapticEffect(fireHaptic, EControllerHand::Left, 1.f, false);
		}
		// 총알 발사

		AEnergyBullet* bullet;
		bulletPool.Dequeue(bullet);

		if (bullet != nullptr) {
			bInFire = true;
			FTransform fireTrans = firePos->GetComponentTransform();
			fireTrans.SetScale3D(FVector(0.3f));
			bullet->SetActorTransform(fireTrans, false, nullptr, ETeleportType::TeleportPhysics);
			bullet->SetActive(true);

			gunMesh->PlayAnimation(recoilGunAnimation, false);
			bulletCount -= 1;
			SetWidget(bulletCount);

			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), muzzleFX_1, fireTrans);
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), fireSound, fireTrans.GetLocation());
		}
		
		
		// 딜레이
		bInDelay = true;
		GetWorldTimerManager().SetTimer(fireDelayTimer, FTimerDelegate::CreateLambda([&]() {
			bInDelay = !bInDelay;
			bInFire = false;
		}), fireDelay, false);
	}
	else if (owningPlayer != nullptr && bulletCount < 1) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), emptySound, GetActorLocation());
		bInDelay = true;
		FTimerHandle soundTimer;
		GetWorldTimerManager().SetTimer(soundTimer, FTimerDelegate::CreateLambda([&]() {
			bInDelay = !bInDelay;
			}), 0.5f, false);
	}
}

void AGun::CheckSpeed(float Deltatime)
{	
	if (owningPlayer == nullptr) return;

	if (!bReloadPos) {
		// 초기 회전값 저장
		initReloadRot = reloadPos->GetComponentRotation();
		bReloadPos = true; // 장전될 경우 false
	}
	// 현재 회전값 저장
	FRotator curReloadRot = reloadPos->GetComponentRotation();
	FRotator reloadPosDiff = curReloadRot - initReloadRot;
	// 각속도 측정
	FVector reloadAngularVelocity = reloadPosDiff.Euler() / Deltatime;

	if (bisRightHand) {
		if (reloadAngularVelocity.Y >= rightHandReload) Reload();
		else return;
	}
	else {
		if (reloadAngularVelocity.Y <= leftHandReload) Reload();
		else return;
	}
}

void AGun::Reload()
{
	if (bulletCount == 6) return;

	if (owningPlayer != nullptr && owningPlayer->UseEnergy(energyCost)) {
		bInReload = true;
		GetWorldTimerManager().SetTimer(reloadDelayTimer, FTimerDelegate::CreateLambda([&]() {
			gunMesh->PlayAnimation(reloadAnimation, false);
			bulletCount = 6;
			//SetReloadWidget();
			bInReload = false;
			bReloadPos = false;
			//GEngine->AddOnScreenDebugMessage(1, 3.0f, FColor::Red, FString::Printf(TEXT("1111 reload")));
			SetWidget(bulletCount);
			}), 0.1f, false);
	}
}

void AGun::IncreaseBulletDamage(float bulletDamage)
{
	if (bulletPool.IsEmpty()) return;
	AEnergyBullet* bullet = nullptr;
	for (int32 i = 0; i < bulletPoolSize; i++) {
		bullet = nullptr;
		bulletPool.Dequeue(bullet);
		bullet->bulletDamage += bulletDamage;
		bulletPool.Enqueue(bullet);
	}
}

void AGun::SetWidget(int32 bulletNum)
{
	if (bulletNum == 0)
		bulletGaugeNum->SetText(FText::FromString(FString::Printf(TEXT("Reload"))));
	else {
		bulletGaugeNum->SetVisibleFlag(true);
		bulletGaugeNum->SetText(FText::AsNumber(bulletNum));
	}

	float gaugeSize = bulletNum*0.01;
	bulletGaugeBar1->SetRelativeScale3D(FVector(gaugeSize, 0.05, 0.06));
	bulletGaugeBar2->SetRelativeScale3D(FVector(gaugeSize, 0.05, 0.06));
	GEngine->AddOnScreenDebugMessage(2, 3.0f, FColor::Green, FString::Printf(TEXT("4444 setwidget"), gunDamage));
}

void AGun::SetActive(bool activate)
{
	bIsActive = activate;

	if (activate) {
		// 소환되는 이펙트
		gunMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		if (spawnSound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), spawnSound, GetActorLocation());
		}
		if (spawnFX != nullptr) {
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), spawnFX, grabComp->GetComponentLocation());
		}
	}
	else {
		// 사라지는이펙트
		if (releaseSound != nullptr)
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), releaseSound, GetActorLocation());
		if (releaseFX != nullptr)
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), releaseFX, GetActorLocation());

		gunMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		SetActorLocation(FVector(0.f, 0.f, 1000.f), false, nullptr);
		if (owningPlayer != nullptr) owningPlayer->gunPool.Enqueue(this);
	}

	SetActorHiddenInGame(!activate);
}

void AGun::SetReloadWidget()
{
	bulletReloadArrowLeft->bHiddenInGame = true;
	bulletReloadArrowRight->bHiddenInGame = true;
}
