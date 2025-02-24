// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()
public:
	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDWeaponCarriedAmmo(int32 Carried);
protected:
	virtual void BeginPlay() override;
	virtual void OnPossess(APawn* InPawn) override;
private:
	class ABlasterHUD* BlasterHUD;

	float HUDHealth;
	bool bInitializeHealth = false;
	float HUDMaxHealth;
};
