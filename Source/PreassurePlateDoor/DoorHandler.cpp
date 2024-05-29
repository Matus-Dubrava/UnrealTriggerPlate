// Fill out your copyright notice in the Description page of Project Settings.


#include "DoorHandler.h"
#include "PressurePlateDoorUtils.h"

// Sets default values for this component's properties
UDoorHandler::UDoorHandler() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UDoorHandler::BeginPlay() {
	Super::BeginPlay();

	if (const AActor* Owner = GetOwner()) {
		if (const UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(Owner->GetRootComponent())) {
			StoreCollisionResponses(Component, OriginalCollisionResponses);
		}
	}
}


// Called every frame
void UDoorHandler::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UDoorHandler::Open() {
	IsClosed = false;
	if (const AActor* Owner = GetOwner()) {
		if (UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(Owner->GetRootComponent())) {
			Component->SetVisibility(false, true);
			DisableAndStoreCollisionResponses(Component, OriginalCollisionResponses);
		}
		else {
			UE_LOG(LogTemp, Error,
			       TEXT("Error while opening door: Owner's root component is not a primitive component"));
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error while opening door: Owner is nullptr"));
	}
}

void UDoorHandler::Close() {
	IsClosed = true;
	if (const AActor* Owner = GetOwner()) {
		if (UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(Owner->GetRootComponent())) {
			Component->SetVisibility(true, true);
			RestoreCollisionResponses(Component, OriginalCollisionResponses);
		}
		else {
			UE_LOG(LogTemp, Error,
			       TEXT("Error while closing door: Owner's root component is not a primitive component"));
		}
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Error while closeing door: Owner is nullptr"));
	}
}
