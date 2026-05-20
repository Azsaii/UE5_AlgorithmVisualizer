#include "ControlPanelWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
#include "Components/Border.h"
#include "Components/TextBlock.h"
#include "Components/Image.h"
#include "Components/CanvasPanelSlot.h"
#include "Fonts/FontMeasure.h"
#include "GridManager.h"
#include "AlgorithmPlayerController.h"

void UControlPanelWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // 기본값 설정
    TextBox_Width->SetText(FText::FromString(TEXT("10")));
    TextBox_Height->SetText(FText::FromString(TEXT("10")));

    Btn_TileSet->OnClicked.AddDynamic(this, &UControlPanelWidget::OnTileSetClicked);

    Text_Status->SetText(FText::FromString(TEXT("Enter grid size and click Apply")));

    // 알고리즘 버튼 세팅
    TArray<UButton*> AlgorithmSwitchBtns = { Btn_SwitchBFS, Btn_SwitchDFS, Btn_SwitchAStar, Btn_SwitchJPS,
        Btn_SwitchDijkstra, Btn_SwitchBellmanFord, Btn_SwitchKruskal, Btn_SwitchPrim };

    for (UButton* Btn : AlgorithmSwitchBtns) {
        Btn->OnClicked.AddDynamic(this, &UControlPanelWidget::OnAnyAlgorithmButtonClicked);
        AddButtonToArray(FAlgorithmBtns, Btn);
    }

    SelectedAlgorithmBtn = Btn_SwitchBFS;

    // 거리 측정 버튼 세팅
    TArray<UButton*> DistanceMethodBtns = {
        Btn_DM_G_Manhattan, Btn_DM_G_Euclidean, Btn_DM_H_Manhattan, Btn_DM_H_Euclidean };

    for (UButton* Btn : DistanceMethodBtns) {
        Btn->OnClicked.AddDynamic(this, &UControlPanelWidget::OnAnyDistanceMethodButtonClicked);
        AddButtonToArray(FDistanceMethodBtns, Btn);
    }

    SelectedG_DMBtn = Btn_DM_G_Manhattan;
    SelectedH_DMBtn = Btn_DM_H_Manhattan;
}

void UControlPanelWidget::NativeDestruct()
{
    // 플레이 종료 시 모든 버튼 스타일 복구.
    for (FSelectBtn& Btn : FAlgorithmBtns) {
        if (Btn.Button) Btn.Button->SetStyle(Btn.DefaultStyle);
    }
    for (FSelectBtn& Btn : FDistanceMethodBtns) {
        if (Btn.Button) Btn.Button->SetStyle(Btn.DefaultStyle);
    }

    Super::NativeDestruct();
}

void UControlPanelWidget::OnTileSetClicked()
{
    if (!GridManager) { UE_LOG(LogTemp, Warning, TEXT("GridManager reference is null")); return; }

    // 텍스트를 정수로 변환 + 범위 클램프
    int32 W = FMath::Clamp(FCString::Atoi(*TextBox_Width->GetText().ToString()), MinGridAmount, MaxGridAmount);
    int32 H = FMath::Clamp(FCString::Atoi(*TextBox_Height->GetText().ToString()), MinGridAmount, MaxGridAmount);

    // 범위를 벗어난 값 입력 시 자동 보정해서 다시 표시
    TextBox_Width->SetText(FText::FromString(FString::FromInt(W)));
    TextBox_Height->SetText(FText::FromString(FString::FromInt(H)));

    // 화면 초기화
    ALPC->Input_ClearPath();

    GridManager->GenerateGrid(W, H);

    FString Msg = FString::Printf(TEXT("Grid generated: %d x %d  (%d tiles)"), W, H, W * H);
    UpdateStatusText(Msg);
}

