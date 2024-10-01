

#include "EditorZoomSpline.h"
#include "Components/ArrowComponent.h"
#include <Components/SplineMeshComponent.h>
#include <Components/ArrowComponent.h>
#include <Kismet/GameplayStatics.h>
#include "EditorPlayer.h"

AEditorZoomSpline::AEditorZoomSpline()
{
	PrimaryActorTick.bCanEverTick = false;

	zoomSplineComp = CreateDefaultSubobject<USplineMeshComponent>(TEXT("Zoom Spline"));
	SetRootComponent(zoomSplineComp);
	zoomSplineComp->bAllowSplineEditingPerInstance = true;
	zoomSplineComp->SetEndPosition(FVector(3000, 0, 0));

	startPosition = CreateDefaultSubobject<USceneComponent>(TEXT("Start Position"));
	startPosition->SetupAttachment(RootComponent);
	startPosition->SetRelativeLocation(zoomSplineComp->GetStartPosition());
	endPosition = CreateDefaultSubobject<USceneComponent>(TEXT("End Position"));
	endPosition->SetupAttachment(RootComponent);
	endPosition->SetRelativeLocation(zoomSplineComp->GetEndPosition());
	
	snapPoint = CreateDefaultSubobject<UArrowComponent>(TEXT("SnapPosition"));
	snapPoint->SetupAttachment(RootComponent);
}

void AEditorZoomSpline::BeginPlay()
{
	Super::BeginPlay();

	editor = Cast<AEditorPlayer>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
}

void AEditorZoomSpline::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}
