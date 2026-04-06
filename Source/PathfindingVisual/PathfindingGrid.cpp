#include "PathfindingGrid.h"
#include "Engine/World.h"
#include "TimerManager.h"

APathfindingGrid::APathfindingGrid()
{
    PrimaryActorTick.bCanEverTick = false;
}

void APathfindingGrid::BeginPlay()
{
    Super::BeginPlay();
    BuildGrid();
}

void APathfindingGrid::BuildGrid()
{
    for (AGridCell* Cell : Cells)
    {
        if (Cell) Cell->Destroy();
    }
    Cells.Empty();
    StartCell = nullptr;
    EndCell = nullptr;

    if (!CellClass) return;

    Cells.SetNum(GridWidth * GridHeight);

    FVector Origin = GetActorLocation();
    float OffsetX = -(GridWidth * CellSize) / 2.f;
    float OffsetY = -(GridHeight * CellSize) / 2.f;

    for (int32 Y = 0; Y < GridHeight; ++Y)
    {
        for (int32 X = 0; X < GridWidth; ++X)
        {
            FVector Loc = Origin + FVector(OffsetX + X * CellSize + CellSize * 0.5f, OffsetY + Y * CellSize + CellSize * 0.5f, 0.f);
            FActorSpawnParameters Params;
            Params.Owner = this;

            AGridCell* Cell = GetWorld()->SpawnActor<AGridCell>(CellClass, Loc, FRotator::ZeroRotator, Params);
            if (Cell)
            {
                Cell->SetGridPosition(X, Y);
                Cells[X + Y * GridWidth] = Cell;
            }
        }
    }
}

bool APathfindingGrid::IsValidCell(int32 X, int32 Y) const
{
    return X >= 0 && X < GridWidth && Y >= 0 && Y < GridHeight;
}

AGridCell* APathfindingGrid::GetCell(int32 X, int32 Y) const
{
    if (!IsValidCell(X, Y)) return nullptr;
    return Cells[X + Y * GridWidth];
}

TArray<AGridCell*> APathfindingGrid::GetNeighbours(AGridCell* Cell) const
{
    TArray<AGridCell*> Result;
    if (!Cell) return Result;

    const FIntPoint Dirs[] = { {1,0},{-1,0},{0,1},{0,-1} };
    for (auto& D : Dirs)
    {
        AGridCell* N = GetCell(Cell->GridX + D.X, Cell->GridY + D.Y);
        if (N && N->CellState != ECellState::Obstacle)
        {
            Result.Add(N);
        }
    }
    return Result;
}

void APathfindingGrid::HandleCellClicked(AGridCell* ClickedCell)
{
    if (!ClickedCell || bIsRunning) return;

    switch (PlacementMode)
    {
        case EPlacementMode::PlaceStart:
            if (StartCell) StartCell->SetCellState(ECellState::Empty);
            StartCell = ClickedCell;
            ClickedCell->SetCellState(ECellState::Start);
            break;

        case EPlacementMode::PlaceEnd:
            if (EndCell) EndCell->SetCellState(ECellState::Empty);
            EndCell = ClickedCell;
            ClickedCell->SetCellState(ECellState::End);
            break;

        case EPlacementMode::PlaceObstacle:
            if (ClickedCell != StartCell && ClickedCell != EndCell)
                ClickedCell->SetCellState(ECellState::Obstacle);
            break;

        case EPlacementMode::EraseObstacle:
            if (ClickedCell->CellState == ECellState::Obstacle)
                ClickedCell->SetCellState(ECellState::Empty);
            break;
        }
}

void APathfindingGrid::ClearVisualization()
{
    StopVisualization();
    for (AGridCell* Cell : Cells)
    {
        if (Cell) Cell->ResetCell();
    }
    if (StartCell) StartCell->SetCellState(ECellState::Start);
    if (EndCell) EndCell->SetCellState(ECellState::End);
}

void APathfindingGrid::ClearAll()
{
    StopVisualization();
    StartCell = nullptr;
    EndCell = nullptr;
    for (AGridCell* Cell : Cells)
    {
        if (Cell) Cell->SetCellState(ECellState::Empty);
    }
}

void APathfindingGrid::StartVisualization()
{
    if (!StartCell || !EndCell || bIsRunning) return;

    ClearVisualization();
    VisitOrder.Empty();
    PathOrder.Empty();
    StepIndex = 0;
    bShowingPath = false;

    switch (SelectedAlgorithm)
    {
        case EAlgorithm::BFS:
        {
            RunBFS();
            break;
        }

        case EAlgorithm::DFS:
        {
            RunDFS();
            break;
        }
        case EAlgorithm::Greedy:
        {
            RunGreedy();
            break;
        }
    }

    bIsRunning = true;
    GetWorldTimerManager().SetTimer(StepTimer, this, &APathfindingGrid::TickStep, StepDelay, true);
}

