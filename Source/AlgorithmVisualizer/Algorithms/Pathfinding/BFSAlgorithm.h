#pragma once
#include "BaseAlgorithm.h"

class AGridManager;

class FBFSAlgorithm : public FBaseAlgorithm
{
public:
    FBFSAlgorithm(AGridManager* InGridManager) : GridManager(InGridManager){}
    ~FBFSAlgorithm();

    virtual void StepOnce() override;
    virtual void StepAll() override;
    virtual void Reset() override;

private:
    AGridManager* GridManager = nullptr;
};