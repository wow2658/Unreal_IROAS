
#pragma once

#include "CoreMinimal.h"
#include "EditableActor.h"
#include "IROASEnums.h"
#include "IROASStructs.h"
#include "EditableProp.generated.h"

UCLASS()
class IROAS_API AEditableProp : public AEditableActor
{
	GENERATED_BODY()
	
public:
	AEditableProp();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = MySettings)
	FString propID = "";
	// StaticMesh 이름으로 propInfo->propID 설정 (ui_props1, 등등..)
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FPropInfoTable propInfo;

	UPROPERTY(VisibleAnywhere, Category="MySettings|Components")
	class UStaticMeshComponent* rotateMesh;

	virtual void SetPlayerPtr() override;
	void ShowRotateUI(bool canShow);
};
