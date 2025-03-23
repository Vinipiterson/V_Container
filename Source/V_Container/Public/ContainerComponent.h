// Copyright Vinipi, 2025.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ContainerComponent.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FContainerUpdatedSignature);

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class V_CONTAINER_API UContainerComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UContainerComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	//~Container
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", Replicated)
	TArray<AActor*> Elements;

public:
	UPROPERTY(BlueprintAssignable)
	FContainerUpdatedSignature OnContainerUpdated;

public:
	UFUNCTION(BlueprintCallable, Category = "Container")
	void TryAddElement(AActor* Element, int32 InIndex);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void TryRemoveElement(AActor* Element, int32 InIndex);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void TryRemoveElementByIndex(int32 InIndex);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void TrySwapElement(int32 OldIndex, int32 NewIndex);
	
	UFUNCTION(BlueprintPure, Category = "Container")
	bool HasElement(const AActor* InElement) const;
	UFUNCTION(BlueprintPure, Category = "Container")
	AActor* GetElement(int32 InIndex) const;
	UFUNCTION(BlueprintPure, Category = "Container")
	TArray<AActor*> GetElements() const;
	UFUNCTION(BlueprintPure, Category = "Container")
	int32 GetElementIndex(const AActor* InElement) const;

protected:
	/* Server only */
	virtual bool TryAddElement_Internal(AActor* Element, int32 InIndex);
	/* Server only */
	virtual bool TryRemoveElement_Internal(AActor* Element, int32 InIndex);
	/* Server only */
	virtual bool TryRemoveElementByIndex_Internal(int32 InIndex);
	/* Server only */
	virtual bool TrySwapElement_Internal(int32 OldIndex, int32 NewIndex);

protected:
	AActor* AddElement(AActor* Element, int32 InIndex);
	void RemoveElement(int32 InIndex);
	
	UFUNCTION(Server, Reliable)
	void ServerTryAddElement(AActor* Element, int32 InIndex);
	UFUNCTION(Server, Reliable)
	void ServerTryRemoveElement(AActor* Element, int32 InIndex);
	UFUNCTION(Server, Reliable)
	void ServerTryRemoveElementByIndex(int32 InIndex);
	UFUNCTION(Server, Reliable)
	void ServerTrySwapElement(int32 OldIndex, int32 NewIndex);
	
	UFUNCTION()
	void OnRep_Elements(TArray<AActor*> OldElements);

	int FindFirstAvailableIndex();
	//~Container
};
