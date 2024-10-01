

#include "EnemySpawner.h"
#include <Components/BoxComponent.h>
#include <Components/SphereComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include "BattlePlayer.h"
#include "MeleeEnemy.h"
#include "RangedGroundEnemy.h"
#include "BattleGameModeBase.h"
#include "BattleGameStateBase.h"
#include <Components/StaticMeshComponent.h>
#include <Particles/ParticleSystemComponent.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraFunctionLibrary.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Public/NiagaraComponent.h>
#include <Kismet/GameplayStatics.h>
#include <../Plugins/FX/Niagara/Source/Niagara/Classes/NiagaraSystem.h>
#include <Materials/MaterialInterface.h>
#include "EnergyBullet.h"
#include <Engine/StaticMesh.h>
//#include <GeometryCollectionEngine/Public/GeometryCollection/GeometryCollectionComponent.h>

AEnemySpawner::AEnemySpawner()
{
	PrimaryActorTick.bCanEverTick = true;

	rootComp = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneRoot"));
	SetRootComponent(rootComp);

	underGroundPosition = CreateDefaultSubobject<USceneComponent>(TEXT("UnderGroundPosition"));
	underGroundPosition->SetupAttachment(RootComponent);
	underGroundPosition->SetRelativeLocation(FVector(0.f, 0.f, -500.f));

	actualPosition = CreateDefaultSubobject<USceneComponent>(TEXT("ActualPosition"));
	actualPosition->SetupAttachment(RootComponent);
	actualPosition->SetRelativeLocation(underGroundPosition->GetRelativeLocation());

	// �� collision & staticmesh
	monolithCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("MonolithCollision"));
	monolithCollision->SetupAttachment(actualPosition);
	//monolithCollision->SetCollisionObjectType(ECC_GameTraceChannel3);
	monolithCollision->SetCollisionResponseToAllChannels(ECR_Ignore);

	monolithMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MonolithMesh"));
	monolithMesh->SetupAttachment(monolithCollision);
	monolithMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	/*
	chaosDestructionComp = CreateDefaultSubobject<UGeometryCollectionComponent>(TEXT("ChaosDestruction"));
	chaosDestructionComp->SetupAttachment(monolithMesh);
	*/

	torchComp = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("TorchComp"));
	torchComp->SetupAttachment(monolithMesh);
	torchComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	torchComp->SetRelativeLocation(FVector(37.359176, -52.690432, 309.541520));
	torchComp->SetAutoActivate(false);

	riseSmoke = CreateDefaultSubobject<UParticleSystemComponent>(TEXT("RiseSmoke"));
	riseSmoke->SetupAttachment(RootComponent);
	riseSmoke->SetRelativeLocation(FVector(0.f, 0.f, 10.f));
	riseSmoke->SetAutoActivate(false);
	// ���� collision & staticmesh
	//shieldcollision = CreateDefaultSubobject<USphereComponent>(TEXT("ShieldCollision"));
	//shieldcollision->SetupAttachment(actualPosition);
	//shieldcollision->SetCollisionObjectType(ECC_GameTraceChannel3);
	//shieldcollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	//shieldcollision->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
	
	shieldMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ShieldMesh"));
	shieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	shieldMesh->SetupAttachment(actualPosition);
	shieldMesh->SetVisibility(false);
	shieldMesh->SetCanEverAffectNavigation(false);
	/*
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/ArtTeam/InfinityBladeGrassLands/Environments/Plains/Env_Plains_Statues/StaticMesh/SM_Plains_Man_Statue_01.SM_Plains_Man_Statue_01'"));
	if (tempMesh.Succeeded()) {
		//shieldMesh->SetStaticMesh(tempMesh.Object);
		//shieldMesh->SetRelativeScale3D(FVector(2.25f));
		//shieldMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
		shieldMesh->SetCollisionObjectType(ECC_GameTraceChannel3);
		shieldMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
		shieldMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
	}
	*/

	//shieldFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("ShieldFX"));
	//shieldFX->SetupAttachment(shieldcollision);
	//shieldFX->SetAutoActivate(false);

	ConstructorHelpers::FObjectFinder<UNiagaraSystem> tempImpact(TEXT("/Script/Niagara.NiagaraSystem'/Game/Assets/ShieldAsset/ShieldImpact/FX/Impact/NS_Impact_00.NS_Impact_00'"));
	if (tempImpact.Succeeded()) {
		impactFX = tempImpact.Object;
	}

	ConstructorHelpers::FObjectFinder<UMaterialInterface> tempMat(TEXT("/Script/Engine.MaterialInstanceConstant'/Game/DVN/VFX/StatueShield/Import/MI_ShieldHIT_00_02.MI_ShieldHIT_00_02'"));
	if (tempMat.Succeeded()) {
		sourceMaterial = tempMat.Object;
		shieldMesh->SetMaterial(0, sourceMaterial);
		
	}

}

