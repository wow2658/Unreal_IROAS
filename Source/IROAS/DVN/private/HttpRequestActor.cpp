

#include "HttpRequestActor.h"
#include "HttpModule.h"
#include "EditorGameModeBase.h"
#include "EditGameStateBase.h"
#include "JsonParseLibrary.h"
#include "LobbyGameMode.h"
#include "BattleGameStateBase.h"
#include "BattleGameModeBase.h"


AHttpRequestActor::AHttpRequestActor()
{
	PrimaryActorTick.bCanEverTick = true;

}

void AHttpRequestActor::BeginPlay()
{
	Super::BeginPlay();
	
	switch (gameModeType)
	{
	case EGameModeType::None:
		break;
	case EGameModeType::StoryMode:
		bgm = GetWorld()->GetAuthGameMode<ABattleGameModeBase>();
		if (bgm != nullptr) {
			bgsb = bgm->GetGameState<ABattleGameStateBase>();
		}
		break;
	case EGameModeType::EditorMode:
		egm = GetWorld()->GetAuthGameMode<AEditorGameModeBase>();
		if (egm != nullptr) {
			egsb = egm->GetGameState<AEditGameStateBase>();
			if (egsb != nullptr) {
				//gsb->reqActor = this;
			}
		}
		break;
	case EGameModeType::LobbyMode:
		lgm = GetWorld()->GetAuthGameMode<ALobbyGameMode>();
		break;
	case EGameModeType::CustomMode:
		break;
	default:
		break;
	}

}

void AHttpRequestActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

/*************************
	  맵정보 요청
*************************/
void AHttpRequestActor::SendRequestForMapData(const FString getMapUrl)
{
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedPtr<IHttpRequest> req = httpModule.CreateRequest();
	
	// 서버에 메소드를 요청하기 위한 정보 설정
	req->SetURL(getMapUrl);
	req->SetVerb(TEXT("GET"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// 요청한 메소드가 완료될 때 호출할 함수 바인딩
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedMapData);

	// 메소드 실행 요청
	req->ProcessRequest();
}

void AHttpRequestActor::OnReceivedMapData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	// 정상적인 결과를 받아왔다면,
	if (bConnectedSuccessfully) {
		UE_LOG(LogTemp, Warning, TEXT("Connected!"));
		// Json형태의 정보를 파싱하고
		FString mapDataString = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *mapDataString);
		
		if (lgm != nullptr) {
			TArray<FMapInfoTable> mapList;
			UJsonParseLibrary::JsonParseMapInfoList(mapDataString, &lgm->mapList);
		}
		//mapData = UJsonParseLibrary::JsonParseMapInfo(mapDataString);
		// GameState에 있는 맵정보에 전달한다.
		//if(gsb != nullptr) gsb->mapInfo = mapData;
	}
	else {
		int32 responseConde = Response->GetResponseCode();
		UE_LOG(LogTemp, Warning, TEXT("Failed! Response Code: %d"), responseConde);

	}
}

/*************************
	  맵정보 전송
*************************/
void AHttpRequestActor::PostRequestMapData(const FString mapPostUrl, const FMapInfoTable& mapInfo)
{
	// 맵정보 Json형태만들기
	FString postData = UJsonParseLibrary::MakeJsonMapInfo_String(mapInfo);

	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();
	req->SetURL(mapPostUrl);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetContentAsString(postData);
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnPostedMapData);
	req->ProcessRequest();
}

void AHttpRequestActor::OnPostedMapData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully) {
		UE_LOG(LogTemp, Warning, TEXT("ResultRecieved!\n"));
		FString mapPostResult = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("%s\n"), *mapPostResult);
	}
	else {
		// 응답 코드 확인(400오류인지 404인지 등등..)
		int32 responseConde = Response->GetResponseCode();
		UE_LOG(LogTemp, Warning, TEXT("Failed! Response Code: %d"), responseConde);
		
		// 요청 전송상태 확인
		EHttpRequestStatus::Type status = Request->GetStatus();
		switch (status)
		{
		case EHttpRequestStatus::NotStarted:
			break;
		case EHttpRequestStatus::Processing:
			break;
		case EHttpRequestStatus::Failed:
			break;
		case EHttpRequestStatus::Failed_ConnectionError:
			break;
		case EHttpRequestStatus::Succeeded:
			break;
		default:
			break;
		}
	}
}

/*************************
		로그인
*************************/
void AHttpRequestActor::PostRequestLogin(const FString steamID, bool isSteam)
{
	FString postData = UJsonParseLibrary::MakeJsonSteamId(steamID, isSteam);
	
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();
	req->SetURL(baseURL + loginURL_POST);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetContentAsString(postData);
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedLoginResult);
	req->ProcessRequest();
}

