

#include "PlayerHand.h"
#include <HeadMountedDisplay/Public/MotionControllerComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include <Components/SphereComponent.h>
#include "PlayerHandAnim.h"
#include <Kismet/KismetSystemLibrary.h>
#include "GrabComp.h"
#include "BattlePlayer.h"
#include "Weapon.h"
#include "Sword.h"
#include "Gun.h"
#include <DrawDebugHelpers.h>
#include "LobbyGameMode.h"
#include "LobbyTutorialActor.h"
#include <PhysicsEngine/PhysicsHandleComponent.h>
#include "LobbyGameMode.h"
#include "LobbyTutorialActor.h"
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include <Kismet/GameplayStatics.h>
#include <GameFramework/PlayerController.h>

APlayerHand::APlayerHand()
{
	PrimaryActorTick.bCanEverTick = true;

	gripSocket = CreateDefaultSubobject<USceneComponent>(TEXT("GripSocket"));
	gripSocket->SetupAttachment(motionController);

	offsetComp = CreateDefaultSubobject<USceneComponent>(TEXT("OffsetComponent"));
	offsetComp->SetupAttachment(motionController);
	offsetComp->SetRelativeLocation(FVector(0.f, -2.f, -8.f));
	offsetComp->SetRelativeRotation(FRotator(-85.f, 0.f, 0.f));

	handMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("HandMesh"));
	handMesh->SetupAttachment(gripSocket);

	handSocket = CreateDefaultSubobject<USceneComponent>(TEXT("HandSocket"));
	handSocket->SetupAttachment(gripSocket);
	handSocket->SetRelativeLocation(FVector(-10, 0, 0));
	handSocket->SetRelativeRotation(FRotator(0, 0, 90));

	handDefaultArea = CreateDefaultSubobject<USphereComponent>(TEXT("HandDefaultArea"));
	handDefaultArea->SetupAttachment(handSocket);
	handDefaultArea->SetGenerateOverlapEvents(true);
	handDefaultArea->SetSphereRadius(5.f);
	handDefaultArea->SetRelativeLocation(FVector(10.f, 0.f, 0.f));

	physicsHandle = CreateDefaultSubobject<UPhysicsHandleComponent>(TEXT("PhysicsHandle"));
	physicsHandle->bSoftAngularConstraint = false;
	physicsHandle->bSoftLinearConstraint = false;
	physicsHandle->bInterpolateTarget = false;


	//motionController->bDisableLowLatencyUpdate = true;
}

void APlayerHand::BeginPlay()
{
	Super::BeginPlay();
	
	InitializeVRHand();

}

void APlayerHand::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	if (bIsWeaponSpawnMode) {
		// 이전 프레임과의 위치 차이를 활용한 속도측정
		float distance = FVector::Dist(motionController->GetRelativeLocation(), prevLoc);
		float curSpeed;

		// 손떨림으로 인해 튀는 값 방지
		if(FMath::IsNearlyZero(distance)) curSpeed = 0.0f;
		else curSpeed = distance / DeltaTime;

		// 무기 소환속도를 넘겼으면 소환한다.
		if (curSpeed > weaponSpawnSpeed) {
			SpawnWeapon(motionController->GetComponentLocation().Z);
		}
		prevLoc = motionController->GetRelativeLocation();
	}

	if (holdingWeapon != nullptr && holdingWeapon->weaponType == EWeaponType::Sword) {
		physicsHandle->SetTargetLocationAndRotation(offsetComp->GetComponentLocation(), offsetComp->GetComponentRotation());
	}
	//else if (attachedActor == nullptr && holdingWeapon == nullptr) {
	//	FHitResult hitInfo;
	//	FVector startLoc = gripSocket->GetComponentLocation();
	//	FVector endLoc = startLoc + gripSocket->GetForwardVector() * 5000.f;
	//	//DrawDebugLine(GetWorld(), startLoc, endLoc, FColor::Red);
	//	if (GetWorld()->LineTraceSingleByChannel(hitInfo, startLoc, endLoc, ECC_GameTraceChannel8)) {
	//		AItemBase* tempItem = Cast<AItemBase>(hitInfo.GetActor());
	//		if (tempItem != nullptr) {
	//			if (detectedItem != nullptr && detectedItem != tempItem) {
	//				detectedItem->OnUnhovered();
	//				detectedItem->targetPlayer = nullptr;
	//				detectedItem = nullptr;
	//			}
	//			detectedItem = tempItem;
	//			detectedItem->OnHovered();
	//			detectedItem->targetPlayer = player;
	//		}
	//	}
	//	else if (detectedItem != nullptr) {
	//		detectedItem->OnUnhovered();
	//		detectedItem->targetPlayer = nullptr;
	//		detectedItem = nullptr;
	//	}
	//}

}