void AEnemySpawner::BeginPlay()
{
	Super::BeginPlay();

	gmb = GetWorld()->GetAuthGameMode<ABattleGameModeBase>();
	if (gmb != nullptr) {
		gsb = gmb->GetGameState<ABattleGameStateBase>();
	}

	/*shieldMesh->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnShieldHit);

	mi_Dynamic = shieldMesh->CreateDynamicMaterialInstance(0, sourceMaterial);*/

	//shieldcollision->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnShieldHit);

	// Z�࿡ 0���� ���� �ȵǾ������� ���� ���� -> ���߿� ���ʹ� �������ٶ� Z�� ������ ��ġ�� ������
	//SetActorLocation(FVector(GetActorLocation().X,GetActorLocation().Y,0));

	currentSpawnNum = spawnNum;
	currentSpawnInterval = spawnInterval;

	// ������ƮǮ ũ�� = 1ȸ ���� ������ + �����ı��� ���� ������ * ����
	spawnMaxNum = (FMath::Max(spawnNum, noShieldSpawnNum) + spawnBurstCnt) * 5;

	// ������ ���ʹ� ������ƮǮ ����
	if (enemyType == EEnemyType::Melee && Melee1Factory != nullptr)
	{
		// ���ʹ� Ÿ�Կ� ���� ���� �ܰ� ����
		if (meleeStatueMesh != nullptr) {
			monolithMesh->SetStaticMesh(meleeStatueMesh);
			shieldMesh->SetStaticMesh(meleeStatueMesh);
			/*
			shieldMesh->SetRelativeScale3D(FVector(2.3f, 2.35f, 2.25f));
			shieldMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
			shieldMesh->SetCollisionObjectType(ECC_GameTraceChannel3);
			shieldMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
			shieldMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
			*/
		}
		if (meleeTorch != nullptr) torchComp->SetTemplate(meleeTorch);

		for (int i = 0; i < spawnMaxNum; i++)
		{
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(FVector(0.f, 0.f, -1000.f));
			Melee1 = GetWorld()->SpawnActor<AMeleeEnemy>(Melee1Factory, SpawnTransform, param);

			Melee1Pool.Enqueue(Melee1);
			Melee1->SetActive(false);
			Melee1->SetSpawner(this);
			Melee1->hp = enemyHP;
			Melee1->attackPower = attackPower;
			Melee1->KnockbackDuration = KnockbackDuration;
			Melee1->KnockbackDistance = KnockbackDistance;
			Melee1->gsb = gsb;


			//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("hp: %f, attackPower: %f"), Melee1->hp, Melee1->attackPower));
		}
	}
	else if (enemyType == EEnemyType::Ranged_Ground && RangedGround1Factory != nullptr)
	{
		// ���ʹ� Ÿ�Կ� ���� ���� �ܰ� ����
		if (rangedStatueMesh != nullptr) {
			monolithMesh->SetStaticMesh(meleeStatueMesh);
			shieldMesh->SetStaticMesh(meleeStatueMesh);

		}
		if (rangedTorch != nullptr) torchComp->SetTemplate(rangedTorch);

		for (int i = 0; i < spawnMaxNum; i++)
		{
			FActorSpawnParameters param;
			param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			//RangedGround1 = GetWorld()->SpawnActor<ARangedGroundEnemy>(RangedGround1Factory, param);
			FTransform SpawnTransform;
			SpawnTransform.SetLocation(FVector(0.f, 0.f, -1000.f));
			RangedGround1 = GetWorld()->SpawnActor<ARangedGroundEnemy>(RangedGround1Factory, SpawnTransform, param);

			RangedGround1Pool.Enqueue(RangedGround1);
			RangedGround1->SetActive(false);
			RangedGround1->SetSpawner(this);
			RangedGround1->hp = enemyHP;
			RangedGround1->attackPower = attackPower;
			RangedGround1->KnockbackDuration = KnockbackDuration;
			RangedGround1->KnockbackDistance = KnockbackDistance;
			RangedGround1->gsb = gsb;
			
			//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("hp: %f, attackPower: %f"), RangedGround1->hp, RangedGround1->attackPower));
		}
	}
	shieldMesh->SetRelativeScale3D(FVector(2.3f, 2.35f, 2.25f));
	shieldMesh->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));
	shieldMesh->SetCollisionObjectType(ECC_GameTraceChannel3);
	shieldMesh->SetCollisionResponseToAllChannels(ECR_Ignore);
	shieldMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
	shieldMesh->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnShieldHit);

	monolithMesh->OnComponentBeginOverlap.AddDynamic(this, &AEnemySpawner::OnStatueHit);

	mi_Dynamic = shieldMesh->CreateDynamicMaterialInstance(0, sourceMaterial);

	// ���� �ö���� Ÿ�Ӷ���
	FOnTimelineFloat OnProgressRise;
	OnProgressRise.BindUFunction(this, FName("RiseProcess"));
	FOnTimelineEvent onFinishRise;
	onFinishRise.BindUFunction(this, FName("OnRiseFinished"));

	riseTimeline.AddInterpFloat(riseCurve, OnProgressRise);
	riseTimeline.SetTimelineFinishedFunc(onFinishRise);
	riseTimeline.SetTimelineLength(riseTime);

	// �����ǰ� Ÿ�Ӷ���
	FOnTimelineFloat OnImpact;
	OnImpact.BindUFunction(this, FName("ImpactProcess"));
	/*FOnTimelineEvent onFinishRise;
	onFinishRise.BindUFunction(this, FName("OnRiseFinished"));*/

	impactTimeline.AddInterpFloat(impactCurve, OnImpact);
	//riseTimeline.SetTimelineFinishedFunc(onFinishRise);
	impactTimeline.SetTimelineLength(impactTime);

	FOnTimelineFloat OnShieldOn;
	OnShieldOn.BindUFunction(this, FName("ShieldOnProcess"));
	shieldOnTimeline.AddInterpFloat(shieldOnCurve, OnShieldOn);
	shieldOnTimeline.SetTimelineLength(shieldOnTime);

	FOnTimelineFloat OnShieldOff;
	OnShieldOff.BindUFunction(this, FName("ShieldOffProcess"));
	shieldOffTimeline.AddInterpFloat(shieldOffCurve, OnShieldOff);
	shieldOffTimeline.SetTimelineLength(shieldOffTime);

	riseStartLoc = underGroundPosition->GetRelativeLocation();
	SetActorTickEnabled(false);
	spawnerCurrHP = spawnerHP;

	debrisCurrtime = debrisInterval;
}

