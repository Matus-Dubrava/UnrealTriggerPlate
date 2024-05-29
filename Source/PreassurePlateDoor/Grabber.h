// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "TriggerPlate.h"

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "PhysicsEngine/PhysicsHandleComponent.h"
#include "Grabber.generated.h"

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PREASSUREPLATEDOOR_API UGrabber : public USceneComponent {
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UGrabber();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,
	                           FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	void Grab();

	UFUNCTION(BlueprintCallable)
	void Release();

	// Used to map Grab and Release to a single input key
	UFUNCTION(BlueprintCallable)
	void GrabOrRelease();

private:
	TArray<TEnumAsByte<ECollisionResponse>> OriginalCollisionResponses;
	bool bIsObjectGrabbed = false;

	UPROPERTY(EditAnywhere)
	TArray<UTriggerPlate*> TriggerPlates;

	UPROPERTY(EditAnywhere)
	float GrabDistance = 400;

	UPROPERTY(EditAnywhere)
	float GrabRadius = 100;

	UPROPERTY(EditAnywhere)
	float HoldDistance = 100;

	UPROPERTY(EditAnywhere)
	bool ShouldDrawDebugLines = false;

	UPhysicsHandleComponent* GetPhysicsHandle() const;
	bool PerformConeTrace(const FVector& Start, const FVector& ForwardVector, const float ConeAngle,
	                      const int32 NumTraces,
	                      FHitResult& OutHitResult) const;
	static void UpdateCollisionResponseOnGrab(
		UPrimitiveComponent* Component, TArray<TEnumAsByte<ECollisionResponse>>& OriginalCollisionResponses
	);
	static void UpdateCollisionResponseOnRelease(UPrimitiveComponent* Component,
	                                             TArray<TEnumAsByte<ECollisionResponse>>& OriginalCollisionResponses);
	void UpdateObjectLocationOnRelease(UPrimitiveComponent* Component, const float ForwardDistance) const;
};
