#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"

void StoreCollisionResponses(const UPrimitiveComponent* Component,
                             TArray<TEnumAsByte<ECollisionResponse>>& OriginalCollisionResponses);

void DisableAndStoreCollisionResponses(UPrimitiveComponent* Component,
                                       TArray<TEnumAsByte<ECollisionResponse>>& OriginalCollisionResponses);

void RestoreCollisionResponses(UPrimitiveComponent* Component,
                               TArray<TEnumAsByte<ECollisionResponse>>& OriginalCollisionResponses);
