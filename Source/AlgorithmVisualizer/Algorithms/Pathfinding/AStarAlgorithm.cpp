#include "AStarAlgorithm.h"
#include "GridManager.h"


bool FAStarAlgorithm::CheckState()
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

void FAStarAlgorithm::StepOnce()
{
	if (CheckState() == false) return;
	else if (OpenQueue.IsEmpty()) {
		int32 distx = abs(GridManager->EndTile->GridX - GridManager->StartTile->GridX);
		int32 disty = abs(GridManager->EndTile->GridY - GridManager->StartTile->GridY);
		int32 remain = (distx + disty) * STRAIGHT_DISTANCE;
		if(FindMethod == EUCLID) remain = sqrt(distx * distx + disty * disty)* STRAIGHT_DISTANCE;
		GridManager->StartTile->OpenNode.UpdateOpenNode(
			true, 0, remain);
		OpenQueue.HeapPush(GridManager->StartTile, FTilePredicate());
	}

	int32 width = GridManager->CurrentWidth;
	int32 height = GridManager->CurrentHeight;

	int32 ex = GridManager->EndTile->GridX;
	int32 ey = GridManager->EndTile->GridY;
	
	// 이전 경로 제거
	if (CurrentTile) GridManager->DrawPath(CurrentTile, false);

	// 가중치 가장 작은 노드 뽑기
	OpenQueue.HeapPop(CurrentTile, FTilePredicate());

	// 현재 경로 그리기
	GridManager->DrawPath(CurrentTile, true);

	// 상태 표시
	if (CurrentTile != GridManager->StartTile && CurrentTile != GridManager->EndTile) {
		CurrentTile->SetStateAndColor(ETileState::Closed);
	}
	else CurrentTile->CurrentState = ETileState::Closed;

	// 목표지점 도달 시 종료
	if (CurrentTile == GridManager->EndTile) {
		bFindEnd = true;
		GridManager->ControlPanel->UpdateStatusText(TEXT("Path found!"));
		return;
	}

	int32 cx = CurrentTile->GridX;
	int32 cy = CurrentTile->GridY;

	FString Msg = FString::Printf(TEXT("Open point (%d, %d)"), cx, cy);
	GridManager->ControlPanel->UpdateStatusText(Msg);

	// 인접 노드 탐색
	for (int8 i = 0; i < DIRSIZE; i++) {
		int32 nx = cx + dx[i];
		int32 ny = cy + dy[i];
		if (nx < 0 || nx >= width || ny < 0 || ny >= height) {}
		else {
			ATileActor* OpenTile = GridManager->GetTile(nx, ny);

			// 목적지까지 거리
			int32 rx = abs(ex - nx);
			int32 ry = abs(ey - ny);

			// 기본적으로 직선일 때 맨해튼방식으로 계산해서 초기화.
			int32 nextMoveCount = STRAIGHT_DISTANCE;
			int32 nextRemainDistance = (rx + ry) * STRAIGHT_DISTANCE;

			// i가 홀수면 대각선.
			// 거리 계산시 MANHATTAN 은 2칸으로, EUCLID는 1.4칸으로 계산.
			// 목적지 까지 거리 계산 시 EUCLID는 피타고라스 정리로 계산.
			if (i & 1) {
				switch (FindMethod) {
				case MANHATTAN: {
					nextMoveCount = STRAIGHT_DISTANCE * 2;
					break;
				}
				case EUCLID: {
					nextMoveCount = DIAGONAL_DISTANCE;
					double tmp = sqrt(rx * rx + ry * ry) * STRAIGHT_DISTANCE;
					nextRemainDistance = static_cast<int32>(tmp);
					break;
				}
				}
			}
			nextMoveCount += CurrentTile->OpenNode.MoveCount;

			int32 nextWeight = nextMoveCount + nextRemainDistance;
			bool check = false;

			// 1) 첫 탐색: 시 오픈 리스트에 넣는다.
			if (OpenTile->OpenNode.bVisited == false) {
				check = true;
			}
			// 2) 재 탐색: 가중치 비교해서 이번 경로가 더 좋으면 업데이트한다.
			else if(OpenTile->OpenNode.Weight > nextWeight){
				// pop -> push 로 가중치 기준 재정렬.
				OpenQueue.HeapPop(OpenTile, FTilePredicate());
				check = true;
			}

			if (check) {
				// 노드 업데이트
				OpenTile->OpenNode.UpdateOpenNode(true, nextMoveCount, nextRemainDistance);

				// 부모 변경
				OpenTile->PathParent = CurrentTile;

				// 오픈 리스트에 새로 넣기
				OpenQueue.HeapPush(OpenTile, FTilePredicate());

				// 경로 선 방향 얻기
				OpenTile->PathParentDirection = static_cast<EParentDirection>(i);

				// 상태/색상 업데이트
				if (OpenTile != GridManager->StartTile && OpenTile != GridManager->EndTile) {
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

void FAStarAlgorithm::StepAll()
{
	if (CheckState() == false) return;
	else if (OpenQueue.IsEmpty()) {
		int32 distx = abs(GridManager->EndTile->GridX - GridManager->StartTile->GridX);
		int32 disty = abs(GridManager->EndTile->GridY - GridManager->StartTile->GridY);
		int32 remain = (distx + disty) * STRAIGHT_DISTANCE;
		if (FindMethod == EUCLID) remain = sqrt(distx * distx + disty * disty) * STRAIGHT_DISTANCE;
		GridManager->StartTile->OpenNode.UpdateOpenNode(
			true, 0, remain);
		OpenQueue.HeapPush(GridManager->StartTile, FTilePredicate());
	}

	int32 width = GridManager->CurrentWidth;
	int32 height = GridManager->CurrentHeight;

	int32 ex = GridManager->EndTile->GridX;
	int32 ey = GridManager->EndTile->GridY;

	while (!OpenQueue.IsEmpty())
	{
		// 가중치 가장 작은 노드 뽑기
		OpenQueue.HeapPop(CurrentTile, FTilePredicate());

		// 상태 표시
		CurrentTile->CurrentState = ETileState::Closed;

		// 목표지점 도달 시 종료
		if (CurrentTile == GridManager->EndTile) {
			bFindEnd = true;
			GridManager->ControlPanel->UpdateStatusText(TEXT("Path found!"));
			GridManager->DrawPath(GridManager->EndTile, true); // 경로 그리기
			break;
		}

		int32 cx = CurrentTile->GridX;
		int32 cy = CurrentTile->GridY;

		// 인접 노드 탐색
		for (int8 i = 0; i < DIRSIZE; i++) {
			int32 nx = cx + dx[i];
			int32 ny = cy + dy[i];
			if (nx < 0 || nx >= width || ny < 0 || ny >= height) {}
			else {
				ATileActor* OpenTile = GridManager->GetTile(nx, ny);

				// 목적지까지 거리
				int32 rx = abs(ex - nx);
				int32 ry = abs(ey - ny);

				// 기본적으로 직선일 때 맨해튼방식으로 계산해서 초기화.
				int32 nextMoveCount = STRAIGHT_DISTANCE;
				int32 nextRemainDistance = (rx + ry) * STRAIGHT_DISTANCE;

				// i가 홀수면 대각선.
				// 거리 계산시 MANHATTAN 은 2칸으로, EUCLID는 1.4칸으로 계산.
				// 목적지 까지 거리 계산 시 EUCLID는 피타고라스 정리로 계산.
				if (i & 1) {
					switch (FindMethod) {
					case MANHATTAN: {
						nextMoveCount = STRAIGHT_DISTANCE * 2;
						break;
					}
					case EUCLID: {
						nextMoveCount = DIAGONAL_DISTANCE;
						double tmp = sqrt(rx * rx + ry * ry) * STRAIGHT_DISTANCE;
						nextRemainDistance = static_cast<int32>(tmp);
						break;
					}
					}
				}
				nextMoveCount += CurrentTile->OpenNode.MoveCount;

				int32 nextWeight = nextMoveCount + nextRemainDistance;
				bool check = false;

				// 1) 첫 탐색: 시 오픈 리스트에 넣는다.
				if (OpenTile->OpenNode.bVisited == false) {
					check = true;
				}
				// 2) 재 탐색: 가중치 비교해서 이번 경로가 더 좋으면 업데이트한다.
				else if (OpenTile->OpenNode.Weight > nextWeight) {
					// pop -> push 로 가중치 기준 재정렬.
					OpenQueue.HeapPop(OpenTile, FTilePredicate());
					check = true;
				}

				if (check) {
					// 노드 업데이트
					OpenTile->OpenNode.UpdateOpenNode(true, nextMoveCount, nextRemainDistance);

					// 부모 변경
					OpenTile->PathParent = CurrentTile;

					// 오픈 리스트에 새로 넣기
					OpenQueue.HeapPush(OpenTile, FTilePredicate());

					// 경로 선 방향 얻기
					OpenTile->PathParentDirection = static_cast<EParentDirection>(i);

					// 상태만 업데이트
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

	// 모든 타일 색상 업데이트
	GridManager->UpdateAllTileColor();
}

void FAStarAlgorithm::ClearPath()
{
	// 이전 경로 제거
	if (CurrentTile) GridManager->DrawPath(CurrentTile, false);

	OpenQueue.Empty();
	GridManager->ResetTileState();
	GridManager->ResetOpenNode();
	GridManager->ControlPanel->UpdateStatusText(TEXT("Path cleared!"));
	bFindEnd = false;
	bUnreachable = false;
}
