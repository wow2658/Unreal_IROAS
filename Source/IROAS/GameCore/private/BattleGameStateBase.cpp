

#include "BattleGameStateBase.h"
#include "BattleGameModeBase.h"
#include "IROASGameInstance.h"
#include "EnemySpawner.h"
#include <Engine/StaticMeshActor.h>
#include <Engine/DataTable.h>
#include <Kismet/GameplayStatics.h>
#include "MovableFrame.h"
#include <Engine/StaticMesh.h>
#include <UObject/ConstructorHelpers.h>
#include "ShootableUIActor.h"
#include <GameFramework/Actor.h>
#include "BattlePlayer.h"
#include "HttpRequestActor.h"
#include "RankingBoardWidget.h"
#include "BattleResultActor.h"
#include "Boss.h"
#include "BossFSM.h"
#include "BaseEnemy.h"
#include "SnapMovementComponent.h"
#include "BattlePlayerWidgetManager.h"
#include <Sound/AmbientSound.h>
#include <Components/AudioComponent.h>
#include <GameFramework/PlayerController.h>

ABattleGameStateBase::ABattleGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
	//ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/YSL/UI/3DButton/ui_Building1.ui_Building1'"));
	//for (auto path : meshPaths) {
	//	UStaticMesh* tempMesh;
	//	FString assetPath = path.Value;
	//	ConstructorHelpers::FObjectFinder<UStaticMesh> loadedMesh(*assetPath);
	//	//ConstructorHelpers::FObjectFinder<UStaticMesh> loadedMesh((TEXT("%s"), *path.Value));
	//	if (loadedMesh.Succeeded()) {
	//		tempMesh = loadedMesh.Object;
	//		meshAssets[path.Key] = tempMesh;
	//	}
	//}	
 
	
}

