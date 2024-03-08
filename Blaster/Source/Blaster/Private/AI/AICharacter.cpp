// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/AICharacter.h"
#include "Weapon/Weapon.h"
#include "Components/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Weapon/ProjectileWeapon.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "Kismet/KismetMathLibrary.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"

AAICharacter::AAICharacter()
{
	CombatAI = CreateDefaultSubobject<UCombatComponent>(TEXT("CombatComponentAI"));
	//WeaponClass = AProjectileWeapon::StaticClass();
}

void AAICharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if (CombatAI)
	{
		CombatAI->AICharacter = this;
	}
}

void AAICharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AAICharacter::Tick(float DeltaTime)
{
	//EquipAI();
	AimOffset(DeltaTime);
}

void AAICharacter::EquipAI(AWeapon* WeaponToEquip)
{

}

bool AAICharacter::IsWeaponEquipped()
{
	bool b = (CombatAI == nullptr);
	//UE_LOG(LogTemp, Error, TEXT("EquippedWeaponAI!!!-- %i"), b)
	//bool a = (Combat->EquippedWeapon == nullptr);
	//UE_LOG(LogTemp, Error, TEXT("GetEquippedWeapon %i"), a)
	return (CombatAI && CombatAI->EquippedWeapon);
}

AWeapon* AAICharacter::GetEquippedWeapon()
{
	 if (CombatAI == nullptr) return nullptr;
	return CombatAI->EquippedWeapon;
}
void AAICharacter::AimOffset(float DeltaTime)
{
	if (CombatAI && CombatAI->EquippedWeapon == nullptr) return;

	auto Velocity = GetVelocity();
	Velocity.Z = 0.f;
	float Speed = Velocity.Size();

	bool bIsInAir = GetCharacterMovement()->IsFalling();

	if (Speed == 0.f && !bIsInAir)
	{
		FVector ForwardVector = FRotationMatrix(GetActorRotation()).GetScaledAxis(EAxis::X);
		FRotator CurrentAimRotation = ForwardVector.Rotation();
		//UE_LOG(LogTemp, Warning, TEXT("CurrentAimRotation: %s"), *ForwardVector.ToString());
		FRotator DeltaAimRotation = UKismetMathLibrary::NormalizedDeltaRotator(StartingAimRotation, CurrentAimRotation);
		AO_Yaw = DeltaAimRotation.Yaw;

		//DrawDebugLine(GetWorld(), GetActorLocation(), GetActorLocation() + 100, FColor::Red);
	}

	if (FirstTime)
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;

		FirstTime = false;
		//TurningInPlace = ETurningInPlace::ETIP_NotTurning;
	}
	if (Speed > 0.f || bIsInAir)
	{
		StartingAimRotation = FRotator(0.f, GetBaseAimRotation().Yaw, 0.f);
		AO_Yaw = 0.f;
	}
}

FVector AAICharacter::GetHitTarget() const
{
	
	if (CombatAI == nullptr)
	{
		UE_LOG(LogTemp, Error, TEXT("CombatAI == nullptr"))
		return FVector();
	}
	return CombatAI->HitTarget;
}

void AAICharacter::PlayReloadMontage()
{
	if (CombatAI == nullptr || CombatAI->EquippedWeapon == nullptr) return;
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && ReloadMontage)
	{
		AnimInstance->Montage_Play(ReloadMontage);
		FName SectioName;
		UE_LOG(LogTemp, Error, TEXT("Reload:::AI"))
		switch (CombatAI->EquippedWeapon->GetWeaponType())
		{
		case EWeaponType::EWT_AssaultRifle:
			SectioName = FName("Rifle");
			break;
		}
		AnimInstance->Montage_JumpToSection(SectioName);
	}
}