void UControlPanelWidget::UpdateStatusText(const FString& Message)
{
    // 텍스트 변경
    Text_Status->SetText(FText::FromString(Message));

    // 폰트 정보로 텍스트 너비 직접 계산
    const FSlateFontInfo& FontInfo = Text_Status->GetFont();
    FVector2D TextSize = FVector2D::ZeroVector;

    TSharedRef<FSlateFontMeasure> FontMeasure =
        FSlateApplication::Get().GetRenderer()->GetFontMeasureService();

    TextSize = FontMeasure->Measure(Message, FontInfo);

    float PaddingX = 70.f;
    float PaddingY = 20.f;

    FVector2D NewSize = FVector2D(TextSize.X + PaddingX, TextSize.Y + PaddingY);

    UCanvasPanelSlot* BackgroundSlot = Cast<UCanvasPanelSlot>(StatusBackground->Slot);
    if (BackgroundSlot) BackgroundSlot->SetSize(NewSize);
}

// 눌린 버튼을 호버로 판별
void UControlPanelWidget::OnAnyAlgorithmButtonClicked()
{
    for (FSelectBtn& Btn : FAlgorithmBtns) {
        if (Btn.Button->IsHovered()) {
            SelectAlgorithmBtn(Btn.Button);
            return;
        }
    }
}

void UControlPanelWidget::OnAnyDistanceMethodButtonClicked()
{
    for (FSelectBtn& Btn : FDistanceMethodBtns) {
        if (Btn.Button->IsHovered()) {
            SelectDistanceMethodBtn(Btn.Button);
            return;
        }
    }
}

void UControlPanelWidget::SelectAlgorithmBtn(UButton* InButton)
{
    FSelectBtn prevSelectedBtn(nullptr);
    FSelectBtn curSelectedBtn(nullptr);

    for (FSelectBtn& FBtn : FAlgorithmBtns) {
        if (FBtn.Button == SelectedAlgorithmBtn) prevSelectedBtn = FBtn;
        if (FBtn.Button == InButton) curSelectedBtn = FBtn;
    }

    // 이전 선택되었던 버튼 스타일 복구
    if (prevSelectedBtn.Button) {
        prevSelectedBtn.Button->SetStyle(prevSelectedBtn.DefaultStyle);
    }

    // 거리 측정 방법 메뉴 일단 숨기기. A* or JPS 면 아래에서 보이게 함
    SetVisibilityDistanceMethodPanel(ESlateVisibility::Hidden);

    // 선택된 버튼 스타일 변경
    SelectedAlgorithmBtn = InButton;
    SelectedAlgorithmBtn->SetStyle(curSelectedBtn.SelectedStyle);

    if (InButton == Btn_SwitchBFS) {
        ALPC->SwitchAlgorithm(EAlgorithmType::BFS);
    }
    else if (InButton == Btn_SwitchDFS) {
        ALPC->SwitchAlgorithm(EAlgorithmType::DFS);
    }
    else if (InButton == Btn_SwitchAStar) {
        ALPC->SwitchAlgorithm(EAlgorithmType::ASTAR);
        SetVisibilityDistanceMethodPanel(ESlateVisibility::Visible);
        ALPC->SwitchDistanceMethod(G_IsManhattan, H_IsManhattan);
    }
    else if (InButton == Btn_SwitchJPS) {
        ALPC->SwitchAlgorithm(EAlgorithmType::JPS);
        SetVisibilityDistanceMethodPanel(ESlateVisibility::Visible);
        ALPC->SwitchDistanceMethod(G_IsManhattan, H_IsManhattan);
    }
    else if (InButton == Btn_SwitchDijkstra) {
        ALPC->SwitchAlgorithm(EAlgorithmType::DIJKSTRA);
    }
    else if (InButton == Btn_SwitchBellmanFord) {
        ALPC->SwitchAlgorithm(EAlgorithmType::BELLMAN_FORD);
    }
    else if (InButton == Btn_SwitchKruskal) {
        ALPC->SwitchAlgorithm(EAlgorithmType::KRUSKAL);
    }
    else if (InButton == Btn_SwitchPrim) {
        ALPC->SwitchAlgorithm(EAlgorithmType::PRIM);
    }
}