void AEnemySpawner::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	
	riseTimeline.TickTimeline(DeltaTime);
	impactTimeline.TickTimeline(DeltaTime);
	shieldOnTimeline.TickTimeline(DeltaTime);
	//shieldOffTimeline.TickTimeline(DeltaTime);

	if (bIsRising) {
		debrisCurrtime -= DeltaTime;
		if (debrisCurrtime < 0.f) {
			if (riseDebris != nullptr) {
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), riseDebris, riseSmoke->GetComponentLocation());
			}
			debrisCurrtime = debrisInterval;
		}
	}

	if (spawnerState == ESpawnerState::Inactive || spawnerState == ESpawnerState::Destroyed) return;

	currentTime += DeltaTime;

	if (currentTime > currentSpawnInterval) {

		//if (spawnerState == ESpawnerState::WithShield) DamageShield();

		//if (gsb != nullptr) gsb->totalEnemyCnt += currentSpawnNum;
		

		if (enemyType == EEnemyType::Melee || GetName().Contains(FString(TEXT("Melee")))) {
			SpawnMelee1();
			currentTime = 0;
		}
		else if (enemyType == EEnemyType::Ranged_Ground || GetName().Contains(FString(TEXT("RangedGround")))) {
			SpawnRangedGround1();
			currentTime = 0;
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, FString::Printf(TEXT("Spawn")));
		}
	}

	
}

