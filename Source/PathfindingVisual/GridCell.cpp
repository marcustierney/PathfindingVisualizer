#include "GridCell.h"

AGridCell::AGridCell()
{
    PrimaryActorTick.bCanEverTick = false;
    CellMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CellMesh"));
    RootComponent = CellMesh;
    //Collisions for click detection
    CellMesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
    CellMesh->SetGenerateOverlapEvents(false);
}

void AGridCell::BeginPlay()
{
    Super::BeginPlay();
    SetCellState(ECellState::Empty);
}

void AGridCell::SetGridPosition(int32 X, int32 Y)
{
    GridX = X;
    GridY = Y;
}

void AGridCell::SetCellState(ECellState NewState)
{
    CellState = NewState;

    UMaterialInterface* Mat = nullptr;
    switch (NewState)
    {
    case ECellState::Empty:
    {
        Mat = EmptyMaterial;
        break;
    }

    case ECellState::Obstacle:
    {
        Mat = ObstacleMaterial;
        break;
    }

    case ECellState::Start:
    {
        Mat = StartMaterial;
        break;
    }

    case ECellState::End:
    {
        Mat = EndMaterial;
        break;
    }

    case ECellState::Visited:
    {
        Mat = VisitedMaterial;
        break;
    }

    case ECellState::Frontier:
    {
        Mat = FrontierMaterial;
        break;
    }

    case ECellState::Path:
    {
        Mat = PathMaterial;
        break;
    }

    default:
    {
        Mat = nullptr;
        break;
    }
    }

    if (Mat)
    {
        CellMesh->SetMaterial(0, Mat);
    }
}

void AGridCell::ResetCell()
{
    if (CellState != ECellState::Start && CellState != ECellState::End && CellState != ECellState::Obstacle)
    {
        SetCellState(ECellState::Empty);
    }
}