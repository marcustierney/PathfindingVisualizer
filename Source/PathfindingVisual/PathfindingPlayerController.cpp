#include "PathfindingPlayerController.h"
#include "GridCell.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"

APathfindingPlayerController::APathfindingPlayerController()
{
    bShowMouseCursor = true;
    bEnableClickEvents = true;
    bEnableMouseOverEvents = true;
    DefaultMouseCursor = EMouseCursor::Crosshairs;
}

void APathfindingPlayerController::BeginPlay()
{
    Super::BeginPlay();

    //Find Grid
    if (!Grid)
    {
        TArray<AActor*> Found;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APathfindingGrid::StaticClass(), Found);
        if (Found.Num() > 0)
        {
            Grid = Cast<APathfindingGrid>(Found[0]);
        }
    }
}

void APathfindingPlayerController::SetupInputComponent()
{
    Super::SetupInputComponent();

    InputComponent->BindAction("LeftClick", IE_Pressed, this, &APathfindingPlayerController::HandleLeftClick);
    InputComponent->BindAction("LeftClick", IE_Released, this, &APathfindingPlayerController::OnLeftClickReleased);
    InputComponent->BindAction("RunAlgo", IE_Pressed, this, &APathfindingPlayerController::TriggerVisualization); //Space
    InputComponent->BindAction("ClearVis", IE_Pressed, this, &APathfindingPlayerController::TriggerClearVisualization); //C
    InputComponent->BindAction("ClearAll", IE_Pressed, this, &APathfindingPlayerController::TriggerClearAll); //R
}

void APathfindingPlayerController::HandleLeftClick()
{
    bIsHolding = true;

    FHitResult Hit;
    if (GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, Hit))
    {
        if (AGridCell* Cell = Cast<AGridCell>(Hit.GetActor()))
        {
            if (Grid) Grid->HandleCellClicked(Cell);
        }
    }
}

void APathfindingPlayerController::OnLeftClickReleased()
{
    bIsHolding = false;
}


void APathfindingPlayerController::SetPlacementMode(EPlacementMode Mode)
{
    if (Grid) Grid->PlacementMode = Mode;
}

void APathfindingPlayerController::TriggerVisualization()
{
    if (Grid) Grid->StartVisualization();
}

void APathfindingPlayerController::TriggerClearVisualization()
{
    if (Grid) Grid->ClearVisualization();
}

void APathfindingPlayerController::TriggerClearAll()
{
    if (Grid) Grid->ClearAll();
}

void APathfindingPlayerController::HandleLeftClickHeld()
{
    //
}