void AEnemySpawner::OnDamage_Interface(float DamageAmount)
{
	//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("OnDamageInterface1")));
	switch (spawnerState)
	{
	case ESpawnerState::WithShield:
		//OnShieldHit();
		break;
	case ESpawnerState::NoShield:
		DamageMonolith(DamageAmount);
		break;
	case ESpawnerState::Inactive:
		break;
	case ESpawnerState::Destroyed:
		break;
	default:
		break;
	}
}

void AEnemySpawner::OnWaveStarted()
{
	SetActorTickEnabled(true);
	if (gsb != nullptr) gsb->monolithCnt++;
	
	FTimerHandle riseTimer;
	GetWorldTimerManager().SetTimer(riseTimer, FTimerDelegate::CreateLambda([&]() {
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), riseSound, GetActorLocation(), 0.1);
		riseSmoke->Activate(true);

		bIsRising = true;
		riseTimeline.PlayFromStart();
		}), 1.f, false);
}

void AEnemySpawner::RiseProcess(float alpha)
{
	FVector moveLoc = FMath::Lerp(riseStartLoc, FVector::ZeroVector, alpha);
	float x = FMath::RandRange(-7.f, 7.f);
	float y = FMath::RandRange(-7.f, 7.f);

	actualPosition->SetRelativeLocation(FVector(x, y, moveLoc.Z));

}

void AEnemySpawner::OnRiseFinished()
{
	spawnerState = ESpawnerState::WithShield;
	riseSmoke->Deactivate();
	bIsRising = false;

	shieldOnTimeline.PlayFromStart();
	FTimerHandle fireTimer;
	GetWorldTimerManager().SetTimer(fireTimer, FTimerDelegate::CreateLambda([&]() {
		switch (enemyType)
		{
		case EEnemyType::Melee:
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), meleeBurst, torchComp->GetComponentLocation(), FRotator::ZeroRotator, FVector(50.f));
			break;
		case EEnemyType::Ranged_Ground:
			UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), rangedBurst, torchComp->GetComponentLocation(), FRotator::ZeroRotator, FVector(50.f));
			break;
		default:
			break;
		}
		torchComp->SetActive(true);
	}), 1.f, false);
	shieldMesh->SetVisibility(true);
	shieldMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	//shieldFX->SetActive(true);
	//shieldcollision->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);
}

void AEnemySpawner::DamageShield()
{
	shieldCountDown--;
	if (shieldCountDown < 1) {
		// �����Ȱ��ȭ
		OnShieldDestroyed();
	}
}

void AEnemySpawner::OnShieldHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnergyBullet* bullet = Cast<AEnergyBullet>(OtherActor);
	if (bullet != nullptr) {
		// ������
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("Damage Shield")));

		// ����ü�°���
		shieldCountDown--;
		// �����ǰ�����Ʈ
		UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), impactFX, SweepResult.ImpactPoint, UKismetMathLibrary::MakeRotFromX(SweepResult.ImpactNormal));
		mi_Dynamic->SetVectorParameterValue(FName("Pos"),  UKismetMathLibrary::Conv_VectorToLinearColor(SweepResult.ImpactPoint));

		//mi_Dynamic = shieldMesh->CreateDynamicMaterialInstance(0, sourceMaterial);
		impactTimeline.PlayFromStart();
		// �����ǰݻ���
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), impactSound, SweepResult.ImpactPoint);

		// �����ı���
		if (shieldCountDown < 1) {
			// �����Ȱ��ȭ
			OnShieldDestroyed();
		}
	}
}

