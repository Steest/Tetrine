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
		CurrentTetromino->SpawnShape(GenerateRandomTetromino());
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
			CurrentTetromino->EndLife(grid);
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