void AHttpRequestActor::OnReceivedLoginResult(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully) {
		// 디버깅
		//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("LOGIN ResultRecieved!\n")));
		UE_LOG(LogTemp, Warning, TEXT("LOGIN ResultRecieved!"));
		
		FString loginResult = Response->GetContentAsString();

		if (lgm != nullptr) {
			// Response Body 내용 파싱하기
			FString message, token, nickname;
			UJsonParseLibrary::JsonParseLoginResult(loginResult, &token, &message, &nickname);
			// 토큰이 비어있으면
			if (token.IsEmpty()/*message.Equals(TEXT("저장된 플레이어가 없습니다."))*/) {
				//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("No Data, Proceed SignUp")));
				UE_LOG(LogTemp, Warning, TEXT("No Data, Proceed SignUp"));
				// 신규 회원가입 요청
				lgm->isFirstTime = true;
				PostRequestNewPlayer(lgm->myID, lgm->isSteamId);
			}
			// 토큰이 받아졌으면
			else {
				// 게임모드에 토큰 전달
				lgm->token = token;
				// 게임모드에 닉네임 전달, 게임인스턴스에 올리기
				lgm->userNickname = nickname;
				lgm->DeliverNicknameToGI();

				lgm->SpawnTutorialActor();

				//lgm->SpawnStartOrb();
				
				//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("token: %s\n%s"), *lgm->token, *message));
				UE_LOG(LogTemp, Warning, TEXT("LOGIN Success! token: %s\n%s"), *lgm->token, *message);
				//GetRequestPlayerInfo();
			}
		}
	}
	else {
		// 응답 코드 확인(400오류인지 404인지 등등..)
		int32 responseCode = Response->GetResponseCode();
		//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Login Failed! Response Code: %d"), responseCode));
		UE_LOG(LogTemp, Warning, TEXT("Login Failed! Response Code: %d"), responseCode);
		if (lgm != nullptr) {
			//lgm->token.Empty();
			//lgm->resultMessage.Empty();
			if (responseCode == 400) {
				//PostRequestNewPlayer(lgm->MyName);
			}
		}

		// 요청 전송상태 확인
		/*
		EHttpRequestStatus::Type status = Request->GetStatus();
		switch (status)
		{
		case EHttpRequestStatus::NotStarted:
			break;
		case EHttpRequestStatus::Processing:
			break;
		case EHttpRequestStatus::Failed:
			break;
		case EHttpRequestStatus::Failed_ConnectionError:
			break;
		case EHttpRequestStatus::Succeeded:
			break;
		default:
			break;
		}
		*/
	}
}

/*************************
	신규 플레이어 추가
*************************/
void AHttpRequestActor::PostRequestNewPlayer(const FString& steamID, bool isSteam)
{
	FString postData = UJsonParseLibrary::MakeJsonSteamId(steamID, isSteam);

	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();
	req->SetURL(baseURL + newPlayerURL_POST);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetContentAsString(postData);
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedNewPlayerResult);
	req->ProcessRequest();
}

void AHttpRequestActor::OnReceivedNewPlayerResult(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully) {

		FString playerAddResult = Response->GetContentAsString();

		if (lgm != nullptr) {
			// Response Body 내용 파싱하기
			FString message, token, nickname;
			UJsonParseLibrary::JsonParseLoginResult(playerAddResult, &token, &message, &nickname);
			// 토큰이 비어있으면
			if (token.IsEmpty()) {
				GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("No Data, player Not added\t%s"), *message));
				UE_LOG(LogTemp, Warning, TEXT("No Data, player Not added"));
				
			}
			// 토큰이 받아졌으면
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Player added!\n")));
				UE_LOG(LogTemp, Warning, TEXT("Player added!\n"));
				// 게임모드에 토큰 전달
				lgm->token = token;
				UE_LOG(LogTemp, Warning, TEXT("Token received!: %s\n%s"), *lgm->token, *message);
				// 게임모드에 닉네임 전달, 게임인스턴스에 올리기
				/*lgm->userNickname = nickname;
				lgm->DeliverNicknameToGI();
				lgm->SpawnTutorialActor();*/
				GetRequestPlayerInfo();
				GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("token: %s\n%s"), *lgm->token, *message));
			}
		}

	}
	else {
		// 플레이어 추가를 실패했는데 토큰이 있는경우, 초기화
		if (lgm != nullptr && !lgm->token.IsEmpty()) {
			lgm->token.Empty();
		}
		// 응답 코드 확인(400오류인지 404인지 등등..)
		int32 responseConde = Response->GetResponseCode();
		//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("PlayerAdd Failed! Response Code: %d"), responseConde));
		UE_LOG(LogTemp, Warning, TEXT("PlayerAdd Failed! Response Code: %d"), responseConde);

		// 요청 전송상태 확인
		/*
		EHttpRequestStatus::Type status = Request->GetStatus();
		switch (status)
		{
		case EHttpRequestStatus::NotStarted:
			break;
		case EHttpRequestStatus::Processing:
			break;
		case EHttpRequestStatus::Failed:
			break;
		case EHttpRequestStatus::Failed_ConnectionError:
			break;
		case EHttpRequestStatus::Succeeded:
			break;
		default:
			break;
		}
		*/
	}
}

