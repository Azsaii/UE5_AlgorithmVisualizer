#include "ControlPanelWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/Button.h"
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
    AlgorithmSwitchBtns = { Btn_SwitchBFS, Btn_SwitchDFS, Btn_SwitchAStar, Btn_SwitchJPS,
        Btn_SwitchDijkstra, Btn_SwitchBellmanFord, Btn_SwitchKruskal, Btn_SwitchPrim };

    for (UButton* Btn : AlgorithmSwitchBtns) {
        Btn->OnClicked.AddDynamic(this, &UControlPanelWidget::OnAnyButtonClicked);
        AddAlgorithmButton(Btn);
    }
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
void UControlPanelWidget::OnAnyButtonClicked()
{
    for (FAlgorithmBtn& Btn : FAlgorithmBtns) {
        if (Btn.Button->IsHovered()) {
            SelectAlgorithmBtn(Btn.Button);
            return;
        }
    }
}

void UControlPanelWidget::SelectAlgorithmBtn(UButton* InButton)
{
    FAlgorithmBtn prevSelectedBtn;
    FAlgorithmBtn curSelectedBtn;

    for (FAlgorithmBtn& FBtn : FAlgorithmBtns) {
        if (FBtn.Button == SelectedAlgorithmBtn) prevSelectedBtn = FBtn;
        if (FBtn.Button == InButton) curSelectedBtn = FBtn;
    }


    if (SelectedAlgorithmBtn) {
        prevSelectedBtn.Button->SetStyle(prevSelectedBtn.DefaultStyle);
    }

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
    }
    else if (InButton == Btn_SwitchJPS) {
        ALPC->SwitchAlgorithm(EAlgorithmType::JPS);
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

void UControlPanelWidget::AddAlgorithmButton(UButton* btn)
{
    FAlgorithmBtn& Data = FAlgorithmBtns.AddDefaulted_GetRef();

    Data.Button = btn;
    Data.DefaultStyle = btn->GetStyle();
    Data.SelectedStyle = Data.DefaultStyle;

    auto Darken = [](FSlateColor Color)
        {
            FLinearColor HSV =
                Color.GetSpecifiedColor().LinearRGBToHSV();

            // 밝기(Value) 강하게 감소
            HSV.B = FMath::Clamp(HSV.B * 0.35f, 0.f, 1.f);

            // 채도 약간 증가하면 더 진하게 보임
            HSV.G = FMath::Clamp(HSV.G * 1.2f, 0.f, 1.f);

            return FSlateColor(HSV.HSVToLinearRGB());
        };

    Data.SelectedStyle.Normal.TintColor = Darken(Data.SelectedStyle.Normal.TintColor);
}