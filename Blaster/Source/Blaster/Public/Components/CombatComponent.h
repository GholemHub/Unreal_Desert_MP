#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GameFramework/Character.h"
#include "HUD/BlasterHUD.h"
#include "Weapon/WeaponTypes.h"
#include "Blaster/BlasterTypes/CombatState.h"

#include "CombatComponent.generated.h" 

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();
	friend class ABlasterCharacter;
	friend class AAICharacter;
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	void EquipWeaponAI();
	void EquipWeapon(class AWeapon* WeaponToEquip);
	//void EquipWeaponAI(class AWeapon* WeaponToEquip);
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();
	void Fire();

	void FireButtonPressed(bool bPressed);
	/**
* Automatic Fire
*/

	FTimerHandle FireTimer;

	bool bCanFire = true;

	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();

protected:
	virtual void BeginPlay() override;
	void SetAiming(bool bIsAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);
	UFUNCTION()
	void OnRep_EquippedWeapon();
	

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);
	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);
	void TraceUnderCrosshairsAI(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);

	float CrosshairVelocityFactor;
	float CrosshairInAirFactor;
	float CrosshaurAimFactor;
	float CrosshaurShootingFactor;

	FVector HitTarget;

	FHUDPackage HUDPackage;

	float DefaultFOV;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV = 30.f;
	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed = 20.f;

	float CurrentFOV;

	void InterpFOV(float Deltatime);



	//Carried ammo
	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_CarriedAmmo, Category = "Weapon Propertie")
	int32 CarriedAmmo;

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;
	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo = 3;

	void InitializeCarriedAmmo();

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandlReload();

	int32 AmountToReload();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState = ECombatState::ECS_Unoccupied;
	UFUNCTION()
	void OnRep_CombatState();
	
private:
	class ABlasterCharacter* Character;
	class AAICharacter* AICharacter;
	
	class ABlasterPlayerController* Controller;
	class ABlasterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	class AWeapon* EquippedWeapon;
	class AWeapon* EquippedWeaponAI;

	UPROPERTY(Replicated)
	bool bAiming;
	bool bFireBtnPressed; 

	void UpdateAmmoValues();

	//void SpawnWeapon();
	
};
