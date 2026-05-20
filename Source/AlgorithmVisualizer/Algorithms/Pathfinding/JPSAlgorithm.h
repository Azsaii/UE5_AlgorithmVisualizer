#pragma once
#include "AStarAlgorithm.h"

// 모서리 조건 기록
struct ForcedInfo {
    struct ForcedCheck {
        int32 block_dx, block_dy; // 장애물이어야 하는 위치
        int32 open_dx, open_dy;   // 열려 있어야 하는 위치
    };
    TArray<ForcedCheck> ForcedArray;
};

class FJPSAlgorithm : public FAStarAlgorithm
{
public:
    FJPSAlgorithm(AGridManager* InGridManager);

    virtual bool CheckState() override;
    virtual void StepOnce() override;
    virtual void StepAll() override;
    virtual void ClearPath() override;

    void Init();
    void UpdateNode(bool IsStraight, ATileActor* Current, ATileActor* Parent, int32 ParentX = -1, int32 ParentY = -1);
    bool SearchUpdateNode(int32 CurrentX, int32 CurrentY, DIR InDirection, ATileActor* Parent, int32 ParentX = -1, int32 ParentY = -1);
    void Jump(int32 CurrentX, int32 CurrentY, DIR InDirection, ATileActor* Parent);
    

private:
    // 그리드에서 방향 저장
    TArray<DIR> DirectionArray;

    // 대각선 타일의 자식 타일 저장.
    // 대각탐색이 타일 A에서 시작. 
    // 수평/수직 탐색 -> 후보 타일 B발견 -> 하지만 B는 A를 경유하므로 A 먼저 넣어야 함.
    // 그런데 방향 재탐색 막아놓아서 타일 B로 재탐색할 수 없음 따라서 A의 자식으로 추가해둔다.
    TArray<ATileActor*> ChildArray;

    // 방향 별 모서리 정보 저장
    ForcedInfo ForcedTable[8];
};