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
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> HighlightArrowAsset(TEXT("PaperSprite'/Game/Art/Highlight2Arrow_Sprite.Highlight2Arrow_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> HighlightRowAsset(TEXT("PaperSprite'/Game/Art/HighlightArrow_Sprite.HighlightArrow_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> ArrowSpriteAsset(TEXT("PaperSprite'/Game/Art/Arrow_Sprite.Arrow_Sprite'"));
	
	GhostSprite = GhostSpriteAsset.Get();
	HighlightArrowSprite = HighlightArrowAsset.Get();
	HighlightRowSprite = HighlightRowAsset.Get();
	ArrowSprite = ArrowSpriteAsset.Get();
	
	TetrineTheme = CreateDefaultSubobject<UAudioComponent>(TEXT("TetrineTheme"));
	ConstructorHelpers::FObjectFinder<USoundBase> TetrineThemeAsset(TEXT("SoundWave'/Game/SFX/TetrisTheme.TetrisTheme'"));
	TetrineTheme->SetSound(TetrineThemeAsset.Object);

	DropSound = CreateDefaultSubobject<UAudioComponent>(TEXT("DropSound"));
	ConstructorHelpers::FObjectFinder<USoundBase> DropSoundAsset(TEXT("SoundWave'/Game/SFX/Drop1.Drop1'"));
	DropSound->SetSound(DropSoundAsset.Object);
	
	OneCorrectSound = CreateDefaultSubobject<UAudioComponent>(TEXT("OneCorrectSound"));
	ConstructorHelpers::FObjectFinder<USoundBase> OneCorrectSoundAsset(TEXT("SoundWave'/Game/SFX/OneCorrect2.OneCorrect2'"));
	OneCorrectSound->SetSound(OneCorrectSoundAsset.Object);

	AllCorrectSound = CreateDefaultSubobject<UAudioComponent>(TEXT("AllCorrectSound"));
	ConstructorHelpers::FObjectFinder<USoundBase> AllCorrectSoundAsset(TEXT("SoundWave'/Game/SFX/AllCorrect1.AllCorrect1'"));
	AllCorrectSound->SetSound(AllCorrectSoundAsset.Object);

	OneWrongSound = CreateDefaultSubobject<UAudioComponent>(TEXT("OneWrongSound"));
	ConstructorHelpers::FObjectFinder<USoundBase> OneWrongSoundAsset(TEXT("SoundWave'/Game/SFX/OneWrong1.OneWrong1'"));
	OneWrongSound->SetSound(OneWrongSoundAsset.Object);

	AllWrongSound = CreateDefaultSubobject<UAudioComponent>(TEXT("AllWrongSound"));
	ConstructorHelpers::FObjectFinder<USoundBase> AllWrongSoundAsset(TEXT("SoundWave'/Game/SFX/AllWrong1.AllWrong1'"));
	AllWrongSound->SetSound(AllWrongSoundAsset.Object);

	RotateSound = CreateDefaultSubobject<UAudioComponent>(TEXT("RotateSound"));
	ConstructorHelpers::FObjectFinder<USoundBase> RotateSoundAsset(TEXT("SoundWave'/Game/SFX/Rotate1.Rotate1'"));
	RotateSound->SetSound(RotateSoundAsset.Object);

	NextTetromino = "none";
	SavedTetromino = "none";
	LandedTetromino = "none";
	FallTimeElapsed = 0.0f;
	FastFallTimeElapsed = 0.0f;
	HorizontalTimeElapsed = 0.0f;
	FastHorizTimeElapsed = 0.0f;
	LandedTimeElapsed = 0.0f;
	PreviousHorizontalMove = 0.0f;
	CurrentHorizontalMove = 0.0f;
	ArrowMiniTimeElapsed = 0.0f;

	bIsFastHorizontal = false;
	bHasInitiatedHorizMove = false;
	bIsFastFall = false;
	bHasTetrominoLanded = false;
	bHasMatchStarted = false;
	bIsGameOver = false;
	bIsRotating = false;
	bIsRotationKeyHeld = false;
	bHasChangedPositions = true;
	bhasSavedTetromino = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RotationMatrix.Emplace(FVector2D(0, 1));
	RotationMatrix.Emplace(FVector2D(-1, 0));
	MaxWrongTries = 2;
	CurrentWrongTries = 0;
	Lines = 0;
	Level = 1;
	Score = 0;
	ScoreMultiplier = 1;

	FinalFallTL = 0.05f;
	FinalLandedTL = 0.2f;
	FinalArrowMiniTL = 1.25f;
	InitialFallTL = FallTimeLimit = 1.00f;
	InitialLandedTL = LandedTimeLimit = 0.50f;
	InitialArrowMiniTL = ArrowMiniTimeLimit = 3.50f;
	FastFallTimeLimit = 0.035f;
	HorizontalTimeLimit = 0.175f;
	FastHorizTimeLimit = 0.0175f;
	FallMultiplier = 0.050f;
	LandedMultiplier = 0.025f;
	ArrowMiniMultiplier = 0.1f;

	TetrominoOnGridTimer = 0.0f;
}

void APossessor::BeginPlay()
{
	Super::BeginPlay();

	MainCamera->SetWorldLocation(FVector(1000.0f, 7000.0f, 3000.0f));
	MainCamera->SetWorldRotation(FRotator(0.0f, -90.0f, 0.0f));
	MainCamera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	MainCamera->SetOrthoWidth(15000.0f);
	MainCamera->SetConstraintAspectRatio(true);

	DropSound->Stop();
	OneCorrectSound->Stop();
	AllCorrectSound->Stop();
	OneWrongSound->Stop();
	AllWrongSound->Stop();
	RotateSound->Stop();
	TetrineTheme->Stop();
}

void APossessor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bHasMatchStarted && !bIsGameOver)
	{
		if (grid == nullptr) { UE_LOG(Possessor_log, Error, TEXT("Grid1 == nullptr")); return; }
		if (CurrentTetromino == nullptr)
		{
			CurrentTetromino = SpawnTetromino();
			CurrentTetromino->SpawnShape(NextTetromino);
			if (NextTetromino != "none") { CurrentTetromino->SpawnShape(NextTetromino); }
			else { CurrentTetromino->SpawnShape(GenerateRandomTetromino()); }
			LandedTetromino = NextTetromino;
			NextTetromino = GenerateRandomTetromino();
			CurrentTetromino->MoveTetrominoOnGrid(FVector2D(0, 0), grid);
			bHasTetrominoLanded = false;
			bHasChangedPositions = true;
		}
		if (!bHasRowsToDelete)
		{
			TetrominoOnGridTimer += DeltaTime;
			if (bIsSaveTetroKeyHeld && !bhasSavedTetromino) { SaveTetromino(); CurrentTetromino->Destroy(); CurrentTetromino = nullptr; bhasSavedTetromino = true; return; }
			if (CurrentHorizontalMove != 0.0f) { UpdateHorizontalElapsed(DeltaTime);  bHasChangedPositions = true; }
			UpdateFallElapsed(DeltaTime);
			if (bIsInstantDropped) { InstantDrop(); bHasTetrominoLanded = true; bIsInstantDropped = false; LandedTimeElapsed = LandedTimeLimit; }
			if (bIsRotating) { UpdateRotations(); bHasChangedPositions = true; }
			if (bHasChangedPositions) { UpdateGhostTetromino(); }
			if (bHasTetrominoLanded && UpdateLandedElapsed(DeltaTime)) { bHasRowsToDelete = true; CalculateArrowSequence(); bIsKeyProcessed = true; }
		}
		else 
		{ 
			if (CurrentWrongTries >= MaxWrongTries || !UpdateArrowMiniGame(DeltaTime)) 
			{
				StartDeletionProcess(ExtraRowsToDelete); 
				bHasRowsToDelete = bHasTetrominoLanded = false; 
				CurrentArrow = "none"; 
				ArrowMiniTimeElapsed = TetrominoOnGridTimer = 0.0f;
				ExtraRowsToDelete = CurrentWrongTries = 0;
				grid->SetRowArrowSprite(ArrowSprite, ArrowSequencePosition.Y);
				bIsGameOver = grid->IsBlockInDeadZone();
				bhasSavedTetromino = false;
			}
		}
	}
	else if (bIsGameOver)
	{
		TetrineTheme->Stop();
		grid->Clear();
	}
	else
	{
		if (GetWorld() == nullptr) { UE_LOG(Possessor_log, Error, TEXT("GetWorld1 == nullptr")); return; }
		//grid = SpawnGrid();
		if (grid == nullptr) { UE_LOG(Possessor_log, Error, TEXT("Grid2 == nullptr")); return; }

		CurrentTetromino = SpawnTetromino();
		if (CurrentTetromino == nullptr) { UE_LOG(Possessor_log, Error, TEXT("Tetro2 == nullptr")); return; }
		CurrentTetromino->SpawnShape(GenerateRandomTetromino());
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

	InputComponent->BindAxis("MoveHorizontally", this, &APossessor::MoveHorizontally);
	InputComponent->BindAxis("MoveVertically", this, &APossessor::MoveVertically);

	InputComponent->BindAction("InstantDrop", EInputEvent::IE_Pressed, this, &APossessor::InstantDropPressed);
	InputComponent->BindAction("InstantDrop", EInputEvent::IE_Released, this, &APossessor::InstantDropReleased);

	InputComponent->BindAction("SaveTetro", EInputEvent::IE_Pressed, this, &APossessor::SaveTetroPressed);
	InputComponent->BindAction("SaveTetro", EInputEvent::IE_Released, this, &APossessor::SaveTetroReleased);
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
	if (randy <= .143f) { return "square"; }
	else if (randy <= .143f * 2.0f) { return "right bicep"; }
	else if (randy <= .143f * 3.0f) { return "left bicep"; }
	else if (randy <= .143f * 4.0f) { return "flat"; }
	else if (randy <= .143f * 5.0f) { return "s"; }
	else if (randy <= .143f * 6.0f) { return  "z"; }
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

bool APossessor::UpdateLandedElapsed(float deltaTime)
{
	LandedTimeElapsed += deltaTime;
	bool hasRowsToDelete = false;
	if (HasReachedTimeLimit(LandedTimeElapsed, LandedTimeLimit))
	{
		if (CurrentTetromino->DoesTetrominoCollide(FVector2D(0,-1),grid) == true)// block must be below in order to drop block																 
		{   
			if(FilterForDeletion(CurrentTetromino->GetTetrominoRows()).Num() != 0) 
			{ // checks if there is a row to delete, to which mini game is activated and deletion is deferred until after
				hasRowsToDelete = true;
			}
			else
			{
				StartDeletionProcess(0);
				LandedTimeElapsed = 0.0f;
				bHasTetrominoLanded = false;
				FallTimeElapsed = 0.0f;
				DropSound->Play();
				bIsGameOver = grid->IsBlockInDeadZone();
				bhasSavedTetromino = false;
				TetrominoOnGridTimer = 0.0f;
			}
		}
	}
	return hasRowsToDelete;
}

void APossessor::MoveHorizontally(float axisValue)
{
	PreviousHorizontalMove = CurrentHorizontalMove;
	CurrentHorizontalMove = axisValue;
	if (CurrentHorizontalMove == 0.0f ||
		(PreviousHorizontalMove >= 0.0f && CurrentHorizontalMove < 0.0f) ||
		(PreviousHorizontalMove <= 0.0f && CurrentHorizontalMove > 0.0f))
	{
		bIsFastHorizontal = false;
		bHasInitiatedHorizMove = false;
		FastHorizTimeElapsed = 0.0f;
		HorizontalTimeElapsed = 0.0f;

		if (CurrentHorizontalMove > 0.0f && bIsKeyProcessed ) { CurrentArrow = "right"; bIsKeyProcessed = false; }
		else if (CurrentHorizontalMove < 0.0f && bIsKeyProcessed ) { CurrentArrow = "left"; bIsKeyProcessed = false; }
		else if (CurrentVerticalMove == 0.0f) { CurrentArrow = "none"; bIsKeyProcessed = true; }
	}
}

void APossessor::MoveVertically(float axisValue)
{
	PreviousVerticalMove = CurrentVerticalMove;
	CurrentVerticalMove = axisValue;
	if (CurrentVerticalMove == 0.0f) 
	{ 
		if (CurrentHorizontalMove == 0.0f) { CurrentArrow = "none"; bIsKeyProcessed = true; }
		bIsFastFall = bIsRotationKeyHeld = false;
		FastFallTimeElapsed = 0.0f;
	}
	else if (CurrentVerticalMove > 0.0f) // rotation (up)
	{
		if (!bIsRotationKeyHeld && !bIsRotating) { bIsRotating = true; }
		if (PreviousVerticalMove <= 0.0f && bIsKeyProcessed) { CurrentArrow = "up"; bIsKeyProcessed = false; }
		bIsRotationKeyHeld = true;
		bIsFastFall = false;
		FastFallTimeElapsed = 0.0f;
	}
	else if (CurrentVerticalMove < 0.0f) // fast fall (down)
	{
		if (PreviousVerticalMove >= 0.0f && bIsKeyProcessed) { CurrentArrow = "down"; bIsKeyProcessed = false; }
		bIsFastFall = true;
		bIsRotationKeyHeld = false;
		FallTimeElapsed = 0.0f;
	}
}

FVector2D APossessor::GetHorizontalMovement()
{
	if (CurrentHorizontalMove > 0) { return FVector2D(1, 0); }
	else if (CurrentHorizontalMove < 0) { return FVector2D(-1, 0); }
	else { return FVector2D(0, 0); }
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
	TArray<FVector2D> newPositions = CurrentTetromino->CalculateRotation(RotationMatrix);
	CurrentTetromino->ShiftPositions(newPositions, grid);
	if (CurrentTetromino->CanShiftPositions(newPositions, grid))
	{
		CurrentTetromino->ApplyRotation(newPositions, grid);
		RotateSound->Play();
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
	
		grid->GetBlock(OldGhostPositions[i])->SetBlockStatus(2);
		grid->GetBlock(OldGhostPositions[i])->SetBlockSprite(2);

		CurrentTetromino->blocks[i]->SetPosition(OldGhostPositions[i]);
		CurrentTetromino->blocks[i]->SetActorLocation(CurrentTetromino->blocks[i]->GetDimensions().X * FVector(OldGhostPositions[i].X, 0, OldGhostPositions[i].Y));
	}
}

void APossessor::StartDeletionProcess(int8 extraRowsToDelete)
{
	TArray<int8> RowsToDelete = FilterForDeletion(CurrentTetromino->GetTetrominoRows());
	RowsToDelete = grid->GetExtraRows(RowsToDelete, extraRowsToDelete);
	CurrentTetromino->EndLife(grid);
	DeleteRows(RowsToDelete);
	grid->DropRows();
	CurrentTetromino = nullptr;
	Lines += RowsToDelete.Num();
	//add up new score
	if(bHasRowsToDelete){ CalculateMultiplier(); }
	AddToScore( (CalculateRowDropScore(RowsToDelete) + CalculateTetroDropScore())*GetMultiplier() );
	ChangeLevel();
}

bool APossessor::UpdateArrowMiniGame(float deltaTime)
{
	ArrowMiniTimeElapsed += deltaTime;
	if (!HasReachedTimeLimit(ArrowMiniTimeElapsed, ArrowMiniTimeLimit))
	{
		UpdateArrowMiniTimerBar();
		grid->SetRowArrowSprite(HighlightRowSprite, ArrowSequencePosition.Y);
		grid->SetArrowSprite(HighlightArrowSprite, ArrowSequencePosition);
		if (ArrowSequence.IsValidIndex(ArrowSequencePosition.X) && CurrentArrow == ArrowSequence[ArrowSequencePosition.X])
		{
			++ArrowSequencePosition.X;
			CurrentArrow = "none";
			
			if (ArrowSequencePosition.X == ArrowSequence.Num())
			{
				ExtraRowsToDelete = 2;
				AllCorrectSound->Play();
				return false;
			}
			else { OneCorrectSound->Play(); }
		}
		else if (ArrowSequence.IsValidIndex(ArrowSequencePosition.X) && CurrentArrow != "none")
		{
			++CurrentWrongTries;
			if (CurrentWrongTries >= MaxWrongTries) { AllWrongSound->Play(); }
			else { OneWrongSound->Play(); }
		}
		bIsKeyProcessed = true;
		CurrentArrow = "none";
		return true;
	}
	ArrowMiniTimeElapsed = 0.0f;
	return false; // false == finished mini game
}

void APossessor::CalculateArrowSequence()
{
	ArrowSequence.Empty();
	ArrowSequencePosition.X = 0;
	TArray<int8> rowsToExtract = FilterForDeletion(CurrentTetromino->GetTetrominoRows());
	MapTetrominoArrows(); // grid has "none" as arrow direction for some parts, need to map tetro to grid
	for (int i = 0; i < rowsToExtract.Num(); ++i)
	{
		for (int j = 0; j < grid->GetWidth(); ++j)
		{
			FString arrow = grid->GetBlock(FVector2D(j, rowsToExtract[i]))->GetArrowDirection();
			ArrowSequence.Add(arrow);
		}
		ArrowSequencePosition.Y = rowsToExtract[i];
		break; // only need one row
	}
	grid->SetRowColor("grey",ArrowSequencePosition.Y);
}

TArray<FString> APossessor::GetArrowSequence()
{
	return ArrowSequence;
}

void APossessor::MapTetrominoArrows()
{
	for (int i = 0; i < 4; i++)
	{
		FVector2D tempPos = CurrentTetromino->blocks[i]->GetPosition();
		grid->GetBlock(tempPos)->SetArrowDirection(CurrentTetromino->blocks[i]->GetArrowDirection());
		grid->GetBlock(tempPos)->SetArrowVisibility(1);
		CurrentTetromino->blocks[i]->AddActorLocalOffset(FVector(0.0f, -4.0f, 0.0f));
	}
}

void APossessor::UpdateArrowMiniTimerBar()
{
	int blocksToColor = ArrowMiniTimeElapsed / ArrowMiniTimeLimit*10;
	for (int i = 0; i < blocksToColor; ++i)
	{
		grid->GetBlock(FVector2D(i, ArrowSequencePosition.Y))->ChangeColor("pink");
	}
}

void APossessor::SaveTetroPressed()
{
	bIsSaveTetroKeyHeld = true;
}

void APossessor::SaveTetroReleased()
{
	bIsSaveTetroKeyHeld = false;
}

void APossessor::SaveTetromino()
{
	NextTetromino = SavedTetromino;
	SavedTetromino = CurrentTetromino->Shape;
	for (int i = 0; i < 4; ++i)
	{
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->SetBlockStatus(0);
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->SetBlockSprite(0);
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->SetColor("empty");
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->ChangeColor(0);
		UE_LOG(Possessor_log, Error, TEXT("Saved: i: %d"),i);
		CurrentTetromino->blocks[i]->Destroy();
		CurrentTetromino->blocks[i] = nullptr;
	}
}

int APossessor::GetLevel()
{
	return Level;
}

void APossessor::SetLevel(int level)
{
	Level = level;
}

void APossessor::ChangeLevel()
{
	int level = (Score / 2500)+1;
	SetLevel(level);
	FallTimeLimit = FMath::Clamp(InitialFallTL - (level*FallMultiplier), FinalFallTL, InitialFallTL );
	LandedTimeLimit = FMath::Clamp(InitialLandedTL - (level*LandedMultiplier), FinalLandedTL, InitialLandedTL);
	ArrowMiniTimeLimit = FMath::Clamp(InitialArrowMiniTL - (level*ArrowMiniMultiplier), FinalArrowMiniTL, InitialArrowMiniTL );
}

int APossessor::GetMultiplier()
{
	return ScoreMultiplier;
}

void APossessor::CalculateMultiplier()
{
	if (CurrentWrongTries == 0)
	{
		ScoreMultiplier = FMath::Clamp( (2*ScoreMultiplier) , 0, 8);
	}
	else if (CurrentWrongTries == 1) { return; }
	else { ScoreMultiplier = 1; }
}

int APossessor::GetScore()
{
	return Score;
}

void APossessor::SetScore(int score)
{
	Score = score;
}

int APossessor::CalculateRowDropScore(TArray<int8> rowsDeleted)
{
	int result = 0;
	for (int i = 0; i < rowsDeleted.Num(); ++i)
	{
		int temp = rowsDeleted[i] / 5;
		if (temp == 0) { temp = 100; }
		else if(temp == 1){ temp = 150; }
		else if (temp == 2) { temp = 200; }
		else if (temp == 3) { temp = 250; }
		else { temp = 300; }
		UE_LOG(Possessor_log, Warning, TEXT("ROW DROP SCORE: %d"), temp);
		result += temp;
	}
	return result;
}

int APossessor::CalculateTetroDropScore()
{
	int result = FMath::Clamp( int(((grid->GetHeight()/2) - TetrominoOnGridTimer) * 5),0,300);
	UE_LOG(Possessor_log, Warning, TEXT("TETRO DROP SCORE: %d"),result );
	return result;
}

void APossessor::AddToScore(int score)
{
	Score += score;
}