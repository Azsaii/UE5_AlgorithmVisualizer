// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlPanelWidget.generated.h"

class UEditableTextBox;
class UButton;
class AGridManager;
class UImage;
class UTextBlock;
class AAlgorithmPlayerController;

USTRUCT()
struct FAlgorithmBtn
{
    GENERATED_BODY()

    UPROPERTY()
    UButton* Button = nullptr;
    FButtonStyle DefaultStyle;
    FButtonStyle SelectedStyle;
};

UCLASS()
class ALGORITHMVISUALIZER_API UControlPanelWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
    // BindWidget = UMG 에디터의 위젯 이름과 반드시 일치해야 함
    UPROPERTY(meta = (BindWidget)) UEditableTextBox* TextBox_Width;
    UPROPERTY(meta = (BindWidget)) UEditableTextBox* TextBox_Height;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_TileSet;
    UPROPERTY(meta = (BindWidget)) UImage* StatusBackground;
    UPROPERTY(meta = (BindWidget)) UTextBlock* Text_Status;
    

    UPROPERTY(meta = (BindWidget)) UButton* Btn_SwitchBFS;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_SwitchDFS;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_SwitchAStar;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_SwitchJPS;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_SwitchDijkstra;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_SwitchBellmanFord;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_SwitchKruskal;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_SwitchPrim;

    // GridManager 참조 (BeginPlay에서 주입)
    UPROPERTY(BlueprintReadWrite, Category = "Grid")
    AGridManager* GridManager = nullptr;

    AAlgorithmPlayerController* ALPC = nullptr;

    void UpdateStatusText(const FString& Message);

    // 컨트롤러에서 버튼 초기 선택 시 사용되어 public에 둠.
    UFUNCTION()
    void SelectAlgorithmBtn(UButton* Btn);

protected:
    virtual void NativeConstruct() override;

private:
    UFUNCTION() void OnTileSetClicked();

    UFUNCTION() void OnAnyButtonClicked();

    // 버튼마다 기본색, 선택 시 색을 구하고 FAlgorithmBtns 에 저장
    void AddAlgorithmButton(UButton* InButton);

    TArray<UButton*> AlgorithmSwitchBtns;
    TArray<FAlgorithmBtn> FAlgorithmBtns;
    UButton* SelectedAlgorithmBtn = nullptr;

    FLinearColor AlgorithmBtnColor_Default;
    FLinearColor AlgorithmBtnColor_Selected;

    UPROPERTY(EditDefaultsOnly, Category = "Grid")
    int32 MinGridAmount = 2;

    UPROPERTY(EditDefaultsOnly, Category = "Grid")
    int32 MaxGridAmount = 50;
};
