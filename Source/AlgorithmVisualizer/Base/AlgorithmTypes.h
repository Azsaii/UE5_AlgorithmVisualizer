#pragma once
#include "CoreMinimal.h"
#include "AlgorithmTypes.generated.h"

UENUM(BlueprintType)
enum class EAlgorithmType : uint8
{
    BFS,
    DFS,
    ASTAR,
    JPS,

    DIJKSTRA,
    BELLMAN_FORD,
    KRUSKAL,
    PRIM,
};