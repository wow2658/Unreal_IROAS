

#include "EditGameStateBase.h"
#include "JsonParseLibrary.h"
#include <Engine/Engine.h>
#include "HttpRequestActor.h"
#include "MovableFrame.h"
#include <Kismet/GameplayStatics.h>
#include "EditableActor.h"
#include "EditableProp.h"
#include "EditableSpawner.h"
#include "IROASGameInstance.h"


AEditGameStateBase::AEditGameStateBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AEditGameStateBase::BeginPlay()
{
	Super::BeginPlay();

	// Delegate �� callback �Լ���
	AActor::OnDestroyed.AddDynamic(this, &AEditGameStateBase::OnDestroyed);

	actorSpawnedDelegate = FOnActorSpawned::FDelegate::CreateUObject(this, &AEditGameStateBase::HandleActorSpawned);
	actorDestroyedDelegate = FOnActorDestroyed::FDelegate::CreateUObject(this, &AEditGameStateBase::HandleActorDestroyed);

	UWorld* world = GetWorld();
	if (world != nullptr) {
		world->AddOnActorSpawnedHandler(actorSpawnedDelegate);
		world->AddOnActorDestroyedHandler(actorDestroyedDelegate);

	}

	// GameInstance �������� & �����̸�, ���̸� ����.
	UIROASGameInstance* tempInstance = Cast<UIROASGameInstance>(GetGameInstance());
	if(tempInstance != nullptr) {
		gi = tempInstance;
		mapInfo.mapCreator = gi->userNickName;
		mapInfo.mapName = TEXT("VSquadMap");
		//MapID�� ���߿� ������ �ø��� �������� �ڵ�������
		mapInfo.mapID = TEXT("testMapID_1");
	}
	
	fullPath = contentPath + FString::Printf(TEXT("CreatedMaps/%s_%s.json"), *mapInfo.mapCreator, *mapInfo.mapName);

	// MovableFrame ĳ���صα�
	AEditableActor* tempStart = Cast<AEditableActor>(UGameplayStatics::GetActorOfClass(GetWorld(), bp_playerStart));
	if (tempStart != nullptr) {
		playerStart = tempStart;
	}
	// HttpRequestActor ����
	reqActor = GetWorld()->SpawnActor<AHttpRequestActor>();
	
}

void AEditGameStateBase::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	//if (!tested && reqActor != nullptr) {
	//	tested = true;
	//	FString fullURL = baseURL + mapURL + "?size=10&offset=1&sort=createTime&direction=asc";
	//	mapInfo.mapCreator = gi->userNickName;
	//	//reqActor->PostRequestMapData(fullURL, mapInfo);
	//	reqActor->SendRequestForMapData(fullURL);
	//}
	
}

void AEditGameStateBase::HandleActorSpawned(AActor* actor)
{
	// 1. ��ġ�� �������� Ȯ��
	AEditableActor* tempActor = Cast<AEditableActor>(actor);
	// 2. ��ġ�� ���Ͷ��
	if (tempActor != nullptr) {
		// 2.1. �����̶�� �����迭�� �߰�
		if (tempActor->isProp) {
			AEditableProp* tempProp = Cast<AEditableProp>(tempActor);
			if(tempProp != nullptr)OnPropInfoUpdate(tempProp);
		}
		// 2.2. �����ʶ�� �����ʹ迭�� �߰�
		else {
			AEditableSpawner* tempSpawner = Cast<AEditableSpawner>(tempActor);
			if(tempSpawner != nullptr)OnSpawnerInfoUpdate(tempSpawner);
		}
		//SaveMapData();
	}

}

void AEditGameStateBase::HandleActorDestroyed(AActor* actor)
{
	
	// 1. ��ġ�� �������� Ȯ��
	AEditableActor* tempActor = Cast<AEditableActor>(actor);
	if(tempActor == nullptr) return;

	int32 idx;
	
	// 2. �����̶��
	if (tempActor->isProp) {
		AEditableProp* tempProp = Cast<AEditableProp>(tempActor);
		if (tempProp != nullptr) {
			// 2.1. �迭���� ã��	
			idx = propActors.Find(tempProp);
			// 2.2. �迭���� ����
			if(propActors.IsValidIndex(idx) && mapInfo.propList.IsValidIndex(idx)) {
				mapInfo.propList.RemoveAt(idx);
				propActors.RemoveAt(idx);
			}
		}
	}
	// 3. �����ʷ� ĳ���� ���� ��
	else {
		AEditableSpawner* tempSpawner = Cast<AEditableSpawner>(tempActor);
		if (tempSpawner != nullptr) {
			// 3.1. �迭���� ã��
			idx = spawnerActors.Find(tempSpawner);
			// 3.2. �迭���� ����
			if(spawnerActors.IsValidIndex(idx) && mapInfo.spawnerList.IsValidIndex(idx)) {
				mapInfo.spawnerList.RemoveAt(idx);
				spawnerActors.RemoveAt(idx);
			}
		}
	}
	//UploadMapData();
	//SaveMapData();
	
}

void AEditGameStateBase::OnPlayerStartUpdate()
{
	if(playerStart == nullptr) return;
	FVector playerStartLocation = playerStart->GetActorLocation();
	float playerRotation = playerStart->GetActorRotation().Yaw;
	mapInfo.SetPlayerStart(playerStartLocation.X, playerStartLocation.Y, playerStartLocation.Z, playerRotation);
}