void ABattleGameStateBase::BeginPlay()
{
	Super::BeginPlay();
	gm = GetWorld()->GetAuthGameMode<ABattleGameModeBase>();

	if (gm != nullptr) {
		gi = gm->gi;
		if (gi != nullptr) {
			token = gi->token;
			userNickName = gi->userNickName;
			bIsSteamId = gi->bIsSteamId;
			// 커스텀맵이면 맵정보 읽어오기
			while (bIsCustomMap && mapBuildInfo == nullptr) {
				mapBuildInfo = &(gi->customMapInfo);
			}
		}

		for (auto spawner : gm->spawners) {
			switch (spawner->activationWave)
			{
			case 1:
				wave_1_spawners.Add(spawner);
				break;
			case 2:
				wave_2_spawners.Add(spawner);
				break;
			default:
				break;
			}
			 
		}
		allSpawners.Add(wave_1_spawners);
		allSpawners.Add(wave_2_spawners);
		gm->onBossSpawned.AddDynamic(this, &ABattleGameStateBase::GetbossRef);
	}

	/*
	gi = Cast< UIROASGameInstance>(GetGameInstance());
	if (gi != nullptr) {
		token = gi->token;
		userNickName = gi->userNickName;
		bIsSteamId = gi->bIsSteamId;
		// 커스텀맵이면 맵정보 읽어오기
		while (bIsCustomMap && mapBuildInfo == nullptr) {
			mapBuildInfo = &(gi->customMapInfo);
		}
	}
	*/
	
	// 커스텀맵일 경우에 읽어온 맵정보대로 빌드하기.
	if (bIsCustomMap) {
		ABattleGameModeBase* tempGM = Cast<ABattleGameModeBase>(GetWorld()->GetAuthGameMode());
		if (tempGM != nullptr) gm = tempGM;
	
		AMovableFrame* tempPlayerStart = Cast<AMovableFrame>(UGameplayStatics::GetActorOfClass(GetWorld(), AMovableFrame::StaticClass()));
		
		if (mapDatas != nullptr) {
			mapBuildInfo = mapDatas->FindRow<FMapInfoTable>(TEXT("NewRow"), TEXT("ContextString"));
			
		}
		if (tempPlayerStart != nullptr) {
			tempPlayerStart->SetActorLocation(FVector(mapBuildInfo->playerStart_x, mapBuildInfo->playerStart_y, mapBuildInfo->playerStart_z));
			tempPlayerStart->SetActorRotation(FRotator(0, mapBuildInfo->playerStart_yaw, 0));
		}
		BuildMap();
	}

	// 시작트리거 구체 소환
	if (!bIsLobby && bp_startTrigger != nullptr) {
		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		startTriggerRef = GetWorld()->SpawnActor<AShootableUIActor>(bp_startTrigger, FVector(-187.f, 1023.f, 926.f), FRotator(0.f), Param);

		// 시작트리거가 파괴될 때(게임 시작) 처리할 작업 바인딩.

		startTriggerRef->OnDestroyed.AddDynamic(this, &ABattleGameStateBase::StartGame);
		
	}
	// 랭킹보드 액터 소환
	if (!bIsLobby && bp_rankingUI != nullptr) {
		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform spawnTrans;
		spawnTrans.SetLocation(FVector(1600.f, 40000.f, 720.f));
		spawnTrans.SetRotation(FRotator(0.f, -90.f, 0.f).Quaternion());
		//spawnTrans.SetScale3D(FVector(0.3f));
		rankingUIRef = GetWorld()->SpawnActor<ABattleResultActor>(bp_rankingUI, spawnTrans, Param);
		//rankingUIRef->SetActorScale3D(FVector(0.3f));
		rankingUIRef->SetActorHiddenInGame(true);
		TArray<AActor*> childActors;
		rankingUIRef->GetAllChildActors(childActors);
		for (AActor* child : childActors) {
			child->SetActorEnableCollision(false);
		}
		rankingUIRef->SetActorEnableCollision(false);

	}
	// 보상UI소환
	if (!bIsLobby && bp_rewardUI != nullptr) {
		FActorSpawnParameters Param;
		Param.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		FTransform spawnTrans;
		spawnTrans.SetLocation(FVector(-180.000000, -280.000000, 140.000000));
		spawnTrans.SetRotation(FRotator(0.f, -90.f, 0.f).Quaternion());
		rewardUIRef = GetWorld()->SpawnActor<ABattleResultActor>(bp_rewardUI, spawnTrans, Param);
		rewardUIRef->SetActorScale3D(FVector(0.3f));
		rewardUIRef->SetActorHiddenInGame(true);
		TArray<AActor*> childActors;
		rewardUIRef->GetAllChildActors(childActors);
		for (AActor* child : childActors) {
			child->SetActorEnableCollision(false);
		}
		rewardUIRef->SetActorEnableCollision(false);
		rewardUIRef->OnDestroyed.AddDynamic(this, &ABattleGameStateBase::OnRewardSelected);
	}
	
	// HTTP통신액터 소환
	reqActor = GetWorld()->SpawnActorDeferred<AHttpRequestActor>(AHttpRequestActor::StaticClass(), FTransform::Identity);
	if (reqActor != nullptr) {
		reqActor->gameModeType = EGameModeType::StoryMode;
		reqActor->FinishSpawning(FTransform::Identity);
	}
	
	//reqActor = GetWorld()->SpawnActor<AHttpRequestActor>();
	
	bgmPlayer = GetWorld()->SpawnActor<AAmbientSound>();
	if (bgmPlayer != nullptr) {
		if (waveBgm != nullptr) {
			bgmPlayer->GetAudioComponent()->SetSound(waveBgm);
			bgmPlayer->GetAudioComponent()->FadeIn(1.f, 0.65f, 0.f, EAudioFaderCurve::Logarithmic);
		}
	}

	slomoCurrTime = slowMoTime;
	
}

void ABattleGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (bIsSlowMotion) {
		slomoCurrTime -= DeltaSeconds;
		if (slomoCurrTime < 0.f) {
			bIsSlowMotion = false;
			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1.0f);
		}
	}

	if (bWaveInProgress) {
		if ((monolithCnt == 0 && totalEnemyCnt == 0) || (bossFsmRef != nullptr && bossFsmRef->mBossState == EEnemyState::Dead)) {

			// 웨이브 1이 끝났으면 할 일들
			if (currWave == 1) {
				bWaveInProgress = false;

				UGameplayStatics::SetGlobalTimeDilation(GetWorld(), sloMoRatio);
				slomoCurrTime = slowMoTime;
				bIsSlowMotion = true;

				bgmPlayer->GetAudioComponent()->FadeOut(2.f, 0.f, EAudioFaderCurve::Logarithmic);
				//bgmPlayer->GetAudioComponent()->SetSound(bossBgm);
				if (player_1 != nullptr) player_1->bIsUIMode = true;
				// 1. 보상 띄우기
				player_1->widgetManager->SetDefaultIndicator();
				player_1->widgetManager->DieIndicate();
				player_1->widgetManager->bIndicatorOff = true;
				rewardUIRef->SetActorHiddenInGame(false);
				TArray<AActor*> childActors;
				rewardUIRef->GetAllChildActors(childActors);
				for (AActor* child : childActors) {
					child->SetActorEnableCollision(true);
					if (AShootableUIActor* reward = Cast<AShootableUIActor>(child)) {
						reward->OnOrbActivate();
					}
				}
				rewardUIRef->SetActorEnableCollision(true);
				// 2. 보상 선택 --> 플레이어 사격.
				// 3. 보상 UI없애기 --> 보상 선택 시 UI파괴됨

				// 4. 보스 등장 시네마틱 재생 --> 보상 파괴 시 OnRewardSelected함수에서 자동호출 
				// 5. 보스 포효타이밍에 웨이브 2 시작--> 보상 파괴 시 OnRewardSelected함수에서 자동호출 
				
			}
			else if (currWave == 2) {
				if (bossFsmRef != nullptr && bossFsmRef->mBossState == EEnemyState::Dead) {
					bClearYN = true;
					OnGameFinished();
				}
			}
		}
	}

	// 타이머
	if (bIsGameStarted && !bIsGameFinished && player_1 != nullptr) {
		
		if (player_1->bIsUIMode) {
			uiModetime += DeltaSeconds;
		}
		else {
			// 게임이 끝날때까지 시간 기록
			gameTimeElapsed = GetWorld()->GetTimeSeconds() - gameStartTime - uiModetime;
			// 위젯 시계 동기화
			player_1->widgetManager->CallSetTimer(gameTimeElapsed);

			GEngine->AddOnScreenDebugMessage(1, 90, FColor::Green, FString::Printf(TEXT("Elapsed Time: %.2f\nCurrentEnemycount: %d\nStatueCount: %d\nWaveInProgress: %s"), gameTimeElapsed, totalEnemyCnt, monolithCnt, bWaveInProgress ? *FString::Printf(TEXT("TRUE")) : *FString::Printf(TEXT("FALSE"))));
		}
	}
}

