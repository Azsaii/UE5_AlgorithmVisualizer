#pragma once
#include "BaseAlgorithm.h"
#include "Containers/Queue.h"

class ATileActor;

class FBFSAlgorithm : public FBaseAlgorithm
{
public:
    FBFSAlgorithm(AGridManager* InGridManager) { GridManager = InGridManager; }

    virtual bool CheckState() override;
    virtual void StepOnce() override;
    virtual void StepAll() override;
    virtual void ClearPath() override;

private:
    TQueue<ATileActor*> OpenQueue;
};