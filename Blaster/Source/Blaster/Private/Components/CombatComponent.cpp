// Fill out your copyright notice in the Description page of Project Settings.

#include "Components/CombatComponent.h"
#include "Character/BlasterCharacter.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Components/SphereComponent.h"
#include "Camera/CameraComponent.h"
#include "PlayerController/BlasterPlayerController.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/Character.h"
#include "TimerManager.h"
#include "Sound/SoundCue.h"
#include "AI/BlasterAIController.h"
#include "AI/AICharacter.h"
#include "Weapon/ProjectileWeapon.h"


UCombatComponent::UCombatComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UCombatComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UCombatComponent, EquippedWeapon);
	DOREPLIFETIME(UCombatComponent, EquippedWeaponAI);

	DOREPLIFETIME(UCombatComponent, bAiming);
	DOREPLIFETIME(UCombatComponent, CombatState);
	DOREPLIFETIME_CONDITION(UCombatComponent, CarriedAmmo, COND_OwnerOnly);
}

void UCombatComponent::EquipWeapon(AWeapon* WeaponToEquip)
{
	if (Character == nullptr || WeaponToEquip == nullptr) return;
	if (EquippedWeapon)
	{
		EquippedWeapon->Dropped();
	}
	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}
	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->SetHUDAmmo();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	if (EquippedWeapon->EquipSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			EquippedWeapon->EquipSound,
			Character->GetActorLocation()
			);
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDWeaponCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::EquipWeaponAI()
{
	auto Owner = GetOwner();
	if (Owner && Owner->IsA<AAICharacter>())
	{
		auto AICharacterLoc = Cast<AAICharacter>(Owner);
		AICharacter = AICharacterLoc;
		if (AICharacter && AICharacter->Weapon)
		{
			auto Weapon = AICharacter->Weapon;
			FRotator myRot(0, 0, 0);
			FVector myLoc(0, 0, 0);
			AWeapon* WeaponToEquip = GetWorld()->SpawnActor<AWeapon>(Weapon, myLoc, myRot);
			
			EquippedWeaponAI = WeaponToEquip;
			EquippedWeapon = EquippedWeaponAI;
			
			UE_LOG(LogTemp, Error, TEXT("EquipWeaponAI"))

			if (EquippedWeapon)
			{
				EquippedWeapon->Dropped();
			}

			//
		
			EquippedWeaponAI->SetWeaponState(EWeaponState::EWS_Equipped);
			EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
			const USkeletalMeshSocket* HandSocket = AICharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
			if (HandSocket)
			{
				HandSocket->AttachActor(EquippedWeaponAI, AICharacter->GetMesh());
				HandSocket->AttachActor(EquippedWeapon, AICharacter->GetMesh());
			}
			EquippedWeapon->SetOwner(AICharacter);
			EquippedWeaponAI->SetOwner(AICharacter);
		}
	}
}

void UCombatComponent::Reload()
{
	if (CarriedAmmo > 0 && CombatState != ECombatState::ECS_Reloading)
	{
		ServerReload();
	}
}

void UCombatComponent::FinishReloading()
{
	if (Character == nullptr) return;
	if (Character->HasAuthority())
	{
		CombatState = ECombatState::ECS_Unoccupied;
		UpdateAmmoValues();
	}
	if (bFireBtnPressed)
	{
		Fire();
	}
}

void UCombatComponent::HandlReload()
{
	if (Character) {
		Character->PlayReloadMontage();
	}
	if (AICharacter)
	{
		AICharacter->PlayReloadMontage();
	}
}

int32 UCombatComponent::AmountToReload()
{
	if (EquippedWeapon == nullptr) return 0;
	int32 RoomInMag = EquippedWeapon->GetMagCopacity() - EquippedWeapon->GetAmmo();
	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		int32 AmountCarried = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
		int32 Least = FMath::Min(RoomInMag, AmountCarried);
		return FMath::Clamp(RoomInMag, 0, Least);
	}
	return 0;
}