void ABattleGameStateBase::BuildMap()
{
	
	
	FTransform loadTrans;
	loadTrans.SetLocation(FVector(0, 0, -10000));

	for (auto spawnerInfo : mapBuildInfo->spawnerList) {
		//AEnemySpawner* tempSpawner = GetWorld()->SpawnActorDeferred<AEnemySpawner>(bp_spawner, loadTrans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		AEnemySpawner* tempSpawner = GetWorld()->SpawnActorDeferred<AEnemySpawner>(AEnemySpawner::StaticClass(), loadTrans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (tempSpawner != nullptr) {
			tempSpawner->Melee1Factory = Melee1Factory;
			tempSpawner->RangedGround1Factory = RangedGround1Factory;
			tempSpawner->enemyType = spawnerInfo.enemyType;
			tempSpawner->spawnInterval = spawnerInfo.spawnInterval;
			tempSpawner->enemyHP = spawnerInfo.enemyHP;
			tempSpawner->attackPower = spawnerInfo.enemyPower;

			loadTrans.SetLocation(FVector(spawnerInfo.x, spawnerInfo.y, spawnerInfo.z));
			spawners.Add(tempSpawner);
			tempSpawner->FinishSpawning(loadTrans);
		}
	}
	
	FTransform propLoadTrans;
	propLoadTrans.SetLocation(FVector(0, 0, -5000));
	for (auto propInfo : mapBuildInfo->propList) {
		AStaticMeshActor* prop = GetWorld()->SpawnActorDeferred<AStaticMeshActor>(AStaticMeshActor::StaticClass(), propLoadTrans, this, nullptr, ESpawnActorCollisionHandlingMethod::AlwaysSpawn);
		if (prop != nullptr) {
			int32 idx = props.Add(prop);
			/*if (propMeshes.IsValidIndex(idx) && meshAssets.Contains(propInfo.propID)) {
				props[idx]->GetStaticMeshComponent()->SetStaticMesh(meshAssets[propInfo.propID]);
			}*/
			if (meshAssets.Contains(propInfo.propID)) {
				props[idx]->GetStaticMeshComponent()->SetStaticMesh(meshAssets[propInfo.propID]);
			}
			propLoadTrans.SetLocation(FVector(propInfo.x, propInfo.y, propInfo.z));
			propLoadTrans.SetRotation(FQuat::MakeFromRotator(FRotator(0, propInfo.yaw, 0)));
			prop->FinishSpawning(propLoadTrans);
		}
	}
}


/**********************************
명    칭: StartGame
기    능: 시작 트리거 파괴 시 때 처리할 작업들
	1. 타이머 시작
	2. 시작시간 기록
	3. 웨이브 시작
**********************************/
void ABattleGameStateBase::StartGame(AActor* DestroyedActor)
{
	// 1. 타이머 시작
	FDateTime CurrentTime = FDateTime::UtcNow();
	FDateTime KoreaNow = CurrentTime + FTimespan::FromHours(9);
	playStartTime = KoreaNow.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
	gameTimeElapsed = 0.0f;
	bIsGameStarted = true;
	// 시간 기록 시작
	gameStartTime = GetWorld()->GetTimeSeconds();
	// 2. 전투 활성화
	StartNextWave();

}

/**********************************
명    칭: OnRewardSelected
기    능: 보상 선택 시 호출될 콜백함수
	1. 카메라 페이드인/아웃
	2. 플레이어 이동
	3. 보스 스폰(시네마틱재생 포함)
	4. 웨이브시작
**********************************/
void ABattleGameStateBase::OnRewardSelected(AActor* DestroyedActor)
{
	if (gm != nullptr) {
		if (player_1 == nullptr) return;
		//player_1->pc->PlayerCameraManager->StartCameraFade(0.f, 1.f, 3.0f, FLinearColor::Black, false, true);
		player_1->widgetManager->loadingCubeRef->SetActorHiddenInGame(false);
		player_1->widgetManager->SetSecondeLodingCube();
		player_1->snapMoveComp->SwitchMovableFrame();
		
		// 웨이브 2 시작 전 전체 회복
		player_1->GainEnergy(player_1->maxEnergy - player_1->energy);
		player_1->OnHeal(player_1->maxHealth - player_1->health);
		player_1->ReloadAllGuns();
		player_1->widgetManager->bIndicatorOff = false;
		player_1->widgetManager->curHealthPer = 1.f;

		/*FTimerHandle teleportTimer;
		GetWorldTimerManager().SetTimer(teleportTimer, FTimerDelegate::CreateLambda([&]() {
			player_1->snapMoveComp->SwitchMovableFrame();
			player_1->pc->PlayerCameraManager->StartCameraFade(1.f, 0.f, 4.f, FLinearColor::Black, false, false);
			}), 3.8f, false);*/

		FTimerHandle bossSpawnTimer;
		GetWorldTimerManager().SetTimer(bossSpawnTimer, FTimerDelegate::CreateLambda([&]() {
			gm->SpawnBoss();
			player_1->widgetManager->loadingCubeRef->SetActorHiddenInGame(true);
			bgmPlayer->GetAudioComponent()->SetSound(bossBgm);
			bgmPlayer->FadeIn(2.f, 1.f);
			}), 4.f, false);

		FTimerHandle hapticTimer;
		GetWorldTimerManager().SetTimer(hapticTimer, FTimerDelegate::CreateLambda([&]() {
			player_1->pc->PlayHapticEffect(roarHaptic, EControllerHand::Left, 1.f, false);
			player_1->pc->PlayHapticEffect(roarHaptic, EControllerHand::Right, 1.f, false);
			}), 19.f, false);

		FTimerHandle nextWaveTimer;
		GetWorldTimerManager().SetTimer(nextWaveTimer, FTimerDelegate::CreateLambda([&]() {
			//currWave++;
			StartNextWave();
			}), 15.f, false);
	}
}

