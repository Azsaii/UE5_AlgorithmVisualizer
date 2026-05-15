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

void FBFSAlgorithm::StepOnce()
{
	if (CheckState() == false) return;
	else if (OpenQueue.IsEmpty()) {
		OpenQueue.Enqueue(GridManager->StartTile);
	}
	
	int32 width = GridManager->CurrentWidth;
	int32 height = GridManager->CurrentHeight;

	// 이전 경로 제거
	if (CurrentTile) GridManager->DrawPath(CurrentTile, false);

	bool bSuccess = OpenQueue.Dequeue(CurrentTile);
	if (bSuccess == true) {
		
		// 현재 경로 그리기
		GridManager->DrawPath(CurrentTile, true);

		if (CurrentTile != GridManager->StartTile && CurrentTile != GridManager->EndTile) {
			CurrentTile->SetStateAndColor(ETileState::Closed);
		}
		else CurrentTile->CurrentState = ETileState::Closed;

		if (CurrentTile == GridManager->EndTile) {
			bFindEnd = true;
			GridManager->ControlPanel->UpdateStatusText(TEXT("Path found!"));
			return;
		}

		int32 cx = CurrentTile->GridX;
		int32 cy = CurrentTile->GridY;

		FString Msg = FString::Printf(TEXT("Open point (%d, %d)"), cx, cy);
		GridManager->ControlPanel->UpdateStatusText(Msg);

		for (int8 i = 0; i < DIRSIZE; i++) {
			int32 nx = cx + dx[i];
			int32 ny = cy + dy[i];
			if(nx < 0 || nx >= width || ny < 0 || ny >= height){}
			else {
				ATileActor* OpenTile = GridManager->GetTile(nx, ny);
				if (OpenTile->CurrentState == ETileState::Unvisited ||
					OpenTile->CurrentState == ETileState::Goal) {
					OpenTile->PathParent = CurrentTile;
					OpenTile->PathParentDirection = static_cast<EParentDirection>(i);
					OpenQueue.Enqueue(OpenTile);

					if (OpenTile != GridManager->EndTile) {
						OpenTile->SetStateAndColor(ETileState::Open);
					}
					else OpenTile->CurrentState = ETileState::Open;
				}
			}
		}

		if (OpenQueue.IsEmpty()) {
			bUnreachable = true;
			GridManager->ControlPanel->UpdateStatusText(TEXT("Target Unreachable"));
			return;
		}
	}
}

void FBFSAlgorithm::StepAll()
{
	if (CheckState() == false) return;
	else if (OpenQueue.IsEmpty()) {
		OpenQueue.Enqueue(GridManager->StartTile);
	}

	int32 width = GridManager->CurrentWidth;
	int32 height = GridManager->CurrentHeight;

	while (!OpenQueue.IsEmpty()) {
		OpenQueue.Dequeue(CurrentTile);
		CurrentTile->CurrentState = ETileState::Closed;

		if (CurrentTile == GridManager->EndTile) {
			bFindEnd = true;
			GridManager->ControlPanel->UpdateStatusText(TEXT("Path found!"));
			GridManager->DrawPath(GridManager->EndTile, true); // 경로 그리기
			break;
		}

		int32 cx = CurrentTile->GridX;
		int32 cy = CurrentTile->GridY;

		for (int8 i = 0; i < DIRSIZE; i++) {
			int32 nx = cx + dx[i];
			int32 ny = cy + dy[i];
			if (nx < 0 || nx >= width || ny < 0 || ny >= height) {}
			else {
				ATileActor* OpenTile = GridManager->GetTile(nx, ny);
				if (OpenTile->CurrentState == ETileState::Unvisited ||
					OpenTile->CurrentState == ETileState::Goal) {
					OpenTile->PathParent = CurrentTile;
					OpenTile->PathParentDirection = static_cast<EParentDirection>(i);
					OpenQueue.Enqueue(OpenTile);
					OpenTile->CurrentState = ETileState::Open;
				}
			}
		}

		if (OpenQueue.IsEmpty()) {
			bUnreachable = true;
			GridManager->ControlPanel->UpdateStatusText(TEXT("Target Unreachable"));
			return;
		}
	}

	GridManager->UpdateAllTileColor();
}

void FBFSAlgorithm::ClearPath()
{
	// 이전 경로 제거
	if (CurrentTile) GridManager->DrawPath(CurrentTile, false);

	OpenQueue.Empty();
	GridManager->ResetTileState();
	GridManager->ControlPanel->UpdateStatusText(TEXT("Path cleared!"));
	bFindEnd = false;
	bUnreachable = false;
}
