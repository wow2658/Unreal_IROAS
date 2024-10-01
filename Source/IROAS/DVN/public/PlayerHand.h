

#pragma once

#include "CoreMinimal.h"
#include "Hand.h"
#include "IROASEnums.h"
#include "PlayerHand.generated.h"

class USphereComponent;
class USkeletalMeshComponent;
class USkeletalMesh;
class UPlayerHandAnim;
class UGrabComp;
class AWeapon;
class ABattlePlayer;
class ASword;
class AGun;
class UPhysicsHandleComponent;
class UNiagaraSystem;
class USoundBase;

UCLASS()
class IROAS_API APlayerHand : public AHand
{
	GENERATED_BODY()

public:
	APlayerHand();
protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* lHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMesh* rHand;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USkeletalMeshComponent* handMesh = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UPhysicsHandleComponent* physicsHandle;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* gripSocket = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* offsetComp = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USceneComponent* handSocket = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USphereComponent* handDefaultArea = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float gripRadius = 10.0f;

	ABattlePlayer* player;
	UPlayerHandAnim* handAnim;

	FVector prevLoc;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float weaponSpawnSpeed = 500.f;
	bool bIsWeaponSpawnMode = false;
	float spawnDeterminationHeight = 0.f;
	
	int8 tutorialSwordSpawnCount = 0;
	int8 tutorialGunSpawnCount = 0;
	bool tutorialSwordSpawn = false;
	bool tutorialGunSpawn = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	UNiagaraSystem* weaponDrawEffect;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	USoundBase* weaponReleaseSound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Haptics")
	UHapticFeedbackEffect_Base* gunSpawnHaptic;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Haptics")
	UHapticFeedbackEffect_Base* swordSpawnHaptic;

	//AItemBase* detectedItem = nullptr;
	//UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	//AItemBase* holdingItem = nullptr;

private: //Grab
	UGrabComp* heldComponentRef = nullptr;

	UPROPERTY(VisibleAnywhere, Category = "MySettings|Components")
	AWeapon* holdingWeapon = nullptr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = true))
	EHandState handState = EHandState::Default;

public:
	UFUNCTION()
	void SpawnWeapon(float triggeredHeight);
	//UFUNCTION()
	//void GetItem();

	USceneComponent* GetGripSocket() { return gripSocket; };

	void SetHeldComponent(UGrabComp* grabComponent) { heldComponentRef = grabComponent; }
	void SetHandState(EHandState _handstate) { handState = _handstate; };
	void SetAttachedActor(AActor* _attachedactor);
	AWeapon* GetAttachedWeapon() { return holdingWeapon; }

	UFUNCTION(BlueprintCallable)
	UGrabComp* GetGrabComponentNearGripPos(USceneComponent* grabSocket, float radius, bool right = true);
	void OnGripPressed();
	void OnGripReleased();
	void OnTriggerPressed();
	void OnTriggerReleased();

	virtual void InitializeVRHand() override;
};