/**********************************
명    칭: WaveStart
기    능: 웨이브 시작 시 작업
	1. 웨이브 카운트 증가
	2. 플레이어 UI모드 끄기
	3. 해당 웨이브 스포너(비석)들 활성화하기
	4. 웨이브 진행중 플래그 키기
**********************************/
void ABattleGameStateBase::StartNextWave()
{
	currWave++;
	if (player_1 != nullptr) player_1->bIsUIMode = false;
	for (auto spnr : allSpawners[currWave - 1]) {
		spnr->OnWaveStarted();
	}
	bWaveInProgress = true;
}

void ABattleGameStateBase::OnGameFinished()
{
	if (player_1 != nullptr) {
		bWaveInProgress = false;
		bIsGameFinished = true;
		player_1->bIsUIMode = true;
		onGameFinishDelegate.Broadcast(player_1->IsDead());
		// 1. 플레이어 모든 행동 불가, UI상호작용만 가능
		
		// 2. 끝시간 기록
		FDateTime CurrentTime = FDateTime::UtcNow();
		FDateTime KoreaNow = CurrentTime + FTimespan::FromHours(9);
		playClearTime = KoreaNow.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
		//secondWaveTakenTime = gameTimeElapsed - firstWaveTakenTime;
		elapsedTimeMiliSec = static_cast<int64>(gameTimeElapsed * 1000);

		// 3. 성공여부 기록
		bClearYN = !player_1->IsDead();
		// 3.1. 서버에 결과보내기.
		if (bIsDebuggingMode) {
			DebuggingLearderBoards();
			reqActor->leaderBoards = tempLeaderBoards;
			if (!tempLeaderBoards.IsEmpty()) {
				for (int32 i = 0; i < tempLeaderBoards.Num(); i++) {
					rankingUIRef->resultWidget->SetLeaderBoardPlayerName(i, tempLeaderBoards[i].playerNickname);
					rankingUIRef->resultWidget->SetLeaderBoardPlayTime(i, tempLeaderBoards[i].playMilliSecond);
				}
				rankingUIRef->resultWidget->SetPlayerTime();
			}
			rankingUIRef->SetActorHiddenInGame(false);
			TArray<AActor*> childActors;
			rankingUIRef->GetAllChildActors(childActors);
			for (AActor* child : childActors) {
				child->SetActorEnableCollision(true);
			}
			rankingUIRef->SetActorEnableCollision(true);
		}
		else if (reqActor != nullptr) {
			reqActor->bgsb = this;
			SetData();
		}

		for (auto spawner : wave_1_spawners) {
			if(spawner != nullptr) spawner->spawnerState = ESpawnerState::Destroyed;
		}
		for (auto spawner : wave_2_spawners) {
			if (spawner != nullptr) spawner->spawnerState = ESpawnerState::Destroyed;
		}
		TArray<AActor*> allEnemies;
		UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), allEnemies);
		for (AActor* enemy : allEnemies) {
			enemy->Destroy();
		}
		for (auto spawner : wave_1_spawners) {
			if (spawner != nullptr) spawner->Destroy();
		}
		for (auto spawner : wave_2_spawners) {
			if (spawner != nullptr) spawner->Destroy();
		}
		
		// 4. 클리어를 못했다면 (적들이 존재함, 플레이어 죽음)
		if (!bClearYN) {
			// 4.1. 적들 공격 중단
			/*for (auto spawner : wave_1_spawners) {
				spawner->spawnerState = ESpawnerState::Destroyed;
			}
			for (auto spawner : wave_2_spawners) {
				spawner->spawnerState = ESpawnerState::Destroyed;
			}
			TArray<AActor*> allEnemies;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABaseEnemy::StaticClass(), allEnemies);
			for (AActor* enemy : allEnemies) {
				enemy->Destroy();
			}
			for (auto spawners : allSpawners) {
				for (auto spanwer : spawners) {
					spanwer->Destroy();
				}
			}*/
			//// 4.2. 실패 UI띄우기
			//FVector failUILoc;
			//failUILoc = player_1->GetActorLocation() + player_1->GetActorForwardVector() * 1500.f;
			//failUILoc.Z = 660.f;
			//rankingUIRef->SetActorLocation(failUILoc);
			//rankingUIRef->SetActorHiddenInGame(false);
			//TArray<AActor*> childActors;
			//rankingUIRef->GetAllChildActors(childActors);
			//for (AActor* child : childActors) {
			//	child->SetActorEnableCollision(true);
			//	if (AShootableUIActor* failActor = Cast<AShootableUIActor>(child)) {
			//		failActor->OnOrbActivate();
			//	}
			//}
			//rankingUIRef->SetActorEnableCollision(true);
			//rankingUIRef->resultWidget->SwitchCanvas(1);
		}

		// 5. 클리어했다면 (모든 적 없음, 플레이어 생존)
		else {
			// 5.1. 랭킹 추가(DB) --> 위에서 끝시간 기록할때 하는것으로 수정
			
			// 5.2. 랭킹 UI띄우기
			//	// 5.2.1. 랭킹 조회(DB)
			//reqActor->bgsb = this;
			//reqActor->GetRequestLeaderboard(mapID, 0, 6, "playTime.playMilliSecond", "asc");		
			////DebuggingLearderBoards();
			////reqActor->leaderBoards = tempLeaderBoards;
			//if (!leaderBoards.IsEmpty()/*!tempLeaderBoards.IsEmpty()*/) {
			//	// 5.2.2. 조회된 랭킹 정리
			//	// 
			//	// 5.2.3. UI에 정보 넘기기
			//	/*
			//	for (int32 i = 0; i < tempLeaderBoards.Num(); i++) {
			//		rankingUIRef->resultWidget->SetLeaderBoardPlayerName(i, tempLeaderBoards[i].playerNickname);
			//		rankingUIRef->resultWidget->SetLeaderBoardPlayTime(i, tempLeaderBoards[i].playMilliSecond);
			//	}
			//	*/
			//	for (int8 i = 0; i < leaderBoards.Num(); i++) {
			//		rankingUIRef->resultWidget->SetLeaderBoardPlayerName(i, leaderBoards[i].playerNickname);
			//		rankingUIRef->resultWidget->SetLeaderBoardPlayTime(i, leaderBoards[i].playMilliSecond);
			//		if (leaderBoards[i].playerNickname.Compare(userNickName) == 0)
			//			rankingUIRef->resultWidget->CheckPlayerName(i);
			//	}
			//	rankingUIRef->resultWidget->SetPlayerTime();
			//	// 5.2.4. UI 콜리전 켜기

			//	rankingUIRef->SetActorHiddenInGame(false);
			//	TArray<AActor*> childActors;
			//	rankingUIRef->GetAllChildActors(childActors);
			//	for (AActor* child : childActors) {
			//		child->SetActorEnableCollision(true);
			//		if (AShootableUIActor* rankingActor = Cast<AShootableUIActor>(child)) {
			//			rankingActor->OnOrbActivate();
			//		}
			//	}
			//	rankingUIRef->SetActorEnableCollision(true);
			//}
		}

	}
}

