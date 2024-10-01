
#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "OnDamage_Interface.h"
#include "BattlePlayer.generated.h"

class UCapsuleComponent;
class USkeletalMeshComponent;
class APlayerHand;
class AWeapon;
class ASword;
class AGun;
class UBasicMovementComponent;
class USnapMovementComponent;
class UBattlePlayerIKAnimComp;
class UBattlePlayerWidgetManager;
class UWidgetComponent;
class UPlayerHandAnimComponent;
class ABattleGameStateBase;
class ALobbyGameMode;
class USoundBase;
class UNiagaraComponent;
class ALevelSequenceActor;
class ARageTriggerBox;


UCLASS()
class IROAS_API ABattlePlayer : public ABaseCharacter, public IOnDamage_Interface
{
	GENERATED_BODY()

public:
	ABattlePlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

public:

	UPROPERTY(EditDefaultsOnly, Category = "MySettings|Components")
	USkeletalMeshComponent* characterMeshComp;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings|Components")
	UCapsuleComponent* capsuleComp;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings|Components")
	UWidgetComponent* widgetComp;

	UPROPERTY(EditDefaultsOnly, Category = "MySettings|Components")
	UBasicMovementComponent* moveComp;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings|Components")
	USnapMovementComponent* snapMoveComp;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings|Components")
	UBattlePlayerIKAnimComp* ikAnimComp;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings|Components")
	UBattlePlayerWidgetManager* widgetManager;

	UPROPERTY()
	float desiredHalfHeight = 0.f;
	UPROPERTY()
	float capsuleRadius = 35.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerHand* leftHand = nullptr;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	APlayerHand* rightHand = nullptr;

	UPROPERTY(EditDefaultsOnly, Category = "MySettings|PlayerAttributes")
	float maxHealth = 100.0f;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings|PlayerAttributes")
	float maxEnergy = 100.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings|PlayerAttributes")
	float health = 0.0f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings|PlayerAttributes")
	float energy = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<ASword> bp_Sword;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<AGun> bp_Gun;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	// TQueue는 UPROPERTY 적용할수없음
	TQueue<ASword*> swordPool;
	TQueue<AGun*> gunPool;

	ABattleGameStateBase* gsb = nullptr;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsUIMode = true;

	ALobbyGameMode* lgm;
	
	//튜토리얼 추가
	bool bCanMove = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* gainEnergySound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* deadSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* hitbyMeleeSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* hitbyRangedSound;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	USoundBase* hitbyBossSound;

	//UPROPERTY(EditAnywhere, BlueprintReadWrite)
	//UNiagaraComponent* moveTrail;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float minSpeed = 10.f;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float maxSpeed = 15.f;
private:
	bool bIsDead = false;

	FVector lPrevloc;
	FQuat lPrevQuat;

	FVector rPrevloc;
	FQuat rPrevQuat;

	FTransform hmdPrevTrans;

	FVector actorPrevLoc = FVector::ZeroVector;

public:

	virtual void InitializeVRHand() override;
	void InitializeIndicator();
	void InitializeLoadingCube();
	
	void ReloadAllGuns();
public:
	// Left
	virtual void HandleLeftGripPressed() override;
	virtual void HandleLeftGripReleased() override;
	virtual void HandleLeftTriggerPressed() override;
	virtual void HandleLeftTriggerReleased() override;
	virtual void HandleLeftThumbstickAxis(const FInputActionValue& Value) override;
	virtual void HandleLeftThumbstickLeft() override;
	virtual void HandleLeftThumbstickRight() override;
	virtual void HandleLeftMenu() override;

	// Right
	virtual void HandleRightGripPressed() override;
	virtual void HandleRightGripReleased() override;
	virtual void HandleRightTriggerPressed() override;
	virtual void HandleRightTriggerReleased() override;
	virtual void HandleRightThumbstickAxis(const FInputActionValue& Value) override;
	//virtual void HandleRightThumbstickLeft() override;
	virtual void HandleRightThumbstickRight() override;


	void HandleRightButtonA();
	void HandleRightButtonACompleted();
	void HandleLeftButtonX();
	void HandleLeftButtonXCompleted();

	UFUNCTION()
	void BattleModeDebug();
	UFUNCTION()
	void LobbyModeDebug();

	bool debugA = false, debugX = false;
	bool debugModeExecuted = false;

public:
	FORCEINLINE float GetHeath() { return health; }
	FORCEINLINE float GetEnergy() { return energy; }
	FVector GetCameraLoc();

	void SetHealth(float hp) { health = hp; }
	void SetEnergy(float ep) { energy = ep; }
	bool IsDead() { return bIsDead; }

	virtual void OnDamage_Interface(float DamageAmount) override;
	void OnDamage(float deal);
	void OnHeal(float heal);

	bool UseEnergy(float energyCost);
	UFUNCTION(BlueprintCallable)
	void GainEnergy(float amount);

	void DieProcess();

	void HitbyMeleeSound();
	void HitbyRangedSound();
	void HitbyBossSound();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings|Components")
	FTransform StartTransform; 
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings|Components")
	ALevelSequenceActor* startCineActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings|Components")
	ALevelSequenceActor* bossCineActor;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "MySettings|Components")
	ARageTriggerBox* rageBox;

	UFUNCTION(BlueprintCallable)
	void OnStartCineFinished();
};
