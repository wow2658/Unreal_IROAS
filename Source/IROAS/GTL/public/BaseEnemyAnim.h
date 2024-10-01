#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "IROASEnums.h"
#include "BaseEnemyAnim.generated.h"


UCLASS()
class IROAS_API UBaseEnemyAnim : public UAnimInstance
{
	GENERATED_BODY()
	
public:
	virtual void NativeInitializeAnimation() override;
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyAnim|FSM")
	EEnemyState animState;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyAnim|Settings")
	float speed;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemyAnim|Settings")
	bool bAttackPlay = false;	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAnim|Settings")
	int32 DamageSectionInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAnim|Settings")
	int32 DieSectionInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAnim|Settings")
	int32 DieSectionInt_RangedG;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAnim|Settings")
	int32 DieRandomInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAnim|Settings")
	int32 runTypeInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAnim|Settings")
	int32 AttackTypeInt;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAnim|Settings")
	int32 RageTypeInt;

	bool bDamage = false;
	bool bDiePlay = false;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "EnemyAnim|Settings")
	bool Iamvisible_Anim;
	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_AttackStart();
	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_AttackEnd();	
	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_MeleeDieEnd();		
	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_MeleeAttackEnd();
	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_Melee_DamageStart();	
	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_RangedDieEnd();	
	UFUNCTION(BlueprintCallable, Category = "EnemyAnim|FSM")
	void AnimNotify_MeleeRageEnd();


	class ABaseEnemy* enemy;
	class AMeleeEnemy* MeleeEnemy = nullptr;
	class UFloatingPawnMovement* moveComp;
	class ABattleGameModeBase* GameMode;
};
