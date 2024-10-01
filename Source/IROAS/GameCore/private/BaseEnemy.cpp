

#include "BaseEnemy.h"
#include <Components/CapsuleComponent.h>
#include <Components/SkeletalMeshComponent.h>
#include "Components/SphereComponent.h"
#include "BattlePlayer.h"
#include <EngineUtils.h>
#include <NavigationSystem/Public/NavigationSystem.h>
#include <AIModule/Classes/AIController.h>
#include <Components/TextRenderComponent.h>
#include <GameFramework/FloatingPawnMovement.h>
#include "BaseEnemyAnim.h"
#include "RangedEnemyAnim.h"
#include <Kismet/KismetMathLibrary.h>
#include "Materials/MaterialInstanceConstant.h"
#include "Sword.h"
#include "BattleGameStateBase.h"


ABaseEnemy::ABaseEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	//BaseEnemyRootComp = CreateDefaultSubobject<USceneComponent>(TEXT("BaseEnemyRootComp"));
	//RootComponent = BaseEnemyRootComp;

	//meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComp"));
	//meshComp->SetupAttachment(RootComponent);


	capsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	SetRootComponent(capsuleComp);

	capsuleComp->SetCapsuleHalfHeight(150.f);
	capsuleComp->SetCapsuleRadius(80.f);
	capsuleComp->SetRelativeLocation(FVector(0, 0, 150));

	meshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(RootComponent);
	meshComp->SetRelativeScale3D(FVector(1.5f));
	meshComp->SetRelativeLocationAndRotation(FVector(0, 0, -150), FRotator(0, -90.f, 0));

	// 헤드 Head

	HeadComp = CreateDefaultSubobject<USphereComponent>(TEXT("head"));
	HeadComp->SetupAttachment(meshComp, FName("head"));
	HeadComp->ComponentTags.Add(FName("head"));

	HeadComp->InitSphereRadius(20.0f);

	HeadComp->SetCollisionProfileName(TEXT("EnemyBody"));



	/////////////////////////////////////////////////////////////////////////////

	// ZHeight구하기용 Arrow

	curState = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Current State"));
	curState->SetupAttachment(RootComponent);


	pawnMovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	//pawnMovementComp->bOrientRotationToMovement = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;

	SetReplicates(true);
	SetReplicateMovement(true);
}

void ABaseEnemy::BeginPlay()
{
	Super::BeginPlay();

	// 스폰 될 때 플레이어 찾기
	float mindistance = 100000.f;

	for (TActorIterator<ABattlePlayer> it(GetWorld(), ABattlePlayer::StaticClass()); it; ++it)
	{
		ABattlePlayer* player = *it;
		dist = (player->GetActorLocation() - GetActorLocation()).Length();

		if (dist < mindistance)
		{
			mindistance = dist;
			target = player;
		}
	}



	// ai 컨트롤러
	aiController = Cast<AAIController>(GetController());






	// 기본 재질 저장
	OriginalMaterial = meshComp->GetMaterial(0);

	UMaterialInstanceConstant* HitOverlayMaterialInstanceConstant = LoadObject<UMaterialInstanceConstant>(nullptr, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/GTL/HitsContent/Hit_Outline/M_Overlay_Inst_Damage.M_Overlay_Inst_Damage'"));

	// 오버레이 재질 인스턴스 생성 (피격 아웃라인)
	//UMaterial* OverlayMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Script/Engine.Material'/Game/GTL/EnemyComtent/Hit_Outline/M_Overlay.M_Overlay'"));
	if (HitOverlayMaterialInstanceConstant != nullptr)
	{
		HitOverlayMaterialInstance = UMaterialInstanceDynamic::Create(HitOverlayMaterialInstanceConstant, this);


	}

	UMaterialInstanceConstant* DieOverlayMaterialInstanceConstant = LoadObject<UMaterialInstanceConstant>(nullptr, TEXT("/Script/Engine.MaterialInstanceConstant'/Game/GTL/HitsContent/Hit_Outline/M_Overlay_Inst_Die.M_Overlay_Inst_Die'"));
	// 오버레이 재질 인스턴스 생성 (피격 아웃라인 Hit)
	if (DieOverlayMaterialInstanceConstant != nullptr)
	{
		DieOverlayMaterialInstance = UMaterialInstanceDynamic::Create(DieOverlayMaterialInstanceConstant, this);
	}

}

void ABaseEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);


	switch (enemyState)
	{
	case EEnemyState::Idle:
		FindTarget();
		break;
	case EEnemyState::Dead:
		DieDown();
		break;
	case EEnemyState::Move:
		Move();
		break;
	case EEnemyState::Attack:
		ApplyDamage();
		break;
	case EEnemyState::Damage:
		OnTakeDamage();
		//StartKnockback();
		break;
	case EEnemyState::Rage:
		RageState();
		break;
	}

	// 테스트를 위해 textrender 띄우는 것.
	FString stateString = EnumToString(enemyState);
	curState->SetText(FText::FromString(stateString));
	//curState->SetText(FText::FromString(FString::Printf(TEXT("%f"), curTime)));


}


