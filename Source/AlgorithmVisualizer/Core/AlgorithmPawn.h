#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "AlgorithmPawn.generated.h"

class UCameraComponent;
class UInputMappingContext;
class UInputAction;
struct FInputActionValue;

UCLASS()
class ALGORITHMVISUALIZER_API AAlgorithmPawn : public APawn
{
	GENERATED_BODY()

public:
    AAlgorithmPawn();

    // 외부에서 위치 설정 (GridManager 가 호출)
    void SetCameraLocation(FVector NewLocation);

protected:
    virtual void BeginPlay() override;
    virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
    virtual void Tick(float DeltaTime) override;

    UPROPERTY(VisibleAnywhere)
    UCameraComponent* CameraComp;

    // Enhanced Input 에셋 - 블루프린트에서 할당
    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputMappingContext* MappingContext;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_Move;

    UPROPERTY(EditDefaultsOnly, Category = "Input")
    UInputAction* IA_Zoom;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float ZScaleReference = 1000.f; // 이 높이일 때 MoveSpeed 그대로 적용

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float MoveSpeed = 100.f;

    // 줌 설정
    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float ZoomSpeed = 200.f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float ZoomMin = 500.f;

    UPROPERTY(EditDefaultsOnly, Category = "Movement")
    float ZoomMax = 10000.f;

private:
    FVector2D MoveInput = FVector2D::ZeroVector;

    void Input_Move(const FInputActionValue& Value);
    void Input_Zoom(const FInputActionValue& Value);
};
