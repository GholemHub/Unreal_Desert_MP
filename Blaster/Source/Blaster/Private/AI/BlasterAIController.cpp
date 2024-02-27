// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/BlasterAIController.h"
#include "AI/AICharacter.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "AI/Components/BlasterAIPerceptionComponent.h"


ABlasterAIController::ABlasterAIController()
{
	BlasterPerceptionComponent = CreateDefaultSubobject<UBlasterAIPerceptionComponent>("BlasterPerceptionComponent");
	SetPerceptionComponent(*BlasterPerceptionComponent);
}

void ABlasterAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	const auto AICharacter = Cast<AAICharacter>(InPawn);
	if (AICharacter)
	{
		RunBehaviorTree(AICharacter->BechaviorTreeAsset);
	}

}

void ABlasterAIController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	const auto AimActor = BlasterPerceptionComponent->GetClossesEnemy();
	SetFocus(AimActor);
	//UE_LOG(LogTemp, Error, TEXT("Tick"))
}

AActor* ABlasterAIController::GetFocusOnActor() const
{
	if (!GetBlackboardComponent()) return nullptr;

	return Cast<AActor>(GetBlackboardComponent()->GetValueAsObject(FocusInKeyName));
}
