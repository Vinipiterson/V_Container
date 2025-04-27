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

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;

	//~Container
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", ReplicatedUsing="OnRep_Slots")
	int32 Slots = 5;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State", ReplicatedUsing="OnRep_Elements")
	TArray<AActor*> Elements;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "State")
	TArray<TSubclassOf<AActor>> DefaultElements;

public:
	UPROPERTY(BlueprintAssignable)
	FContainerUpdatedSignature OnContainerUpdated;

public:
	UFUNCTION(BlueprintCallable, Category = "Container")
	void TryAddElement(AActor* Element, int32 InIndex = -1);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void TryRemoveElement(AActor* Element, int32 InIndex);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void TryRemoveElementByIndex(int32 InIndex);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void TrySwapElement(int32 OldIndex, int32 NewIndex);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void TrySetSlots(int32 NewSlots);
	UFUNCTION(BlueprintCallable, Category = "Container")
	void NotifyElementUpdated(AActor* Element);
	
	UFUNCTION(BlueprintPure, Category = "Container")
	bool HasElement(const AActor* InElement) const;
	UFUNCTION(BlueprintPure, Category = "Container")
	bool HasElementFromClass(TSubclassOf<AActor> InClass) const;
	UFUNCTION(BlueprintPure, Category = "Container")
	AActor* GetElement(int32 InIndex) const;
	
	UFUNCTION(BlueprintPure, Category = "Container")
	AActor* GetElementFromClass(TSubclassOf<AActor> InClass) const;
	UFUNCTION(BlueprintPure, Category = "Container")
	TArray<AActor*> GetElements() const;
	UFUNCTION(BlueprintPure, Category = "Container")
	int GetSlots() const;
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
	/* Server only */
	virtual bool TrySetSlots_Internal(int32 NewSlots);

protected:
	AActor* AddElement(AActor* Element, int32 InIndex);
	virtual void RemoveElement(int32 InIndex);
	
	UFUNCTION(Server, Reliable)
	void ServerTryAddElement(AActor* Element, int32 InIndex);
	UFUNCTION(Server, Reliable)
	void ServerTryRemoveElement(AActor* Element, int32 InIndex);
	UFUNCTION(Server, Reliable)
	void ServerTryRemoveElementByIndex(int32 InIndex);
	UFUNCTION(Server, Reliable)
	void ServerTrySwapElement(int32 OldIndex, int32 NewIndex);
	UFUNCTION(Server, Reliable)
	void ServerTrySetSlots(int32 NewSlots);
	
	UFUNCTION()
	void OnRep_Elements(TArray<AActor*> OldElements);
	UFUNCTION()
	void OnRep_Slots(int32 OldSlots);

	int FindFirstAvailableIndex();

	void InitializeDefaultElements();
	//~Container
};