void ABattleGameStateBase::OnGameFailed()
{
	// 4.2. 실패 UI띄우기
	FVector failUILoc;
	failUILoc = player_1->GetActorLocation() + player_1->GetActorForwardVector() * 1500.f;
	failUILoc.Z = 660.f;
	rankingUIRef->SetActorLocation(failUILoc);
	rankingUIRef->SetActorHiddenInGame(false);
	TArray<AActor*> childActors;
	rankingUIRef->GetAllChildActors(childActors);
	for (AActor* child : childActors) {
		child->SetActorEnableCollision(true);
		if (AShootableUIActor* failActor = Cast<AShootableUIActor>(child)) {
			failActor->OnOrbActivate();
		}
	}
	rankingUIRef->SetActorEnableCollision(true);
	rankingUIRef->resultWidget->SwitchCanvas(1);
}

void ABattleGameStateBase::OnGameCleared()
{
	
	if (!leaderBoards.IsEmpty()) {
		// 5.2.2. 조회된 랭킹 정리
		int8 myRecordIndex = -1;
		int8 arrayNum = leaderBoards.Num();
		bool bNickNameExists = false;

		// 5.2.2.1 닉네임 검사
		for (int8 i = 0; i < arrayNum; i++) {
			// 닉네임이 있을 때
			if (leaderBoards[i].playerNickname.Compare(userNickName) == 0) {
				// 기록된 시간이 현재 기록보다 느리면
				if (leaderBoards[i].playElapsedTime >= elapsedTimeMiliSec) {
					leaderBoards[i].playElapsedTime = elapsedTimeMiliSec;
					myRecordIndex = i;
				}
				bNickNameExists = true;
				break;
			}
		}

		// 닉네임이 없을 때
		if (!bNickNameExists) {
			// 시간 검사
			for (int8 j = 0; j < arrayNum; j++) {
				// 기록된 시간이 현재 기록보다 느리면 기록 추가
				if (leaderBoards[j].playElapsedTime > elapsedTimeMiliSec) {
					FLeaderboardInfo myInfo;
					myInfo.playerNickname = userNickName;
					myInfo.playElapsedTime = elapsedTimeMiliSec;
					leaderBoards.Insert(myInfo, j);
					myRecordIndex = j;
					break;
				}
			}
			if (myRecordIndex == -1)
				myRecordIndex = 11;
		}

		// 5.2.3. UI에 정보 넘기기
		for (int8 i = 0; i < 10; i++) {
			if (!leaderBoards.IsValidIndex(i)) break;
			rankingUIRef->resultWidget->SetLeaderBoardPlayerName(i, leaderBoards[i].playerNickname);
			rankingUIRef->resultWidget->SetLeaderBoardPlayTime(i, leaderBoards[i].playElapsedTime);
			if (i == myRecordIndex)
				rankingUIRef->resultWidget->CheckPlayerName(i);
		}
		rankingUIRef->resultWidget->SetPlayerTime();

		// 5.2.4. UI 콜리전 켜기
		rankingUIRef->SetActorHiddenInGame(false);
		TArray<AActor*> childActors;
		rankingUIRef->GetAllChildActors(childActors);
		for (AActor* child : childActors) {
			child->SetActorEnableCollision(true);
			if (AShootableUIActor* rankingActor = Cast<AShootableUIActor>(child)) {
				rankingActor->OnOrbActivate();
			}
		}
		rankingUIRef->SetActorEnableCollision(true);
	}
}

