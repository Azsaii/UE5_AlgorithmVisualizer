// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AlgorithmTypes.h"
#include "BaseAlgorithm.h"
#include "AlgorithmPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;
class ATileActor;
class UControlPanelWidget;
class AGridManager;
class AGraphManager;

UENUM()
enum class EEditMode : uint8
{
    None,
    SetStart,
    SetEnd,
    SetObstacle
};

UCLASS()
class ALGORITHMVISUALIZER_API AAlgorithmPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    AAlgorithmPlayerController();

    // GridManager 가 타일 생성 후 호출해서 고정 위치 저장
    void SetFixedCameraLocation(FVector NewLocation);

    UPROPERTY()
    UControlPanelWidget* ControlPanel;

    AGridManager* GridManager = nullptr;
    AGraphManager* GraphManager = nullptr;

    void SwitchAlgorithm(EAlgorithmType Type);

    // ui에서 호출됨
    void Input_ClearPath();

protected:
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaTime) override;
    virtual void SetupInputComponent() override;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_SetStart;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_SetEnd;

    UPROPERTY(EditDefaultsOnly, Category="Input")
    UInputAction* IA_Click;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_SwapStartEnd;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_ClearScreen;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_ClearPath;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_StepOnce;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_StepAll;

private:
    // 타일 생성 시 저장되는 고정 카메라 위치
    FVector FixedCameraLocation = FVector::ZeroVector;

    // 현재 호버된 타일
    ATileActor* CurrentHoveredTile;

    void UpdateHoveredTile();

    // 현재 수정모드
    EEditMode CurrentEditMode = EEditMode::None;

    // 함수 트리거 쿨타임 기능에 사용
    float LastCallTime = 0.f;
    float FuncCallCoolTime = 0.2f;

    bool bRemovingObstacle = false;
    bool bIsMousePressed = false;

    void Input_SetStart();
    void Input_SetEnd();
    void Input_ClickStarted();
    void Input_ClickTriggered();
    void Input_ClickCompleted();

    void Input_SwapStartEnd();
    void Input_ClearScreen();
    void Input_StepOnce();
    void Input_StepAll();

    ATileActor* GetTileUnderCursor();

    TUniquePtr<FBaseAlgorithm> CurrentAlgorithm;
};
