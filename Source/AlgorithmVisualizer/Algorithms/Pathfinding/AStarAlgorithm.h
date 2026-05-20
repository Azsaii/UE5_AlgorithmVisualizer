#pragma once
#include "BaseAlgorithm.h"
#include "Containers/Queue.h"
#include "TileActor.h"

class ATileActor;

enum FFindMethod
{
    MANHATTAN,
    EUCLID,
};

// TArray를 우선순위 큐로 쓰기 위한 커스텀 구조체
struct FTilePredicate
{
    bool operator()(const ATileActor& A, const ATileActor& B) const
    {
        if (A.OpenNode.Weight == B.OpenNode.Weight)
        {
            return A.OpenNode.RemainDistance < B.OpenNode.RemainDistance;
        }
        return A.OpenNode.Weight < B.OpenNode.Weight;
    }
};

class FAStarAlgorithm : public FBaseAlgorithm
{
public:
    FAStarAlgorithm() = default;
    FAStarAlgorithm(AGridManager* InGridManager) { GridManager = InGridManager; }
    
    virtual bool CheckState() override;
    virtual void StepOnce() override;
    virtual void StepAll() override;
    virtual void ClearPath() override;

    FFindMethod FindMethod = MANHATTAN;

protected:
    static constexpr int STRAIGHT_DISTANCE = 10; // 직선 거리
    static constexpr int DIAGONAL_DISTANCE = 14; // 대각선 거리

    TArray<ATileActor*> OpenQueue;
};