void UControlPanelWidget::SelectDistanceMethodBtn(UButton* InButton)
{
    FSelectBtn prevSelectedBtn(nullptr);
    FSelectBtn curSelectedBtn(nullptr);

    if (InButton == Btn_DM_G_Manhattan || InButton == Btn_DM_G_Euclidean) {
        for (FSelectBtn& btn : FDistanceMethodBtns) {
            if (btn.Button == SelectedG_DMBtn) prevSelectedBtn = btn;
            if (btn.Button == InButton) curSelectedBtn = btn;
        }

        SelectedG_DMBtn = InButton;
    }
    else {
        for (FSelectBtn& btn : FDistanceMethodBtns) {
            if (btn.Button == SelectedH_DMBtn) prevSelectedBtn = btn;
            if (btn.Button == InButton) curSelectedBtn = btn;
        }

        SelectedH_DMBtn = InButton;
    }

    // 이전 선택되었던 버튼 스타일 복구
    if (prevSelectedBtn.Button) {
        prevSelectedBtn.Button->SetStyle(prevSelectedBtn.DefaultStyle);
    }

    // 선택된 버튼 스타일 변경
    InButton->SetStyle(curSelectedBtn.SelectedStyle);

    if (InButton == Btn_DM_G_Manhattan) {
        G_IsManhattan = true;
    }
    else if (InButton == Btn_DM_G_Euclidean) {
        G_IsManhattan = false;
    }
    else if (InButton == Btn_DM_H_Manhattan) {
        H_IsManhattan = true;
    }
    else if (InButton == Btn_DM_H_Euclidean) {
        H_IsManhattan = false;
    }
    ALPC->SwitchDistanceMethod(G_IsManhattan, H_IsManhattan);
}

FSlateColor UControlPanelWidget::GetDarkColor(FSlateColor Color)
{
    FLinearColor HSV = Color.GetSpecifiedColor().LinearRGBToHSV();

    // 밝기(Value) 강하게 감소
    HSV.B = FMath::Clamp(HSV.B * 0.35f, 0.f, 1.f);

    // 채도 약간 증가하면 더 진하게 보임
    HSV.G = FMath::Clamp(HSV.G * 1.2f, 0.f, 1.f);

    return FSlateColor(HSV.HSVToLinearRGB());
}

void UControlPanelWidget::AddButtonToArray(TArray<FSelectBtn>& BtnArray, UButton* btn)
{
    FSelectBtn& Data = BtnArray.AddDefaulted_GetRef();

    Data.Button = btn;
    Data.DefaultStyle = btn->GetStyle();
    Data.SelectedStyle = Data.DefaultStyle;

    Data.SelectedStyle.Normal.TintColor = GetDarkColor(Data.SelectedStyle.Normal.TintColor);
    Data.SelectedStyle.Hovered.TintColor = GetDarkColor(Data.DefaultStyle.Hovered.TintColor);
    Data.SelectedStyle.Pressed.TintColor = GetDarkColor(Data.DefaultStyle.Pressed.TintColor);
}

void UControlPanelWidget::SetVisibilityDistanceMethodPanel(ESlateVisibility InVisibility)
{
    Border_DistanceMethod->SetVisibility(InVisibility);
    TextBlock_DM->SetVisibility(InVisibility);
    TextBlock_G->SetVisibility(InVisibility);
    TextBlock_H->SetVisibility(InVisibility);
    Btn_DM_G_Manhattan->SetVisibility(InVisibility);
    Btn_DM_G_Euclidean->SetVisibility(InVisibility);
    Btn_DM_H_Manhattan->SetVisibility(InVisibility);
    Btn_DM_H_Euclidean->SetVisibility(InVisibility);
}