

#include "JsonParseLibrary.h"
#include "Json.h"

/*************** PARSE JSON ***************/
/**********************************
��    Ī: JsonParseMapInfo
��    ��: Json ���� �������� �Ľ��Ѵ�.
��    ��: JSon���� FString
��    ��: FMapInfoTable
**********************************/
FMapInfoTable UJsonParseLibrary::JsonParseMapInfo(const FString& originData)
{

	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(originData);

	TSharedPtr<FJsonObject> resultJsonObj = MakeShareable(new FJsonObject);

	FMapInfoTable mapInfo;
	if (FJsonSerializer::Deserialize(reader, resultJsonObj)) {
		TSharedPtr<FJsonObject> dtoObject;
		if (dtoObject->HasField("creatorMapDto")) {
			dtoObject = resultJsonObj->GetObjectField("creatorMapDto");
		}
		else {
			dtoObject = resultJsonObj;
		}
		FString mName, mCreator, mID, mType;
		FVector playerStartLoc;
		float playerRotation;
		int32 creator;
		ETimeZone tZone;
		FString tZone_string;

		dtoObject->TryGetStringField("creatorMapId", mID);
		dtoObject->TryGetStringField("creatorMapName", mName);
		dtoObject->TryGetStringField("creatorMapType", mType);
		dtoObject->TryGetNumberField("creator", creator);
		mCreator = FString::Printf(TEXT(" #%d"), creator);
		//�� �ٿ��� �ð��ޱ�
		dtoObject->TryGetNumberField("playerStartPointXLocation", playerStartLoc.X);
		dtoObject->TryGetNumberField("playerStartPointYLocation", playerStartLoc.Y);
		dtoObject->TryGetNumberField("playerStartPointZLocation", playerStartLoc.Z);
		dtoObject->TryGetNumberField("playerStartPointYaw", playerRotation);
		dtoObject->TryGetStringField("timeZone", tZone_string);
		tZone = StringToTimeZoneEnum(tZone_string);

		mapInfo.SetPlayerStart(playerStartLoc.X, playerStartLoc.Y, playerStartLoc.Z, playerRotation);
		mapInfo.SetMapInfo(mName, mCreator, mID, tZone);

		// Parse "spawnerList" array
		if (dtoObject->HasField(TEXT("spawnerList")))
		{
			JsonParseSpawnerInfo(dtoObject, mapInfo);
		}

		// Parse "propList" array in a similar way
		if (dtoObject->HasField(TEXT("propList")))
		{
			JsonParsePropInfo(dtoObject, mapInfo);
		}

	}

	return mapInfo;
}

void UJsonParseLibrary::JsonParseMapInfoList(const FString& originData, TArray<FMapInfoTable>* mapList)
{
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(originData);

	TSharedPtr<FJsonObject> resultJsonObj = MakeShareable(new FJsonObject);
	
	if (FJsonSerializer::Deserialize(reader, resultJsonObj)) {
		TSharedPtr<FJsonObject> Body = resultJsonObj->GetObjectField("body");
		TArray<TSharedPtr<FJsonValue>> Content = Body->GetArrayField("content");

		if(!mapList->IsEmpty()) mapList->Empty();
		for (auto& Item : Content)
		{
			TSharedPtr<FJsonObject> ContentObject = Item->AsObject();

			FMapInfoTable mapInfo;
			FString mName, mCreator, mID, mType;
			FVector playerStartLoc;
			float playerRotation;
			int32 creator;
			ETimeZone tZone;
			FString tZone_string;

			ContentObject->TryGetStringField("creatorMapId", mID);
			ContentObject->TryGetStringField("creatorMapName", mName);
			ContentObject->TryGetStringField("creatorMapType", mType);
			ContentObject->TryGetNumberField("creator", creator);
			mCreator = FString::Printf(TEXT(" #%d"), creator);
			//�� �ٿ��� �ð��ޱ�
			ContentObject->TryGetNumberField("playerStartPointXLocation", playerStartLoc.X);
			ContentObject->TryGetNumberField("playerStartPointYLocation", playerStartLoc.Y);
			ContentObject->TryGetNumberField("playerStartPointZLocation", playerStartLoc.Z);
			ContentObject->TryGetNumberField("playerStartPointYaw", playerRotation);
			ContentObject->TryGetStringField("timeZone", tZone_string);
			tZone = StringToTimeZoneEnum(tZone_string);

			mapInfo.SetPlayerStart(playerStartLoc.X, playerStartLoc.Y, playerStartLoc.Z, playerRotation);
			mapInfo.SetMapInfo(mName, mCreator, mID, tZone);

			// Parse "spawnerList" array
			if (ContentObject->HasField(TEXT("spawnerList")))
			{
				JsonParseSpawnerInfo(ContentObject, mapInfo);
			}

			// Parse "propList" array in a similar way
			if (ContentObject->HasField(TEXT("propList")))
			{
				JsonParsePropInfo(ContentObject, mapInfo);
			}

			mapList->Add(mapInfo);

		}
	}
}

