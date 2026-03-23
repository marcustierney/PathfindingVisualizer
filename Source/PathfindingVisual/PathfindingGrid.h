#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCell.h"
#include "PathfindingGrid.generated.h"

UENUM(BlueprintType)
enum class EPlacementMode : uint8
{
    PlaceStart UMETA(DisplayName = "Place Start"),
    PlaceEnd UMETA(DisplayName = "Place End"),
    PlaceObstacle UMETA(DisplayName = "Place Obstacle"),
    EraseObstacle UMETA(DisplayName = "Erase Obstacle")
};

UENUM(BlueprintType)
enum class EAlgorithm : uint8
{
    BFS UMETA(DisplayName = "Breadth-First Search"),
    DFS UMETA(DisplayName = "Depth-First Search")
};

UCLASS()
class PATHFINDINGVISUAL_API APathfindingGrid : public AActor
{
    GENERATED_BODY()

public:
    APathfindingGrid();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridWidth = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridHeight = 50;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    float CellSize = 120.f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    TSubclassOf<AGridCell> CellClass;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
    float StepDelay = 0.03f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Interaction")
    EPlacementMode PlacementMode = EPlacementMode::PlaceStart;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Algorithm")
    EAlgorithm SelectedAlgorithm = EAlgorithm::BFS;
    
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void BuildGrid();

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearVisualization();

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearAll();

    UFUNCTION(BlueprintCallable, Category = "Grid")
    void HandleCellClicked(AGridCell* ClickedCell);

    UFUNCTION(BlueprintCallable, Category = "Algorithm")
    void StartVisualization();

    UFUNCTION(BlueprintCallable, Category = "Algorithm")
    void StopVisualization();

    UFUNCTION(BlueprintPure, Category = "Grid")
    bool IsRunning() const {return bIsRunning;}

protected:
    virtual void BeginPlay() override;

private:

    UPROPERTY()
    TArray<AGridCell*> Cells;

    UPROPERTY()
    AGridCell* StartCell = nullptr;

    UPROPERTY()
    AGridCell* EndCell = nullptr;

    bool bIsRunning = false;

    TArray<FIntPoint> VisitOrder;
    TArray<FIntPoint> PathOrder;
    int32 StepIndex = 0;
    bool bShowingPath = false;
    FTimerHandle StepTimer;

    AGridCell* GetCell(int32 X, int32 Y) const;
    TArray<AGridCell*> GetNeighbours(AGridCell* Cell) const;
    bool IsValidCell(int32 X, int32 Y) const;
    void RunBFS();
    void RunDFS();
    void TickStep();
};