// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetrine.h"
#include "Possessor.h"
#include "Tetromino.h"
#include "Block.h"
#include "Grid.h"

DEFINE_LOG_CATEGORY(Possessor_log);
// Sets default values
APossessor::APossessor()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentTetromino = nullptr;
	//grid = nullptr;
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	RootComponent = MainCamera;

	NextTetromino = "none";
	FallTimeElapsed = 0.0f;
	FastFallTimeElapsed = 0.0f;
	HorizontalTimeElapsed = 0.0f;
	FastHorizTimeElapsed = 0.0f;
	LandedTimeElapsed = 0.0f;
	PreviousHorizontalMove = 0.0f;
	CurrentHorizontalMove = 0.0f;

	bIsFastFall = false;
	bIsFastHorizontal = false;
	bHasMatchStarted = false;
	bHasTetrominoLanded = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	FallTimeLimit = 1.0f;
	FastFallTimeLimit = 0.1f;
	HorizontalTimeLimit = 0.3f;
	FastHorizTimeLimit = 0.05f;
	LandedTimeLimit = 0.5f;

	rotationMatrix.Emplace(FVector2D(0, -1));
	rotationMatrix.Emplace(FVector2D(1, 0));
}

// Called when the game starts or when spawned
void APossessor::BeginPlay()
{
	Super::BeginPlay();

	MainCamera->SetWorldLocation(FVector(1000.0f, 7000.0f, 3000.0f));
	MainCamera->SetWorldRotation(FRotator(0.0f, -90.0f, 0.0f));
}

// Called every frame
void APossessor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasMatchStarted)
	{
		if (grid == nullptr) { UE_LOG(Possessor_log, Log, TEXT("Grid1 == nullptr")); return; }

		if (CurrentTetromino == nullptr)
		{
			CurrentTetromino = SpawnTetromino();
			CurrentTetromino->SpawnShape(NextTetromino);
			NextTetromino = GenerateRandomTetromino();
			CurrentTetromino->MoveTetrominoOnGrid(FVector2D(0, 0), grid);
			bHasTetrominoLanded = false;
		}

		if (bIsRotating)
		{
			TArray<FVector2D> oldPositions = Obtain2DBlockPositions();
			TArray<FVector2D> newPositions;
			if (CanRotate(oldPositions, &newPositions))
			{
				ApplyRotation(newPositions);
			}
			bIsRotating = false;
		}

		if (CurrentHorizontalMove != 0.0f) { UpdateHorizontalElapsed(DeltaTime); }
		UpdateFallElapsed(DeltaTime);
		if (bHasTetrominoLanded) { UpdateLandedElapsed(DeltaTime); }
	}
	else
	{
		if (GetWorld() == nullptr) { UE_LOG(Possessor_log, Log, TEXT("GetWorld1 == nullptr")); return; }
		//grid = SpawnGrid();
		if (grid == nullptr) { UE_LOG(Possessor_log, Log, TEXT("Grid2 == nullptr")); return; }

		CurrentTetromino = SpawnTetromino();
		if (CurrentTetromino == nullptr) { UE_LOG(Possessor_log, Log, TEXT("Tetro2 == nullptr")); return; }
		//CurrentTetromino->SpawnShape(GenerateRandomTetromino());
		CurrentTetromino->SpawnShape("t");
		CurrentTetromino->MoveTetrominoOnGrid(FVector2D(0, 0), grid);

		NextTetromino = GenerateRandomTetromino();
		bHasMatchStarted = true;
	}
}

// Called to bind functionality to input
void APossessor::SetupPlayerInputComponent(class UInputComponent* InputComponent)
{
	Super::SetupPlayerInputComponent(InputComponent);

	InputComponent->BindAxis("MoveHorizontal", this, &APossessor::MoveHorizontal);
	InputComponent->BindAxis("MoveDown", this, &APossessor::MoveDown);

	InputComponent->BindAction("Rotate", EInputEvent::IE_Pressed, this, &APossessor::RotateKeyPressed);
	InputComponent->BindAction("Rotate", EInputEvent::IE_Released, this, &APossessor::RotateKeyReleased);
}

