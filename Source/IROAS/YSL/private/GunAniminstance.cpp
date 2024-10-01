// Fill out your copyright notice in the Description page of Project Settings.


#include "GunAniminstance.h"
#include <Kismet/GameplayStatics.h>
#include "Gun.h"

void UGunAniminstance::NativeInitializeAnimation()
{
	gun = Cast<AGun>(GetOwningActor());
}

void UGunAniminstance::NativeUpdateAnimation(float DeltaSeconds)
{
	if(gun == nullptr) return;
	if(gun->bInFire && !bInRecoil) {
		bInRecoil = true;
		RecoilAnim();
	}
	if(gun->bInReload && !bInReload) {
		bInReload = true;
		ReloadAnim();
	}
}

void UGunAniminstance::RecoilAnim()
{
	Montage_Play(recoilMontage);
}

void UGunAniminstance::ReloadAnim()
{
	Montage_Play(reloadMontage);
}

void UGunAniminstance::AnimNotify_RecoilEnd()
{
	bInRecoil = false;
}

void UGunAniminstance::AnimNotify_ReloadEnd()
{
	bInReload = false;
}