void UCombatComponent::ServerReload_Implementation()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	int32 ReloadAmount = AmountToReload();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDWeaponCarriedAmmo(CarriedAmmo);
	}

	EquippedWeapon->AddAmmo(-ReloadAmount);

	CombatState = ECombatState::ECS_Reloading;
	HandlReload();
}

void UCombatComponent::OnRep_EquippedWeapon()
{
	if (EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
		}
		Character->GetCharacterMovement()->bOrientRotationToMovement = false;
		Character->bUseControllerRotationYaw = true;
	}	
}

void UCombatComponent::OnRep_EquippedWeaponAI()
{
	if (EquippedWeaponAI && AICharacter)
	{
		EquippedWeaponAI->SetWeaponState(EWeaponState::EWS_Equipped);
		const USkeletalMeshSocket* HandSocket = AICharacter->GetMesh()->GetSocketByName(FName("RightHandSocket"));
		if (HandSocket)
		{
			UE_LOG(LogTemp, Error, TEXT("OnRep_EquippedWeaponAI"))
			HandSocket->AttachActor(EquippedWeaponAI, AICharacter->GetMesh());
		}
		//AICharacter->GetCharacterMovement()->bOrientRotationToMovement = false;
		//AICharacter->bUseControllerRotationYaw = true;
	}
}

void UCombatComponent::FireButtonPressed(bool bPressed)
{
	UE_LOG(LogTemp, Warning, TEXT("FireButtonPressed"))
	bFireBtnPressed = bPressed;
	if (bFireBtnPressed) {
		Fire();
	}
}

void UCombatComponent::Fire()
{
	UE_LOG(LogTemp, Error, TEXT("Fire::Fire %i"), CanFire())
	
	if (CanFire())
	{	
		bCanFire = false;
		ServerFire(HitTarget);
		if (EquippedWeapon)
		{
			UE_LOG(LogTemp, Error, TEXT("Fire::"))
			//CrosshaurShootingFactor = .7f;
		}
		StartFireTimer();
	}
}

void UCombatComponent::Server_TraceUnderCrosshairsAI_Implementation(const FHitResult& TraceHitResult)
{
	Multicast_TraceUnderCrosshairsAI(TraceHitResult);
}

void UCombatComponent::Multicast_TraceUnderCrosshairsAI_Implementation(const FHitResult& TraceHitResult)
{
	auto t = TraceHitResult;
	TraceUnderCrosshairsAI(t);
}

void UCombatComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D VeiwportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(VeiwportSize);
	}

	FVector2D CrosshairLocation(VeiwportSize.X / 2.f, VeiwportSize.Y / 2.f);
	FVector CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);

	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		if (Character)
		{
			float DistanceToCharacter = (Character->GetActorLocation() - Start).Size();
			Start += CrosshairWorldDirection * (DistanceToCharacter + 100.f);
		}
		FVector End = Start + CrosshairWorldDirection * 80000.f;
		
		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility//,
			//CollisionParams
		);

		//DrawDebugLine(GetWorld(), Start, End, FColor::White, false, -1, 0, 2.0f);

		if (!TraceHitResult.bBlockingHit)
		{
			TraceHitResult.ImpactPoint = End;
		}

		if (TraceHitResult.GetActor() && TraceHitResult.GetActor()->Implements<UInteractWithCrosshairsInterface>())
		{
			HUDPackage.CrosshairsColor = FLinearColor::Red;
		}
		else {
			HUDPackage.CrosshairsColor = FLinearColor::White;
		}
	}
}