/**********************************
��    Ī: JsonParseSpawnerInfo
��    ��: Json ���� ������ ������ �Ľ��Ѵ�.
��    ��: JSon���� FString
��    ��: ����(mapInfo �ȿ� �ٷ� ���)
**********************************/
void UJsonParseLibrary::JsonParseSpawnerInfo(const TSharedPtr<FJsonObject>& originJsonObject, FMapInfoTable& mapInfo)
{
	const TArray<TSharedPtr<FJsonValue>> SpawnerList = originJsonObject->GetArrayField(TEXT("enemySpawnerList"));
	// SpawnerList ���� ���鼭 spawnerinfoTable�� ����, �迭�� �߰�.
	for (const TSharedPtr<FJsonValue>& SpawnerValue : SpawnerList)
	{
		if (SpawnerValue->Type == EJson::Object)
		{

			TSharedPtr<FJsonObject> SpawnerObject = SpawnerValue->AsObject();
			float X = SpawnerObject->GetNumberField(TEXT("enemyStartPointXLocation"));
			float Y = SpawnerObject->GetNumberField(TEXT("enemyStartPointYLocation"));
			float Z = SpawnerObject->GetNumberField(TEXT("enemyStartPointZLocation"));
			int32 SpawnAmount = SpawnerObject->GetNumberField(TEXT("enemySpawnAmount"));
			float SpawnStartDelay = SpawnerObject->GetNumberField(TEXT("enemySpawnerStartDelay"));
			float SpawnInterval = SpawnerObject->GetNumberField(TEXT("enemySpawnerInterval"));
			FString EnemyType = SpawnerObject->GetStringField(TEXT("enemyType"));
			EEnemyType _EnemyType = StringToEnemyTypeEnum(EnemyType);
			float EnemyHP = SpawnerObject->GetNumberField(TEXT("enemyHP"));
			float EnemyPower = SpawnerObject->GetNumberField(TEXT("enemyPower"));

			// spawnerinfotable�� ����
			FSpawnerInfoTable spawner;
			spawner.SetAllFields(X, Y, Z, _EnemyType, EnemyHP, EnemyPower, SpawnAmount, SpawnStartDelay, SpawnInterval);

			// spawnerInfotable �迭�� �߰�
			mapInfo.AddSpawner(spawner);

		}
	}
}

/**********************************
��    Ī: JsonParsePropInfo
��    ��: Json ���� ���� ������ �Ľ��Ѵ�.
��    ��: JSon���� FString
��    ��: ����(mapInfo �ȿ� �ٷ� ���)
**********************************/
void UJsonParseLibrary::JsonParsePropInfo(const TSharedPtr<FJsonObject>& originJsonObject, FMapInfoTable& mapInfo)
{
	const TArray<TSharedPtr<FJsonValue>> PropList = originJsonObject->GetArrayField(TEXT("propList"));
	for (const TSharedPtr<FJsonValue>& PropValue : PropList)
	{
		if (PropValue->Type == EJson::Object)
		{
			TSharedPtr<FJsonObject> PropObject = PropValue->AsObject();
			FString PropID = PropObject->GetStringField(TEXT("propId"));
			FString PropName = PropObject->GetStringField(TEXT("propName"));
			float X = PropObject->GetNumberField(TEXT("propLocationX"));
			float Y = PropObject->GetNumberField(TEXT("propLocationY"));
			float Z = PropObject->GetNumberField(TEXT("propLocationZ"));
			float Yaw = PropObject->GetNumberField(TEXT("propYawValue"));

			FPropInfoTable prop;
			prop.SetAllFields(X, Y, Z, Yaw, PropID);
			mapInfo.AddProp(prop);
			//props.Add(prop);
		}
	}
}

