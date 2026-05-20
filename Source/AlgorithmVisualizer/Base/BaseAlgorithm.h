#pragma once

class AGridManager;
class ATileActor;

enum DIR {
    UU, UR, RR, DR, DD, LD, LL, UL, DIRSIZE, DIRNONE
};

class FBaseAlgorithm
{
public:
    virtual ~FBaseAlgorithm() = default;

    virtual bool CheckState() = 0;
    virtual void StepOnce() = 0;
    virtual void StepAll() = 0;
    virtual void ClearPath() = 0;

    // 맵 범위 벗어나는지, 장애물인지 체크. 둘 다 아니면 true
    bool IsWalkable(int32 X, int32 Y);

    // UU, UR, RR, DR, DD, LD, LL, UL
    int32 dx[DIRSIZE] = { 1, 1, 0, -1, -1, -1, 0, 1 };
    int32 dy[DIRSIZE] = { 0, 1, 1, 1, 0, -1, -1, -1 };

    bool bFindEnd = false;
    bool bUnreachable = false;

    ATileActor* CurrentTile = nullptr;
    AGridManager* GridManager = nullptr;
};