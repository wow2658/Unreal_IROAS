

#include "Sword.h"
#include "Components/BoxComponent.h"
#include <Components/StaticMeshComponent.h>
#include <Components/SphereComponent.h>
#include "BattlePlayer.h"
#include "GrabComp.h"
#include "OnDamage_Interface.h"
#include <Components/ArrowComponent.h>
#include "Projectile.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <Kismet/GameplayStatics.h>
#include "SlashProjectile.h"
#include "RangedEnemyProjectile.h"
#include "NiagaraFunctionLibrary.h"
#include <Kismet/KismetMathLibrary.h>
#include <GameFramework/PlayerController.h>


ASword::ASword()
{
	PrimaryActorTick.bCanEverTick = true;

	weaponType = EWeaponType::Sword;

	//defaultSceneRoot->RemoveFromRoot();

	//meshComp->SetupAttachment(nullptr);
	//SetRootComponent(meshComp);

	/*handleRoot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("HandleRoot"));
	SetRootComponent(handleRoot);
	handleRoot->SetCollisionProfileName(FName("PhysicsActor"));
	handleRoot->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Ignore);
	handleRoot->SetCollisionResponseToChannel(ECC_WorldStatic, ECR_Ignore);
	handleRoot->SetCollisionResponseToChannel(ECC_WorldDynamic, ECR_Ignore);
	handleRoot->SetCollisionResponseToChannel(ECC_Pawn, ECR_Ignore);
	handleRoot->SetSimulatePhysics(false);

	meshComp->SetupAttachment(handleRoot);*/

	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/DVN/Assets/Weapon/sword/SM_Sword_1.SM_Sword_1'"));
	if (tempMesh.Succeeded()) {
		meshComp->SetStaticMesh(tempMesh.Object);
		meshComp->SetRelativeRotation(FRotator(0.f, 0.f, -90.f));
		grabComp->SetRelativeLocationAndRotation(FVector(0.f, -50.f, 0.f), FRotator(0.f, 0.f, 90.f));
		meshComp->SetNotifyRigidBodyCollision(true);
		meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		meshComp->SetCollisionProfileName(FName("PhysicsBody"));
	}
	


	bladeCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BladeCollision"));
	bladeCollision->SetupAttachment(meshComp);
	bladeCollision->SetCollisionProfileName("PlayerAttack");
	bladeCollision->SetBoxExtent(FVector(2.7f, 55.f, 1.3f));
	bladeCollision->SetRelativeLocationAndRotation(FVector(0.f, 31.f, 0.f), FRotator(0.f, 0.f, 0.f));
	bladeCollision->bMultiBodyOverlap = true;


	slashDir = CreateDefaultSubobject<UArrowComponent>(TEXT("SlashDirection"));
	slashDir->SetupAttachment(meshComp);
	slashDir->SetRelativeRotation(FRotator(0.f, 100.f, -90.f));

	slashEffect = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SlashVFX"));
	slashEffect->SetupAttachment(meshComp);
	slashEffect->SetRelativeLocationAndRotation(FVector(0.f, -60.f, 0.f), FRotator(0.f, 90.f, 90.f));
	slashEffect->SetAutoActivate(false);
	ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempNS(TEXT("/Script/Niagara.NiagaraSystem'/Game/Assets/NiagaraSwordAndShield/FX/Sword/NS_SwordFx_03.NS_SwordFx_03'"));
	if (tempNS.Succeeded()) {
		slashEffect->SetAsset(tempNS.Object);
	}
	
	swordTrail = CreateDefaultSubobject<UNiagaraComponent>(TEXT("SwordTrailVFX"));
	swordTrail->SetupAttachment(meshComp);
	swordTrail->SetRelativeLocation(FVector(0.f, 50.f, 0.f));
	swordTrail->SetVisibility(false);

	//grabComp->simulateOnDrop = true;

	attackEnergy = energyCost;
	Tags.Add(FName("Sword"));
}
 
void ASword::BeginPlay()
{
	Super::BeginPlay();
	bladeCollision->OnComponentBeginOverlap.AddDynamic(this, &ASword::OnBladeCollided);
	//meshComp->OnComponentHit.AddDynamic(this, &ASword::OnMeshComponentHit);
	
	// slashPool
	for (int32 i = 0; i < 6; i++) {
		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

		ASlashProjectile* slash = GetWorld()->SpawnActor<ASlashProjectile>(bp_testprojectile, Param);
		slash->owningSword = this;
		slash->attackDamage = slashDamage;
		slash->speed = slashProjectileSpeed;
		slashPool.Enqueue(slash);

	}

	//bIsActive = false;
	SetActorHiddenInGame(!bIsActive);
	SetActorEnableCollision(bIsActive);

}

void ASword::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!bIsActive) return;

	FVector slashVelocity = bladeCollision->GetPhysicsLinearVelocity();
	if(slashVelocity.Size() > minSwingSpeed) {
		PlaySwingSound();
		swordTrail->SetVisibility(true);
	}
	else swordTrail->SetVisibility(false);

	if (bIsSlashMode && !bIsCoolTime) {
		topSwingSpeed = FMath::Max<float>(slashVelocity.Size(), topSwingSpeed);
	}

	GEngine->AddOnScreenDebugMessage(3, 3.0f, FColor::Green, FString::Printf(TEXT("swordDamage : %.2f, slashDamage : %.2f"), normalAttackDamage, slashDamage));
}

