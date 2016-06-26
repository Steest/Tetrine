// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Pawn.h"
#include "Possessor.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Possessor_log, Log, All);
UCLASS()
class TETRINE_API APossessor : public APawn
{
	GENERATED_BODY()

public:
	APossessor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// variables
	UPROPERTY(EditAnywhere, Category = "Tetromino")
	class ATetromino* CurrentTetromino;
	UPROPERTY(EditAnywhere, Category = "Grid")
	class AGrid* grid;
	UPROPERTY(EditAnywhere, Category = "Camera")
	UCameraComponent* MainCamera;
	UPROPERTY(EditAnywhere, Category = "Tetromino")
	FString NextTetromino;
	UPROPERTY(EditAnywhere, Category = "Debug")
	FString DebugString;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float FallTimeLimit;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float FallTimeElapsed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float FastFallTimeLimit;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float FastFallTimeElapsed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float HorizontalTimeLimit;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float HorizontalTimeElapsed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float FastHorizTimeLimit;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float FastHorizTimeElapsed;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float LandedTimeLimit;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float LandedTimeElapsed;
	float PreviousHorizontalMove;
	float CurrentHorizontalMove;
	bool bIsFastHorizontal;
	bool bHasInitiatedHorizMove;
	bool bIsFastFall;
	bool bHasTetrominoLanded;
	UPROPERTY(EditAnywhere, Category = "Game")
	bool bHasMatchStarted;

	bool bRotateKeyPressed;
	TArray<FVector2D> blockPositions;
	FVector2D blockOriginPosition;
	TArray<FVector2D> rotationMatrix;

	// methods
	class ATetromino* SpawnTetromino();
	class AGrid* SpawnGrid();
	FString GenerateRandomTetromino();
	bool HasReachedTimeLimit(float &elapsed, float &timeLimit);
	void UpdateFallElapsed(float deltaTime);
	void UpdateHorizontalElapsed(float deltaTime);
	void UpdateLandedElapsed(float deltaTime);
	void MoveHorizontal(float axisValue);
	void MoveDown(float axisValue);
	FVector2D GetHorizontalMovement();

	TArray<FVector2D> Obtain2DBlockPositions();
	bool CanRotate(TArray<FVector2D> oldPositions, TArray<FVector2D> *newPositions);
	void ApplyRotation(TArray<FVector2D> newPositions);
	TArray<FVector2D> WallKick(TArray<FVector2D> newPositions);
	
	void RotateKeyPressed();
	void RotateKeyReleased();
	void RotateKeyHeld();

	TArray<int8> FilterForDeletion(TArray<int8> potentialRows);
	void DeleteRows(TArray<int8> deletionRows);
};
