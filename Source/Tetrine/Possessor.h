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

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	//TSubclassOf<class UUserWidget> wSideBarDisplay;
	//UUserWidget* MySideBarDisplay;

	APossessor();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	// variables
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Game")
	TArray<FString> NextTetrominos;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	FString LandedTetromino;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Game")
	FString SavedTetromino;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Game")
	class ATetromino* CurrentTetromino;
	UPROPERTY(EditAnywhere, Category = "Grid")
	class AGrid* grid;
	UPROPERTY(EditAnywhere, Category = "Camera")
	UCameraComponent* MainCamera;
	UPROPERTY(EditAnywhere, Category = "Anim")
	class UPaperSprite* GhostSprite;
	UPROPERTY(EditAnywhere, Category = "Anim")
	class UPaperSprite* HighlightArrowSprite;
	UPROPERTY(EditAnywhere, Category = "Anim")
	class UPaperSprite* HighlightRowSprite;
	UPROPERTY(EditAnywhere, Category = "Anim")
	class UPaperSprite* ArrowSprite;
	UPROPERTY(EditAnywhere, Category = "Anim")
	class UPaperFlipbook* BlockDestroyAnim;
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAudioComponent* TetrineTheme;
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAudioComponent* DropSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAudioComponent* OneCorrectSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAudioComponent* AllCorrectSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAudioComponent* OneWrongSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAudioComponent* AllWrongSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAudioComponent* RotateSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAudioComponent* TickSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAudioComponent* ExplosionSound;
	UPROPERTY(EditAnywhere, Category = "Audio")
	class UAudioComponent* LevelUpSound;
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
	UPROPERTY(EditAnywhere, Category = "Movement")
	float ArrowMiniTimeLimit;
	UPROPERTY(EditAnywhere, Category = "Movement")
	float ArrowMiniTimeElapsed;
	UPROPERTY(EditAnywhere, Category = "Anim")
	float RowDestroyAnimTimeElapsed;
	UPROPERTY(EditAnywhere, Category = "Anim")
	float RowDestroyAnimTimeLimit;
	float PreviousHorizontalMove;
	float CurrentHorizontalMove;
	float PreviousVerticalMove;
	float CurrentVerticalMove;
	bool bIsFastHorizontal;
	bool bHasInitiatedHorizMove;
	bool bIsFastFall;
	UPROPERTY(VisibleAnywhere,Category="Game")
	bool bHasTetrominoLanded;
	UPROPERTY(VisibleAnywhere, Category = "Game")
	bool bHasMatchStarted;
	UPROPERTY(BlueprintReadWrite,VisibleAnywhere, Category = "Game")
	bool bIsGameOver;
	UPROPERTY(VisibleAnywhere, Category = "Game")
	int8 MaxWrongTries;
	UPROPERTY(VisibleAnywhere, Category = "Game")
	int8 CurrentWrongTries;
	UPROPERTY(BlueprintReadWrite,EditAnywhere, Category = "Game")
	int Lines;
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Game")
	int Level;
	bool bIsRotating;
	bool bIsRotationKeyHeld;
	bool bIsInstantDropped;
	bool bIsInstantDropKeyHeld;
	bool bHasChangedPositions;
	bool bHasRowsToDelete;
	bool bIsKeyProcessed;
	bool bhasSavedTetromino;
	bool bIsSaveTetroKeyHeld;
	bool bIsRowDestroyAnimFin;
	bool bIsArrowMiniFinished;

	UPROPERTY(EditAnywhere, Category = "Game")
	float FinalFallTL;
	UPROPERTY(EditAnywhere, Category = "Game")
	float FinalLandedTL;
	UPROPERTY(EditAnywhere, Category = "Game")
	float FinalArrowMiniTL;
	UPROPERTY(EditAnywhere, Category = "Game")
	float FallMultiplier;
	UPROPERTY(EditAnywhere, Category = "Game")
	float LandedMultiplier;
	UPROPERTY(EditAnywhere, Category = "Game")
	float ArrowMiniMultiplier;
	UPROPERTY(EditAnywhere, Category = "Game")
	float InitialFallTL;
	UPROPERTY(EditAnywhere, Category = "Game")
	float InitialLandedTL;
	UPROPERTY(EditAnywhere, Category = "Game")
	float InitialArrowMiniTL;
	int8 TickedBlocksToColor;

	TArray<FVector2D> OldGhostPositions;
	TArray<FVector2D> RotationMatrix;
	UPROPERTY(VisibleAnywhere, Category="Game")
	TArray<FString> ArrowSequence;
	UPROPERTY(VisibleAnywhere, Category = "Game")
	FString CurrentArrow;
	UPROPERTY(VisibleAnywhere, Category = "Game")
	FVector2D ArrowSequencePosition;
	UPROPERTY(VisibleAnywhere, Category = "Game")
	int8 ExtraRowsToDelete;

	// methods
	class ATetromino* SpawnTetromino();
	class AGrid* SpawnGrid();
	FString GenerateRandomTetromino();
	bool HasReachedTimeLimit(float &elapsed, float &timeLimit);
	void UpdateFallElapsed(float deltaTime);
	void UpdateHorizontalElapsed(float deltaTime);
	bool UpdateLandedElapsed(float deltaTime);
	void MoveHorizontally(float axisValue);
	void MoveVertically(float axisValue);
	FVector2D GetHorizontalMovement();
	TArray<int8> FilterForDeletion(TArray<int8> potentialRows);
	void DeleteRows(TArray<int8> deletionRows);
	void UpdateRotations();
	void UpdateGhostTetromino();
	void InstantDropPressed();
	void InstantDropReleased();
	void InstantDrop();
	void StartDeletionProcess(int8 extraRowsToDelete);
	bool UpdateArrowMiniGame(float deltaTime);
	void CalculateArrowSequence();
	TArray<FString> GetArrowSequence();
	void MapTetrominoArrows();
	void UpdateArrowMiniTimerBar();
	void SaveTetroPressed();
	void SaveTetroReleased();
	void SaveTetromino();
	int GetLevel();
	void SetLevel(int level);
	void ChangeLevel();
	int GetMultiplier();
	void CalculateMultiplier();
	void UpdateRowDeletion();
	bool IsRowDeletionAnimFin(float deltaTime);
	float SetUpRowsDestroyAnim(TArray<int8> rowsToDestroy);
	void SetDownRowsDestroyAnim(TArray<int8> rowsToDestroy);

	// scoring system - might be placed into a separate actor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Game")
	int Score;
	UPROPERTY(EditAnywhere,BluePrintReadWrite, Category = "Game")
	int ScoreMultiplier;
	UPROPERTY(EditAnywhere, Category = "Game")
	float TetrominoOnGridTimer;
	int GetScore();
	void SetScore(int score);
	int CalculateRowDropScore(TArray<int8> rowsDeleted);
	int CalculateTetroDropScore();
	void AddToScore(int score);
	void SpawnScoreBox(FString score,FVector scoreBoxLocation);
	UPROPERTY(EditAnywhere, Category = "Game")
	FVector ScoreBoxLocation;
	UPROPERTY(EditAnywhere, Category = "Game")
	FVector LevelUpgradeLocation;
};