#pragma once
#include "BaseAlgorithm.h"
#include "Containers/Queue.h"

class AGridManager;
class ATileActor;

class FBFSAlgorithm : public FBaseAlgorithm
{
public:
    FBFSAlgorithm(AGridManager* InGridManager) : GridManager(InGridManager){}

    virtual bool CheckState() override;
    virtual bool StepOnce() override;
    virtual void StepAll() override;
    virtual void ClearPath() override;

private:
    AGridManager* GridManager = nullptr;
    TQueue<ATileActor*> OpenQueue;
    bool bFindEnd = false;
    bool bUnreachable = false;
};