void UCombatComponent::TraceUnderCrosshairsAI(FHitResult& TraceHitResult)
{
	FVector ActorLocation = AICharacter->GetActorLocation();
	FVector ActorForwardVector = AICharacter->GetActorForwardVector();
	ActorLocation += FVector(0,0,100); // TODO This is KOSTYL'
	float LineLength = 3000.0f;

	// Calculate the end point of the line
	FVector LineEndPoint = ActorLocation + ActorForwardVector * LineLength;

	// Draw a debug line from the actor's location to the calculated end point (the direction the actor is facing)
	DrawDebugLine(GetWorld(), ActorLocation, LineEndPoint, FColor::Green, false, -1, 0, 2.0f);

	// Calculate the end point of the line
	FVector End = ActorLocation + ActorForwardVector * 80000.f;

	// Perform a line trace from the actor's location to the end point
	GetWorld()->LineTraceSingleByChannel(
		TraceHitResult,
		ActorLocation,
		End,
		ECollisionChannel::ECC_Visibility
	);
}

void UCombatComponent::SetHUDCrosshairs(float DeltaTime)
{
	if (Character == nullptr || Character->Controller == nullptr) return;
	
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		HUD = HUD == nullptr ? Cast<ABlasterHUD>(Controller->GetHUD()) : HUD;
		if (HUD)
		{
			
			if (EquippedWeapon) 
			{
				HUDPackage.CrosshaursCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairsLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairsRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairsTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairsBottom = EquippedWeapon->CrosshairsBottom;
			}
			else 
			{
				//UE_LOG(LogTemp, Warning, TEXT("log6"))
				HUDPackage.CrosshaursCenter = nullptr;
				HUDPackage.CrosshairsLeft = nullptr;
				HUDPackage.CrosshairsRight = nullptr;
				HUDPackage.CrosshairsTop = nullptr;
				HUDPackage.CrosshairsBottom = nullptr;
			}

			//Calculate crosshair spread
			FVector2D WalkSpeedRange(0.f, Character->GetCharacterMovement()->MaxWalkSpeed);
			FVector2D VelocityMultiplayerRange(0.f, 1.f);

			FVector Velocity = Character->GetVelocity();
			Velocity.Z = 0.f;

			CrosshairVelocityFactor = FMath::GetMappedRangeValueClamped(WalkSpeedRange, VelocityMultiplayerRange, Velocity.Size());

			if (Character->GetCharacterMovement()->IsFalling())
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 2.25f, DeltaTime, 2.25f);
			}
			else
			{
				CrosshairInAirFactor = FMath::FInterpTo(CrosshairInAirFactor, 0.f, DeltaTime, 30.f);
			}

			if (bAiming)
			{
				CrosshaurAimFactor = FMath::FInterpTo(CrosshaurAimFactor, 0.58f, DeltaTime, 30.f);
			}
			else
			{
				CrosshaurAimFactor = FMath::FInterpTo(CrosshaurAimFactor, 0.f, DeltaTime, 30.f);
			}

			CrosshaurShootingFactor = FMath::FInterpTo(CrosshaurShootingFactor, 0.f, DeltaTime, 20.f);

			HUDPackage.CrosshairSpread = 0.35 +
				CrosshairVelocityFactor + 
				CrosshairInAirFactor -
				CrosshaurAimFactor +
				CrosshaurShootingFactor;

			HUD->SetHUDPackage(HUDPackage);

		}
	}
}

void UCombatComponent::InterpFOV(float Deltatime)
{
	if (EquippedWeapon == nullptr) return;

	if (bAiming) 
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, EquippedWeapon->GetZoomedFOV(), Deltatime, EquippedWeapon->GetZoomInterpSpeed());

	}
	else
	{
		CurrentFOV = FMath::FInterpTo(CurrentFOV, DefaultFOV, Deltatime, ZoomInterpSpeed);
	}
	if (Character && Character->GetFollowCamera())
	{
		Character->GetFollowCamera()->SetFieldOfView(CurrentFOV);
	}
}

