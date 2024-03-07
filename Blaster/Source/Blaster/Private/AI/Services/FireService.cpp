// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/FireService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Components/BlasterAIPerceptionComponent.h"
#include "AIController.h"
#include "Components/CombatComponent.h"


UFireService::UFireService()
{
	NodeName = "Fire";
}

void UFireService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	const auto Controller = OwnerComp.GetAIOwner();
	auto Blackboard = OwnerComp.GetBlackboardComponent();

	auto HasAim = Blackboard && Blackboard->GetValueAsObject(EnemyActorKey.SelectedKeyName);

	//UE_LOG(LogTemp, Error, TEXT("Fire01"))

	if (Controller)
	{
		//UE_LOG(LogTemp, Error, TEXT("Fire02"))
			
		const auto Combat = Controller->GetPawn()->FindComponentByClass<UCombatComponent>();
		
		if (Combat)
		{
			HasAim ? Combat->FireButtonPressed(true) : Combat->FireButtonPressed(false);
			UE_LOG(LogTemp, Error, TEXT("Fire"))
		}
	}

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
