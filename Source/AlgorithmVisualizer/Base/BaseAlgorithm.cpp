#include "BaseAlgorithm.h"
#include "GridManager.h"
#include "TileActor.h"

bool FBaseAlgorithm::IsWalkable(int32 X, int32 Y)
{
	ATileActor* TileActor = GridManager->GetTile(X, Y);
	return !(TileActor == nullptr || TileActor->CurrentState == ETileState::Obstacle);
}