#pragma once

// A*, JPS 에서 사용
struct FOpenNode
{
	bool bVisited;
	int32 MoveCount;
	int32 RemainDistance;
	int32 Weight;

	FOpenNode() : bVisited(false), MoveCount(0), RemainDistance(0), Weight(0) {}
	void ResetOpenNode() {
		bVisited = false;
		MoveCount = 0;
		RemainDistance = 0;
		Weight = 0;
	}
	void UpdateOpenNode(bool inbVisited, int32 InMoveCount, int32 InRemainDistance) {
		bVisited = inbVisited;
		MoveCount = InMoveCount;
		RemainDistance = InRemainDistance;
		Weight = InMoveCount + InRemainDistance;
	}
};