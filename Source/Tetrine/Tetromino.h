// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Tetromino.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Tetromino_log, Log, All);
UCLASS()
class TETRINE_API ATetromino : public AActor
{
	GENERATED_BODY()

public:
	ATetromino();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// variables
	UPROPERTY(EditAnywhere, Category = "Blocks")
		TArray<class ABlock*> blocks;
	UPROPERTY(EditAnywhere, Category = "Debug")
		FString DebugString;
	UPROPERTY(EditAnywhere, Category = "Blocks")
		FString Shape;

	// methods
	void EndLife(class AGrid* grid);
	class ABlock* SpawnBlock();
	void SpawnShape(FString shape);
	bool DoesTetrominoCollide(FVector2D movement, class AGrid* grid);
	void MoveTetrominoOnGrid(FVector2D movement, class AGrid* grid);
	void SetArrowsVisibility(int8 blockStatus);
	void SetBlocksStatus(int8 blockStatus);
	TArray<int8> GetTetrominoRows();
	FVector2D GetPivotPosition();
	void ShiftPositions(TArray<FVector2D> &positions, AGrid* grid);
	bool CanShiftPositions(const TArray<FVector2D> &newPositions, AGrid* grid);
	TArray<FVector2D> CalculateRotation(const TArray<FVector2D> &rotationMatrix);
	void ApplyRotation(TArray<FVector2D> newPositions,AGrid* grid);
	TArray<FVector2D> GetPositions();
	FString GenerateRandomArrowDirection();
};