#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridCell.generated.h"

UENUM(BlueprintType)
enum class ECellState : uint8
{
    Empty       UMETA(DisplayName = "Empty"),
    Obstacle    UMETA(DisplayName = "Obstacle"),
    Start       UMETA(DisplayName = "Start"),
    End         UMETA(DisplayName = "End"),
    Visited     UMETA(DisplayName = "Visited"),
    Frontier    UMETA(DisplayName = "Frontier"),
    Path        UMETA(DisplayName = "Path")
};

UCLASS()
class PATHFINDINGVISUAL_API AGridCell : public AActor
{
    GENERATED_BODY()

public:
    AGridCell();

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    UStaticMeshComponent* CellMesh;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* EmptyMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* ObstacleMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* StartMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* EndMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* VisitedMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* FrontierMaterial;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    UMaterialInterface* PathMaterial;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 GridX;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    int32 GridY;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
    ECellState CellState;

    UFUNCTION(BlueprintCallable)
    void SetCellState(ECellState NewState);

    UFUNCTION(BlueprintCallable)
    void SetGridPosition(int32 X, int32 Y);

    UFUNCTION(BlueprintCallable)
    void ResetCell();

protected:
    virtual void BeginPlay() override;
};