AGrid* APossessor::SpawnGrid()
{
	return GetWorld()->SpawnActor<AGrid>(AGrid::StaticClass());
}

ATetromino* APossessor::SpawnTetromino()
{
	return GetWorld()->SpawnActor<ATetromino>(ATetromino::StaticClass());
}

FString APossessor::GenerateRandomTetromino()
{ // we know there are 7 different types of blocks

	float randy = FMath::RandRange(0.0f, 1.0f); // break up into evenly random block probabilities
	if (randy < .143) { return "square"; }
	else if (randy < .143 * 2) { return "right bicep"; }
	else if (randy < .143 * 3) { return "left bicep"; }
	else if (randy < .143 * 4) { return "flat"; }
	else if (randy < .143 * 5) { return "s"; }
	else if (randy < .143 * 6) { return  "z"; }
	else { return "t"; }
}

bool APossessor::HasReachedTimeLimit(float &elapsed, float &timeLimit)
{
	if (elapsed >= timeLimit)
	{
		elapsed = 0.0f;
		return true;
	}
	return false;
}

void APossessor::UpdateFallElapsed(float deltaTime)
{
	if (bIsFastFall)
	{
		FastFallTimeElapsed += deltaTime;
		if (HasReachedTimeLimit(FastFallTimeElapsed, FastFallTimeLimit))
		{
			FVector2D movement = FVector2D(0, -1);
			if (CurrentTetromino->DoesTetrominoCollide(movement, grid) == false) // recall grid starts from bottom left and goes up right 
			{
				CurrentTetromino->MoveTetrominoOnGrid(movement, grid);
				LandedTimeElapsed = 0.0f;
				bHasTetrominoLanded = false;
			}
			else // if block cant move down then we know it landed (were in fall update) so we start the timer for that
			{
				bHasTetrominoLanded = true;
			}
		}
	}
	else 
	{
		FallTimeElapsed += deltaTime;
		if (HasReachedTimeLimit(FallTimeElapsed, FallTimeLimit))
		{
			FVector2D movement = FVector2D(0, -1);
			if (CurrentTetromino->DoesTetrominoCollide(movement, grid) == false)
			{
				CurrentTetromino->MoveTetrominoOnGrid(movement, grid);
				LandedTimeElapsed = 0.0f;
				bHasTetrominoLanded = false;
			}
			else // if block cant move down then we know it landed (were in fall update) so we start the timer for that
			{
				bHasTetrominoLanded = true;
			}
		}
	}
}

void APossessor::UpdateHorizontalElapsed(float deltaTime)
{
	if (CurrentHorizontalMove == 0) { return; }
	else if (!bHasInitiatedHorizMove)
	{
		bHasInitiatedHorizMove = true;
		FVector2D movement = GetHorizontalMovement();
		if (CurrentTetromino->DoesTetrominoCollide(movement, grid) == false)
		{
			CurrentTetromino->MoveTetrominoOnGrid(movement, grid);
		}
	}
	else if (!bIsFastHorizontal)
	{
		HorizontalTimeElapsed += deltaTime;
		if (HasReachedTimeLimit(HorizontalTimeElapsed, HorizontalTimeLimit))
		{
			FVector2D movement = GetHorizontalMovement();
			if (CurrentTetromino->DoesTetrominoCollide(movement, grid) == false)
			{
				CurrentTetromino->MoveTetrominoOnGrid(movement, grid);
				bIsFastHorizontal = true;
			}
		}
	}
	else
	{
		FastHorizTimeElapsed += deltaTime;
		if (HasReachedTimeLimit(FastHorizTimeElapsed, FastHorizTimeLimit))
		{
			FVector2D movement = GetHorizontalMovement();
			if (CurrentTetromino->DoesTetrominoCollide(movement, grid) == false)
			{
				CurrentTetromino->MoveTetrominoOnGrid(movement, grid);
			}
			FastHorizTimeElapsed = 0.0f;
		}
	}
}