void APlayerHand::SpawnWeapon(float triggeredHeight)
{
	// 그립 눌렀던 위치보다 위쪽이면 검 소환
	if (triggeredHeight > spawnDeterminationHeight) {
		// 튜토리얼 코드
		if (player->ActorHasTag("LobbyPlayer") && !tutorialSwordSpawn) return;
		
		ASword* spawnedSword;
		player->swordPool.Dequeue(spawnedSword);

		// 소환이펙트
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), weaponDrawEffect, gripSocket->GetComponentLocation());

		spawnedSword->SetActorLocation(gripSocket->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
		spawnedSword->SetActive(true);

		// 물리적용 잡기
		/*
		physicsHandle->GrabComponentAtLocationWithRotation(Cast<UPrimitiveComponent>(spawnedSword->meshComp), NAME_None, spawnedSword->grabComp->GetComponentLocation(), spawnedSword->grabComp->GetComponentRotation());

		FAttachmentTransformRules rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		rules.LocationRule = EAttachmentRule::SnapToTarget;
		rules.RotationRule = EAttachmentRule::SnapToTarget;
		rules.ScaleRule = EAttachmentRule::KeepRelative;
		rules.bWeldSimulatedBodies = true;

		// gripsocket자체를 칼에 붙이기
		gripSocket->AttachToComponent(spawnedSword->grabComp, rules);
		*/


		// grabbedComponent를 집은 다른손 초기화
		APlayerHand* grabbedHand = spawnedSword->grabComp->GetGrabbedHand();
		if (nullptr != grabbedHand && this != grabbedHand) {
			grabbedHand->SetHeldComponent(nullptr);
			grabbedHand->SetAttachedActor(nullptr);
		}

		// 잡기 시도.
		if (spawnedSword->grabComp->TryGrab(this, gripSocket) == false) {
			spawnedSword->SetActive(false);
			bIsWeaponSpawnMode = false;
			return;
		}

		// 햅틱
		player->pc->PlayHapticEffect(swordSpawnHaptic, bIsRight ? EControllerHand::Right : EControllerHand::Left, 1.f, false);

		SetHeldComponent(spawnedSword->grabComp);
		SetAttachedActor(spawnedSword->grabComp->GetOwner());
		holdingWeapon = spawnedSword;
		holdingWeapon->bisRightHand = bIsRight;
		bIsWeaponSpawnMode = false;
		tutorialSwordSpawnCount++;
	}

	// 그립 눌렀던 위치보다 아래쪽이면 총 소환
	else if (triggeredHeight < spawnDeterminationHeight) {
		
		// 튜토리얼 코드
		if (player->ActorHasTag("LobbyPlayer") && !tutorialGunSpawn) return;
		
		AGun* spawnedGun;
		player->gunPool.Dequeue(spawnedGun);

		// 소환이펙트
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), weaponDrawEffect, gripSocket->GetComponentLocation());

		spawnedGun->SetActorLocation(gripSocket->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
		spawnedGun->SetActive(true);
		spawnedGun->bisRightHand = bIsRight;

		// grabbedComponent를 집은 다른손 초기화
		APlayerHand* grabbedHand = spawnedGun->grabComp->GetGrabbedHand();
		if (nullptr != grabbedHand && this != grabbedHand) {
			grabbedHand->SetHeldComponent(nullptr);
			grabbedHand->SetAttachedActor(nullptr);
		}

		if (spawnedGun->grabComp->TryGrab(this, gripSocket) == false) {
			spawnedGun->SetActive(false);
			bIsWeaponSpawnMode = false;
			return;
		}

		// 햅틱
		player->pc->PlayHapticEffect(gunSpawnHaptic, bIsRight ? EControllerHand::Right : EControllerHand::Left, 1.f, false);

		SetHeldComponent(spawnedGun->grabComp);
		SetAttachedActor(spawnedGun->grabComp->GetOwner());
		holdingWeapon = spawnedGun;
		holdingWeapon->bisRightHand = bIsRight;
		bIsWeaponSpawnMode = false;
		tutorialGunSpawnCount++;
	}
	
}

