#pragma once
#include "BaseAlgorithm.h"
#include "Containers/Queue.h"

class AGridManager;
class ATileActor;

class FDFSAlgorithm : public FBaseAlgorithm
{
public:
    FDFSAlgorithm(AGridManager* InGridManager) : GridManager(InGridManager) {}

    virtual bool CheckState() override;
    virtual void StepOnce() override;
    virtual void StepAll() override;
    virtual void ClearPath() override;

private:
    AGridManager* GridManager = nullptr;
    TArray<ATileActor*> OpenStack;
};