// Fill out your copyright notice in the Description page of Project Settings.


#include "AI/Components/BlasterAIPerceptionComponent.h"
#include "AIController.h"
#include "Perception/AISense_Sight.h"
#include "Components/HealthComponent.h"



AActor* UBlasterAIPerceptionComponent::GetClossesEnemy() const
{
	//UE_LOG(LogTemp, Error, TEXT("LOG1"))
	TArray<AActor*> PercieveActors;
	GetCurrentlyPerceivedActors(UAISense_Sight::StaticClass(), PercieveActors);
	if (PercieveActors.Num() == 0) return nullptr;
	//UE_LOG(LogTemp, Error, TEXT("LOG2"))
	const auto Controller = Cast<AAIController>(GetOwner());
	if (!Controller) return nullptr;
	//UE_LOG(LogTemp, Error, TEXT("LOG3"))
	const auto Pawn = Controller->GetPawn();

	if (!Pawn) return nullptr; 
	//UE_LOG(LogTemp, Error, TEXT("LOG4"))

	float BestDistance = MAX_FLT;
	AActor* BestPawn = nullptr;

    for (AActor* PerceivedActor : PercieveActors)
    {
		//UE_LOG(LogTemp, Error, TEXT("LOG5"))
		auto HealthComponent = PerceivedActor->FindComponentByClass<UHealthComponent>();
		
		if (HealthComponent && HealthComponent->IsAlive())
		{
			//UE_LOG(LogTemp, Error, TEXT("Is alive2"))
			auto CurrentDistance = (PerceivedActor->GetActorLocation() - Pawn->GetActorLocation()).Size();
			if (CurrentDistance < BestDistance)
			{
				//UE_LOG(LogTemp, Error, TEXT("Is alive3"))
				BestDistance = CurrentDistance;
				BestPawn = PerceivedActor;
			}
		}
		else {
			//UE_LOG(LogTemp, Error, TEXT("LOG6"))
		}
    }

	return BestPawn;
}
