

#include "EditableActor.h"
#include "EditorPlayer.h"
#include <Components/SphereComponent.h>
#include <Kismet/KismetMathLibrary.h>
#include <Kismet/GameplayStatics.h>


AEditableActor::AEditableActor()
{
	PrimaryActorTick.bCanEverTick = true;
	limitedRange = CreateDefaultSubobject<USphereComponent>(TEXT("Limited Range"));
	SetRootComponent(limitedRange);
	limitedRange->SetSphereRadius(200);
	limitedRange->bHiddenInGame = true;
	limitedRange->SetCollisionProfileName(FName("EditCheckOverlap"));
	transformComp = CreateDefaultSubobject<USceneComponent>(TEXT("TransformComp"));
	transformComp->SetupAttachment(RootComponent);
	transformComp->SetRelativeRotation(FRotator(0, -90, 0));
	spawnableStaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("StaticMesh"));
	spawnableStaticMesh->SetupAttachment(transformComp);
	spawnableStaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	spawnableSkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	spawnableSkeletalMesh->SetupAttachment(transformComp);
	spawnableSkeletalMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void AEditableActor::BeginPlay()
{
	Super::BeginPlay();
	editor = Cast<AEditorPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));

	
}

void AEditableActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AEditableActor::CheckSpawn()
{
	TArray<AActor*> result;
	GetOverlappingActors(result, AEditableActor::StaticClass());

	float scaleMultiplier = 0.9f;

	if (result.Num() > 0) {
		cantSpawn = true;
		scaleMultiplier = 1.1f;
	}
	else
		cantSpawn = false;

	if (spawnableSkeletalMesh->GetSkeletalMeshAsset() != nullptr) {
		FVector newScale = UKismetMathLibrary::VLerp(spawnableSkeletalMesh->GetRelativeScale3D(), spawnableSkeletalMesh->GetRelativeScale3D() * scaleMultiplier, 0.7f);
		if (spawnableSkeletalMesh->GetName().Contains(FString::Printf(TEXT("boss")))) {
			newScale.X = FMath::Clamp(newScale.X, 2.0f, 3.0f);
			newScale.Y = FMath::Clamp(newScale.Y, 2.0f, 3.0f);
			newScale.Z = FMath::Clamp(newScale.Z, 2.0f, 3.0f);
		}
		else {
			newScale.X = FMath::Clamp(newScale.X, 1.0f, 2.0f);
			newScale.Y = FMath::Clamp(newScale.Y, 1.0f, 2.0f);
			newScale.Z = FMath::Clamp(newScale.Z, 1.0f, 2.0f);
		}
		spawnableSkeletalMesh->SetRelativeScale3D(newScale);
	}
	else if (spawnableStaticMesh->GetStaticMesh() != nullptr) {
		FVector newScale = UKismetMathLibrary::VLerp(spawnableStaticMesh->GetRelativeScale3D(), spawnableStaticMesh->GetRelativeScale3D() * scaleMultiplier, 0.7f);
		newScale.X = FMath::Clamp(newScale.X, 1.0f, 1.3f);
		newScale.Y = FMath::Clamp(newScale.Y, 1.0f, 1.3f);
		newScale.Z = FMath::Clamp(newScale.Z, 1.0f, 1.3f);
		spawnableStaticMesh->SetRelativeScale3D(newScale);
	}

	ShowRedBoundary(cantSpawn);
}

void AEditableActor::ShowRedBoundary(bool spawn)
{
	/*if (!select) {*/
	/*stencilValue = 1.0f;
	if (editor->matInstance != nullptr)
		editor->matInstance->SetScalarParameterValue(FName("StencilValue"), stencilValue);*/
	if (spawnableSkeletalMesh->GetSkeletalMeshAsset() != nullptr)
		spawnableSkeletalMesh->SetRenderCustomDepth(spawn);
	else if (spawnableStaticMesh->GetStaticMesh() != nullptr)
		spawnableStaticMesh->SetRenderCustomDepth(spawn);
	/*}
	else {
		stencilValue = -1.0f;
		if (editor->matInstance != nullptr)
			editor->matInstance->SetScalarParameterValue(FName("StencilValue"), stencilValue);
		if (spawnableSkeletalMesh != nullptr)
			spawnableSkeletalMesh->SetRenderCustomDepth(spawn);
		else if (spawnableStaticMesh != nullptr)
			spawnableStaticMesh->SetRenderCustomDepth(spawn);
	}*/
}


void AEditableActor::CalculateRotationAngle()
{
	// 절대좌표 회전값 계산할 함수
}
