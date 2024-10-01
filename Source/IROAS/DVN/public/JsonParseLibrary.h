
#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Engine/DataTable.h"
#include "IROASEnums.h"
#include "IROASStructs.h"
#include "JsonParseLibrary.generated.h"


UCLASS()
class IROAS_API UJsonParseLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/*************** PARSE JSON ***************/
	static FMapInfoTable JsonParseMapInfo(const FString& originData);

	static void JsonParseMapInfoList(const FString& originData, TArray<FMapInfoTable>* mapList);

	static void JsonParseSpawnerInfo(const TSharedPtr<FJsonObject>& originJsonObject, FMapInfoTable& mapInfo);

	static void JsonParsePropInfo(const TSharedPtr<FJsonObject>& originJsonObject, FMapInfoTable& mapInfo);

	static void JsonParseLoginResult(const FString& originData, FString* token, FString* message, FString* nickname);

	static void JsonParseLeaderBoard(const FString& originData, TArray<FLeaderboardInfo>* leaderBoards);

	static void JsonParsePlayerInfo(const FString& originData, FString* nickname);

	/*************** MAKE JSON ***************/
	static FString MakeJsonMapInfo_String(const FMapInfoTable& mapInfo);

	static TSharedPtr<FJsonObject> MakeJsonSpawnerInfo_Object(const FSpawnerInfoTable& spawnerInfo);

	static TSharedPtr<FJsonObject> MakeJsonPropInfo_Object(const FPropInfoTable& propInfo);

	static FString MakeJsonPlayResult(const FPlayResultInfo& playInfo);

	static FString MakeJsonSteamId(const FString& steamId, bool isSteam);

	// Enum 변환용함수들
	static ETimeZone StringToTimeZoneEnum(const FString& tzoneEnum);

	static EEnemyType StringToEnemyTypeEnum(const FString& etypeEnum);

	static FString GetEnemyEnumValueDisplayName(EEnemyType EnumValue);

	static FString GetTimzoneEnumValueDisplayName(ETimeZone EnumValue);
};
