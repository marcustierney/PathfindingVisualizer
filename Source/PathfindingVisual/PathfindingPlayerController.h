#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PathfindingGrid.h"
#include "PathfindingPlayerController.generated.h"

UCLASS()
class PATHFINDINGVISUAL_API APathfindingPlayerController : public APlayerController
{
    GENERATED_BODY()

public:
    APathfindingPlayerController();

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pathfinding")
    APathfindingGrid* Grid;

    UFUNCTION(BlueprintCallable)
    void SetPlacementMode(EPlacementMode Mode);

    UFUNCTION(BlueprintCallable)
    void TriggerVisualization();

    UFUNCTION(BlueprintCallable)
    void TriggerClearVisualization();

    UFUNCTION(BlueprintCallable)
    void TriggerClearAll();

protected:
    virtual void BeginPlay() override;
    virtual void SetupInputComponent() override;

private:
    void HandleLeftClick();
    void HandleLeftClickHeld();
    void OnLeftClickReleased();
    bool bIsHolding = false;
};