FString ABaseEnemy::EnumToString(EEnemyState state)
{
	switch (state)
	{
	case EEnemyState::Idle:
		return FString("Idle");
	case EEnemyState::Move:
		return FString("Move");
	case EEnemyState::Attack:
		return FString("Attack");
	case EEnemyState::Damage:
		return FString("Damage");
	case EEnemyState::Dead:
		return FString("Dead");

	default:
		return FString("Unknown");
	}


}

void ABaseEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void ABaseEnemy::FindTarget()
{
	// target을 찾은 경우 move로 변경
	if (target != nullptr && !IamInvisible && !bIsAlreadyDead)
	{

		enemyState = EEnemyState::Move;
		if (enemyAnim != nullptr)
			enemyAnim->animState = enemyState;
			
	}
}

void ABaseEnemy::Move()
{
	FVector dest = target->GetActorLocation();
	direction = dest - GetActorLocation();
	dist = direction.Length();


	FRotator rot = UKismetMathLibrary::MakeRotFromXZ(direction.GetSafeNormal(), GetActorUpVector());

	rot.Pitch = GetActorRotation().Pitch;
	rot.Roll = GetActorRotation().Roll;

	SetActorRotation(rot);

	/*FNavAgentProperties props = aiController->GetNavAgentPropertiesRef();
	EPathFollowingRequestResult::Type result = aiController->MoveToLocation(dest);*/
	/*FAIMoveRequest moveReq;
	moveReq.SetAcceptanceRadius(10.0f);
	moveReq.SetReachTestIncludesAgentRadius(true);
	moveReq.SetGoalActor(target);
	EPathFollowingRequestResult::Type result = aiController->MoveTo(moveReq, nullptr);
	UE_LOG(LogTemp, Warning, TEXT("%.2f, %.2f, %.2f"), aiController->GetNavAgentLocation().X, aiController->GetNavAgentLocation().Y, aiController->GetNavAgentLocation().Z);
	UE_LOG(LogTemp, Warning, TEXT("%s"), *target->GetName());*/
	//UE_LOG(LogTemp, Warning, TEXT("%s"), *UEnum::GetValueAsString(result));
	//SetActorLocation(direction);
	//AddMovementInput(direction);

	auto naviSystem = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FPathFindingQuery query;
	FAIMoveRequest req;
	req.SetGoalLocation(target->GetActorLocation());
	req.SetAcceptanceRadius(3);

	aiController->BuildPathfindingQuery(req, query);
	auto r = naviSystem->FindPathSync(query);

	// 갈 수 있다면
	if (r.Result == ENavigationQueryResult::Success)
	{
		if (aiController != nullptr)
		{
			aiController->MoveToLocation(dest);
		}
	}
	// 못 간다면
	else
	{
		auto result = aiController->MoveToLocation(randomDest);

		// 도착했다면
		if (result == EPathFollowingRequestResult::AlreadyAtGoal)
		{
			// 랜덤 위치 재설정
			//GetRandomPosInNavMesh(GetActorLocation(), 500, randomDest);
		}
	}

	if (dist <= attackRange)
	{
		enemyState = EEnemyState::Attack;
		if (enemyAnim != nullptr)
			enemyAnim->animState = enemyState;
		curTime = attackDelay;
		aiController->StopMovement();
	}
}

