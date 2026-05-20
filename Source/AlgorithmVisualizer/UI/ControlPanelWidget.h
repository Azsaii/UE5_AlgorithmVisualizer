// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ControlPanelWidget.generated.h"

class UEditableTextBox;
class UButton;
class AGridManager;
class UImage;
class UBorder;
class UTextBlock;
class AAlgorithmPlayerController;

// 버튼 눌렸을 때 색 지정을 위해 사용함.
USTRUCT()
struct FSelectBtn
{
    GENERATED_BODY()

    FSelectBtn() = default;
    FSelectBtn(UButton* in) : Button(in){}

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


    // -------------- 거리 측정 관련 --------------------
    UPROPERTY(meta = (BindWidget)) UBorder* Border_DistanceMethod;

    UPROPERTY(meta = (BindWidget)) UTextBlock* TextBlock_DM;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TextBlock_G;
    UPROPERTY(meta = (BindWidget)) UTextBlock* TextBlock_H;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_DM_G_Manhattan;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_DM_G_Euclidean;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_DM_H_Manhattan;
    UPROPERTY(meta = (BindWidget)) UButton* Btn_DM_H_Euclidean;

    // GridManager 참조 (BeginPlay에서 주입)
    UPROPERTY(BlueprintReadWrite, Category = "Grid")
    AGridManager* GridManager = nullptr;

    AAlgorithmPlayerController* ALPC = nullptr;

    void UpdateStatusText(const FString& Message);

    // 컨트롤러에서 버튼 초기 선택 시 사용되어 public에 둠.
    UFUNCTION() void SelectAlgorithmBtn(UButton* InButton);

    UFUNCTION() void SelectDistanceMethodBtn(UButton* InButton);

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    UFUNCTION() void OnTileSetClicked();

    UFUNCTION() void OnAnyAlgorithmButtonClicked();
    UFUNCTION() void OnAnyDistanceMethodButtonClicked();

    UFUNCTION() void SetVisibilityDistanceMethodPanel(ESlateVisibility InVisibility);

    // 어두운 색 구하기
    FSlateColor GetDarkColor(FSlateColor Color);

    // 버튼마다 기본색, 선택 시 색을 구하고 FAlgorithmBtns 에 저장
    void AddButtonToArray(TArray<FSelectBtn>& BtnArray, UButton* InButton);

    // ------------------------------------------------------------
    // 알고리즘 버튼 관련
    TArray<FSelectBtn> FAlgorithmBtns;
    UButton* SelectedAlgorithmBtn = nullptr;
    // ------------------------------------------------------------

    // ------------------------------------------------------------
    // 거리 계산 버튼 관련
    TArray<FSelectBtn> FDistanceMethodBtns;
    UButton* SelectedG_DMBtn = nullptr;
    UButton* SelectedH_DMBtn = nullptr;
    bool G_IsManhattan = true;
    bool H_IsManhattan = true;
    // ------------------------------------------------------------


    UPROPERTY(EditDefaultsOnly, Category = "Grid")
    int32 MinGridAmount = 2;

    UPROPERTY(EditDefaultsOnly, Category = "Grid")
    int32 MaxGridAmount = 50;
};
