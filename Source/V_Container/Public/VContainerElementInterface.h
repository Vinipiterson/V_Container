// Copyright Vinipi, 2025.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "VContainerElementInterface.generated.h"

class UContainerComponent;
// This class does not need to be modified.
UINTERFACE()
class UVContainerElementInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class V_CONTAINER_API IVContainerElementInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	UFUNCTION(BlueprintNativeEvent)
	void AddedToContainer(UContainerComponent* Container);
	UFUNCTION(BlueprintNativeEvent)
	void RemovedFromContainer(UContainerComponent* Container);
};
