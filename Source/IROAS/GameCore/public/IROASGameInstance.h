
#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "IROASStructs.h"
#include "IROASGameInstance.generated.h"

struct FMapInfoTable;

UCLASS()
class IROAS_API UIROASGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
	public:
	
	UIROASGameInstance(const FObjectInitializer& ObjectInitializer);

	virtual void Init();

	UFUNCTION(Exec)
	void Join(const FString& Address);


public:
	TSharedPtr<const FUniqueNetId> UserId;

	FString userNickName = "VSQUAD_DVN";
	FString MyID = "";

	bool bIsSteamId = true;
	bool bIsFirstTime = true;

	FString token = "";

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FMapInfoTable customMapInfo;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TArray<FMapInfoTable> mapList;
};