void ABattleGameStateBase::GetbossRef()
{
	if(gm->bossRef != nullptr) {
		bossRef = gm->bossRef;
		bossFsmRef = bossRef->BossFSM;
	}
}

void ABattleGameStateBase::SetData()
{
	//killRangedEnemy = killTotalEnemy - killMeleeEnemy;
	//recoveryEnergy = recoveryEnergybyMelee + recoveryEnergybyRanged;
	//accuracyHitByGun = ((static_cast<float>(accuracyHit) + static_cast<float>(spawnerHitCount) - static_cast<float>(startTriggerHP)) / static_cast<float>(shootCount)) * 100.0f;
	//if (bClearYN) {
	//	// 보스 포함
	//	killTotalEnemy++;
	//}

	playResultInfo.SetPlayResult(mapID, playStartTime, playClearTime, elapsedTimeMiliSec, bClearYN/*, killTotalEnemy, killRangedEnemy, killMeleeEnemy, choiceReward, firstWaveTakenTime, secondWaveTakenTime, headShot, shootCount, feverMode, recoveryEnergy, accuracyHit, playerHitCount, summonCount*/);

	//UE_LOG(LogTemp, Error, TEXT("BGSB : killTotalEnemy: %d\nkillRangedEnemy: %d\nkillMeleeEnemy: %d\nchoiceReward: %s\nfirstWaveTakenTime: %d\nsecondWaveTakenTime: %d\nheadShot: %d\nshootCount: %d\nfeverMode: %d\nrecoveryEnergy: %.1f\naccuracyHit: %d\nplayerHitCount: %d\nsummonCount: %d\nelapsedTimeMiliSec: %d"), /*killTotalEnemy, killRangedEnemy, killMeleeEnemy, *choiceReward, firstWaveTakenTime, secondWaveTakenTime, headShot, shootCount, feverMode, recoveryEnergy, accuracyHit, playerHitCount, summonCount, */elapsedTimeMiliSec);
	UE_LOG(LogTemp, Error, TEXT("BGSB : elapsedTimeMiliSec: %d"), elapsedTimeMiliSec);

	reqActor->PostRequestNewRank(playResultInfo);
}

