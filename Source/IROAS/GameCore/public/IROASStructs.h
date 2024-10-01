

#pragma once


#include "CoreMinimal.h"
#include "IROASEnums.h"
#include "Engine/DataTable.h"
#include "IROASStructs.generated.h"

USTRUCT(BlueprintType)
struct IROAS_API FSpawnerInfoTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	float x = 0.f;
	UPROPERTY(EditAnywhere)
	float y = 0.f;
	UPROPERTY(EditAnywhere)
	float z = 0.f;
	UPROPERTY(EditAnywhere)
	EEnemyType enemyType = EEnemyType::None;
	UPROPERTY(EditAnywhere)
	float enemyHP = 0.f;
	UPROPERTY(EditAnywhere)
	float enemyPower = 0.f;
	UPROPERTY(EditAnywhere)
	int32 spawnAmount = 0;
	UPROPERTY(EditAnywhere)
	float spawnStartDelay = 0.f;
	UPROPERTY(EditAnywhere)
	float spawnInterval = 0.f;

	/*FSpawnerInfoTable() {
		x = 0.f;
		y = 0.f;
		z = 0.f;
		enemyType = EEnemyType::None;
		enemyHP = 0.f;
		enemyPower = 0.f;
		spawnAmount = 0;
		spawnStartDelay = 0.f;
		spawnInterval = 0.f;
	};*/

	FORCEINLINE void SetAllFields(float _x, float _y, float _z, EEnemyType _enemyType, float _enemyHP, float _enemyPower, int32 _spawnAmount, float _spawnStartDelay, float _spawnInterval){ 
		x = _x;
		y = _y;
		z = _z;
		enemyType = _enemyType;
		enemyHP = _enemyHP;
		enemyPower = _enemyPower;
		spawnAmount = _spawnAmount;
		spawnStartDelay = _spawnStartDelay;
		spawnInterval = _spawnInterval;
		};
		
};

USTRUCT(BlueprintType)
struct IROAS_API FPropInfoTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	float x = 0.f;
	UPROPERTY(EditAnywhere)
	float y = 0.f;
	UPROPERTY(EditAnywhere)
	float z = 0.f;
	UPROPERTY(EditAnywhere)
	float yaw = 0.f;
	UPROPERTY(EditAnywhere)
	FString propID = "";

	/*FPropInfoTable() {
		x = 0.f;
		y = 0.f;
		z = 0.f;
		yaw = 0.f;
		propID = "";
	};*/

	FORCEINLINE void SetAllFields(float _x, float _y, float _z, float _yaw, const FString& _propID){ 
		x = _x;
		y = _y;
		z = _z;
		yaw = _yaw;
		propID = _propID;
		};
		
};

USTRUCT(BlueprintType)
struct IROAS_API FMapInfoTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	FString mapName = "";
	UPROPERTY(EditAnywhere)
	FString mapCreator = "";
	UPROPERTY(EditAnywhere)
	FString mapID = "";
	UPROPERTY(EditAnywhere)
	float playerStart_x = 0.f;
	UPROPERTY(EditAnywhere)
	float playerStart_y = 0.f;
	UPROPERTY(EditAnywhere)
	float playerStart_z = 0.f;
	UPROPERTY(EditAnywhere)
	float playerStart_yaw = 0.f;
	UPROPERTY(EditAnywhere)
	ETimeZone timeZone = ETimeZone::Morning;
	UPROPERTY(EditAnywhere)
	TArray<FSpawnerInfoTable> spawnerList = {};
	UPROPERTY(EditAnywhere)
	TArray<FPropInfoTable> propList = {};

	FORCEINLINE void SetPlayerStart(float _x, float _y, float _z, float _yaw){ 
		playerStart_x = _x;
		playerStart_y = _y;
		playerStart_z = _z;
		playerStart_yaw = _yaw;
		};
	FORCEINLINE void SetMapInfo(const FString& name, const FString& creator, const FString& iD, ETimeZone _timeZone) {
		mapName = name;
		mapCreator = creator;
		mapID = iD;
		timeZone = _timeZone;
	};
	FORCEINLINE void AddSpawner(const FSpawnerInfoTable& spawnerInfo) {
		spawnerList.Push(spawnerInfo);
	};
	FORCEINLINE void AddProp(const FPropInfoTable& propInfo) {
		propList.Push(propInfo);
	};
};

USTRUCT(BlueprintType)
struct IROAS_API FPlayDataTable : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	TArray<FString> playerNames = {"", ""};
	UPROPERTY(EditAnywhere)
	float playTime_m = 0.f;
	UPROPERTY(EditAnywhere)
	int32 playCount = 0;
	UPROPERTY(EditAnywhere)
	int32 clearCount = 0;

	FORCEINLINE void UpdatePlayData(const FString& firstPlayer, const FString& secondPlayer, float playTime, int32 _playCount, int32 _clearCount){
		playerNames[0] = firstPlayer;
		playerNames[1] = secondPlayer;
		playTime_m = playTime;
		playCount = _playCount;
		clearCount = _clearCount;
	};
};

