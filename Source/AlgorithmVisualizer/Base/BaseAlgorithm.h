#pragma once

class FBaseAlgorithm
{
public:
    virtual ~FBaseAlgorithm() = default;

    virtual bool CheckState() = 0;
    virtual bool StepOnce() = 0;
    virtual void StepAll() = 0;
    virtual void ClearPath() = 0;

    int32 dx[4] = { 0, 0, 1, -1 };
    int32 dy[4] = { 1, -1, 0, 0 };
};