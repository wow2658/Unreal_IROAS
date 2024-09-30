// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BaseCharacter.h"
#include "InputActionValue.h"
#include "Interfaces/OnlineSessionInterface.h"

#include "LobbyPlayer.generated.h"

class APlayerHand;
class UBasicMovementComponent;
class UCapsuleComponent;

UCLASS()
class IROAS_API ALobbyPlayer : public ABaseCharacter
{
	GENERATED_BODY()

public:
	ALobbyPlayer();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
public:

	UPROPERTY(EditDefaultsOnly, Category = "MySettings|Components")
	UBasicMovementComponent* moveComp;
	UPROPERTY(EditDefaultsOnly, Category = "MySettings|Components")
	UCapsuleComponent* CapsuleCollision;

	APlayerHand* leftHand = nullptr;
	APlayerHand* rightHand = nullptr;

public:

	virtual void InitializeVRHand() override;

public:
	// Left
	virtual void HandleLeftGripPressed() override;
	virtual void HandleLeftGripReleased() override;
	virtual void HandleLeftTriggerPressed() override;
	virtual void HandleLeftTriggerReleased() override;
	virtual void HandleLeftThumbstickAxis(const FInputActionValue& Value) override;
	virtual void HandleLeftThumbstickLeft() override;
	virtual void HandleLeftThumbstickRight() override;
	virtual void HandleLeftMenu() override;

	// Right
	UFUNCTION(BlueprintCallable)
	virtual void HandleRightGripPressed() override;
	UFUNCTION(BlueprintCallable)
	virtual void HandleRightGripReleased() override;
	virtual void HandleRightTriggerPressed() override;
	virtual void HandleRightTriggerReleased() override;
	virtual void HandleRightThumbstickAxis(const FInputActionValue& Value) override;
	virtual void HandleRightThumbstickLeft() override;
	virtual void HandleRightThumbstickRight() override;

public:

	// Pointer to the online session interface
	IOnlineSessionPtr OnlineSessionInterface;
	// TSharedPtr<class IOnlineSessionPtr, ESPMode::ThreadSafe> OnlineSessionInterface;

protected:
	UFUNCTION(BlueprintCallable)
	void CreateGameSession();

	UFUNCTION(BlueprintCallable)
	void JoinGameSession();

	void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);
	void OnFindSessionsComplete(bool bWasSuccessful);
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	//UFUNCTION(BlueprintCallable)
	//void OpenLobby();

	//UFUNCTION(BlueprintCallable)
	//void CallOpenLevel(const FString& Address);

	//UFUNCTION(BlueprintCallable)
	//void CallClientTravel(const FString& Address);

private:

	FOnCreateSessionCompleteDelegate CreateSessionCompleteDelegate;
	FOnFindSessionsCompleteDelegate FindSessionsCompleteDelegate;
	TSharedPtr<FOnlineSessionSearch> SessionSearch;
	FOnJoinSessionCompleteDelegate JoinSessionCompleteDelegate;


	void Move(const FInputActionValue& Value);
	void Rotate(const FInputActionValue& Value);
};