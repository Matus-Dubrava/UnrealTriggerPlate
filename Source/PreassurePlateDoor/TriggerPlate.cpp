// Fill out your copyright notice in the Description page of Project Settings.

#include "TriggerPlate.h"
#include "Tags.h"

// Sets default values for this component's properties
UTriggerPlate::UTriggerPlate() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UTriggerPlate::BeginPlay() {
	Super::BeginPlay();
}


// Called every frame
void UTriggerPlate::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// PerformTriggerAction();
}

void UTriggerPlate::PerformTriggerAction() {
	if (AActor* AcceptableActor = GetAcceptableActor()) {
		AcceptableActor->SetActorLocationAndRotation(GetPlacementLocationForActor(AcceptableActor),
		                                             GetOwner()->GetActorRotation());

		if (UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(AcceptableActor->GetRootComponent())) {
			Component->SetSimulatePhysics(false);
			Component->AttachToComponent(this, FAttachmentTransformRules::KeepWorldTransform);
		}
	}
}

AActor* UTriggerPlate::GetAcceptableActor() const {
	AActor* AcceptableActor = nullptr;
	TArray<AActor*> OverlappingActors;
	GetOverlappingActors(OverlappingActors);

	for (const auto Actor : OverlappingActors) {
		if (Actor->Tags.Contains(AcceptableActorName) && !Actor->Tags.Contains(GRABBED_TAG)) {
			AcceptableActor = Actor;
		}
	}

	return AcceptableActor;
}

FVector UTriggerPlate::GetPlacementLocationForActor(const AActor* Actor, const float ZOffset) const {
	const UBoxComponent* TriggerPlateBox = Cast<UBoxComponent>(this);
	const FVector TriggerPlateCenter = TriggerPlateBox->Bounds.GetBox().GetCenter();

	FVector ActorOrigin;
	FVector ActorBoxExtent;
	Actor->GetActorBounds(false, ActorOrigin, ActorBoxExtent);

	FVector PlacementLocation = TriggerPlateCenter - FVector(ActorBoxExtent.X, ActorBoxExtent.Y, 0);
	PlacementLocation.Z = ZOffset;

	return PlacementLocation;
}
