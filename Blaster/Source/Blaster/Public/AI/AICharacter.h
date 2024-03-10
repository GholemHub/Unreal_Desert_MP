// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BlasterCharacter.h"
#include "Weapon/ProjectileWeapon.h"	
#include "Components/CombatComponent.h"
#include "AICharacter.generated.h"

/**
 * 
 */


UCLASS()
class BLASTER_API AAICharacter : public ABlasterCharacter
{
	GENERATED_BODY()
public:
	
	AAICharacter();

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AI)
	class UBehaviorTree* BechaviorTreeAsset;
	UPROPERTY(EditAnywhere)
	TSubclassOf<class AWeapon> Weapon;
	virtual bool IsWeaponEquipped() override;
	virtual AWeapon* GetEquippedWeapon();

	FORCEINLINE float GetAO_Yaw() const { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; }
	virtual FVector GetHitTarget() const override;
	virtual void PlayReloadMontage() override;
	virtual ECombatState GetCombatState() const override;
protected:
	virtual void PostInitializeComponents() override;
	void EquipAI(AWeapon* WeaponToEquip);
	virtual void AimOffset(float DeltaTime) override;


	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* CombatAI;

	bool FirstTime = true;
	float AO_Yaw;
	float IntorpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;


};