/*************************
	플레이어 정보 조회
*************************/
void AHttpRequestActor::GetRequestPlayerInfo()
{
	FString fullURL = baseURL + playerInfoURL_GET;

	FHttpModule& httpModule = FHttpModule::Get();
	TSharedPtr<IHttpRequest> req = httpModule.CreateRequest();

	FString bearerToken = "Bearer " + lgm->token;

	// 서버에 메소드를 요청하기 위한 정보 설정
	req->SetURL(fullURL);
	req->SetVerb(TEXT("GET"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetHeader(TEXT("Authorization"), bearerToken);

	// 요청한 메소드가 완료될 때 호출할 함수 바인딩
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedPlayerInfo);

	// 메소드 실행 요청
	req->ProcessRequest();
}

void AHttpRequestActor::OnReceivedPlayerInfo(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully) {

		FString playerAddResult = Response->GetContentAsString();

		if (lgm != nullptr) {
			UJsonParseLibrary::JsonParsePlayerInfo(playerAddResult, &lgm->userNickname);
			GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Nickname(LobbyGameMode): %s"), *lgm->userNickname));
			UE_LOG(LogTemp, Warning, TEXT("Nickname: %s"), *lgm->userNickname);
			lgm->DeliverNicknameToGI();
			lgm->SpawnTutorialActor();
		}

	}
	else {
		// 응답 코드 확인(400오류인지 404인지 등등..)
		int32 responseConde = Response->GetResponseCode();

		UE_LOG(LogTemp, Warning, TEXT("PlayerAdd Failed! Response Code: %d"), responseConde);

		// 요청 전송상태 확인
		/*
		EHttpRequestStatus::Type status = Request->GetStatus();
		switch (status)
		{
		case EHttpRequestStatus::NotStarted:
			break;
		case EHttpRequestStatus::Processing:
			break;
		case EHttpRequestStatus::Failed:
			break;
		case EHttpRequestStatus::Failed_ConnectionError:
			break;
		case EHttpRequestStatus::Succeeded:
			break;
		default:
			break;
		}
		*/
	}
}

/*************************
	신규 랭킹 추가
*************************/
void AHttpRequestActor::PostRequestNewRank(const FPlayResultInfo playResult)
{
	// 플레이완료정보 MakeJson
	/*FPlayResultInfo playResult;
	playResult.creatorMapId = testMapID;
	playResult.playStartTime = playStartTime;
	playResult.playClearTime = playEndTime;
	playResult.clearYn = bCleared;*/
	FString postData = UJsonParseLibrary::MakeJsonPlayResult(playResult);

	FString bearerToken = "Bearer " + bgsb->token;

	FHttpModule& httpModule = FHttpModule::Get();
	TSharedRef<IHttpRequest> req = httpModule.CreateRequest();
	req->SetURL(baseURL + newRankURL_POST);
	req->SetVerb(TEXT("POST"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetHeader(TEXT("Authorization"), bearerToken);
	req->SetContentAsString(postData);
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedAddingNewRank);
	req->ProcessRequest();

	//UE_LOG(LogTemp, Error, TEXT("requestActor :\nkillTotalEnemy: %d\nkillRangedEnemy: %d\nkillMeleeEnemy: %d\nchoiceReward: %s\nfirstWaveTakenTime: %d\nsecondWaveTakenTime: %d\nheadShot: %d\nshootCount: %d\nfeverMode: %d\nrecoveryEnergy: %.1f\naccuracyHit: %d\nplayerHitCount: %d\nsummonCount: %d\nelapsedTime: %d"), playResult.killTotalEnemy, playResult.killRangedEnemy, playResult.killMeleeEnemy, *playResult.choiceReward, playResult.firstWaveTakenTime, playResult.secondWaveTakenTime, playResult.headShot, playResult.shootCount, playResult.feverMode, playResult.recoveryEnergy, playResult.accuracyHit, playResult.playerHitCount, playResult.summonCount, playResult.elapsedTime);
	UE_LOG(LogTemp, Error, TEXT("requestActor : elapsedTime: %d"), playResult.elapsedTime);

}

void AHttpRequestActor::OnReceivedAddingNewRank(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully) {
		FString addedResult = Response->GetContentAsString();

		GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Ranking added!")));
		UE_LOG(LogTemp, Warning, TEXT("Add ranking requested!"));

		GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("%s"), *addedResult));
		UE_LOG(LogTemp, Warning, TEXT("%s"), *addedResult);

		FTimerHandle rankingHandle;
		GetWorldTimerManager().SetTimer(rankingHandle, FTimerDelegate::CreateLambda([&]() {
			if (bgsb->bClearYN)
				GetRequestLeaderboard(bgsb->mapID, 0, 10, "playTime.playElapsedTime", "asc");
			else
				bgsb->OnGameFailed();
			}), 1.f, false);

	}
	else {
		// 응답 코드 확인(400오류인지 404인지 등등..)
		int32 responseConde = Response->GetResponseCode();
		//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("RankingAdd Failed! Response Code: %d"), responseConde));
		UE_LOG(LogTemp, Warning, TEXT("RankingAdd Failed! Response Code: %d"), responseConde);

		// 요청 전송상태 확인
		/*
		EHttpRequestStatus::Type status = Request->GetStatus();
		switch (status)
		{
		case EHttpRequestStatus::NotStarted:
			break;
		case EHttpRequestStatus::Processing:
			break;
		case EHttpRequestStatus::Failed:
			break;
		case EHttpRequestStatus::Failed_ConnectionError:
			break;
		case EHttpRequestStatus::Succeeded:
			break;
		default:
			break;
		}
		*/
	}
}

