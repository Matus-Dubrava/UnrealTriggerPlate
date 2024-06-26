// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "DoorHandler.h"
#include "Components/BoxComponent.h"
#include "TriggerPlate.generated.h"

/**
 * 
 */
UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREASSUREPLATEDOOR_API UTriggerPlate : public UBoxComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTriggerPlate();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	void OnTriggerCallback();
	void OnReleaseCallback() const;

	UFUNCTION(BlueprintCallable)
	void SetDoorHandler(UDoorHandler* NewDoorHandler);

private:
	UPROPERTY(EditAnywhere)
	FName AcceptableActorName;

	UPROPERTY()
	UDoorHandler* DoorHandler;

	AActor* GetAcceptableActor() const;
	FVector GetPlacementLocationForActor(const AActor* Actor, const float ZOffset = 25) const;
};