void ASword::OnBladeCollided(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!bIsActive) return;
	IOnDamage_Interface* OnDamage = Cast<IOnDamage_Interface>(OtherActor);
	if (OnDamage != nullptr)
	{
		/*
		if (Cast<ARangedEnemyProjectile>(OtherActor) != nullptr) {
			FRotator inversedDir = OtherActor->GetActorForwardVector().ToOrientationRotator().GetInverse();
			OtherActor->SetActorRotation(inversedDir);
		}
		*/		
		OnDamage->OnDamage_Interface(normalAttackDamage);
		AbsorbEnergy();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), sliceSounds, bladeCollision->GetRelativeLocation());
		if (owningPlayer != nullptr) {
			owningPlayer->pc->PlayHapticEffect(swordSliceHaptic, bisRightHand ? EControllerHand::Right : EControllerHand::Left, 1.f, false);
		}
		if (hitEffect != nullptr) {
			FVector effectLoc = OtherComp->GetComponentLocation();
			//FRotator effectRot = UKismetMathLibrary::FindLookAtRotation(OtherComp->GetComponentLocation(), owningPlayer->GetCameraLoc());
			//effectRot.Yaw -= 90.f;
			
			FRotator effectRot = meshComp->GetComponentRotation();
			
			FTransform effectTrans;
			effectTrans.SetLocation(effectLoc);
			effectTrans.SetRotation(effectRot.Quaternion());
			UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), hitEffect, effectTrans);

			//UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), hitEffect, effectLoc, effectRot);
		}
		
	}
	else if (Cast<ARangedEnemyProjectile>(OtherActor) != nullptr) {
		ARangedEnemyProjectile* enemyProjectile = Cast<ARangedEnemyProjectile>(OtherActor);
		
		FVector inversedDir = -enemyProjectile->GetActorForwardVector();
		enemyProjectile->SetActorRotation(inversedDir.Rotation());
		enemyProjectile->magicCollision->SetCollisionProfileName("PlayerAttack");
	}
	/*else if (Cast<AWeapon>(OtherActor) != nullptr) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), clashSounds_Weapons, meshComp->GetRelativeLocation());
	}
	else {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), clashSounds_Rock, meshComp->GetRelativeLocation());
	}*/
	

}

/*
void ASword::OnMeshComponentHit(UPrimitiveComponent* HitComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (OtherActor->ActorHasTag(FName("Weapon")) && OtherActor->ActorHasTag(FName("Sword"))) {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), clashSounds_Weapons, meshComp->GetRelativeLocation());
	}
	else {
		switch (OtherComp->GetCollisionObjectType())
		{
		case ECollisionChannel::ECC_WorldDynamic:
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), clashSounds_Rock, meshComp->GetRelativeLocation());
			break;
		case ECollisionChannel::ECC_WorldStatic:
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), clashSounds_Rock, meshComp->GetRelativeLocation());
			break;
		default:
			break;
		}
	}
}
*/



void ASword::PlaySwingSound()
{
	if (GetWorldTimerManager().IsTimerActive(swingSoundTimer)) return;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), swingSound, slashDir->GetRelativeLocation());
	
	float soundLength = swingSound->GetDuration();;
	GetWorldTimerManager().SetTimer(swingSoundTimer, FTimerDelegate::CreateLambda([&]() {
		soundLength = 0;
		}), soundLength, false);
}

void ASword::AbsorbEnergy()
{
	if (owningPlayer != nullptr) {
		owningPlayer->GainEnergy(absorbAmount);
	}
}

void ASword::Attack()
{
	if (!bIsActive) return;
	if (owningPlayer != nullptr) {
		if (!bIsSlashMode) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), slashModeSound, bladeCollision->GetRelativeLocation());
			owningPlayer->pc->PlayHapticEffect(slashChargeHaptic, bisRightHand ? EControllerHand::Right : EControllerHand::Left, 1.f, false);
		}
		bIsSlashMode = true;
		slashEffect->SetActive(bIsSlashMode);
	
	}
}

void ASword::SlashAttack(const FTransform& slashTrans)
{
	
	if (owningPlayer != nullptr && owningPlayer->UseEnergy(energyCost)) {
		// 발사

		ASlashProjectile* slash;
		slashPool.Dequeue(slash);

		if (slash != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), slashSound, slashTrans.GetLocation());
			
			slash->SetActorLocation(slashTrans.GetLocation());
			slash->SetActorRotation(slashTrans.GetRotation());
			slash->SetActive(true);

		}

	}
}

void ASword::EndSlash()
{
	if (!bIsActive) return;
	if (topSwingSpeed > minSlashSpeed) {
		SlashAttack(slashDir->GetComponentTransform());
	}
	bIsSlashMode = false;
	slashEffect->SetActive(bIsSlashMode);
	
	topSwingSpeed = 0.f;
}

void ASword::SetActive(bool activate)
{
	bIsActive = activate;

	if (activate) {
		// 소환되는 이펙트
		/*
		meshComp->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		bladeCollision->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		*/
		if (spawnSound != nullptr) {
			UGameplayStatics::PlaySoundAtLocation(GetWorld(), spawnSound, GetActorLocation());
		}
	}
	else {
		// 사라지는이펙트
		/*
		meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		bladeCollision->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		*/
		SetActorLocation(FVector(0.f, 0.f, -5000.f), false, nullptr, ETeleportType::TeleportPhysics);
		if (owningPlayer != nullptr) owningPlayer->swordPool.Enqueue(this);
	}
	
	//meshComp->SetSimulatePhysics(activate);
	SetActorEnableCollision(activate);
	SetActorHiddenInGame(!activate);
}
