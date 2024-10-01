

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
	  ������ ��û
*************************/
void AHttpRequestActor::SendRequestForMapData(const FString getMapUrl)
{
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedPtr<IHttpRequest> req = httpModule.CreateRequest();
	
	// ������ �޼ҵ带 ��û�ϱ� ���� ���� ����
	req->SetURL(getMapUrl);
	req->SetVerb(TEXT("GET"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));

	// ��û�� �޼ҵ尡 �Ϸ�� �� ȣ���� �Լ� ���ε�
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedMapData);

	// �޼ҵ� ���� ��û
	req->ProcessRequest();
}

void AHttpRequestActor::OnReceivedMapData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully)
{
	// �������� ����� �޾ƿԴٸ�,
	if (bConnectedSuccessfully) {
		UE_LOG(LogTemp, Warning, TEXT("Connected!"));
		// Json������ ������ �Ľ��ϰ�
		FString mapDataString = Response->GetContentAsString();
		UE_LOG(LogTemp, Warning, TEXT("%s"), *mapDataString);
		
		if (lgm != nullptr) {
			TArray<FMapInfoTable> mapList;
			UJsonParseLibrary::JsonParseMapInfoList(mapDataString, &lgm->mapList);
		}
		//mapData = UJsonParseLibrary::JsonParseMapInfo(mapDataString);
		// GameState�� �ִ� �������� �����Ѵ�.
		//if(gsb != nullptr) gsb->mapInfo = mapData;
	}
	else {
		int32 responseConde = Response->GetResponseCode();
		UE_LOG(LogTemp, Warning, TEXT("Failed! Response Code: %d"), responseConde);

	}
}

