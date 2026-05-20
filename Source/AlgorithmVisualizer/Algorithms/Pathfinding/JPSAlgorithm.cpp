#include "JPSAlgorithm.h"
#include "GridManager.h"

FJPSAlgorithm::FJPSAlgorithm(AGridManager* InGridManager)
{
	GridManager = InGridManager;
	int32 sz = GridManager->GridTiles.Num();
	for (int32 i = 0; i < sz; i++)DirectionArray.Add(DIR::DIRNONE);
	for (int32 i = 0; i < sz; i++) ChildArray.Add(nullptr);

	ForcedTable[0] = { {{0, -1, 1, -1}, {0, 1, 1, 1}} };	// UU
	ForcedTable[2] = { {{1, 0, 1, 1}, {-1, 0, -1, 1}} };	// RR
	ForcedTable[4] = { {{0, 1, -1, 1}, {0, -1, -1, -1}} };	// DD
	ForcedTable[6] = { {{-1, 0, -1, -1}, {1, 0, 1, -1}} };	// LL

	ForcedTable[1] = { {{0, -1, 1, -1}, {-1, 0, -1, 1}, {0, -1, 0, 0}, {-1, 0, 0, 0}} };	// UR
	ForcedTable[3] = { {{1, 0, 1, 1}, {0, -1, -1, -1}, {1, 0, 0, 0}, {0, -1, 0, 0}} };	// DR
	ForcedTable[5] = { {{1, 0, 1, -1}, {0, 1, -1, 1}, {0, 1, 0, 0}, {1, 0, 0, 0}} };	// DL
	ForcedTable[7] = { {{-1, 0, -1, -1}, {0, 1, 1, 1}, {-1, 0, 0, 0}, {0, 1, 0, 0}} };	// UL
}

void FJPSAlgorithm::Init()
{
	int32 sz = GridManager->GridTiles.Num();
	DirectionArray.Empty();
	ChildArray.Empty();

	for (int32 i = 0; i < sz; i++) DirectionArray.Add(DIR::DIRNONE);
	for (int32 i = 0; i < sz; i++) ChildArray.Add(nullptr);
}

bool FJPSAlgorithm::CheckState()
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