USTRUCT(BlueprintType)
struct IROAS_API FPlayResultInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 creatorMapId;
	UPROPERTY(EditAnywhere)
	FString playStartTime = "";
	UPROPERTY(EditAnywhere)
	FString playClearTime = "";
	UPROPERTY(EditAnywhere)
	int64 elapsedTime;
	UPROPERTY(EditAnywhere)
	bool clearYn = false;
	//UPROPERTY(EditAnywhere)
	//int32 killTotalEnemy;
	//UPROPERTY(EditAnywhere)
	//int32 killRangedEnemy;
	//UPROPERTY(EditAnywhere)
	//int32 killMeleeEnemy;
	//UPROPERTY(EditAnywhere)
	//FString choiceReward = "";
	//UPROPERTY(EditAnywhere)
	//int64 firstWaveTakenTime;
	//UPROPERTY(EditAnywhere)
	//int64 secondWaveTakenTime;
	//UPROPERTY(EditAnywhere)
	//int32 headShot;
	//UPROPERTY(EditAnywhere)
	//int32 shootCount;
	//UPROPERTY(EditAnywhere)
	//int32 feverMode;
	//UPROPERTY(EditAnywhere)
	//float recoveryEnergy;
	//UPROPERTY(EditAnywhere)
	//int32 accuracyHit;
	//UPROPERTY(EditAnywhere)
	//int32 reloadCount;
	//UPROPERTY(EditAnywhere)
	//int32 playerHitCount;
	//UPROPERTY(EditAnywhere)
	//int32 summonCount;


	FORCEINLINE void SetPlayResult(const int32 _creatorMapId, const FString& _playStartTime, const FString& _playClearTime, int64 _elapsedTime, bool _clearYn/*, const int32 _killTotalEnemy, const int32 _killRangedEnemy, const int32 _killMeleeEnemy, const FString& _choiceReward, const int64 _firstWaveTakenTime, const int64 _secondWaveTakenTime, const int32 _headShot, const int32 _shootCount, const int32 _feverMode, const float _recoveryEnergy, const int32 _accuracyHit, const int32 _playerHitCount, const int32 _summonCount*/) {
		creatorMapId = _creatorMapId;
		playStartTime = _playStartTime;
		playClearTime = _playClearTime;
		elapsedTime = _elapsedTime;
		clearYn = _clearYn;
		/*killTotalEnemy = _killTotalEnemy;
		killRangedEnemy = _killRangedEnemy;
		killMeleeEnemy = _killMeleeEnemy;
		choiceReward = _choiceReward;
		firstWaveTakenTime = _firstWaveTakenTime;
		secondWaveTakenTime = _secondWaveTakenTime;
		headShot = _headShot;
		shootCount = _shootCount;
		feverMode = _feverMode;
		recoveryEnergy = _recoveryEnergy;
		accuracyHit = _accuracyHit;
		playerHitCount = _playerHitCount;
		summonCount = _summonCount;*/
	};

};

USTRUCT(BlueprintType)
struct IROAS_API FLeaderboardInfo : public FTableRowBase
{
	GENERATED_USTRUCT_BODY()

public:
	UPROPERTY(EditAnywhere)
	int32 rankingId = 0;
	UPROPERTY(EditAnywhere)
	FString playerNickname = "";
	UPROPERTY(EditAnywhere)
	FString creatorMapId = "";
	UPROPERTY(EditAnywhere)
	FString playStartTime = "";
	UPROPERTY(EditAnywhere)
	FString playClearTime = "";
	UPROPERTY(EditAnywhere)
	int32 playMilliSecond = 0;
	UPROPERTY(EditAnywhere)
	int64 playElapsedTime = 0;
	UPROPERTY(EditAnywhere)
	int32 playCount = 0;
	UPROPERTY(EditAnywhere)
	int32 clearCount = 0;
	/*UPROPERTY(EditAnywhere)
	int32 killTotalEnemy = 0;
	UPROPERTY(EditAnywhere)
	int32 killRangedEnemy = 0;
	UPROPERTY(EditAnywhere)
	int32 killMeleeEnemy = 0;
	UPROPERTY(EditAnywhere)
	FString accuracyHitByGun = "";*/

	FORCEINLINE void SetLeaderboardInfo(int32 _rankingId, const FString& _playerNickname, const FString& _creatorMapId, const FString& _playStartTime, const FString& _playClearTime, int32 _playMilliSecond, int64 _playElapsedTime, int32 _playCount, int32 _clearCount/*, int32 _killTotalEnemy, int32 _killRangedEnemy, int32 _killMeleeEnemy, const FString& _accuracyHitByGun*/) {
		rankingId = _rankingId;
		playerNickname = _playerNickname;
		creatorMapId = _creatorMapId;
		playStartTime = _playStartTime;
		playClearTime = _playClearTime;
		playMilliSecond = _playMilliSecond;
		playElapsedTime = _playElapsedTime;
		playCount = _playCount;
		clearCount = _clearCount;
		/*killTotalEnemy = _killTotalEnemy;
		killRangedEnemy = _killRangedEnemy;
		killMeleeEnemy = _killMeleeEnemy;
		accuracyHitByGun = _accuracyHitByGun;*/
	};
};