/*************************
	  ������ ����
*************************/
void AHttpRequestActor::PostRequestMapData(const FString mapPostUrl, const FMapInfoTable& mapInfo)
{
	// ������ Json���¸����
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
		// ���� �ڵ� Ȯ��(400�������� 404���� ���..)
		int32 responseConde = Response->GetResponseCode();
		UE_LOG(LogTemp, Warning, TEXT("Failed! Response Code: %d"), responseConde);
		
		// ��û ���ۻ��� Ȯ��
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
		�α���
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
		// �����
		//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("LOGIN ResultRecieved!\n")));
		UE_LOG(LogTemp, Warning, TEXT("LOGIN ResultRecieved!"));
		
		FString loginResult = Response->GetContentAsString();

		if (lgm != nullptr) {
			// Response Body ���� �Ľ��ϱ�
			FString message, token, nickname;
			UJsonParseLibrary::JsonParseLoginResult(loginResult, &token, &message, &nickname);
			// ��ū�� ���������
			if (token.IsEmpty()/*message.Equals(TEXT("����� �÷��̾ �����ϴ�."))*/) {
				//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("No Data, Proceed SignUp")));
				UE_LOG(LogTemp, Warning, TEXT("No Data, Proceed SignUp"));
				// �ű� ȸ������ ��û
				lgm->isFirstTime = true;
				PostRequestNewPlayer(lgm->myID, lgm->isSteamId);
			}
			// ��ū�� �޾�������
			else {
				// ���Ӹ�忡 ��ū ����
				lgm->token = token;
				// ���Ӹ�忡 �г��� ����, �����ν��Ͻ��� �ø���
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
		// ���� �ڵ� Ȯ��(400�������� 404���� ���..)
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

		// ��û ���ۻ��� Ȯ��
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
	�ű� �÷��̾� �߰�
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
			// Response Body ���� �Ľ��ϱ�
			FString message, token, nickname;
			UJsonParseLibrary::JsonParseLoginResult(playerAddResult, &token, &message, &nickname);
			// ��ū�� ���������
			if (token.IsEmpty()) {
				GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("No Data, player Not added\t%s"), *message));
				UE_LOG(LogTemp, Warning, TEXT("No Data, player Not added"));
				
			}
			// ��ū�� �޾�������
			else {
				//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Player added!\n")));
				UE_LOG(LogTemp, Warning, TEXT("Player added!\n"));
				// ���Ӹ�忡 ��ū ����
				lgm->token = token;
				UE_LOG(LogTemp, Warning, TEXT("Token received!: %s\n%s"), *lgm->token, *message);
				// ���Ӹ�忡 �г��� ����, �����ν��Ͻ��� �ø���
				/*lgm->userNickname = nickname;
				lgm->DeliverNicknameToGI();
				lgm->SpawnTutorialActor();*/
				GetRequestPlayerInfo();
				GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("token: %s\n%s"), *lgm->token, *message));
			}
		}

	}
	else {
		// �÷��̾� �߰��� �����ߴµ� ��ū�� �ִ°��, �ʱ�ȭ
		if (lgm != nullptr && !lgm->token.IsEmpty()) {
			lgm->token.Empty();
		}
		// ���� �ڵ� Ȯ��(400�������� 404���� ���..)
		int32 responseConde = Response->GetResponseCode();
		//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("PlayerAdd Failed! Response Code: %d"), responseConde));
		UE_LOG(LogTemp, Warning, TEXT("PlayerAdd Failed! Response Code: %d"), responseConde);

		// ��û ���ۻ��� Ȯ��
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
	�÷��̾� ���� ��ȸ
*************************/
void AHttpRequestActor::GetRequestPlayerInfo()
{
	FString fullURL = baseURL + playerInfoURL_GET;

	FHttpModule& httpModule = FHttpModule::Get();
	TSharedPtr<IHttpRequest> req = httpModule.CreateRequest();

	FString bearerToken = "Bearer " + lgm->token;

	// ������ �޼ҵ带 ��û�ϱ� ���� ���� ����
	req->SetURL(fullURL);
	req->SetVerb(TEXT("GET"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetHeader(TEXT("Authorization"), bearerToken);

	// ��û�� �޼ҵ尡 �Ϸ�� �� ȣ���� �Լ� ���ε�
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedPlayerInfo);

	// �޼ҵ� ���� ��û
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
		// ���� �ڵ� Ȯ��(400�������� 404���� ���..)
		int32 responseConde = Response->GetResponseCode();

		UE_LOG(LogTemp, Warning, TEXT("PlayerAdd Failed! Response Code: %d"), responseConde);

		// ��û ���ۻ��� Ȯ��
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
	�ű� ��ŷ �߰�
*************************/
void AHttpRequestActor::PostRequestNewRank(const FPlayResultInfo playResult)
{
	// �÷��̿Ϸ����� MakeJson
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
		// ���� �ڵ� Ȯ��(400�������� 404���� ���..)
		int32 responseConde = Response->GetResponseCode();
		//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("RankingAdd Failed! Response Code: %d"), responseConde));
		UE_LOG(LogTemp, Warning, TEXT("RankingAdd Failed! Response Code: %d"), responseConde);

		// ��û ���ۻ��� Ȯ��
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
		��ŷ ��ȸ
*************************/
void AHttpRequestActor::GetRequestLeaderboard(const int32 creatorMapId, int32 pageNum, int32 pageSize, const FString& sort, const FString& direction)
{
	FString fullURL = FString::Printf(TEXT("%s%s?creatorMapId=%d&page=%d&size=%d&sort=%s&direction%s"), *baseURL, *leaderboardURL_GET, creatorMapId, pageNum, pageSize, *sort, *direction);
	FHttpModule& httpModule = FHttpModule::Get();
	TSharedPtr<IHttpRequest> req = httpModule.CreateRequest();

	FString bearerToken = "Bearer " + bgsb->token;

	// ������ �޼ҵ带 ��û�ϱ� ���� ���� ����
	req->SetURL(fullURL);
	req->SetVerb(TEXT("GET"));
	req->SetHeader(TEXT("Content-Type"), TEXT("application/json"));
	req->SetHeader(TEXT("Authorization"), bearerToken);

	// ��û�� �޼ҵ尡 �Ϸ�� �� ȣ���� �Լ� ���ε�
	req->OnProcessRequestComplete().BindUObject(this, &AHttpRequestActor::OnReceivedLeaderBoard);

	// �޼ҵ� ���� ��û
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
				// ��Ʋ���Ӹ�� �޾Ƽ� ��ŷ���� �Ѱ��ֱ�
				bgsb->leaderBoards = leaderBoards;
				bgsb->OnGameCleared();
			}
			else
				UE_LOG(LogTemp, Warning, TEXT("LeaderBoard read Failed!"));
		}

	}
	else {

		// ���� �ڵ� Ȯ��(400�������� 404���� ���..)
		int32 responseConde = Response->GetResponseCode();
		//GEngine->AddOnScreenDebugMessage(-1, 90, FColor::Green, FString::Printf(TEXT("Reading Leaderboard Failed! Response Code: %d"), responseConde));
		UE_LOG(LogTemp, Warning, TEXT("Reading Leaderboard Failed!Response Code : % d"), responseConde);

		// ��û ���ۻ��� Ȯ��
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