//void APlayerHand::GetItem()
//{
//	if (detectedItem == nullptr) return;
//	if (detectedItem->targetPlayer != nullptr) return;
//	APlayerHand* grabbedHand = detectedItem->grabComp->GetGrabbedHand();
//	if (nullptr != grabbedHand && this != grabbedHand) {
//		grabbedHand->SetHeldComponent(nullptr);
//		grabbedHand->SetAttachedActor(nullptr);
//	}
//
//	if (detectedItem->grabComp->TryGrab(this, gripSocket) == false) return;
//	
//	SetHeldComponent(detectedItem->grabComp);
//	SetAttachedActor(detectedItem->grabComp->GetOwner());
//	holdingItem = detectedItem;
//	detectedItem = nullptr;
//}

void APlayerHand::SetAttachedActor(AActor* _attachedactor)
{
	/*if (holdingWeapon != nullptr) {
		holdingWeapon->owningPlayer = nullptr;
	}*/

	AWeapon* tempWeapon = Cast<AWeapon>(_attachedactor);
	holdingWeapon = tempWeapon;
	if (holdingWeapon != nullptr) {
		handAnim->holdingWeapon = holdingWeapon;
	}
	
	attachedActor = _attachedactor;
}

UGrabComp* APlayerHand::GetGrabComponentNearGripPos(USceneComponent* grabSocket, float radius, bool right /*= true*/)
{
	const FVector gripPos = grabSocket->GetComponentLocation();

	UGrabComp* nearestComponent = nullptr;
	float nearestDistance = TNumericLimits<float>::Max();

	FVector start = gripPos;
	FVector end = start;
	TArray<TEnumAsByte<EObjectTypeQuery>> objectTypes;
	objectTypes.Emplace(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_Visibility));


	TArray<AActor*> ignoreActors; // 무시할 액터들.
	TArray<FHitResult> outHitResults; // 히트 결과 값 받을 변수.

	bool result = UKismetSystemLibrary::SphereTraceMultiForObjects(
		GetWorld(), 
		start,
		end, 
		radius,
		objectTypes, 
		false,
		ignoreActors,
		EDrawDebugTrace::None,
		outHitResults,
		true,
		FLinearColor::Red,
		FLinearColor::Green,
		1.0f
	);

	if (result)
	{
		for (FHitResult& hitResult : outHitResults)
		{
			TArray<UGrabComp*> outComponents;
			//hitResult.GetActor()->GetComponents<UGrabComp>(outComponents);
			//hitResult.GetActor()->GetComponentsByClass(UGrabComp::StaticClass());
			hitResult.GetActor()->GetComponents(outComponents);


			for (UGrabComp* component : outComponents)
			{
				if (component->CanGrab(this, grabSocket) == false)
					continue;

				FVector location = component->GetComponentLocation();
				float distance = FVector::DistSquared(gripPos, location);

				if (distance < nearestDistance)
				{
					nearestDistance = distance;
					nearestComponent = component;
				}
			}
		}
	}

	return nearestComponent;
}

void APlayerHand::OnGripPressed()
{
	if (handState != EHandState::Default)
		return;

	/*if (detectedItem != nullptr) {
		GetItem();
	}*/

	// 주변에 잡을수 있는(grabComp가 있는 ) 물체가 있는지 확인
	UGrabComp* grabComponent = GetGrabComponentNearGripPos(gripSocket, gripRadius, false);
	
	// 범위 내에 잡을 수 있는 물체가 없다면 무기소환모드 활성화
	if (grabComponent == nullptr) {
		bIsWeaponSpawnMode = true;
		spawnDeterminationHeight = motionController->GetComponentLocation().Z;
		return;
	}

	// 잡을수 있는 물체가 있는데, 다른 손이 집고 있었다면, 그 손에서 떼어내기.
	APlayerHand* grabbedHand = grabComponent->GetGrabbedHand();
	if (nullptr != grabbedHand && this != grabbedHand){
		grabbedHand->SetHeldComponent(nullptr);
		grabbedHand->SetAttachedActor(nullptr);
	}

	if (grabComponent->TryGrab(this, gripSocket) == false)
		return;

	SetHeldComponent(grabComponent);
	SetAttachedActor(grabComponent->GetOwner());


}

