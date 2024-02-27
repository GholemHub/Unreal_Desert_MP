// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "BlasterAIController.generated.h"

/**
 * 
 */

class UBlasterAIPerceptionComponent;
UCLASS()
class BLASTER_API ABlasterAIController : public AAIController
{
	GENERATED_BODY()

public:
	ABlasterAIController();

protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	UBlasterAIPerceptionComponent* BlasterPerceptionComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	FName FocusInKeyName = "EnemyActor";

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
private:
	AActor* GetFocusOnActor() const;

};
