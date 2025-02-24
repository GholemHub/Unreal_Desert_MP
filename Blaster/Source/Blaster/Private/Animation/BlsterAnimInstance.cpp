// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/BlsterAnimInstance.h"
#include "Character/BlasterCharacter.h"
#include "Weapon/Weapon.h"
#include "Kismet/KismetMathLibrary.h"
#include "Blaster/BlasterTypes/CombatState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AI/AICharacter.h"

void UBlsterAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	AICharacter = Cast<AAICharacter>(TryGetPawnOwner());
}

void UBlsterAnimInstance::NativeUpdateAnimation(float DeltaTime)
{
	Super::NativeUpdateAnimation(DeltaTime);


	NativeUpdateAnimationAI(DeltaTime);

	if (BlasterCharacter == nullptr)
	{
		BlasterCharacter = Cast<ABlasterCharacter>(TryGetPawnOwner());
	}
	if (BlasterCharacter == nullptr) return;

	FVector Velocity = BlasterCharacter->GetVelocity();
	Velocity.Z = 0.f;

	Speed = Velocity.Size();
	bIsInAir = BlasterCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = BlasterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true: false;
	bWeaponEquipped = BlasterCharacter->IsWeaponEquipped();
	EquippedWeapon = BlasterCharacter->GetEquippedWeapon();
	bIsCrouched = BlasterCharacter->bIsCrouched;
	bIsAimed = BlasterCharacter->IsAiming();
	TurningInPlace = BlasterCharacter->GetTurningInPlace();
	bElimmed = BlasterCharacter->IsElimmed();

	const auto VelocityNormal = BlasterCharacter->GetVelocity().GetSafeNormal();
	const auto AngleBetween = FMath::Acos(FVector::DotProduct(BlasterCharacter->GetActorForwardVector(), VelocityNormal));
	const auto CrossProduct = FVector::CrossProduct(BlasterCharacter->GetActorForwardVector(), VelocityNormal);
	Direction = FMath::RadiansToDegrees(AngleBetween) * FMath::Sign(CrossProduct.Z);

	AO_Yaw = BlasterCharacter->GetAO_Yaw();
	AO_Pitch = BlasterCharacter->GetAO_Pitch();

	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaoponMesh() && BlasterCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaoponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		BlasterCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (BlasterCharacter->IsLocallyControlled()) 
		{
			bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaoponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - BlasterCharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);

			FTransform MuzzleTipTransform = EquippedWeapon->GetWeaoponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
			FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + MuzzleX * 1000.f, FColor::Red);
			//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), BlasterCharacter->GetHitTarget(), FColor::Orange);
		}
	}

	bUseFABRIK = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAnimOffsets = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bTransformRightHand = BlasterCharacter->GetCombatState() != ECombatState::ECS_Reloading;
}

void UBlsterAnimInstance::NativeUpdateAnimationAI(float DeltaTime)
{
	
	if (AICharacter == nullptr)
	{
		AICharacter = Cast<AAICharacter>(TryGetPawnOwner());
	}
	if (AICharacter == nullptr) return;
	
	FVector Velocity = AICharacter->GetVelocity();
	Velocity.Z = 0.f;

	Speed = Velocity.Size();
	bIsInAir = AICharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = AICharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f ? true : false;
	bWeaponEquipped = AICharacter->IsWeaponEquipped();
	EquippedWeapon = AICharacter->GetEquippedWeapon();
	//EquippedWeaponAI = AICharacter->GetEquippedWeapon();
	bIsCrouched = AICharacter->bIsCrouched;
	bIsAimed = AICharacter->IsAiming();
	TurningInPlace = AICharacter->GetTurningInPlace();
	bElimmed = AICharacter->IsElimmed();

	const auto VelocityNormal = AICharacter->GetVelocity().GetSafeNormal();
	const auto AngleBetween = FMath::Acos(FVector::DotProduct(AICharacter->GetActorForwardVector(), VelocityNormal));
	const auto CrossProduct = FVector::CrossProduct(AICharacter->GetActorForwardVector(), VelocityNormal);
	Direction = FMath::RadiansToDegrees(AngleBetween) * FMath::Sign(CrossProduct.Z);

	AO_Yaw = AICharacter->GetAO_Yaw();
	AO_Pitch = AICharacter->GetAO_Pitch();
	//UE_LOG(LogTemp, Error, TEXT("EquippedWeapon %i"), bWeaponEquipped)
	if (bWeaponEquipped && EquippedWeapon && EquippedWeapon->GetWeaoponMesh() && AICharacter->GetMesh())
	{
		//UE_LOG(LogTemp, Error, TEXT("NativeUpdateAnimationAI1"))
		LeftHandTransform = EquippedWeapon->GetWeaoponMesh()->GetSocketTransform(FName("LeftHandSocket"), ERelativeTransformSpace::RTS_World);
		FVector OutPosition;
		FRotator OutRotation;
		AICharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));

		if (AICharacter)
		{
			//bLocallyControlled = true;
			FTransform RightHandTransform = EquippedWeapon->GetWeaoponMesh()->GetSocketTransform(FName("hand_r"), ERelativeTransformSpace::RTS_World);
			FRotator LookAtRotation = UKismetMathLibrary::FindLookAtRotation(RightHandTransform.GetLocation(), RightHandTransform.GetLocation() + (RightHandTransform.GetLocation() - AICharacter->GetHitTarget()));
			RightHandRotation = FMath::RInterpTo(RightHandRotation, LookAtRotation, DeltaTime, 30.f);

			FTransform MuzzleTipTransform = EquippedWeapon->GetWeaoponMesh()->GetSocketTransform(FName("MuzzleFlash"), ERelativeTransformSpace::RTS_World);
			FVector MuzzleX(FRotationMatrix(MuzzleTipTransform.GetRotation().Rotator()).GetUnitAxis(EAxis::X));

			//UE_LOG(LogTemp, Error, TEXT("AICharacter"))

			//DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), MuzzleTipTransform.GetLocation() + 100, FColor::Purple);
			DrawDebugLine(GetWorld(), MuzzleTipTransform.GetLocation(), AICharacter->GetHitTarget(), FColor::Orange);
		}
	}

	bUseFABRIK = AICharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bUseAnimOffsets = AICharacter->GetCombatState() != ECombatState::ECS_Reloading;
	bTransformRightHand = AICharacter->GetCombatState() != ECombatState::ECS_Reloading;
}
