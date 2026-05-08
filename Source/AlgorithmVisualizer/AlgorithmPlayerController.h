// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "AlgorithmPlayerController.generated.h"

class UInputMappingContext;
class UInputAction;

UENUM(BlueprintType)
enum class ECameraMode : uint8
{
	Fixed,  // 마우스 커서 활성, UI 조작 가능
	Free    // 자유 시점 이동
};

UCLASS()
class ALGORITHMVISUALIZER_API AAlgorithmPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
    AAlgorithmPlayerController();

    // GridManager 가 타일 생성 후 호출해서 고정 위치 저장
    void SetFixedCameraLocation(FVector NewLocation);
    void ToggleCameraMode();

protected:
    virtual void BeginPlay() override;

private:
    ECameraMode CurrentCameraMode = ECameraMode::Fixed;

    // 타일 생성 시 저장되는 고정 카메라 위치
    FVector FixedCameraLocation = FVector::ZeroVector;

    void ApplyCameraMode(ECameraMode NewMode);
};
