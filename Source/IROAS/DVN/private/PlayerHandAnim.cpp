

#include "PlayerHandAnim.h"
#include "Gun.h"
#include "Sword.h"

void UPlayerHandAnim::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

}

void UPlayerHandAnim::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (PoseAlphaGrasp == 0) holdingWeapon = nullptr;
	if (holdingWeapon == nullptr) {
		bHoldingGun = false;
		bHoldingSword = false;
	}
	else {
		gun = Cast<AGun>(holdingWeapon);
		sword = Cast<ASword>(holdingWeapon);
		if (gun != nullptr) {
			bHoldingGun = true;
			bHoldingSword = false;
			if (PoseAlphaIndexCurl >= 1.0f && gun->bInFire) RecoilAnim();
		}
		else if (sword != nullptr) {
			bHoldingGun = false;
			bHoldingSword = true; 
		}
	}
		
	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, FString::Printf(TEXT("%.1f \r\n %.1f"), bHoldingGun, bHoldingSword));
}

void UPlayerHandAnim::RecoilAnim()
{
	int32 index = FMath::RandRange(0, 2);
	FString sectionName = FString::Printf(TEXT("Recoil%d"), index);
	PlayRecoilAnim(*sectionName);
}