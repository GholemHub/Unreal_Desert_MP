
#include "Components/WeaponComponent.h"
#include "Components/CombatComponent.h"
#include "GameFramework/Character.h"
#include "Weapon/ProjectileWeapon.h"
#include "AI/AICharacter.h"

UWeaponComponent::UWeaponComponent()
{

	PrimaryComponentTick.bCanEverTick = false;

}

void UWeaponComponent::Fire()
{
	if (!CurrentWeapon) return;

	//CurrentWeapon->Fire();
}


void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();
	//SpawnWeapon();
}

void UWeaponComponent::SpawnWeapon()
{
	if (!GetWorld()) return;

	ACharacter* Character = Cast<ACharacter>(GetOwner());
	if (!Character) return;

	//CurrentWeapon = GetWorld()->SpawnActor<AWeapon>(WeaponClass);
	if (!CurrentWeapon) return;

	
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, false);
	CurrentWeapon->AttachToComponent(Character->GetMesh(), AttachmentRules, "RightHandSocket");
}