void AEnemySpawner::OnStatueHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AEnergyBullet* bullet = Cast<AEnergyBullet>(OtherActor);
	if (bullet != nullptr) {
		// ������
		//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("Damage Statue")));

		// �����ǰ�����Ʈ
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), statueHitImpact, SweepResult.ImpactPoint, SweepResult.ImpactNormal.ToOrientationRotator());
		// tjrtkd�ǰݻ���
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(), impactSound, SweepResult.ImpactPoint);

	}
}

void AEnemySpawner::ImpactProcess(float alpha)
{
	mi_Dynamic->SetScalarParameterValue(FName("Radius"), alpha * 250.f);
}

void AEnemySpawner::DamageMonolith(float damage)
{
	// ������
	//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("Damage Monolith")));
	spawnerCurrHP = FMath::Max(0.f, spawnerCurrHP - damage);
	if (spawnerCurrHP < 1.f) {
		OnSpawnerDestroyed();
	}
}

void AEnemySpawner::ShieldOnProcess(float alpha)
{
	mi_Dynamic->SetScalarParameterValue(FName("Opacity"), alpha);
}

void AEnemySpawner::ShieldOffProcess(float alpha)
{
	mi_Dynamic->SetScalarParameterValue(FName("Opacity"), alpha);
}

void AEnemySpawner::OnShieldDestroyed()
{
	spawnerState = ESpawnerState::NoShield;
	//mi_Dynamic = shieldMesh->CreateDynamicMaterialInstance(0, sourceMaterial);
	shieldOnTimeline.ReverseFromEnd();
	//shieldOffTimeline.PlayFromStart();

	FTimerHandle destroyShieldTimer;
	GetWorldTimerManager().SetTimer(destroyShieldTimer, FTimerDelegate::CreateLambda([&]() {
		shieldMesh->SetVisibility(false);
		}), 1.f, false);
	shieldMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	monolithCollision->SetCollisionObjectType(ECC_GameTraceChannel3);
	monolithCollision->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);

	monolithMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	monolithMesh->SetCollisionObjectType(ECC_GameTraceChannel3);
	monolithMesh->SetCollisionResponseToChannel(ECC_GameTraceChannel4, ECR_Block);

	currentSpawnNum = noShieldSpawnNum;
	currentSpawnInterval = noShieldSpawnInterval;
}

void AEnemySpawner::OnSpawnerDestroyed()
{
	// ������
	//GEngine->AddOnScreenDebugMessage(-1, 50.0f, FColor::Red, FString::Printf(TEXT("Spawner Destroyed")));

	spawnerState = ESpawnerState::Destroyed;

	UGameplayStatics::PlaySoundAtLocation(GetWorld(), destroySound, GetActorLocation());
	FVector destructionLoc = GetActorLocation();
	destructionLoc.Z += 450.f;
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), riseDebris, destructionLoc);
	SetActorEnableCollision(false);
	SetActorHiddenInGame(true);
	if(gsb != nullptr) gsb->monolithCnt--;
	SetActorTickEnabled(false);
	PlayChaosDestruction();
}

