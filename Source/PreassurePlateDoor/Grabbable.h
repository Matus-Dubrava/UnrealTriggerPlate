// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TriggerPlate.h"
#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "Grabbable.generated.h"


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREASSUREPLATEDOOR_API UGrabbable : public USceneComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGrabbable();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void SetTriggerPlate(UTriggerPlate* NewTriggerPlate);

	UTriggerPlate* GetTriggerPlate() const;

private:
	UPROPERTY()
	UTriggerPlate* TriggerPlate;
};
