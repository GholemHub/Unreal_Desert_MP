// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Character/BlasterCharacter.h"
#include "AICharacter.generated.h"

/**
 * 
 */


UCLASS()
class BLASTER_API AAICharacter : public ABlasterCharacter
{
	GENERATED_BODY()
public:
	
	AAICharacter(const FObjectInitializer& ObjInit);

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = AI)
	class UBehaviorTree* BechaviorTreeAsset;
};
