// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Services/FindEnemyService.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Components/BlasterAIPerceptionComponent.h"
#include "AIController.h"

UFindEnemyService::UFindEnemyService()
{
	NodeName = "Find Enemy";
}

void UFindEnemyService::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	auto Blackboard = OwnerComp.GetBlackboardComponent();
	if (Blackboard)
	{
		//AAIController* Controller = Cast<AAIController>(OwnerComp.GetAIOwner());
		auto Controller = OwnerComp.GetAIOwner();
		auto PerceptionComponent = Controller->FindComponentByClass<UBlasterAIPerceptionComponent>();
		if (PerceptionComponent)
		{
			Blackboard->SetValueAsObject(EnemyActorKey.SelectedKeyName, PerceptionComponent->GetClossesEnemy());
		}
	}
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
}
 