void FJPSAlgorithm::UpdateNode(bool IsStraight, ATileActor* Current, ATileActor* Parent, int32 ParentX, int32 ParentY)
{
	ATileActor* OpenTile = Current;
	int width = GridManager->CurrentWidth;

	// 대각선 탐색 중 직선탐색하다 추가하는 경우 우선 대각선 방향 탐색 노드부터 추가해야함.
	int32 pOffset = ParentX + ParentY * width;
	if (ParentX != -1) {
		OpenTile = GridManager->GridTiles[pOffset];
		IsStraight = false;
	}

	int32 ex = GridManager->EndTile->GridX;
	int32 ey = GridManager->EndTile->GridY;

	// 목적지까지 거리
	int32 rx = abs(ex - OpenTile->GridX);
	int32 ry = abs(ey - OpenTile->GridY);

	// 부모와 좌표 차이
	int32 gx = abs(Parent->GridX - OpenTile->GridX);
	int32 gy = abs(Parent->GridY - OpenTile->GridY);
	if (ParentX != -1) {
		gx = abs(ParentX - Parent->GridX);
		gy = abs(ParentY - Parent->GridY);
	}

	// 기본적으로 직선일 때 맨해튼방식으로 계산해서 초기화.
	int32 nextMoveCount = STRAIGHT_DISTANCE;
	int32 nextRemainDistance = (rx + ry) * STRAIGHT_DISTANCE;

	// 대각선 시 계산
	// 거리 계산시 MANHATTAN 은 2칸으로, EUCLID는 1.4칸으로 계산.
	// 목적지 까지 거리 계산 시 EUCLID는 피타고라스 정리로 계산.
	if (!IsStraight) {
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

		// 부모와 좌표 차이만큼 곱해주기.
		nextMoveCount *= gx;
	}
	// 직선인 경우 부모와 좌표 차이만큼 곱해주기.
	else {
		if (gx != 0) nextMoveCount *= gx;
		else nextMoveCount *= gy;
	}

	// 현재 뽑힌 타일의 이동횟수 더해주기.
	nextMoveCount += CurrentTile->OpenNode.MoveCount;

	int32 nextWeight = nextMoveCount + nextRemainDistance;
	bool check = false;

	// 1) 첫 탐색: 오픈 리스트에 넣는다.
	if (OpenTile->OpenNode.bVisited == false) {
		check = true;
	}
	// 2) 재 탐색: 가중치 비교해서 이번 경로가 더 좋으면 업데이트한다.
	else if (OpenTile->OpenNode.MoveCount > nextMoveCount) {

		// 오픈 리스트에서 제거
		int32 id = OpenQueue.Find(OpenTile);
		if (id != INDEX_NONE) {
			OpenQueue.RemoveAt(id);
			OpenQueue.Heapify(FTilePredicate()); // 힙 재구성
		}

		check = true;
	}

	if (check) {
		// 노드 업데이트
		OpenTile->OpenNode.UpdateOpenNode(true, nextMoveCount, nextRemainDistance);

		// OpenTile의 부모가 가진 자식 중 OpenTile을 제거한다.
		int offset = Parent->GridX + Parent->GridY * width;
		if (ChildArray[offset] == OpenTile) {
			ChildArray[offset] = nullptr;
		}

		// 부모 변경
		OpenTile->PathParent = Parent;

		// 오픈 리스트에 새로 넣기
		OpenQueue.HeapPush(OpenTile, FTilePredicate());

		// 경로 선 방향 얻기
		offset = OpenTile->GridX + OpenTile->GridY * width;
		OpenTile->PathParentDirection = static_cast<EParentDirection>(DirectionArray[offset]);

		// 상태/색상 업데이트
		if (OpenTile != GridManager->StartTile && OpenTile != GridManager->EndTile) {
			OpenTile->SetStateAndColor(ETileState::Open);
		}
		else OpenTile->CurrentState = ETileState::Open;

		// 원래 넣어야 할 노드는 자식으로 추가해둔다.
		// 자식으로 추가되면 추후 오픈리스트에서 부모가 뽑히고 오픈 리스트에 들어감
		// @주의: pOffset 은 ParentX, ParentY가 -1이 아닐 때만 유효함.
		if (ParentX != -1) ChildArray[pOffset] = Current;
	}
}

