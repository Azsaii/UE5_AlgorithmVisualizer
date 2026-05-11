#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NodeActor.generated.h"

UENUM()
enum class ENodeState : uint8 
{
	Unvisited,
	Start,
	Goal,
	Open,       // 탐색 후보 (frontier)
	Closed,     // 탐색 완료
	Path,       // 최종 경로
	Weighted    // Dijkstra/Bellman-Ford 비용 표시
};

UCLASS()
class ALGORITHMVISUALIZER_API ANodeActor : public AActor
{
	GENERATED_BODY()
	
public:	
	ANodeActor();

private:

	ENodeState CurrentState = ENodeState::Unvisited;
};