/**********************************
��    Ī: JsonParseLoginResult
��    ��: Json ���� �α��� Response �Ľ�
��    ��: JSon���� FString
��    ��: ����
**********************************/
void UJsonParseLibrary::JsonParseLoginResult(const FString& originData, FString* token, FString* message, FString* nickname)
{
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(originData);

	TSharedPtr<FJsonObject> resultJsonObj = MakeShareable(new FJsonObject);

	if (FJsonSerializer::Deserialize(reader, resultJsonObj)) {
		
		resultJsonObj->TryGetStringField("token", *token);
		resultJsonObj->TryGetStringField("nickname", *nickname);
		resultJsonObj->TryGetStringField("message", *message);

	}
}

/**********************************
��    Ī: JsonParseLeaderBoard
��    ��: Json ���� ��ŷ���� �Ľ�
��    ��: JSon���� FString
��    ��: ����
**********************************/
void UJsonParseLibrary::JsonParseLeaderBoard(const FString& originData, TArray<FLeaderboardInfo>* leaderBoards)
{
	// JSON ���ڿ��� ��Ʈ������ ��ȯ
	TArray<TCHAR> jsonStringArray;
	jsonStringArray.Append(originData);
	jsonStringArray.Add('\0');
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(jsonStringArray.GetData());

	// JSON ��ü�� �Ľ�
	TSharedPtr<FJsonObject> jsonObject;
	if (FJsonSerializer::Deserialize(reader, jsonObject) && jsonObject.IsValid())
	{
		// "body" ��ü ��������
		const TSharedPtr<FJsonObject>* bodyObject;
		if (jsonObject->TryGetObjectField(TEXT("body"), bodyObject))
		{
			// "content" �迭 ��������
			const TArray<TSharedPtr<FJsonValue>>* contentArray;
			if ((*bodyObject)->TryGetArrayField(TEXT("content"), contentArray))
			{
				// �� "content" �׸��� ��ȸ
				for (const TSharedPtr<FJsonValue>& value : *contentArray)
				{
					FLeaderboardInfo singleInfo;
					TSharedPtr<FJsonObject> contentObject = value->AsObject();

					// �� �ʵ带 ������
					singleInfo.rankingId = contentObject->GetIntegerField(TEXT("rankingId"));
					singleInfo.playerNickname = contentObject->GetStringField(TEXT("playerNickname"));
					singleInfo.creatorMapId = contentObject->GetStringField(TEXT("creatorMapId"));
					singleInfo.playCount = contentObject->GetIntegerField(TEXT("playCount"));
					singleInfo.clearCount = contentObject->GetIntegerField(TEXT("clearCount"));

					// "playTime" ��ü�� ������
					const TSharedPtr<FJsonObject>* playTimeObject;
					if (contentObject->TryGetObjectField(TEXT("playTime"), playTimeObject))
					{
						singleInfo.playStartTime = (*playTimeObject)->GetStringField(TEXT("playStartTime"));
						singleInfo.playClearTime = (*playTimeObject)->GetStringField(TEXT("playClearTime"));
						// "playMilliSecond" �ʵ�� null�� �� �����Ƿ� Ȯ���� �ʿ�
						singleInfo.playMilliSecond = (*playTimeObject)->HasField(TEXT("playMilliSecond")) ? (*playTimeObject)->GetIntegerField(TEXT("playMilliSecond")) : -1;
						singleInfo.playElapsedTime = (*playTimeObject)->HasField(TEXT("playElapsedTime")) ? (*playTimeObject)->GetIntegerField(TEXT("playElapsedTime")) : -1;
					}
					leaderBoards->Push(singleInfo);
				}
			}
		}
	}
	
}

/**********************************
��    Ī: JsonParsePlayerInfo
��    ��: Json ���� �������� �Ľ�
��    ��: JSon���� FString
��    ��: ����
**********************************/
void UJsonParseLibrary::JsonParsePlayerInfo(const FString& originData, FString* nickname)
{
	TSharedRef<TJsonReader<TCHAR>> reader = TJsonReaderFactory<TCHAR>::Create(originData);

	TSharedPtr<FJsonObject> resultJsonObj = MakeShareable(new FJsonObject);

	if (FJsonSerializer::Deserialize(reader, resultJsonObj)) {
		int32 playerID;
		FString playerSteamKey;
		FString playerNickName;
		FString playerType;
		FString playerRole;

		resultJsonObj->TryGetNumberField("playerID", playerID);
		resultJsonObj->TryGetStringField("playerSteamKey", playerSteamKey);
		resultJsonObj->TryGetStringField("playerNickName", playerNickName);
		resultJsonObj->TryGetStringField("playerType", playerType);
		resultJsonObj->TryGetStringField("playerRole", playerRole);

		*nickname = playerNickName;

	}
}