bool FJPSAlgorithm::SearchUpdateNode(int32 CurrentX, int32 CurrentY, DIR InDirection, ATileActor* Parent, int32 ParentX, int32 ParentY)
{
	ATileActor* End = GridManager->EndTile;
	int32 width = GridManager->CurrentWidth;

	// 방향 저장
	int32 idx = static_cast<int32>(InDirection);
	bool isStraight = !(idx & 1);

	// 직선 경로이고 새 후보 추가했으면 true
	bool IsStraightAndUpdated = false;

	while (IsWalkable(CurrentX, CurrentY)) {

		// 이전에 동일 방향으로 탐색한 적 있으면 중단.
		int32 offset = CurrentX + CurrentY * width;
		if (DirectionArray[offset] == InDirection) break;

		// 처음 탐색 시 방향 설정.
		if (DirectionArray[offset] == DIRNONE) 
			DirectionArray[offset] = InDirection;

		ATileActor* Current = GridManager->GridTiles[offset];
		if (Current->CurrentState == ETileState::Unvisited)
			Current->SetStateAndColor(ETileState::Test);

		// 종료 지점이면 업데이트
		if (CurrentX == End->GridX && CurrentY == End->GridY) {
			UpdateNode(isStraight, Current, Parent, ParentX, ParentY);
			if (isStraight) IsStraightAndUpdated = true;
			break;
		}

		// 모서리가 있으면 노드 업데이트
		bool check = false;
		ForcedInfo& f = ForcedTable[idx];
		for (int32 i = 0; i < 2; i++) {
			const auto& arr = f.ForcedArray[i];
			bool isBlocked = !IsWalkable(CurrentX + arr.block_dx, CurrentY + arr.block_dy);
			bool isOpen = IsWalkable(CurrentX + arr.open_dx, CurrentY + arr.open_dy);
			if (isBlocked && isOpen) {
				UpdateNode(isStraight, Current, Parent, ParentX, ParentY);
				if (isStraight) IsStraightAndUpdated = true;
				check = true;
				break;
			}
		}

		if (check) break;

		// 대각선이면 수평수직 탐색.
		// 탐색 도중 새 후보를 찾은 경우 탐색 중단. 현재 탐색지점이 오픈 리스트에 추가되었음.
		if (idx & 1) {
			int32 left = idx - 1;
			if (left < 0) left += 8;
			if (SearchUpdateNode(CurrentX + dx[left], CurrentY + dy[left], static_cast<DIR>(left), Parent, CurrentX, CurrentY) == true) {
				break;
			}

			int32 right = idx + 1;
			if (right > 7) right = 0;
			if (SearchUpdateNode(CurrentX + dx[right], CurrentY + dy[right], static_cast<DIR>(right), Parent, CurrentX, CurrentY) == true) {
				break;
			}
		}

		CurrentX += dx[idx];
		CurrentY += dy[idx];
	}
	
	return IsStraightAndUpdated;
}

void FJPSAlgorithm::Jump(int32 CurrentX, int32 CurrentY, DIR InDirection, ATileActor* Parent)
{
	int32 idx = static_cast<int32>(InDirection);
	int32 nx = CurrentX + dx[idx];
	int32 ny = CurrentY + dy[idx];

	// 처음 방문 시 방향 세팅
	if (IsWalkable(nx, ny)) {
		int32 offset = nx + ny * GridManager->CurrentWidth;
		ATileActor* Next = GridManager->GridTiles[offset];

		// 탐색 시작
		SearchUpdateNode(nx, ny, InDirection, Parent);
	}
}

