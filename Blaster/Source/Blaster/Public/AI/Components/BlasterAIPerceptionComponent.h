// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Perception/AIPerceptionComponent.h"
#include "BlasterAIPerceptionComponent.generated.h"

/**
 * 
 */
UCLASS()
class BLASTER_API UBlasterAIPerceptionComponent : public UAIPerceptionComponent
{
	GENERATED_BODY()
public:
	AActor* GetClossesEnemy() const;
	
};
