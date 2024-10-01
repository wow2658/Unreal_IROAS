
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/IHttpRequest.h"
#include "Interfaces/IHttpResponse.h"
#include "IROASStructs.h"
#include "IROASEnums.h"
#include "HttpRequestActor.generated.h"

class AEditorGameModeBase;
class AEditGameStateBase;
class ALobbyGameMode;
class ABattleGameStateBase;
class ABattleGameModeBase;
UCLASS()
class IROAS_API AHttpRequestActor : public AActor
{
	GENERATED_BODY()
	
public:	
	AHttpRequestActor();

protected:
	virtual void BeginPlay() override;

public:	
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	EGameModeType gameModeType = EGameModeType::None;
	UPROPERTY(VisibleAnywhere)
	ABattleGameModeBase* bgm = nullptr;
	UPROPERTY(VisibleAnywhere)
	ABattleGameStateBase* bgsb = nullptr;
	UPROPERTY(VisibleAnywhere)
	AEditorGameModeBase* egm = nullptr;
	UPROPERTY(VisibleAnywhere)
	ALobbyGameMode* lgm = nullptr;
	UPROPERTY(VisibleAnywhere)
	AEditGameStateBase* egsb;
	UPROPERTY(VisibleAnywhere)
	int32 testMapID = 2;

	// 전달받은 맵데이터
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FMapInfoTable mapData;

	// 랭킹조회 결과
	TArray<FLeaderboardInfo> leaderBoards;

	// API
	//FString baseURL = "http://192.168.0.20:5000";
	//FString baseURL = "http://52.79.89.164";
	//FString baseURL = "http://iroas-api-server-env.eba-aqqrn2rv.ap-northeast-2.elasticbeanstalk.com";
	FString baseURL = "http://3.36.210.198";
	FString loginURL_POST = "/api/v1/player/login";
	FString newPlayerURL_POST = "/api/v1/player";
	FString newRankURL_POST = "/api/v1/ranking";
	FString playerInfoURL_GET = "/api/v1/player/info";
	FString leaderboardURL_GET = "/api/v1/ranking";

public:	// 맵관련
	void SendRequestForMapData(const FString getMapUrl);

	void PostRequestMapData(const FString mapPostUrl, const FMapInfoTable& mapInfo);

private:

	void OnReceivedMapData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void OnPostedMapData(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

public:	// 플레이어정보관련
	void PostRequestLogin(const FString steamID, bool isSteam);

	void PostRequestNewPlayer(const FString& steamID, bool isSteam);

	void GetRequestPlayerInfo();

private:
	void OnReceivedLoginResult(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void OnReceivedNewPlayerResult(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void OnReceivedPlayerInfo(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

public:	// 랭킹관련
	void PostRequestNewRank(const FPlayResultInfo playResult);

	void GetRequestLeaderboard(const int32 creatorMapId, int32 pageNum, int32 pageSize, const FString& sort, const FString& direction);

private:
	void OnReceivedAddingNewRank(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);

	void OnReceivedLeaderBoard(FHttpRequestPtr Request, FHttpResponsePtr Response, bool bConnectedSuccessfully);
};
