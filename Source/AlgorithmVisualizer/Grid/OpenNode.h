#pragma once

// A*, JPS 에서 사용
struct FOpenNode
{
	bool IsVisited; // 타일 상태만으로는 판별 불가해서 필요. goal 지점이 open이라 업데이트할 수도 있음.
	int32 MoveCount;
	int32 RemainDistance;
	int32 Weight;

	FOpenNode() : IsVisited(false), MoveCount(0), RemainDistance(0), Weight(0) {}
	void ResetOpenNode() {
		IsVisited = false;
		MoveCount = 0;
		RemainDistance = 0;
		Weight = 0;
	}
	void UpdateOpenNode(bool InIsVisited, int32 InMoveCount, int32 InRemainDistance) {
		IsVisited = InIsVisited;
		MoveCount = InMoveCount;
		RemainDistance = InRemainDistance;
		Weight = InMoveCount + InRemainDistance;
	}
};