void AEnemySpawner::SpawnMelee1()
{
	if (spawnerState == ESpawnerState::Inactive || spawnerState == ESpawnerState::Destroyed) return;
	// �� �Ͽ� ��ȯ�� ������ ��ŭ ��ȯ
	for (int32 i = 0; i < currentSpawnNum; i++) {
		// ������ƮǮ�� ��������� break
		if (Melee1Pool.IsEmpty()) break;

		AMeleeEnemy* spawnedMelee1;
		Melee1Pool.Dequeue(spawnedMelee1);
		if (gsb != nullptr) gsb->totalEnemyCnt++;
		// �Ʒ� ó���� ƽ�Լ��� �̵�
		//if (gsb != nullptr) gsb->totalEnemyCnt++;


		if (spawnedMelee1 != nullptr && Melee1->target != nullptr) {

			// ���� �������� ����մϴ�. ���⼭�� X�� Y �࿡ ���� �������� �߰��մϴ�.
			float radius = FMath::RandRange(170.f, 400.f);
			float angle = FMath::RandRange(0.f, 360.f);
			float xCoord= radius * FMath::Cos(FMath::DegreesToRadians(angle));
			float yCoord= radius * FMath::Sin(FMath::DegreesToRadians(angle));
			//spawnZHeight = RootComponent->GetComponentLocation().Z;
			FVector RandomOffset = FVector(xCoord, yCoord, spawnZHeight);

			spawnedMelee1->SetActorLocation(GetActorLocation() + RandomOffset);

			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Melee1->target->GetActorLocation());
			FRotator YawOnlyRotation = FRotator(0.0f, LookAtRotation.Yaw, 0.0f);
			spawnedMelee1->SetActorRotation(YawOnlyRotation);

		}
		spawnedMelee1->SetActive(true);
	}

	// ���� �ı� ���� ù ������ ����� �� �߰���������
	if (spawnerState == ESpawnerState::NoShield && !bIsBurstActivated) {
		bIsBurstActivated = true;
		for (int32 i = 0; i < spawnBurstCnt; i++) {
			// ������ƮǮ�� ��������� break
			if (Melee1Pool.IsEmpty()) break;

			AMeleeEnemy* spawnedMelee1;
			Melee1Pool.Dequeue(spawnedMelee1);
			if (gsb != nullptr) gsb->totalEnemyCnt++;
			// �Ʒ� ó���� ƽ�Լ��� �̵�
			//if (gsb != nullptr) gsb->totalEnemyCnt++;


			if (spawnedMelee1 != nullptr && Melee1->target != nullptr) {

				// ���� �������� ����մϴ�. ���⼭�� X�� Y �࿡ ���� �������� �߰��մϴ�.
				float radius = FMath::RandRange(170.f, 400.f);
				float angle = FMath::RandRange(0.f, 360.f);
				float xCoord = radius * FMath::Cos(FMath::DegreesToRadians(angle));
				float yCoord = radius * FMath::Sin(FMath::DegreesToRadians(angle));
				//spawnZHeight = RootComponent->GetComponentLocation().Z;
				FVector RandomOffset = FVector(xCoord, yCoord, spawnZHeight);

				spawnedMelee1->SetActorLocation(GetActorLocation() + RandomOffset);

				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Melee1->target->GetActorLocation());
				FRotator YawOnlyRotation = FRotator(0.0f, LookAtRotation.Yaw, 0.0f);
				spawnedMelee1->SetActorRotation(YawOnlyRotation);

			}
			spawnedMelee1->SetActive(true);
		}
	}
	/*
	for (int i = 0; i < spawnNum && Melee1Pool.Num() > 0; i++)
	{

		AMeleeEnemy* spawnedMelee1;
		Melee1Pool.Dequeue(spawnedMelee1);

		if (gsb != nullptr) gsb->totalEnemyCnt++;
		
		spawnedMelee1->SetActive(true);

		if (spawnedMelee1 != nullptr && Melee1->target !=nullptr) {

			// ���� �������� ����մϴ�. ���⼭�� X�� Y �࿡ ���� �������� �߰��մϴ�.
			FVector RandomOffset = FVector(FMath::RandRange(-200, 200), FMath::RandRange(-300, 300), spawnZHeight);

			spawnedMelee1->SetActorLocation(GetActorLocation()+RandomOffset);

			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), Melee1->target->GetActorLocation());
			FRotator YawOnlyRotation = FRotator(0.0f, LookAtRotation.Yaw, 0.0f);
			spawnedMelee1->SetActorRotation(YawOnlyRotation);

		}
	}
	*/
}