void APossessor::UpdateLandedElapsed(float deltaTime)
{
	LandedTimeElapsed += deltaTime;
	if (HasReachedTimeLimit(LandedTimeElapsed, LandedTimeLimit))
	{
		if (CurrentTetromino->DoesTetrominoCollide(FVector2D(0,-1),grid) == true) // block must be below in order to drop block
		{
			TArray<int8> RowsToDelete = FilterForDeletion(CurrentTetromino->GetTetrominoRows());
			CurrentTetromino->EndLife(grid);
			DeleteRows(RowsToDelete);
			grid->DropRows();
			CurrentTetromino = nullptr;
		}
		LandedTimeElapsed = 0.0f;
		bHasTetrominoLanded = false;
		FallTimeElapsed = 0.0f;
	}
}

void APossessor::MoveHorizontal(float axisValue)
{
	PreviousHorizontalMove = CurrentHorizontalMove;
	CurrentHorizontalMove = axisValue;
	if (CurrentHorizontalMove == 0.0f ||
		(PreviousHorizontalMove > 0.0f && CurrentHorizontalMove < 0.0f) ||
		(PreviousHorizontalMove < 0.0f && CurrentHorizontalMove > 0.0f))
	{
		bIsFastHorizontal = false;
		bHasInitiatedHorizMove = false;
		FastHorizTimeElapsed = 0.0f;
		HorizontalTimeElapsed = 0.0f;
	}
}

void APossessor::MoveDown(float axisValue)
{
	if (axisValue <= 0.0f)
	{
		bIsFastFall = false;
		FastFallTimeElapsed = 0.0f;
	}
	else
	{
		bIsFastFall = true;
		FallTimeElapsed = 0.0f;
	}
}

FVector2D APossessor::GetHorizontalMovement()
{
	if (CurrentHorizontalMove > 0) { return FVector2D(1, 0); }
	else if (CurrentHorizontalMove < 0) { return FVector2D(-1, 0); }
	else { return FVector2D(0, 0); }
}

