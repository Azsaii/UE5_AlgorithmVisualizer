#pragma once

class ATileActor;

class FBaseAlgorithm
{
public:
    virtual ~FBaseAlgorithm() = default;

    virtual bool CheckState() = 0;
    virtual void StepOnce() = 0;
    virtual void StepAll() = 0;
    virtual void ClearPath() = 0;

    static constexpr int32 DIRSIZE = 8;

    // UU, UR, RR, DR, DD, LD, LL, UL
    int32 dx[DIRSIZE] = { 1, 1, 0, -1, -1, -1, 0, 1 };
    int32 dy[DIRSIZE] = { 0, 1, 1, 1, 0, -1, -1, -1 };

    bool bFindEnd = false;
    bool bUnreachable = false;

    ATileActor* CurrentTile = nullptr;
};