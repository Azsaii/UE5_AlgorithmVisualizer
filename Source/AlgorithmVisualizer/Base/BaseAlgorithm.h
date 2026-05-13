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

    int32 dx[4] = { 1, 0, -1, 0 };
    int32 dy[4] = { 0, 1, 0, -1 };

    bool bFindEnd = false;
    bool bUnreachable = false;

    ATileActor* CurentTile = nullptr;
};