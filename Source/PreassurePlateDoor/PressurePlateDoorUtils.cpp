void StoreCollisionResponses(const UPrimitiveComponent* Component,
                             TArray<TEnumAsByte<ECollisionResponse>>& OriginalCollisionResponses) {
	// Clear the OriginalCollisionResponses array before storing the new values.
	OriginalCollisionResponses.Empty();

	for (int32 i = 0; i < ECC_MAX; ++i) {
		OriginalCollisionResponses.Add(Component->GetCollisionResponseToChannel(static_cast<ECollisionChannel>(i)));
	}
}

void DisableAndStoreCollisionResponses(UPrimitiveComponent* Component,
                                       TArray<TEnumAsByte<ECollisionResponse>>& OriginalCollisionResponses) {
	// This is convenience function that disables all collision responses for the given component and stores the original
	// collision responses in a single pass through the array.

	// Clear the OriginalCollisionResponses array before storing the new values.
	OriginalCollisionResponses.Empty();

	for (int32 i = 0; i < ECC_MAX; ++i) {
		OriginalCollisionResponses.Add(Component->GetCollisionResponseToChannel(static_cast<ECollisionChannel>(i)));
		Component->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(i), ECR_Ignore);
	}
}

void RestoreCollisionResponses(UPrimitiveComponent* Component,
                               TArray<TEnumAsByte<ECollisionResponse>>& OriginalCollisionResponses) {
	// Validate that the OriginalCollisionResponses array has the same number of elements as ECC_MAX.
	// This is necessary to prevent crashes if the original collision responses were not stored correctly
	// or if this function is called without storing the original responses first.
	if (OriginalCollisionResponses.Num() == ECC_MAX) {
		for (int32 i = 0; i < ECC_MAX; ++i) {
			Component->SetCollisionResponseToChannel(static_cast<ECollisionChannel>(i),
			                                         OriginalCollisionResponses[i]);
		}
	}
	else {
		UE_LOG(LogTemp, Warning,
		       TEXT(
			       "Original collision responses array is empty or has an incorrect size. Cannot restore collision responses. Actual array size: %d, Expected size: %d"
		       ), OriginalCollisionResponses.Num(), ECC_MAX
		)
	}
}