void APathfindingGrid::StopVisualization()
{
    bIsRunning = false;
    GetWorldTimerManager().ClearTimer(StepTimer);
}

void APathfindingGrid::RunBFS()
{
    TMap<AGridCell*, AGridCell*> CameFrom;
    TQueue<AGridCell*> Frontier;
    TSet<AGridCell*> Visited;
    Frontier.Enqueue(StartCell);
    Visited.Add(StartCell);
    CameFrom.Add(StartCell, nullptr);
    bool bFound = false;

    while (!Frontier.IsEmpty())
    {
        AGridCell* Current;
        Frontier.Dequeue(Current);
        if (Current == EndCell)
        {
            bFound = true;
            break;
        }      
        VisitOrder.Add({Current->GridX, Current->GridY});

        for (AGridCell* Neighbour : GetNeighbours(Current))
        {
            if (!Visited.Contains(Neighbour))
            {
                Visited.Add(Neighbour);
                CameFrom.Add(Neighbour, Current);
                Frontier.Enqueue(Neighbour);
            }
        }
    }

    if (bFound)
    {
        AGridCell* C = EndCell;
        while (C && C != StartCell)
        {
            PathOrder.Add({C->GridX, C->GridY});
            C = CameFrom[C];
        }
        Algo::Reverse(PathOrder);
    }
}

void APathfindingGrid::RunDFS() {
    TArray<AGridCell*> Stack;
    TSet<AGridCell*> Visited;
    TMap<AGridCell*, AGridCell*> CameFrom;
    bool bFound = false;
    Stack.Push(StartCell);
    CameFrom.Add(StartCell, nullptr);

    while (!Stack.IsEmpty()) {
        AGridCell* Current = Stack.Pop();
        if (Visited.Contains(Current)) continue;
        Visited.Add(Current);
        if (Current == EndCell)
        {
            bFound = true;
            break;
        }
        VisitOrder.Add({Current->GridX, Current->GridY});
        for (AGridCell* Neighbour : GetNeighbours(Current))
        {
            if (!Visited.Contains(Neighbour))
            {
                CameFrom.FindOrAdd(Neighbour) = Current;
                Stack.Push(Neighbour);
            }
        }
    }

    if (bFound)
    {
        AGridCell* C = EndCell;
        while (C && C != StartCell)
        {
            PathOrder.Add({ C->GridX, C->GridY });
            C = CameFrom[C];
        }
        Algo::Reverse(PathOrder);
    }
}

void APathfindingGrid::RunGreedy()
{
    TArray<AGridCell*> OpenList;
    TSet<AGridCell*> Visited;
    TMap<AGridCell*, AGridCell*> CameFrom;
    bool bFound = false;
    OpenList.Add(StartCell);
    CameFrom.Add(StartCell, nullptr);
    while (!OpenList.IsEmpty())
    {
        OpenList.Sort([&](AGridCell& A, AGridCell& B)
            {
                return Heuristic(&A, EndCell) > Heuristic(&B, EndCell);
            });
        AGridCell* Current = OpenList.Pop();
        if (Visited.Contains(Current)) continue;
        Visited.Add(Current);
        if (Current == EndCell)
        {
            bFound = true;
            break;
        }
        VisitOrder.Add({ Current->GridX, Current->GridY });
        for (AGridCell* Neighbour : GetNeighbours(Current))
        {
            if (!Visited.Contains(Neighbour))
            {
                CameFrom.FindOrAdd(Neighbour) = Current;
                OpenList.Add(Neighbour);
            }
        }
    }
    if (bFound)
    {
        AGridCell* C = EndCell;
        while (C && C != StartCell)
        {
            PathOrder.Add({ C->GridX, C->GridY });
            C = CameFrom[C];
        }
        Algo::Reverse(PathOrder);
    }
}

float APathfindingGrid::Heuristic(AGridCell* A, AGridCell* B) const
{
    return FMath::Abs(A->GridX - B->GridX) + FMath::Abs(A->GridY - B->GridY);
}

void APathfindingGrid::TickStep()
{
    if (!bShowingPath)
    {
        if (StepIndex < VisitOrder.Num())
        {
            FIntPoint Pos = VisitOrder[StepIndex++];
            AGridCell* Cell = GetCell(Pos.X, Pos.Y);
            if (Cell && Cell->CellState != ECellState::Start && Cell->CellState != ECellState::End)
            {
                Cell->SetCellState(ECellState::Visited);
            }
        }
        else
        {
            bShowingPath = true;
            StepIndex = 0;
        }
    }
    else
    {
        if (StepIndex < PathOrder.Num())
        {
            FIntPoint Pos = PathOrder[StepIndex++];
            AGridCell* Cell = GetCell(Pos.X, Pos.Y);
            if (Cell && Cell->CellState != ECellState::Start && Cell->CellState != ECellState::End)
            {
                Cell->SetCellState(ECellState::Path);
            }
        }
        else
        {
            StopVisualization();
        }
    }
}