/*************** MAKE JSON ***************/
/**********************************
��    Ī: MakeJsonMapInfo_String
��    ��: �� ������ Json���� �����.
��    ��: FMapInfoTable
��    ��: JSon���� FString
**********************************/
FString UJsonParseLibrary::MakeJsonMapInfo_String(const FMapInfoTable& mapInfo)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	// Ÿ�ӽ�����
	FDateTime CurrentTime = FDateTime::UtcNow();
	FString TimeString = CurrentTime.ToString(TEXT("%Y-%m-%d %H:%M:%S"));
	
	
	int32 minInt = 111111;
	int32 maxInt = 999999;
	int32 creatorNum = FMath::RandRange(minInt, maxInt);
	int32 mapModeSelection = FMath::RandRange(1, 2);
	JsonObject->SetStringField(TEXT("creatorMapId"), mapInfo.mapCreator);
	JsonObject->SetStringField(TEXT("creatorMapName"), FString::Printf(TEXT("%s_%d"), *mapInfo.mapCreator, creatorNum));
	JsonObject->SetStringField(TEXT("creatorMapType"), mapModeSelection == 1 ? FString("TIME_ATTACK") : FString("MELEE"));
	JsonObject->SetNumberField(TEXT("creator"), creatorNum);
	JsonObject->SetStringField(TEXT("createTime"), TimeString);
	JsonObject->SetNumberField(TEXT("playerStartPointXLocation"), mapInfo.playerStart_x);
	JsonObject->SetNumberField(TEXT("playerStartPointYLocation"), mapInfo.playerStart_y);
	JsonObject->SetNumberField(TEXT("playerStartPointZLocation"), mapInfo.playerStart_z);
	JsonObject->SetNumberField(TEXT("playerStartPointYaw"), mapInfo.playerStart_yaw);
	JsonObject->SetStringField(TEXT("timezone"), GetTimzoneEnumValueDisplayName(mapInfo.timeZone));
	
	// Convert the spawnerList array to a JSON array
	TArray<TSharedPtr<FJsonValue>> SpawnerArray;
	for (const FSpawnerInfoTable& Spawner : mapInfo.spawnerList)
	{
		TSharedPtr<FJsonObject> SpawnerJson = MakeJsonSpawnerInfo_Object(Spawner);
		TSharedPtr<FJsonValueObject> SpawnerJsonValue = MakeShareable(new FJsonValueObject(SpawnerJson));
		SpawnerArray.Add(SpawnerJsonValue);
	}

	JsonObject->SetArrayField(TEXT("enemySpawnerList"), SpawnerArray);
		
	// Convert the propList array to a JSON array
	TArray<TSharedPtr<FJsonValue>> PropArray;
	for (const FPropInfoTable& Prop : mapInfo.propList)
	{
		TSharedPtr<FJsonValueObject> PropJson = MakeShareable(new FJsonValueObject(MakeJsonPropInfo_Object(Prop)));
		PropArray.Add(PropJson);
	}

	JsonObject->SetArrayField(TEXT("propList"), PropArray);

	FString OutJsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&OutJsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	return OutJsonString;
}

/**********************************
��    Ī: MakeJsonSpawnerInfo_Object
��    ��: ������ ������ Json���� �����.
��    ��: FSpawnerInfoTable
��    ��: JSon Object
**********************************/
TSharedPtr<FJsonObject> UJsonParseLibrary::MakeJsonSpawnerInfo_Object(const FSpawnerInfoTable& spawnerInfo)
{	
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetNumberField(TEXT("enemyStartPointXLocation"), spawnerInfo.x);
	JsonObject->SetNumberField(TEXT("enemyStartPointYLocation"), spawnerInfo.y);
	JsonObject->SetNumberField(TEXT("enemyStartPointZLocation"), spawnerInfo.z);
	JsonObject->SetNumberField(TEXT("enemySpawnAmount"), spawnerInfo.spawnAmount);
	JsonObject->SetNumberField(TEXT("enemySpawnerStartDelay"), spawnerInfo.spawnStartDelay);
	JsonObject->SetNumberField(TEXT("enemySpawnerInterval"), spawnerInfo.spawnInterval);
	JsonObject->SetStringField(TEXT("enemyType"), GetEnemyEnumValueDisplayName(spawnerInfo.enemyType));
	JsonObject->SetNumberField(TEXT("enemyHp"), spawnerInfo.enemyHP);
	JsonObject->SetNumberField(TEXT("enemyPower"), spawnerInfo.enemyPower);
	
	return JsonObject;
}