void APlayerHand::OnGripReleased()
{
	bIsWeaponSpawnMode = false;
	if (heldComponentRef == nullptr)
		return;

	if (heldComponentRef->TryRelease() == false)
		return;

	if (holdingWeapon != nullptr) {
		if (weaponReleaseSound != nullptr) UGameplayStatics::PlaySoundAtLocation(GetWorld(), weaponReleaseSound, gripSocket->GetComponentLocation());

		if (holdingWeapon->weaponType == EWeaponType::Sword) {
			physicsHandle->ReleaseComponent();
			FAttachmentTransformRules rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
			rules.LocationRule = EAttachmentRule::SnapToTarget;
			rules.RotationRule = EAttachmentRule::SnapToTarget;
			rules.ScaleRule = EAttachmentRule::KeepRelative;
			rules.bWeldSimulatedBodies = true;
			
			// 다시 모션컨트롤러에 붙이기
			gripSocket->AttachToComponent(motionController, rules);
			gripSocket->SetRelativeLocation(FVector(0.f, -2.f, -8.f));
			gripSocket->SetRelativeRotation(FRotator(-85.f, 0.f, 0.f));
			
		}
		holdingWeapon->SetActive(false);
		holdingWeapon = nullptr;
		/*if (holdingWeapon->weaponType == EWeaponType::Sword) {
			ASword* holdingSword = Cast<ASword>(holdingWeapon);
			if (holdingSword != nullptr) {
				holdingSword->SetActive(false);
			}
		}
		else if (holdingWeapon->weaponType == EWeaponType::Gun) {
			AGun* holdingGun = Cast<AGun>(holdingWeapon);
			if (holdingGun != nullptr) {
				holdingGun->SetActive(false);
			}
		}*/
		
	}
	
	SetHeldComponent(nullptr);
	SetAttachedActor(nullptr);
}

void APlayerHand::OnTriggerPressed()
{
	if (holdingWeapon != nullptr) {
		
		holdingWeapon->Attack();
	}
	/*else if (holdingItem != nullptr) {
		if (holdingItem->itemType != EItemType::IceElement) {
			holdingItem->OnItemUsed();
			holdingItem = nullptr;
		}
	}*/
}

void APlayerHand::OnTriggerReleased()
{
	if (holdingWeapon != nullptr && holdingWeapon->weaponType == EWeaponType::Sword) {

		// 튜토리얼 위한 코드
		if (player->lgm != nullptr && player->lgm->tutorialActor != nullptr && !player->lgm->tutorialActor->bSwordSlash) return;

		ASword* sword = Cast<ASword>(holdingWeapon);
		sword->EndSlash();
	}
}

void APlayerHand::InitializeVRHand()
{
	if (handMesh == nullptr) return;
	
	

	if (bIsRight == false)
	{
		gripSocket->SetRelativeLocation(FVector(0.f, 2.f, -8.f));
		//gripSocket->SetRelativeLocation(FVector(9.f, 3.f, 0.f));
		gripSocket->SetRelativeRotation(FRotator(-85.f, 0.f, 0.f));
		//gripSocket->SetRelativeRotation(FRotator(180.f, 0.f, 180.f));
		if (lHand != nullptr) {
			handMesh->SetSkeletalMesh(lHand);
			handMesh->SetRelativeLocation(FVector(-7.f, -3.f, 0.f));
			//handMesh->SetRelativeLocation(FVector(9.f, 3.f, 0.f));
			handMesh->SetRelativeRotation(FRotator(-90.f, 0.f, -90.f));
			//handMesh->SetRelativeRotation(FRotator(-90.f, 180.f, -90.f));
			handSocket->SetRelativeLocation(FVector(-9.f, 3.f, 0.f));
			handSocket->SetRelativeRotation(FRotator(-90.f, 0.f, -90.f));
			handDefaultArea->SetRelativeLocation(FVector(0.f, 9.f, 3.f));
		}
		motionController->SetTrackingSource(EControllerHand::Left);
	}
	else
	{
		gripSocket->SetRelativeLocation(FVector(0.f, -2.f, -8.f));
		gripSocket->SetRelativeRotation(FRotator(-85.f, 0.f, 0.f));
		//gripSocket->SetRelativeLocation(FVector(9.f, -3.f, 0.f));
		if (rHand != nullptr) {
			handMesh->SetSkeletalMesh(rHand);
			handMesh->SetRelativeRotation(FRotator(90.0f, -90.0f, 0.0f));
			//handMesh->SetRelativeRotation(FRotator(90.f, 0, 90.f));
			handMesh->SetRelativeLocation(FVector(-7.f, 3.f, 0.f));
			//handMesh->SetRelativeLocation(FVector(-9.f, 3.f, 0.f));
			handSocket->SetRelativeLocation(FVector(-9.f, 3.f, 0.f));
			handSocket->SetRelativeRotation(FRotator(0.f, 0.f, 90.f));
			handDefaultArea->SetRelativeLocation(FVector(9.f, 0.f, -3.f));
		}
		motionController->SetTrackingSource(EControllerHand::Right);
	}
	//motionController->SetRelativeRotation(FRotator(-90.f, 0.f, 0.f));
	handAnim = Cast<UPlayerHandAnim>(handMesh->GetAnimInstance());
	if (handAnim != nullptr) handAnim->bMirror = bIsRight;
}
