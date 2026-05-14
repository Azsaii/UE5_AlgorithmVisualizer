#pragma once
#include "CoreMinimal.h"
#include "ParentDirection.generated.h"

UENUM(BlueprintType)
enum class  EParentDirection : uint8
{
	UU,
	UR,
	RR,
	DR,
	DD,
	DL,
	LL,
	UL
};