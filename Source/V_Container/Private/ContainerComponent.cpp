// Copyright Vinipi, 2025.

#include "ContainerComponent.h"

#include "VContainerElementInterface.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"
#include "Net/Core/PushModel/PushModel.h"

UContainerComponent::UContainerComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bAllowTickOnDedicatedServer = false;

	SetIsReplicatedByDefault(true);
	Elements.SetNum(Slots);
}

void UContainerComponent::BeginPlay()
{
	Super::BeginPlay();

	InitializeDefaultElements();
}

void UContainerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	FDoRepLifetimeParams Parameters;
	Parameters.bIsPushBased = true;

	Parameters.Condition = COND_None;
	Parameters.RepNotifyCondition = REPNOTIFY_Always;
	DOREPLIFETIME_WITH_PARAMS_FAST(ThisClass, Elements, Parameters)
}

void UContainerComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);

	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(ThisClass, Slots))
	{
		Elements.SetNum(Slots);
	}
}

void UContainerComponent::TryAddElement(AActor* Element, int32 InIndex)
{
	if (GetOwnerRole() >= ROLE_Authority)
	{
		TryAddElement_Internal(Element, InIndex);
	}
	else
	{
		ServerTryAddElement(Element, InIndex);
	}
}

void UContainerComponent::TryRemoveElement(AActor* Element, int32 InIndex)
{
	if (GetOwnerRole() >= ROLE_Authority)
	{
		TryRemoveElement_Internal(Element, InIndex);
	}
	else
	{
		ServerTryRemoveElement(Element, InIndex);
	}
}

void UContainerComponent::TryRemoveElementByIndex(int32 InIndex)
{
	if (GetOwnerRole() >= ROLE_Authority)
	{
		TryRemoveElementByIndex_Internal(InIndex);
	}
	else
	{
		ServerTryRemoveElementByIndex(InIndex);
	}
}

void UContainerComponent::TrySwapElement(int32 OldIndex, int32 NewIndex)
{
	if (GetOwnerRole() >= ROLE_Authority)
	{
		TrySwapElement_Internal(OldIndex, NewIndex);
	}
	else
	{
		ServerTrySwapElement(OldIndex, NewIndex);
	}
}

void UContainerComponent::TrySetSlots(int32 NewSlots)
{
	if (GetOwnerRole() >= ROLE_Authority)
	{
		TrySetSlots(NewSlots);
	}
	else
	{
		ServerTrySetSlots(NewSlots);
	}
}

void UContainerComponent::NotifyElementUpdated(AActor* Element)
{
	OnContainerUpdated.Broadcast();
}

bool UContainerComponent::HasElement(const AActor* InElement) const
{
	if (!InElement) return false;
	
	for (const auto& Element : Elements)
	{
		if (Element == InElement)
		{
			return true;
		}
	}

	return false;
}

bool UContainerComponent::HasElementFromClass(TSubclassOf<AActor> InClass) const
{
	if (!InClass) return false;
	
	for (const auto& Element : Elements)
	{
		if (!Element)
			continue;
		
		if (Element->GetClass() == InClass)
		{
			return true;
		}
	}

	return false;
}

AActor* UContainerComponent::GetElement(int32 InIndex) const
{
	if (InIndex == INDEX_NONE) return nullptr;
	if (!Elements.IsValidIndex(InIndex)) return nullptr;

	return Elements[InIndex];
}

AActor* UContainerComponent::GetElementFromClass(TSubclassOf<AActor> InClass) const
{
	if (!InClass) return nullptr;
	
	for (const auto& Element : Elements)
	{
		if (!Element)
			continue;
		
		if (Element->GetClass() == InClass)
		{
			return Element;
		}
	}

	return nullptr;
}

TArray<AActor*> UContainerComponent::GetElements() const
{
	return Elements;
}

int UContainerComponent::GetSlots() const
{
	return Slots;
}

int32 UContainerComponent::GetElementIndex(const AActor* InElement) const
{
	if (!InElement) return INDEX_NONE;
	
	for (int i = 0; i < Elements.Num(); i++)
	{
		if (Elements[i] == InElement)
		{
			return i;
		}
	}

	return INDEX_NONE;
}