void AEditGameStateBase::OnTimezoneUpdate()
{	// ��������: ���ڰ����� ������ EditorPlayer�� timezone�ٲ��ְ�����.
	mapInfo.timeZone = timezone;
}

void AEditGameStateBase::OnSpawnerInfoUpdate(AEditableSpawner* updatedSpawner)
{
	int32 idx = spawnerActors.Find(updatedSpawner);
	if (idx < 0) {
		idx = spawnerActors.Add(updatedSpawner);
		FVector spanwerLoc = updatedSpawner->GetActorLocation();
		spawnerActors[idx]->spawnerInfo.x = spanwerLoc.X;
		spawnerActors[idx]->spawnerInfo.y = spanwerLoc.Y;
		spawnerActors[idx]->spawnerInfo.z = spanwerLoc.Z;
		spawnerActors[idx]->spawnerInfo.enemyType = updatedSpawner->enemyType;
		mapInfo.AddSpawner(updatedSpawner->spawnerInfo);
	}
	else {
		if (mapInfo.spawnerList.IsValidIndex(idx)) {
			FVector spanwerLoc = updatedSpawner->GetActorLocation();
			spawnerActors[idx]->spawnerInfo.x = spanwerLoc.X;
			spawnerActors[idx]->spawnerInfo.y = spanwerLoc.Y;
			spawnerActors[idx]->spawnerInfo.z = spanwerLoc.Z;
			mapInfo.spawnerList[idx] = spawnerActors[idx]->spawnerInfo;
			spawnerActors[idx]->spawnerInfo.enemyType = updatedSpawner->enemyType;
		}
		else OrganizeSpawnerInfo();
	}
}

void AEditGameStateBase::OnPropInfoUpdate(AEditableProp* updatedProp)
{
	int32 idx = propActors.Find(updatedProp);
	if (idx < 0) {
		idx = propActors.Add(updatedProp);
		// ���� ��ġ propinfo�� ����
		FVector propLoc = updatedProp->GetActorLocation();
		propActors[idx]->propInfo.x = propLoc.X;
		propActors[idx]->propInfo.y = propLoc.Y;
		propActors[idx]->propInfo.z = propLoc.Z;
		propActors[idx]->propInfo.yaw = updatedProp->GetActorRotation().Yaw;
		propActors[idx]->propInfo.propID = updatedProp->propID;
		mapInfo.AddProp(updatedProp->propInfo);
	}
	else {
		if (mapInfo.propList.IsValidIndex(idx)) {
			FVector propLoc = updatedProp->GetActorLocation();
			propActors[idx]->propInfo.x = propLoc.X;
			propActors[idx]->propInfo.y = propLoc.Y;
			propActors[idx]->propInfo.z = propLoc.Z;
			propActors[idx]->propInfo.yaw = updatedProp->GetActorRotation().Yaw;
			propActors[idx]->propInfo.propID = updatedProp->propID;
			mapInfo.propList[idx] = propActors[idx]->propInfo;
		}
		else OrganizePropInfo();
	}
}

void AEditGameStateBase::OrganizeSpawnerInfo()
{
	if (spawnerActors.Num() != mapInfo.spawnerList.Num()) {
		mapInfo.spawnerList.Empty(spawnerActors.Num());
		for (int32 i = 0; i < spawnerActors.Num(); i++) {
			mapInfo.AddSpawner(spawnerActors[i]->spawnerInfo);
		}
	}
	else {
		for (int32 i = 0; i < spawnerActors.Num(); i++) {
			if (mapInfo.spawnerList.IsValidIndex(i)) {
				mapInfo.spawnerList[i] = spawnerActors[i]->spawnerInfo;
			}
		}
	}
}

void AEditGameStateBase::OrganizePropInfo()
{
	if (propActors.Num() != mapInfo.propList.Num()) {
		mapInfo.propList.Empty(propActors.Num());
		for (int32 i = 0; i < propActors.Num(); i++) {
			mapInfo.AddProp(propActors[i]->propInfo);
		}
	}
	else {
		for (int32 i = 0; i < propActors.Num(); i++) {
			if (mapInfo.propList.IsValidIndex(i)) {
				mapInfo.propList[i] = propActors[i]->propInfo;
			}
		}
	}
}

void AEditGameStateBase::SaveMapData()
{
	// �̸�, ������, �ʽĺ��� - �ʱ� �ѹ� �����ϸ� ��.
	// playerStart
	OnPlayerStartUpdate();
	// timeZone
	OnTimezoneUpdate();
	// spawnerList
	OrganizeSpawnerInfo();
	// propList
	OrganizePropInfo();

	// ������Ʈ �������� ����
	FFileHelper::SaveStringToFile(UJsonParseLibrary::MakeJsonMapInfo_String(mapInfo), *fullPath);
}

void AEditGameStateBase::UploadMapData()
{
	if (reqActor != nullptr) {

		SaveMapData();

		// ��������
		FString fullUrl = baseURL + mapURL;
		reqActor->PostRequestMapData(fullUrl, mapInfo);
	}

}

void AEditGameStateBase::OnDestroyed(AActor* DestroyedActor)
{

}