/*************************
		랭킹 조회
*************************/
void AHttpRequestActor::GetRequestLeaderboard(const int32 creatorMapId, int32 pageNum, int32 pageSize, const FString& sort, const FString& direction)
{
	FString fullURL = FString::Printf(TEXT("%s%s?creatorMapId=%d&page=%d&size=%d&sort=%s&direction%s"), *baseURL, *leaderboardURL_GET, creatorMapId, pageNum, pageSize, *sort, *direction);
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedPtr<IHttpRequest> req = httpModule.CreateRequest();

	FString bearerToken = "Bearer " + bgsb->token;

	// 서버에 메소드를 요청하기 위한 정보 설정
	req->SetURL(fullURL);
	req->SetVerb(TEXT("GET"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetHeader(TEXT("Authorization"), bearerToken);

	// 요청한 메소드가 완료될 때 호출할 함수 바인딩
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedLeaderBoard);

	// 메소드 실행 요청
	req->ProcessRequest();
}

void AHttpRequestActor::OnReceivedLeaderBoard(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	if (bConnectedSuccessfully) {
		//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Leaderboard read Connected!\n")));
		UE_LOG(LogTemp, Warning, TEXT("Leaderboard read Connected!"));

		FString leaderboard = Response->GetContentAsString();
		if (bgsb != nullptr) {

			UJsonParseLibrary::JsonParseLeaderBoard(leaderboard, &leaderBoards);

			if (!leaderBoards.IsEmpty()) {

				GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("LeaderBoard read Success!")));
				UE_LOG(LogTemp, Warning, TEXT("LeaderBoard read Success!"));
				for (auto score : leaderBoards) {
					GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("RankingID: %d\nPlayerNickname: %s\nCreatorMapID: %s\nPlayStartTime: %s\nPlayClearTime: %s\nPlayMinutes: %d\nPlayCount: %d\nClearCount: %d"), score.rankingId, *score.playerNickname, *score.creatorMapId, *score.playStartTime, *score.playClearTime, score.playMilliSecond, score.playCount, score.clearCount));
					UE_LOG(LogTemp, Warning, TEXT("RankingID: %d\nPlayerNickname: %s\nCreatorMapID: %s\nPlayStartTime: %s\nPlayClearTime: %s\nPlayMinutes: %d\nPlayCount: %d\nClearCount: %d"), score.rankingId, *score.playerNickname, *score.creatorMapId, *score.playStartTime, *score.playClearTime, score.playMilliSecond, score.playCount, score.clearCount);
				}
				// 배틀게임모드 받아서 랭킹정보 넘겨주기
				bgsb->leaderBoards = leaderBoards;
				bgsb->OnGameCleared();
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("LeaderBoard read Failed!"));
		}

	}
	else {

		// 응답 코드 확인(400오류인지 404인지 등등..)
		int32 responseConde = Response->GetResponseCode();
		//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Reading Leaderboard Failed! Response Code: %d"), responseConde));
		UE_LOG(LogTemp, Warning, TEXT("Reading Leaderboard Failed!Response Code : % d"), responseConde);

		// 요청 전송상태 확인
		/*
		EHttpRequestStatus::Type status = Request->GetStatus();
		switch (status)
		{
		case EHttpRequestStatus::NotStarted:
			break;
		case EHttpRequestStatus::Processing:
			break;
		case EHttpRequestStatus::Failed:
			break;
		case EHttpRequestStatus::Failed_ConnectionError:
			break;
		case EHttpRequestStatus::Succeeded:
			break;
		default:
			break;
		}
		*/
	}
}