bool APossessor::CanRotate(TArray<FVector2D> oldPositions, TArray<FVector2D> *newPositions)
{
	TArray<FVector2D> positionsToCalculate;

	UE_LOG(Possessor_log, Log, TEXT("Rotation Matrix: %s"), *rotationMatrix[0].ToString());
	UE_LOG(Possessor_log, Log, TEXT("Rotation Matrix: %s"), *rotationMatrix[1].ToString());
	UE_LOG(Possessor_log, Log, TEXT("Block Pivot Position: %s"), *CurrentTetromino->GetPivotPosition().ToString());
	//step 1. subtract from origin position.
	for (int i = 0;i < 4; ++i)
	{
		FVector2D offsetPosition;
		offsetPosition = oldPositions[i] - CurrentTetromino->GetPivotPosition();
		//UE_LOG(Possessor_log, Log, TEXT("SUBTRACT FROM ORIGIN %d: (%f,%f)"), i, offsetPosition.Y, offsetPosition.X);
		positionsToCalculate.Add(offsetPosition);
	}

	//step 2. multiply by rotation matrix.
	//[0 -1] [ height ]    =   [newHeight = 0 * height + -1 * width]   = [height, width]
	//[1  0] [ width ]	   =   [newWidth = 1 * height + 0 * width ]
	for (int i = 0;i < 4; ++i)
	{
		FVector2D offsetPosition = positionsToCalculate[i];
		UE_LOG(Possessor_log, Log, TEXT("offsetPosition %d: %s"), i, *offsetPosition.ToString());
		float newRow = rotationMatrix[0].X * offsetPosition.X + rotationMatrix[0].Y * offsetPosition.Y;
		float newCol = rotationMatrix[1].X * offsetPosition.X + rotationMatrix[1].Y * offsetPosition.Y;
		//UE_LOG(Possessor_log, Log, TEXT("Normalized vector %d: (%f, %f)"), i, newRow, newCol);
		positionsToCalculate[i] = FVector2D(newRow, newCol);
		UE_LOG(Possessor_log, Log, TEXT("Normalized vector %d: %s"), i, *positionsToCalculate[i].ToString());
	}

	//Possessor_log: offsetPosition 0: X=0.000 Y=1.000
	
	//Possessor_log: Normalized vector 0: x=0.000 Y=1.000

	//step 3. add origin position to the product of each block position calculated in step 2
	for (int i = 0;i < 4; ++i)
	{
		positionsToCalculate[i] = positionsToCalculate[i] + CurrentTetromino->GetPivotPosition();
	}

	for (int i = 0;i < 4;++i)
	{
		newPositions->Emplace(positionsToCalculate[i]);
	}

	//newPositions->Append(positionsToCalculate);

	//check if any of the newpositions overlap with existing blocks on grid.
	//if blocks overlap..... return false

	return true;
}
void APossessor::ApplyRotation(TArray<FVector2D> newPositions)
{
	//TArray<FVector2D> oldPositions(Obtain2DBlockPositions());
	for (int i = 0; i < 4; ++i) // remove old block image
	{
		FVector2D oldPosition = CurrentTetromino->blocks[i]->GetPosition();
		grid->GetBlock(oldPosition)->SetBlockStatus(0); // remember grid has matrix, and tetromino has separate set of blocks
		grid->GetBlock(oldPosition)->SetBlockSprite(0);
	}

	for (int i = 0;i < 4; ++i)
	{
		grid->GetBlock(newPositions[i])->SetBlockStatus(2);
		grid->GetBlock(newPositions[i])->SetBlockSprite(2);
		CurrentTetromino->blocks[i]->SetPosition(newPositions[i]);
		FVector rotatedPosition = CurrentTetromino->blocks[i]->GetDimensions().X * FVector(newPositions[i].X, 0, newPositions[i].Y);
		CurrentTetromino->blocks[i]->SetActorLocation(rotatedPosition);
	}
}

//returns a TArray of of FVector2D's that change the position of each block in the shape by 1 block cell.
TArray<FVector2D> APossessor::WallKick(TArray<FVector2D> overlappedPositions)
{
	TArray<FVector2D> correctPositions;
	for (int i = 0;i < 4; ++i)
	{
		float correctColumn = overlappedPositions[i].X - 1;
		correctPositions.Add(FVector2D(correctColumn, overlappedPositions[i].Y));
	}

	return correctPositions;
}

TArray<FVector2D> APossessor::Obtain2DBlockPositions()
{
	TArray<FVector2D> blockPositions;
	for (int i = 0;i < 4; ++i)
		blockPositions.Add(CurrentTetromino->blocks[i]->GetPosition());
	
	return blockPositions;
}

void APossessor::RotateKeyPressed()
{
	if (!bIsRotationKeyHeld && !bIsRotating) { bIsRotating = true; }
	bIsRotationKeyHeld = true;
}
void APossessor::RotateKeyReleased()
{
	bIsRotationKeyHeld = false;
}

TArray<int8> APossessor::FilterForDeletion(TArray<int8> potentialRows)
{
	TArray<int8> resultingRows;
	for (int i = 0; i < potentialRows.Num(); ++i)
	{
		if (grid->ShouldDeleteRow(potentialRows[i]))
		{
			resultingRows.AddUnique(potentialRows[i]);
		}
	}
	return resultingRows;
}

void APossessor::DeleteRows(TArray<int8> deletionRows)
{
	for (int i = 0; i < deletionRows.Num(); ++i)
	{
		grid->DeleteRow(deletionRows[i]);
	}
}