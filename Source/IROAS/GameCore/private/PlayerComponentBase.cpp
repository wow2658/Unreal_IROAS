

#include "PlayerComponentBase.h"
#include "BaseCharacter.h"

UPlayerComponentBase::UPlayerComponentBase()
{
	PrimaryComponentTick.bCanEverTick = true;


}


void UPlayerComponentBase::BeginPlay()
{
	Super::BeginPlay();
	player = GetOwner<ABaseCharacter>();

	
}


void UPlayerComponentBase::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

}



