// Fill out your copyright notice in the Description page of Project Settings.

#include "Grabber.h"
#include "PressurePlateDoorUtils.h"
#include "Grabbable.h"
#include "Tags.h"

#include "Engine/World.h"

// Sets default values for this component's properties
UGrabber::UGrabber() {
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

// Called when the game starts
void UGrabber::BeginPlay() {
	Super::BeginPlay();
}

// Called every frame
void UGrabber::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) {
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("Physics handle not found"));
		return;
	}

	// Update Physics Handle location and rotation to match the player's hand.
	if (const USkeletalMeshComponent* SkeletalMesh = GetOwner()->FindComponentByClass<USkeletalMeshComponent>()) {
		const FVector HandLocation = SkeletalMesh->GetSocketLocation("hand_r");
		const FRotator HandRotation = SkeletalMesh->GetSocketRotation("hand_r");
		PhysicsHandle->SetTargetLocationAndRotation(HandLocation, HandRotation);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("No skeletal mesh found"));
	}
}

void UGrabber::GrabOrRelease() {
	if (bIsObjectGrabbed) {
		Release();
	}
	else {
		Grab();
	}
}


void UGrabber::Grab() {
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("Physics handle not found"));
		return;
	}

	const FVector Start = GetOwner()->GetActorLocation();
	FHitResult HitResult;

	// Perform Cone Trace and grab the closest grabbable object if any. 
	if (PerformConeTrace(Start, GetOwner()->GetActorForwardVector(), 30, 100, HitResult)) {
		if (!HitResult.GetActor()->Tags.Contains(GRABBED_TAG)) {
			if (UPrimitiveComponent* Component = Cast<UPrimitiveComponent>(HitResult.GetActor()->GetRootComponent());
				Component != nullptr) {
				Component->SetSimulatePhysics(true);
			}

			// If there is a trigger plate attached to the grabbed object, trigger its release callback.
			if (UGrabbable* GrabbableComponent = HitResult.GetActor()->FindComponentByClass<UGrabbable>()) {
				if (UTriggerPlate* TriggerPlate = GrabbableComponent->GetTriggerPlate()) {
					TriggerPlate->OnReleaseCallback();
				}
			}

			HitResult.GetActor()->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
			HitResult.GetActor()->Tags.Add(GRABBED_TAG);

			UPrimitiveComponent* HitComponent = HitResult.GetComponent();
			UpdateCollisionResponseOnGrab(HitComponent, OriginalCollisionResponses);
			PhysicsHandle->GrabComponentAtLocationWithRotation(
				HitComponent,
				NAME_None,
				HitResult.ImpactPoint,
				GetComponentRotation()
			);
			bIsObjectGrabbed = true;
		}
	}
}

void UGrabber::Release() {
	UPhysicsHandleComponent* PhysicsHandle = GetPhysicsHandle();
	if (!PhysicsHandle) {
		UE_LOG(LogTemp, Error, TEXT("Physics handle not found"));
		return;
	}

	if (UPrimitiveComponent* GrabbedComponent = PhysicsHandle->GetGrabbedComponent(); GrabbedComponent) {
		GrabbedComponent->GetOwner()->Tags.Remove(GRABBED_TAG);
		PhysicsHandle->ReleaseComponent();
		bIsObjectGrabbed = false;
		UpdateObjectLocationOnRelease(GrabbedComponent, HoldDistance);
		UpdateCollisionResponseOnRelease(GrabbedComponent, OriginalCollisionResponses);

		// Trigger the trigger plate action if the grabbed object has one.
		if (UGrabbable* Grabbable = Cast<UGrabbable>(
			GrabbedComponent->GetOwner()->GetComponentByClass(UGrabbable::StaticClass()))) {
			if (UTriggerPlate* TriggerPlate = Grabbable->GetTriggerPlate()) {
				TriggerPlate->OnTriggerCallback();
			}
		}
	}
}

UPhysicsHandleComponent* UGrabber::GetPhysicsHandle() const {
	return GetOwner()->FindComponentByClass<UPhysicsHandleComponent>();
}

bool UGrabber::PerformConeTrace(const FVector& Start, const FVector& ForwardVector, const float ConeAngle,
                                const int32 NumTraces,
                                FHitResult& OutHitResult) const {
	bool OverallHasHit = false;

	for (int32 i = 0; i < NumTraces; ++i) {
		// Randomize the direction of the trace.
		const float RandomYAngle = FMath::RandRange(-ConeAngle / 2, ConeAngle / 2);
		const float RandomZAngle = FMath::RandRange(-ConeAngle / 2, ConeAngle / 2);
		FRotator RandomRotator = FRotator(0, RandomYAngle, RandomZAngle);
		FVector TraceDirection = RandomRotator.RotateVector(ForwardVector);

		FVector End = Start + TraceDirection * GrabDistance;
		FHitResult HitResult;
		const bool CurrentHasHit = GetWorld()->SweepSingleByChannel(HitResult, Start, End, FQuat::Identity,
		                                                            ECC_GameTraceChannel1,
		                                                            FCollisionShape::MakeSphere(GrabRadius));
		if (ShouldDrawDebugLines) {
			DrawDebugLine(GetWorld(), Start, End, FColor(255, 128, 128), false, .5);
		}


		// If no object has been hit yet, or the current hit is closer than the previous one, update the hit result.
		// Only objects with the Grabbable component can be grabbed.
		if (CurrentHasHit && IsGrabbable(HitResult.GetActor()) && (!OverallHasHit || HitResult.Distance < OutHitResult.
			Distance)) {
			OutHitResult = HitResult;
			OverallHasHit = true;
		}
	}

	return OverallHasHit;
}

void UGrabber::UpdateCollisionResponseOnGrab(UPrimitiveComponent* Component,
                                             TArray<TEnumAsByte<ECollisionResponse>>& OriginalCollisionResponses) {
	// Once grabbed, disable all collisions for the grabbed object. 

	if (Component) {
		DisableAndStoreCollisionResponses(Component, OriginalCollisionResponses);
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Calling Grab without a valid component"));
	}
}

void UGrabber::UpdateCollisionResponseOnRelease(UPrimitiveComponent* Component,
                                                TArray<TEnumAsByte<ECollisionResponse>>& OriginalCollisionResponses) {
	// Restore original collision responses on release.

	if (Component) {
		RestoreCollisionResponses(Component, OriginalCollisionResponses);
		OriginalCollisionResponses.Empty();
	}
	else {
		UE_LOG(LogTemp, Error, TEXT("Calling Release without a valid component"));
	}
}

void UGrabber::UpdateObjectLocationOnRelease(UPrimitiveComponent* Component, const float ForwardDistance) const {
	// Once released, move the object in front of the player and clear all of its momentum.

	const AActor* PlayerActor = GetOwner();
	const FVector PlayerLocation = PlayerActor->GetActorLocation();
	const FRotator PlayerRotation = PlayerActor->GetActorRotation();
	const FVector ReleasePosition = PlayerLocation + PlayerRotation.Vector() * ForwardDistance;

	Component->GetOwner()->SetActorLocation(ReleasePosition);
	Component->GetOwner()->SetActorRotation(PlayerRotation);
	Component->SetPhysicsLinearVelocity(FVector::ZeroVector);
	Component->SetPhysicsAngularVelocityInDegrees(FVector::ZeroVector);
}

bool UGrabber::IsGrabbable(const AActor* Actor) {
	if (Actor->GetComponentByClass(UGrabbable::StaticClass())) {
		return true;
	}
	return false;
}