void AEnemySpawner::SpawnRangedGround1()
{
	if (spawnerState == ESpawnerState::Inactive || spawnerState == ESpawnerState::Destroyed) return;
	// �� �Ͽ� ��ȯ�� ������ ��ŭ ��ȯ
	for (int32 i = 0; i < currentSpawnNum; i++) {
		// ������ƮǮ�� ��������� break
		if (RangedGround1Pool.IsEmpty()) break;

		ARangedGroundEnemy* spawnedRangedGround;
		RangedGround1Pool.Dequeue(spawnedRangedGround);
		if (gsb != nullptr) gsb->totalEnemyCnt++;
		// �Ʒ� ó���� ƽ�Լ��� �̵�
		//if (gsb != nullptr) gsb->totalEnemyCnt++;

		if (spawnedRangedGround != nullptr && RangedGround1->target != nullptr) {

			// ���� �������� ����մϴ�. ���⼭�� X�� Y �࿡ ���� �������� �߰��մϴ�.
			FVector RandomOffset = FVector(FMath::RandRange(-200, 200), FMath::RandRange(-300, 300), spawnZHeight);

			spawnedRangedGround->SetActorLocation(GetActorLocation() + RandomOffset);

			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), RangedGround1->target->GetActorLocation());
			FRotator YawOnlyRotation = FRotator(0.0f, LookAtRotation.Yaw, 0.0f);
			spawnedRangedGround->SetActorRotation(YawOnlyRotation);
		}

		spawnedRangedGround->SetActive(true);
	}

	// ���� �ı� ���� ù ������ ����� �� �߰���������
	if (spawnerState == ESpawnerState::NoShield && !bIsBurstActivated) {
		bIsBurstActivated = true;
		for (int32 i = 0; i < spawnBurstCnt; i++) {
			// ������ƮǮ�� ��������� break
			if (RangedGround1Pool.IsEmpty()) break;
			if (gsb != nullptr) gsb->totalEnemyCnt++;
			ARangedGroundEnemy* spawnedRangedGround;
			RangedGround1Pool.Dequeue(spawnedRangedGround);

			// �Ʒ� ó���� ƽ�Լ��� �̵�
			//if (gsb != nullptr) gsb->totalEnemyCnt++;

			if (spawnedRangedGround != nullptr && spawnedRangedGround->target != nullptr) {

				// ���� �������� ����մϴ�. ���⼭�� X�� Y �࿡ ���� �������� �߰��մϴ�.
				float radius = FMath::RandRange(170.f, 400.f);
				float angle = FMath::RandRange(0.f, 360.f);
				float xCoord = radius * FMath::Cos(FMath::DegreesToRadians(angle));
				float yCoord = radius * FMath::Sin(FMath::DegreesToRadians(angle));
				//spawnZHeight = RootComponent->GetComponentLocation().Z;
				FVector RandomOffset = FVector(xCoord, yCoord, spawnZHeight);

				spawnedRangedGround->SetActorLocation(GetActorLocation() + RandomOffset);

				FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), spawnedRangedGround->target->GetActorLocation());
				FRotator YawOnlyRotation = FRotator(0.0f, LookAtRotation.Yaw, 0.0f);
				spawnedRangedGround->SetActorRotation(YawOnlyRotation);

			}

			spawnedRangedGround->SetActive(true);
		}
	}

	/*
	for (int i = 0; i < spawnNum && RangedGround1Pool.Num() > 0; i++)
	{
		if (RangedGround1Pool.Num() == 0) break;
		ARangedGroundEnemy* spawnedRangedGround = RangedGround1Pool.Last();

		if (gsb != nullptr) gsb->totalEnemyCnt++;

		spawnedRangedGround->SetActive(true);
		RangedGround1Pool.RemoveAt(RangedGround1Pool.Num() - 1);
		if (spawnedRangedGround != nullptr && RangedGround1->target != nullptr)
		{
			FVector RandomOffset = FVector(FMath::RandRange(-500, 100), FMath::RandRange(-300, 300), spawnZHeight);
			spawnedRangedGround->SetActorLocation(GetActorLocation() + RandomOffset);


			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(GetActorLocation(), RangedGround1->target->GetActorLocation());
			FRotator YawOnlyRotation = FRotator(0.0f, LookAtRotation.Yaw, 0.0f);
			spawnedRangedGround->SetActorRotation(YawOnlyRotation);
		}
		//spawnedRangedGround1Count++;
		//if (spawnedRangedGround1Count >= spawnMaxNum)	break;
	}
	*/
}

void AEnemySpawner::DestroyAll()
{
	Melee1Pool.Empty();
	RangedGround1Pool.Empty();
}