void ABaseEnemy::ApplyDamage()
{
	curTime += GetWorld()->DeltaTimeSeconds;
	if (curTime > attackDelay) {
		if (enemyAnim != nullptr) {
			enemyAnim->bAttackPlay = true;
		}
		curTime = 0;

	}

	float distance = FVector::Dist2D(target->GetActorLocation(), GetActorLocation());
	if (distance > attackRange) {
		enemyState = EEnemyState::Move;
		if (enemyAnim != nullptr)
			enemyAnim->animState = enemyState;
		curTime = 0;
		GetRandomPosInNavMesh(GetActorLocation(), 500, randomDest);
	}


}


void ABaseEnemy::SetActive(bool isActive)
{
	if (meshComp) meshComp->SetVisibility(isActive);

	SetActorEnableCollision(isActive);
	
	if (!isActive)
	{
		//capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		aiController->StopMovement();
		IamInvisible = true;
	}
	else
	{
		//capsuleComp->SetCollisionProfileName(FName("EnemyBody"));
		IamInvisible = false;
		bIsAlreadyDead = false;
		enemyState = EEnemyState::Idle;
		
	}
}

void ABaseEnemy::SetSpawner(class AEnemySpawner* owingSpawner)
{
	spawner = owingSpawner;
}

bool ABaseEnemy::GetRandomPosInNavMesh(FVector center, float radius, FVector& dest)
{
	UNavigationSystemV1* ns = UNavigationSystemV1::GetNavigationSystem(GetWorld());
	FNavLocation loc;
	bool result = ns->GetRandomReachablePointInRadius(center, radius, loc);
	dest = loc.Location;
	return result;
}


// 베이스 에너미가 구독한 인터페이스 내용 (원래 Melee Ranged에서 따로 구현되어있던것들 통합)
void ABaseEnemy::OnDamage_Interface(float DamageAmount)
{
	if (bIsAlreadyDead) return;
	bIsKnockbackActive = false;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("before hp: %f"), hp));

	hp -= DamageAmount;

	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("IsEmpty? : %s"), Tags.IsEmpty() ? TEXT("true") : TEXT("false")));
	GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("after hp: %f"), hp));

	if (hp >= 1)
	{
		enemyState = EEnemyState::Damage;
		//OnTakeDamage();
		//StartKnockback();
		Tags.Empty();
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("after hp: %f"), hp));
	}
	else if (hp < 1 && !bIsAlreadyDead)
	{
		if (gsb != nullptr) {
			//gsb->totalEnemyCnt = FMath::Max(gsb->totalEnemyCnt - 1, 0);

			bIsAlreadyDead = true;
			gsb->totalEnemyCnt--;
			//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("TotalEnemyCnt--!\t\t%s"), *GetName()));
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Green, FString::Printf(TEXT("Die hp: %f"), hp));
		}
		enemyState = EEnemyState::Dead;
		//SetActorEnableCollision(false); 이거 활성화하면 레그돌 적용안됨.
		capsuleComp->SetCanEverAffectNavigation(false);
		Tags.Empty();
		DieDown();
	}

}


void ABaseEnemy::OnTakeDamage()
{
	if (enemyAnim != nullptr)
		enemyAnim->animState = enemyState;
	aiController->StopMovement();

	curTime += GetWorld()->DeltaTimeSeconds;
	if (curTime > damageDelay)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("BaseOnTakeDamege")));
	
		enemyState = EEnemyState::Move;
		curTime = 0;
		if (enemyAnim != nullptr)
			enemyAnim->animState = enemyState;

	}
}