void ABattleGameStateBase::DebuggingLearderBoards()
{
	tempLeaderBoards.Empty();
	tempLeaderBoards.SetNum(6);
	/*tempLeaderBoards[0].SetLeaderboardInfo(1, "Player1", "Map1", "2023-11-11 10:00:00", "2023-11-11 10:10:00", 200000, 3, 1);
	tempLeaderBoards[1].SetLeaderboardInfo(2, "Player2", "Map1", "2023-11-11 10:10:00", "2023-11-11 10:20:00", 300000, 3, 2);
	tempLeaderBoards[2].SetLeaderboardInfo(3, "Player3", "Map1", "2023-11-11 10:30:00", "2023-11-11 10:40:00", 400000, 3, 1);
	tempLeaderBoards[3].SetLeaderboardInfo(4, "Player4", "Map1", "2023-11-11 10:20:00", "2023-11-11 10:30:00", 500000, 3, 4);
	tempLeaderBoards[4].SetLeaderboardInfo(5, "Player5", "Map1", "2023-11-11 10:40:00", "2023-11-11 10:50:00", 600000, 3, 5);
	tempLeaderBoards[5].SetLeaderboardInfo(6, "Player6", "Map1", "2023-11-11 10:50:00", "2023-11-11 10:59:00", 700000, 3, 11);*/
}

void ABattleGameStateBase::DebuggingReward()
{
	rewardUIRef->SetActorHiddenInGame(false);
	TArray<AActor*> childActors;
	rewardUIRef->GetAllChildActors(childActors);
	for (AActor* child : childActors) {
		child->SetActorEnableCollision(true);
		if (AShootableUIActor* reward = Cast<AShootableUIActor>(child)) {
			reward->OnOrbActivate();
		}
	}
}