// Internal use
bool UContainerComponent::TryAddElement_Internal(AActor* Element, int32 InIndex)
{
	if (!Element) return false;
	if (InIndex != INDEX_NONE && !Elements.IsValidIndex(InIndex)) return false;

	if (InIndex == INDEX_NONE)
	{
		const int Index = FindFirstAvailableIndex();
		if (Index != INDEX_NONE)
		{
			AddElement(Element, Index);
			return true;
		}
	}

	if (IsValid(Elements[InIndex]))
	{
		if (TryRemoveElement_Internal(Elements[InIndex], InIndex))
		{
			AddElement(Element, InIndex);
			return true;
		}
	}
	else
	{
		AddElement(Element, InIndex);
		return true;
	}
	
	
	return false;
}

bool UContainerComponent::TryRemoveElement_Internal(AActor* Element, int32 InIndex)
{
	if (!Element) return false;

	if (!Elements[InIndex]) return false;

	if (Elements[InIndex] == Element)
	{
		RemoveElement(InIndex);

		return true;
	}
	
	return false;
}

bool UContainerComponent::TryRemoveElementByIndex_Internal(int32 InIndex)
{
	if (!Elements[InIndex]) return false;

	RemoveElement(InIndex);

	return true;
}

bool UContainerComponent::TrySwapElement_Internal(int32 OldIndex, int32 NewIndex)
{
	if (OldIndex == INDEX_NONE || NewIndex == INDEX_NONE) return false;
	if (!Elements.IsValidIndex(OldIndex) || !Elements.IsValidIndex(NewIndex)) return false;
	
	AActor* CurrentElement = Elements[OldIndex];
	AActor* NewElement = Elements[NewIndex];

	if (!CurrentElement || !NewElement) return false;
	if (CurrentElement == NewElement) return false;

	if (TryRemoveElement_Internal(CurrentElement, OldIndex))
	{
		return TryAddElement_Internal(NewElement, OldIndex);
	}

	return false;
}

bool UContainerComponent::TrySetSlots_Internal(int32 NewSlots)
{
	if (NewSlots < 1) return false;

	const int RemovedSlots = NewSlots - Slots;
	if (RemovedSlots > 0)
	{
		for (int32 i=Elements.Num()-1; i>Slots; i--)
		{
			RemoveElement(i);
		}
	}
	
	Elements.SetNum(Slots);
	return true;
}

AActor* UContainerComponent::AddElement(AActor* Element, int32 InIndex)
{
	const TArray<AActor*> OldElements = Elements;
	
	Elements[InIndex] = Element;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Elements, this);
	OnRep_Elements(OldElements);

	return Element;
}

void UContainerComponent::RemoveElement(int32 InIndex)
{
	TArray<AActor*> OldElements = Elements;

	AActor* RemovedElement = Elements[InIndex];
	
	Elements[InIndex] = nullptr;
	MARK_PROPERTY_DIRTY_FROM_NAME(ThisClass, Elements, this);
	OnRep_Elements(OldElements);
}

void UContainerComponent::ServerTryAddElement_Implementation(AActor* Element, int32 InIndex)
{
	TryAddElement(Element, InIndex);
}

void UContainerComponent::ServerTryRemoveElement_Implementation(AActor* Element, int32 InIndex)
{
	TryRemoveElement(Element, InIndex);
}

void UContainerComponent::ServerTryRemoveElementByIndex_Implementation(int32 InIndex)
{
	TryRemoveElementByIndex(InIndex);
}

void UContainerComponent::ServerTrySwapElement_Implementation(int32 OldIndex, int32 NewIndex)
{
	TrySwapElement(OldIndex, NewIndex);
}

void UContainerComponent::ServerTrySetSlots_Implementation(int32 NewSlots)
{
}

void UContainerComponent::OnRep_Elements(TArray<AActor*> OldElements)
{
	for (auto Element : Elements)
	{
		if (!OldElements.Contains(Element) && Element)
			IVContainerElementInterface::Execute_AddedToContainer(Element, this);
	}

	for (auto Element : OldElements)
	{
		if (!Elements.Contains(Element) && Element)
			IVContainerElementInterface::Execute_RemovedFromContainer(Element, this);
	}
}

void UContainerComponent::OnRep_Slots(int32 OldSlots)
{

}

int UContainerComponent::FindFirstAvailableIndex()
{
	for (int32 i = 0; i < Elements.Num(); i++)
	{
		if (Elements[i])
			continue;

		return i;
	}
	return INDEX_NONE;
}

void UContainerComponent::InitializeDefaultElements()
{
	if (GetOwnerRole() != ROLE_Authority)
		return;

	for (auto Class : DefaultElements)
	{
		AActor* Element = GetWorld()->SpawnActor(Class);
		if (!TryAddElement_Internal(Element, -1))
			Element->Destroy();
	}
}
