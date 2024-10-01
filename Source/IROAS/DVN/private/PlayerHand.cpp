

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
		// ���� �����Ӱ��� ��ġ ���̸� Ȱ���� �ӵ�����
		float distance = FVector::Dist(motionController->GetRelativeLocation(), prevLoc);
		float curSpeed;

		// �ն������� ���� Ƣ�� �� ����
		if(FMath::IsNearlyZero(distance)) curSpeed = 0.0f;
		else curSpeed = distance / DeltaTime;

		// ���� ��ȯ�ӵ��� �Ѱ����� ��ȯ�Ѵ�.
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
	// �׸� ������ ��ġ���� �����̸� �� ��ȯ
	if (triggeredHeight > spawnDeterminationHeight) {
		// Ʃ�丮�� �ڵ�
		if (player->ActorHasTag("LobbyPlayer") && !tutorialSwordSpawn) return;
		
		ASword* spawnedSword;
		player->swordPool.Dequeue(spawnedSword);

		// ��ȯ����Ʈ
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), weaponDrawEffect, gripSocket->GetComponentLocation());

		spawnedSword->SetActorLocation(gripSocket->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
		spawnedSword->SetActive(true);

		// �������� ���
		/*
		physicsHandle->GrabComponentAtLocationWithRotation(Cast<UPrimitiveComponent>(spawnedSword->meshComp), NAME_None, spawnedSword->grabComp->GetComponentLocation(), spawnedSword->grabComp->GetComponentRotation());

		FAttachmentTransformRules rules = FAttachmentTransformRules::SnapToTargetNotIncludingScale;
		rules.LocationRule = EAttachmentRule::SnapToTarget;
		rules.RotationRule = EAttachmentRule::SnapToTarget;
		rules.ScaleRule = EAttachmentRule::KeepRelative;
		rules.bWeldSimulatedBodies = true;

		// gripsocket��ü�� Į�� ���̱�
		gripSocket->AttachToComponent(spawnedSword->grabComp, rules);
		*/


		// grabbedComponent�� ���� �ٸ��� �ʱ�ȭ
		APlayerHand* grabbedHand = spawnedSword->grabComp->GetGrabbedHand();
		if (nullptr != grabbedHand && this != grabbedHand) {
			grabbedHand->SetHeldComponent(nullptr);
			grabbedHand->SetAttachedActor(nullptr);
		}

		// ��� �õ�.
		if (spawnedSword->grabComp->TryGrab(this, gripSocket) == false) {
			spawnedSword->SetActive(false);
			bIsWeaponSpawnMode = false;
			return;
		}

		// ��ƽ
		player->pc->PlayHapticEffect(swordSpawnHaptic, bIsRight ? EControllerHand::Right : EControllerHand::Left, 1.f, false);

		SetHeldComponent(spawnedSword->grabComp);
		SetAttachedActor(spawnedSword->grabComp->GetOwner());
		holdingWeapon = spawnedSword;
		holdingWeapon->bisRightHand = bIsRight;
		bIsWeaponSpawnMode = false;
		tutorialSwordSpawnCount++;
	}

	// �׸� ������ ��ġ���� �Ʒ����̸� �� ��ȯ
	else if (triggeredHeight < spawnDeterminationHeight) {
		
		// Ʃ�丮�� �ڵ�
		if (player->ActorHasTag("LobbyPlayer") && !tutorialGunSpawn) return;
		
		AGun* spawnedGun;
		player->gunPool.Dequeue(spawnedGun);

		// ��ȯ����Ʈ
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), weaponDrawEffect, gripSocket->GetComponentLocation());

		spawnedGun->SetActorLocation(gripSocket->GetComponentLocation(), false, nullptr, ETeleportType::TeleportPhysics);
		spawnedGun->SetActive(true);
		spawnedGun->bisRightHand = bIsRight;

		// grabbedComponent�� ���� �ٸ��� �ʱ�ȭ
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

		// ��ƽ
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


	TArray<AActor*> ignoreActors; // ������ ���͵�.
	TArray<FHitResult> outHitResults; // ��Ʈ ��� �� ���� ����.

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

	// �ֺ��� ������ �ִ�(grabComp�� �ִ� ) ��ü�� �ִ��� Ȯ��
	UGrabComp* grabComponent = GetGrabComponentNearGripPos(gripSocket, gripRadius, false);
	
	// ���� ���� ���� �� �ִ� ��ü�� ���ٸ� �����ȯ��� Ȱ��ȭ
	if (grabComponent == nullptr) {
		bIsWeaponSpawnMode = true;
		spawnDeterminationHeight = motionController->GetComponentLocation().Z;
		return;
	}

	// ������ �ִ� ��ü�� �ִµ�, �ٸ� ���� ���� �־��ٸ�, �� �տ��� �����.
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
			
			// �ٽ� �����Ʈ�ѷ��� ���̱�
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

		// Ʃ�丮�� ���� �ڵ�
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
