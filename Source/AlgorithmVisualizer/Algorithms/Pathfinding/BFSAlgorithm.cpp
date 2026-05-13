#include "BFSAlgorithm.h"
#include "GridManager.h"
#include "TileActor.h"

bool FBFSAlgorithm::CheckState()
{
	if (GridManager->StartTile == nullptr) {
		GridManager->ControlPanel->UpdateStatusText(TEXT("Select a starting point!"));
		return false;
	}
	if (GridManager->EndTile == nullptr) {
		GridManager->ControlPanel->UpdateStatusText(TEXT("Select a goal point!"));
		return false;
	}
	if (bUnreachable == true) {
		GridManager->ControlPanel->UpdateStatusText(TEXT("Target Unreachable! press C to reset"));
		return false;
	}
	else if (bFindEnd == true) {
		GridManager->ControlPanel->UpdateStatusText(TEXT("Path found already! press C to reset"));
		return false;
	}

	return true;
}

bool FBFSAlgorithm::StepOnce()
{
	if (CheckState() == false) return false;
	else if (OpenQueue.IsEmpty()) {
		OpenQueue.Enqueue(GridManager->StartTile);
	}
	
	int32 width = GridManager->CurrentWidth;
	int32 height = GridManager->CurrentHeight;

	ATileActor* Tile = nullptr;
	bool bSuccess = OpenQueue.Dequeue(Tile);
	if (bSuccess == true) {
		GridManager->DrawPath(Tile);

		if (Tile != GridManager->StartTile && Tile != GridManager->EndTile) {
			Tile->SetState(ETileState::Closed);
		}
		else Tile->SetState(ETileState::Closed, false);

		if (Tile == GridManager->EndTile) {
			bFindEnd = true;
			GridManager->ControlPanel->UpdateStatusText(TEXT("Path found!"));
			return false;
		}

		int32 cx = Tile->GridX;
		int32 cy = Tile->GridY;

		FString Msg = FString::Printf(TEXT("Open point (%d, %d)"), cx, cy);
		GridManager->ControlPanel->UpdateStatusText(Msg);

		bool check = true;
		for (int8 i = 0; i < 4; i++) {
			int32 nx = cx + dx[i];
			int32 ny = cy + dy[i];
			if(nx < 0 || nx >= width || ny < 0 || ny >= height){}
			else {
				ATileActor* OpenTile = GridManager->GetTile(nx, ny);
				if (OpenTile->CurrentState == ETileState::Unvisited ||
					OpenTile->CurrentState == ETileState::Goal) {
					OpenTile->PathParent = Tile;
					OpenQueue.Enqueue(OpenTile);
					check = false;

					if (OpenTile != GridManager->EndTile) {
						OpenTile->SetState(ETileState::Open);
					}
					else OpenTile->SetState(ETileState::Open, false);
				}
			}
		}

		if (OpenQueue.IsEmpty()) {
			bUnreachable = true;
			GridManager->ControlPanel->UpdateStatusText(TEXT("Target Unreachable"));
			return false;
		}
	}

	return true;
}

void FBFSAlgorithm::StepAll()
{
	while (StepOnce());
}

void FBFSAlgorithm::ClearPath()
{
	OpenQueue.Empty();
	GridManager->ResetTileState();
	GridManager->ControlPanel->UpdateStatusText(TEXT("Path cleared!"));
	bFindEnd = false;
	bUnreachable = false;
}
