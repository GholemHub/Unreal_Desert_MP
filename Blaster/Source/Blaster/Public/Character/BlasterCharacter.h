// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "Blaster/BlasterTypes/TurningInPlace.h"
#include "Interfaces/InteractWithCrosshairsInterface.h"

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


	/*
	Montage
	*/
	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage;

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage;

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere, Category = Camera)
	float CameraThreshold = 200.f;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const;
	
	UFUNCTION(BlueprintCallable, Category = Movement)
	float GetMovementDirection() const;
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	void PlayFireMontage(bool bAiming);
	void PlayHitReactMontage();

	FORCEINLINE float GetAO_Yaw() const {return AO_Yaw;}
	FORCEINLINE float GetAO_Pitch() const {return AO_Pitch;}
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() const { return TurningInPlace; }
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return CameraComponent; }

	FVector GetHitTarget() const;

	UFUNCTION(NetMulticast, Unreliable)
	void MulticastHit();
private:

	void CheckKameraOverlap();
	UFUNCTION()
		void OnCameraCollisionBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
			int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	UFUNCTION()
		void OnCameraCollisionEndOverlap(
			UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	/*UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
		class UWidgetComponent* OverheadWidget;*/
	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
		class AWeapon* OverlappingWeapon;
	UPROPERTY(VisibleAnywhere)
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

	
};