void FJPSAlgorithm::StepOnce()
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

	// 이전 경로 제거
	if (CurrentTile) GridManager->DrawPath(CurrentTile, false);

	// 가중치 가장 작은 노드 뽑기
	OpenQueue.HeapPop(CurrentTile, FTilePredicate());

	int32 width = GridManager->CurrentWidth;
	int32 height = GridManager->CurrentHeight;

	int32 cx = CurrentTile->GridX;
	int32 cy = CurrentTile->GridY;
	int32 offset = cx + cy * width;

	int32 ex = GridManager->EndTile->GridX;
	int32 ey = GridManager->EndTile->GridY;

	// 자식 노드 있으면 오픈 리스트에 넣기
	ATileActor* child = ChildArray[offset];
	if (child != nullptr) {
		UpdateNode(true, child, CurrentTile);
	}

	// 현재 경로 그리기
	GridManager->DrawPath(CurrentTile, true);

	// 목표지점 도달 시 종료
	if (CurrentTile == GridManager->EndTile) {
		bFindEnd = true;
		GridManager->ControlPanel->UpdateStatusText(TEXT("Path found!"));
		return;
	}

	bool IsStart = CurrentTile == GridManager->StartTile;

	// 상태 표시
	if (CurrentTile != GridManager->StartTile && CurrentTile != GridManager->EndTile) {
		CurrentTile->SetStateAndColor(ETileState::Closed);
	}
	else CurrentTile->CurrentState = ETileState::Closed;

	FString Msg = FString::Printf(TEXT("Open point (%d, %d)"), cx, cy);
	GridManager->ControlPanel->UpdateStatusText(Msg);

	if (IsStart) {
		// 시작점은 방향이 없으므로 주변 8방향 노드 탐색
		for (int8 i = 0; i < DIRSIZE; i++) {
			Jump(cx, cy, static_cast<DIR>(i), CurrentTile);
		}
	}
	else {
		DIR& CurrentDir = DirectionArray[offset];
		int32 idx = static_cast<int32>(CurrentDir);
		Jump(cx, cy, CurrentDir, CurrentTile);

		// ------------------------------------------------
		// 모서리 있으면 현재 방향의 양 옆 방향 탐색
		// 현재 방향이 직선이면 양 옆은 조건 맞을 때만 탐색
		// 대각선이면 양 옆은 조건 없이 탐색
		// ------------------------------------------------
		bool IsStraight = !(idx & 1);

		int32 left = idx - 1;
		if (left < 0) left += 8;

		// 직선 방향이면 조건 하에 옆 방향 탐색
		if (IsStraight) {
			auto arr = ForcedTable[idx].ForcedArray[0];
			bool isBlocked = !IsWalkable(cx + arr.block_dx, cy + arr.block_dy);
			bool isOpen = IsWalkable(cx + arr.open_dx, cy + arr.open_dy);
			if (isBlocked && isOpen) {
				Jump(cx, cy, static_cast<DIR>(left), CurrentTile);
			}
		}
		// 대각선이면 조건 없이 옆 방향 탐색
		else Jump(cx, cy, static_cast<DIR>(left), CurrentTile);

		int32 right = idx + 1;
		if (right > 7) right = 0;
		if (IsStraight) {
			auto arr = ForcedTable[idx].ForcedArray[1];
			bool isBlocked = !IsWalkable(cx + arr.block_dx, cy + arr.block_dy);
			bool isOpen = IsWalkable(cx + arr.open_dx, cy + arr.open_dy);
			if (isBlocked && isOpen) Jump(cx, cy, static_cast<DIR>(right), CurrentTile);
		}
		else Jump(cx, cy, static_cast<DIR>(right), CurrentTile);


		// 현재 방향이 대각선이면 주변 대각선 방향 추가 탐색
		if (!IsStraight) {
			left--;
			if (left < 0) left += 8;
			auto arr = ForcedTable[idx].ForcedArray[2];
			if (!IsWalkable(cx + arr.block_dx, cy + arr.block_dy)) {
				Jump(cx, cy, static_cast<DIR>(left), CurrentTile);
			}

			right++;
			if (right > 7) right = 0;
			arr = ForcedTable[idx].ForcedArray[3];
			if (!IsWalkable(cx + arr.block_dx, cy + arr.block_dy)) {
				Jump(cx, cy, static_cast<DIR>(right), CurrentTile);
			}
		}
	}

	if (OpenQueue.IsEmpty()) {
		bUnreachable = true;
		GridManager->ControlPanel->UpdateStatusText(TEXT("Target Unreachable"));
		return;
	}
}

void FJPSAlgorithm::StepAll()
{
	if (CheckState() == false) return;
	else if (OpenQueue.IsEmpty()) {
		Init();
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

				// 장애물이면 다음 노드 탐색으로 넘어가기.
				if (OpenTile->CurrentState == ETileState::Obstacle) continue;

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

					// 오픈 리스트에서 제거
					int32 idx = OpenQueue.Find(OpenTile);
					if (idx != INDEX_NONE) {
						OpenQueue.RemoveAt(idx);
						OpenQueue.Heapify(FTilePredicate()); // 힙 재구성
					}

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

void FJPSAlgorithm::ClearPath()
{
	// 이전 경로 제거
	if (CurrentTile) GridManager->DrawPath(CurrentTile, false);

	Init();
	OpenQueue.Empty();
	GridManager->ResetTile();
	GridManager->ControlPanel->UpdateStatusText(TEXT("Path cleared!"));
	bFindEnd = false;
	bUnreachable = false;
}
