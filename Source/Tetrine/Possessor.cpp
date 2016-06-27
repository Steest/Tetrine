// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetrine.h"
#include "Possessor.h"
#include "Tetromino.h"
#include "Block.h"
#include "Grid.h"
#include "PaperSpriteComponent.h"

DEFINE_LOG_CATEGORY(Possessor_log);
// Sets default values
APossessor::APossessor()
{
	PrimaryActorTick.bCanEverTick = true;

	CurrentTetromino = nullptr;
	//grid = nullptr;
	MainCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("MainCamera"));
	RootComponent = MainCamera;
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> GhostSpriteAsset(TEXT("PaperSprite'/Game/Art/ghost_block_Sprite.ghost_block_Sprite'"));
	GhostSprite = GhostSpriteAsset.Get();

	NextTetromino = "none";
	FallTimeElapsed = 0.0f;
	FastFallTimeElapsed = 0.0f;
	HorizontalTimeElapsed = 0.0f;
	FastHorizTimeElapsed = 0.0f;
	LandedTimeElapsed = 0.0f;
	PreviousHorizontalMove = 0.0f;
	CurrentHorizontalMove = 0.0f;

	bool bIsFastHorizontal = false;
	bool bHasInitiatedHorizMove = false;
	bool bIsFastFall = false;
	bool bHasTetrominoLanded = false;
	bool bHasMatchStarted = false;
	bool bIsRotating = false;
	bool bIsRotationKeyHeld = false;
	bool bHasChangedPositions = true;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	FallTimeLimit = 1.0f;
	FastFallTimeLimit = 0.1f;
	HorizontalTimeLimit = 0.3f;
	FastHorizTimeLimit = 0.05f;
	LandedTimeLimit = 0.5f;
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
			bHasChangedPositions = true;
		}

		if (bIsRotating) { UpdateRotations(); bHasChangedPositions = true; }
		if (CurrentHorizontalMove != 0.0f) { UpdateHorizontalElapsed(DeltaTime);  bHasChangedPositions = true;}
		UpdateFallElapsed(DeltaTime);
		if (bHasChangedPositions) { UpdateGhostTetromino(); }
		if (bIsInstantDropped) { InstantDrop(); bHasTetrominoLanded = false; bIsInstantDropped = false; }
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
		OldGhostPositions = CurrentTetromino->GetPositions();
		NextTetromino = GenerateRandomTetromino();
		bHasMatchStarted = true;
		bHasChangedPositions = true;
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

	InputComponent->BindAction("InstantDrop", EInputEvent::IE_Pressed, this, &APossessor::InstantDropPressed);
	InputComponent->BindAction("InstantDrop", EInputEvent::IE_Released, this, &APossessor::InstantDropReleased);
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

void APossessor::UpdateRotations()
{
	TArray<FVector2D> newPositions = CurrentTetromino->CalculateRotation();
	CurrentTetromino->ShiftPositions(newPositions, grid);
	if (CurrentTetromino->CanShiftPositions(newPositions, grid))
	{
		CurrentTetromino->ApplyRotation(newPositions, grid);
	}
	bIsRotating = false;
}

void APossessor::UpdateGhostTetromino()
{
	// remove the old ghost blocks
	for (int i = 0; i < OldGhostPositions.Num(); ++i)
	{
		grid->GetBlock(OldGhostPositions[i])->SetBlockSprite(grid->GetBlock(OldGhostPositions[i])->GetBlockStatus());
	}

	TArray<FVector2D> GhostPositions = CurrentTetromino->GetPositions();
	int i = -1;
	while (!CurrentTetromino->DoesTetrominoCollide(FVector2D(0, i), grid)) { --i; }
	for (int j = 0; j < 4; ++j)
	{
		OldGhostPositions[j] = GhostPositions[j] + FVector2D(0, i+1);
		grid->GetBlock(OldGhostPositions[j])->BlockHitBox->SetSprite(GhostSprite);
	}
	bHasChangedPositions = false;
}

void APossessor::InstantDropPressed()
{
	if (!bIsInstantDropKeyHeld && !bIsInstantDropped) { bIsInstantDropped = true; }
	bIsInstantDropKeyHeld = true;
}

void APossessor::InstantDropReleased()
{
	bIsInstantDropKeyHeld = false;
}

void APossessor::InstantDrop()
{
	for (int i = 0; i < OldGhostPositions.Num(); ++i)
	{
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->SetBlockStatus(0);
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->SetBlockSprite(0);
	
		CurrentTetromino->blocks[i]->SetPosition(OldGhostPositions[i]);
		CurrentTetromino->blocks[i]->SetActorLocation(CurrentTetromino->blocks[i]->GetDimensions().X * FVector(OldGhostPositions[i].X, 0, OldGhostPositions[i].Y));
	}
	CurrentTetromino->EndLife(grid);
	CurrentTetromino = nullptr;
}