void ABaseEnemy::OnDamageProcess(float deal)
{
	hp = FMath::Max(0, hp - deal);

	if (hp >= 1.0f) {
		enemyState = EEnemyState::Damage;
	}
	else {
		enemyState = EEnemyState::Dead;
		SetActorEnableCollision(false);
		//capsuleComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}
}

void ABaseEnemy::StartKnockback()
{

	// 넉백이 아직 활성화되지 않았을 때만 넉백을 시작합니다.
	if (!bIsKnockbackActive)
	{

		KnockbackDirection = GetActorForwardVector() * -1;
		KnockbackDirection.Z = 0;
		KnockbackDirection.Normalize();
		StartPosition = GetActorLocation(); // 넉백 시작 위치
		EndPosition = GetActorLocation() + KnockbackDirection * KnockbackDistance;; // 넉백 끝 위치
		//GEngine->AddOnScreenDebugMessage(1, 5.0f, FColor::Green, FString::Printf(TEXT("EndPosition: %s"), *EndPosition.ToString()));

		Alpha = 0.0f;

		OnHitOverlayMaterial();
		bIsKnockbackActive = true;
	}

	if (bIsKnockbackActive && GetWorld() != nullptr)
	{
		float DeltaTime = GetWorld()->GetDeltaSeconds();
		Alpha += DeltaTime / KnockbackDuration;
		Alpha = FMath::Clamp(Alpha, 0.0f, 1.0f); // Alpha 값을 0과 1 사이로 제한합니다.
		NewPosition = FMath::Lerp(StartPosition, EndPosition, Alpha);
		//GEngine->AddOnScreenDebugMessage(-1, KnockbackDuration, FColor::Blue, FString::Printf(TEXT("Alpha  = %f"), Alpha));
		SetActorLocation(NewPosition);
		// 넉백이 끝났는지 체크합니다.
		if (Alpha >= 1.0f)
		{
			//meshComp->bPauseAnims = false;
			ResetMaterial();
		}
	}
}

void ABaseEnemy::OnHitOverlayMaterial() //tick이 아니라 한번만 발동되어야 해서 넉백 플래그 안에서 호출
{
	if (HitOverlayMaterialInstance != nullptr)
	{

		// 오버레이 재질 적용
		meshComp->SetRenderCustomDepth(true);
		meshComp->SetOverlayMaterial(HitOverlayMaterialInstance);
		// 원래 재질로 복원하기 위해 타이머 설정
		GetWorld()->GetTimerManager().SetTimer(TimerHandle_ResetMaterial_FromHit, this, &ABaseEnemy::ResetMaterial, KnockbackDuration, false);

		//meshComp->bPauseAnims = true;

		//FTimerDelegate TimerDel;
		//TimerDel.BindLambda([this]
		//	{
		//		meshComp->bPauseAnims = false;
		//	});
		//GetWorld()->GetTimerManager().SetTimer(TimerHandle_StopAnim, TimerDel, KnockbackDuration / 2, false);

	}
}

void ABaseEnemy::OnDieOverlayMaterial() //tick이 아니라 한번만 발동되어야 해서 넉백 플래그 안에서 호출
{
	if (DieOverlayMaterialInstance != nullptr)
	{

		// 오버레이 재질 적용
		GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ResetMaterial_FromHit);
		meshComp->SetRenderCustomDepth(true);
		meshComp->SetOverlayMaterial(DieOverlayMaterialInstance);

	}

}

void ABaseEnemy::ResetMaterial()
{
	// 원래 재질로 복원
	meshComp->SetOverlayMaterial(OriginalMaterial);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_ResetMaterial_FromHit);
	GetWorld()->GetTimerManager().ClearTimer(TimerHandle_StopAnim);
}

void ABaseEnemy::DieDown()
{
	// 애님 클래스의 die 플레이 true일 때
	aiController->StopMovement();
	enemyState = EEnemyState::Dead;

	curTime += GetWorld()->DeltaTimeSeconds;

	//OnDieOverlayMaterial();


	if (enemyAnim != nullptr)
		enemyAnim->animState = enemyState;



	if (curTime > DownDelay)
	{
		// 아래로 이동
		FVector P = GetActorLocation() + FVector::DownVector * 100 * GetWorld()->DeltaTimeSeconds;
		SetActorLocation(P);
		// 일정 지점까지 내려가면
		if (P.Z < -300)
		{
			//PutObjectPool();
			//meshComp->SetOverlayMaterial(OriginalMaterial);
			//enemyState = EEnemyState::Idle;
			SetActorEnableCollision(false);
			//GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, FString::Printf(TEXT("MeleeDown SetActorEnableCollision(true)")));
			curTime = 0;
			if (enemyAnim != nullptr)
				enemyAnim->animState = enemyState;

		}

	}

}

void ABaseEnemy::RageState()
{
	aiController->StopMovement();
}

FVector ABaseEnemy::CalculateVelocity(AActor* OtherActor, const FVector& CurrentLocation)
{
	if (IsValid(OtherActor))
	{
		FVector NextTickLocation = OtherActor->GetActorLocation();
		return (NextTickLocation - CurrentLocation) / GetWorld()->GetDeltaSeconds();
	}

	return FVector::ZeroVector;
}