void UCombatComponent::StartFireTimer()
{
	if (EquippedWeapon == nullptr) return;

	if (AICharacter != nullptr) {
		AICharacter->GetWorldTimerManager().SetTimer(
			FireTimer,
			this,
			&UCombatComponent::FireTimerFinished,
			EquippedWeapon->FireDelay
		);
		return;
	}

	
	if (Character != nullptr) {
		Character->GetWorldTimerManager().SetTimer(
			FireTimer,
			this,
			&UCombatComponent::FireTimerFinished,
			EquippedWeapon->FireDelay
		);
		return;
	}
}

void UCombatComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr) return;
	bCanFire = true;
	if (bFireBtnPressed && EquippedWeapon->bAutomatic) {
		Fire();
	}
	if (EquippedWeapon->IsEmpty())
	{
		Reload();
	}
}

void UCombatComponent::OnRep_CarriedAmmo()
{
	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDWeaponCarriedAmmo(CarriedAmmo);
	}
}

void UCombatComponent::InitializeCarriedAmmo()
{
	CarriedAmmoMap.Emplace(EWeaponType::EWT_AssaultRifle, StartingARAmmo);
}

void UCombatComponent::OnRep_CombatState()
{
	switch (CombatState)
	{
	case ECombatState::ECS_Reloading:
		HandlReload();
		break;
	case ECombatState::ECS_Unoccupied:
		if (bFireBtnPressed)
		{
			Fire();
		}
		break;
	}
}

void UCombatComponent::UpdateAmmoValues()
{
	if (Character == nullptr || EquippedWeapon == nullptr) return;

	int32 ReloadAmount = AmountToReload();

	if (CarriedAmmoMap.Contains(EquippedWeapon->GetWeaponType()))
	{
		CarriedAmmoMap[EquippedWeapon->GetWeaponType()] -= ReloadAmount;
		CarriedAmmo = CarriedAmmoMap[EquippedWeapon->GetWeaponType()];
	}

	Controller = Controller == nullptr ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (Controller)
	{
		Controller->SetHUDWeaponCarriedAmmo(CarriedAmmo);
	}

	EquippedWeapon->AddAmmo(-ReloadAmount);
}

bool UCombatComponent::CanFire()
{
	if (EquippedWeapon == nullptr) return false;
	return !EquippedWeapon->IsEmpty() && bCanFire && CombatState == ECombatState::ECS_Unoccupied;
}

void UCombatComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (AICharacter && CombatState == ECombatState::ECS_Unoccupied)
	{
		AICharacter->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
		return;
	}
	if (Character && CombatState == ECombatState::ECS_Unoccupied)
	{
		Character->PlayFireMontage(bAiming);
		EquippedWeapon->Fire(TraceHitTarget);
		return;
	}	
}

void UCombatComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);

}

void UCombatComponent::BeginPlay()
{
	Super::BeginPlay();
	if (Character)
	{
		if (Character->GetFollowCamera())
		{
			DefaultFOV = Character->GetFollowCamera()->FieldOfView;
			CurrentFOV = DefaultFOV;
		}
		if (Character->HasAuthority())
		{
			InitializeCarriedAmmo();
		}
	}else if (AICharacter)
	{
		UE_LOG(LogTemp, Error, TEXT("AICharacter"))
			EquipWeaponAI();
	}
}

void UCombatComponent::SetAiming(bool bIsAiming)
{
	bAiming = bIsAiming;
	ServerSetAiming(bIsAiming);
}

void UCombatComponent::ServerSetAiming_Implementation(bool bIsAiming)
{
	bAiming = bIsAiming;
}

void UCombatComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	
	if (Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResultAI);
		
		
		HitTarget = HitResult.ImpactPoint;
		//HitResultAI = HitResultAI.ImpactPoint;

		SetHUDCrosshairs(DeltaTime);
		InterpFOV(DeltaTime);
		
	}

	if (AICharacter)
	{
		FHitResult HitResult;
		TraceUnderCrosshairsAI(HitResult);
		
		HitTarget = HitResult.ImpactPoint;
		
	}
}