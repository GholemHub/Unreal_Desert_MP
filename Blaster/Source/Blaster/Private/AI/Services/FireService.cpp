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
	AActor* ControlledPawn = Controller->GetPawn();
	if (ControlledPawn)
	{
		// Get all components attached to the controlled pawn
		TArray<UCombatComponent*> ComponentsArray;
		ControlledPawn->GetComponents<UCombatComponent>(ComponentsArray);

		if (ComponentsArray.Num() > 0)
		{
			UCombatComponent* LastComponent = ComponentsArray[ComponentsArray.Num() - 1];
			if (LastComponent)
			{
				HasAim ? LastComponent->FireButtonPressed(true) : LastComponent->FireButtonPressed(false);
				UE_LOG(LogTemp, Error, TEXT("FireService"))
			}
			// Use LastComponent as needed
		}
		else
		{
			// Handle case where there are no components attached to the actor
		}
	}

	//if (Controller)
	//{
	//	//UE_LOG(LogTemp, Error, TEXT("Fire02"))
	//		
	//	const auto Combat = Controller->GetPawn()->FindComponentByClass<UCombatComponent>();
	//	
	//	if (Combat)
	//	{
	//		HasAim ? Combat->FireButtonPressed(true) : Combat->FireButtonPressed(false);
	//		UE_LOG(LogTemp, Error, TEXT("FireService"))
	//	}
	//}

	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
