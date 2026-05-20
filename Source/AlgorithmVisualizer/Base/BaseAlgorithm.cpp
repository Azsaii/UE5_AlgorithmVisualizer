#include "BaseAlgorithm.h"
#include "GridManager.h"
#include "TileActor.h"

bool FBaseAlgorithm::IsWalkable(int32 X, int32 Y)
{
	ATileActor* TileActor = GridManager->GetTile(X, Y);
	return !(TileActor == nullptr || TileActor->CurrentState == ETileState::Obstacle);
}

ATileActor* FBaseAlgorithm::IsUpdatable(int32 X, int32 Y)
{
	ATileActor* TileActor = GridManager->GetTile(X, Y);
	if (!(TileActor == nullptr || TileActor->CurrentState == ETileState::Obstacle
		|| TileActor->CurrentState == ETileState::Closed)) {
		return TileActor;
	}
	
	return nullptr;
}