/**********************************
��    Ī: MakeJsonSpawnerInfo_Object
��    ��: ���� ������ Json���� �����.
��    ��: FPropInfoTable
��    ��: JSon Object
**********************************/
TSharedPtr<FJsonObject> UJsonParseLibrary::MakeJsonPropInfo_Object(const FPropInfoTable& propInfo)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetStringField(TEXT("propId"), propInfo.propID);
	JsonObject->SetStringField(TEXT("propName"), propInfo.propID);
	JsonObject->SetNumberField(TEXT("propLocationX"), propInfo.x);
	JsonObject->SetNumberField(TEXT("propLocationY"), propInfo.y);
	JsonObject->SetNumberField(TEXT("propLocationZ"), propInfo.z);
	JsonObject->SetNumberField(TEXT("propYawValue"), propInfo.yaw);

	return JsonObject;
}

/**********************************
��    Ī: MakeJsonPlayResult
��    ��: ���� ����� Json���� �����.
��    ��: FPlayResultInfo
��    ��: FString
**********************************/
FString UJsonParseLibrary::MakeJsonPlayResult(const FPlayResultInfo& playInfo)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);

	JsonObject->SetNumberField(TEXT("creatorMapId"), playInfo.creatorMapId);
	JsonObject->SetStringField(TEXT("playStartTime"), playInfo.playStartTime);
	JsonObject->SetStringField(TEXT("playClearTime"), playInfo.playClearTime);
	JsonObject->SetBoolField(TEXT("clearYn"), playInfo.clearYn);
	JsonObject->SetNumberField(TEXT("elapsedTime"), playInfo.elapsedTime);

	FString OutJsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&OutJsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	return OutJsonString;
}

/**********************************
��    Ī: MakeJsonSteamId
��    ��: �������̵�(Ȥ�� ����ũž���̵�)�� Json���� �����.
��    ��: steamID, type
��    ��: Json���� FString
**********************************/
FString UJsonParseLibrary::MakeJsonSteamId(const FString& steamId, bool isSteam)
{
	TSharedPtr<FJsonObject> JsonObject = MakeShareable(new FJsonObject);
	FString type = isSteam ? "steam" : "local";
	JsonObject->SetStringField(TEXT("key"), steamId);
	JsonObject->SetStringField(TEXT("type"), type);

	FString OutJsonString;
	TSharedRef<TJsonWriter<TCHAR>> JsonWriter = TJsonWriterFactory<TCHAR>::Create(&OutJsonString);
	FJsonSerializer::Serialize(JsonObject.ToSharedRef(), JsonWriter);

	return OutJsonString;
}

/****************************************************************/
ETimeZone UJsonParseLibrary::StringToTimeZoneEnum(const FString& tzoneEnum)
{
	if(tzoneEnum == "Morning") return ETimeZone::Morning;
	else if(tzoneEnum == "Noon") return ETimeZone::Noon;
	else if(tzoneEnum == "Evening") return ETimeZone::Evening;
	else return ETimeZone::None;
}

EEnemyType UJsonParseLibrary::StringToEnemyTypeEnum(const FString& etypeEnum)
{
	if(etypeEnum == "Melee") return EEnemyType::Melee;
	else if(etypeEnum == "Ranged_Ground") return EEnemyType::Ranged_Ground;
	else if(etypeEnum == "Ranged_Air") return EEnemyType::Ranged_Air;
	else if(etypeEnum == "Elite") return EEnemyType::Elite;
	else return EEnemyType::None;
}

FString UJsonParseLibrary::GetEnemyEnumValueDisplayName(EEnemyType EnumValue)
{

	UEnum* EnumPtr = FindObject<UEnum>(StaticClass(), TEXT("EEnemyType"), true);
	if (EnumPtr)
	{
		FString DisplayName = EnumPtr->GetDisplayNameTextByValue((int64)EnumValue).ToString();
		return DisplayName;
	}

	// Handle errors if the enum is not found.
	return FString("Unknown");
}

FString UJsonParseLibrary::GetTimzoneEnumValueDisplayName(ETimeZone EnumValue)
{
	UEnum* EnumPtr = FindObject<UEnum>(StaticClass(), TEXT("ETimeZone"), true);
	if (EnumPtr)
	{
		FString DisplayName = EnumPtr->GetDisplayNameTextByValue((int64)EnumValue).ToString();
		return DisplayName;
	}

	// Handle errors if the enum is not found.
	return FString("Unknown");
}

