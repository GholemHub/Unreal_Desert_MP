// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"
#include "TimerManager.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/PlayerController.h"
#include "Blaster/BlasterTypes/CombatState.h"

#include "BlasterCharacter.generated.h"

class UCameraComponent;
class USpringArmComponent;
class USphereComponent;
class UInputMappingContext;
class UInputAction;

UCLASS()
class BLASTER_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ABlasterCharacter();
	UFUNCTION()
	void UpdateHUDHealth();
	virtual void Destroyed() override;
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USpringArmComponent* SpringArmComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		UCameraComponent* CameraComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
		USphereComponent* CameraCollisionComponent;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputMappingContext* InputMappingContext;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputMappingContext* InputMappingContext_Firing;

	virtual void PostInitializeComponents() override;

	//INPUT
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* MovementInputAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* CameraMovementInputAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* JumpMovementInputAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* EquipInputAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* CrouchInputAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* AimInputAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* AimInputAction2;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* FirePressedInputAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* FireReleasedInputAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* ReloadReleasedInputAction;
	UPROPERTY(EditAnywhere, Category = Input)
		UInputAction* ReloadPressedInputAction;
		

	//Function to call onMove
	void Move(const FInputActionValue& Value);
	void MoveCamera(const FInputActionValue& Value);
	void Equip();
	void CrouchBtnPressed();
	void AimBtnPressed();
	void AimBtnReleased();
	void FireBtnPressed();
	void FireBtnReleased();
	void AimOffset(float DeltaTime);
	void ReloadPressed();
	void ReloadReliesed();

	UFUNCTION()
	void ReceiveDamage(AActor* DamageActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	
	/*
	Montage
	*/
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* DeathMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ReloadMontage;

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraThreshold = 200.f;
	
	/**
	* Dissolve Effect
	**/
	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline;

	FOnTimelineFloat DissloveTrack;

	UFUNCTION()
	void UpdateDessolveMaterial(float DissolveValue);

	void StartDissolve();

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve;

	//Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissloveMaterialInstance;
	// Material instance set on the BP used with the dynamic metarial instance  
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance;
	//Dynamic instance that we can change at runtime
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissloveMaterialInstance2;
	// Material instance set on the BP used with the dynamic metarial instance  
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance2;

	/**
	* ElimBot
	*/
	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect;

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent;

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void ElimTimerFinished();

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDilay = 2.f;

	void Elim();

	UFUNCTION(NetMulticast, Reliable)
	void Multicast_Elim();

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	UFUNCTION(BlueprintCallable, Category = Movement)
	float GetMovementDirection() const;
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayHitReactMontage();
	void PlayDeathMontage();

	AWeapon* GetEquippedWeapon();

	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw;}
	FORCEINLINE float GetAO_Pitch() const {return AO_Pitch;}
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return CameraComponent; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }

	ECombatState GetCombatState() const;

	FVector GetHitTarget() const;
private:

	void CheckKameraOverlap();
	UFUNCTION()
		void OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnCameraCollisionEndOverlap(
			UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
		void ServerEquipButtonPressed();
	UFUNCTION()
		void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	bool FirstTime = true;
	float AO_Yaw;
	float IntorpAO_Yaw;
	float AO_Pitch;
	FRotator StartingAimRotation;// = FRotator::ZeroRotator;
	float CalculateSpeed();

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	/**
	* PlayerHealth
	*/
	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth = 100.f;
	UPROPERTY(ReplicatedUsing = OnRep_Health, VisibleAnywhere, Category = "Player Stats")
	float Health = 100.f;
	UFUNCTION()
	void OnRep_Health();

	class ABlasterPlayerController* BlasterPlayerController;

	bool bElimmed = false;
	
};
