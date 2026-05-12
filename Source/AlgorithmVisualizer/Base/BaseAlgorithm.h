#pragma once

class FBaseAlgorithm
{
public:
    virtual ~FBaseAlgorithm() = default;

    virtual void StepOnce() = 0;
    virtual void StepAll() = 0;
    virtual void Reset() = 0;
};