// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetrine.h"

#include "Possessor.h"
#include "Tetromino.h"
#include "Block.h"
#include "Grid.h"
#include "ScoreBox.h"

#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"
#include "Blueprint/UserWidget.h"

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
	ConstructorHelpers::FObjectFinderOptional<UPaperFlipbook> BlockDestroyAnimAsset(TEXT("PaperFlipbook'/Game/Art/RowDestroyAnim/BlockDestroyAnim.BlockDestroyAnim'"));

	GhostSprite = GhostSpriteAsset.Get();
	HighlightArrowSprite = HighlightArrowAsset.Get();
	HighlightRowSprite = HighlightRowAsset.Get();
	ArrowSprite = ArrowSpriteAsset.Get();
	BlockDestroyAnim = BlockDestroyAnimAsset.Get();

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

	TickSound = CreateDefaultSubobject<UAudioComponent>(TEXT("TickSound"));
	ConstructorHelpers::FObjectFinder<USoundBase> TickSoundAsset(TEXT("SoundWave'/Game/SFX/tick1.tick1'"));
	TickSound->SetSound(TickSoundAsset.Object);
	
	ExplosionSound = CreateDefaultSubobject<UAudioComponent>(TEXT("ExplosionSound"));
	ConstructorHelpers::FObjectFinder<USoundBase> ExplosionSoundAsset(TEXT("SoundWave'/Game/SFX/Explosion1.Explosion1'"));
	ExplosionSound->SetSound(ExplosionSoundAsset.Object);
	
	LevelUpSound = CreateDefaultSubobject<UAudioComponent>(TEXT("LevelUpSound"));
	ConstructorHelpers::FObjectFinder<USoundBase> LevelUpSoundAsset(TEXT("SoundWave'/Game/SFX/LevelUp1.LevelUp1'"));
	LevelUpSound->SetSound(LevelUpSoundAsset.Object);


	NextTetrominos.Add("none");
	NextTetrominos.Add("none");
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
	bIsRowDestroyAnimFin = false;
	bStartUIAnimation = false;
	bStartUILandedAnim = false;
	bStartUISavedAnim = false;

	AutoPossessPlayer = EAutoReceiveInput::Player0;

	RotationMatrix.Emplace(FVector2D(0, 1));
	RotationMatrix.Emplace(FVector2D(-1, 0));
	MaxWrongTries = ExtraRowsToDelete = 2;
	CurrentWrongTries = 0;
	Lines = 0;
	Level = 1;
	Score = 0;
	TickedBlocksToColor = 0;
	ScoreMultiplier = 1;
	
	FinalFallTL = 0.05f;
	FinalLandedTL = 0.5f;
	FinalArrowMiniTL = 1.25f;
	InitialFallTL = FallTimeLimit = 1.25f;
	InitialLandedTL = LandedTimeLimit = 0.50f;
	InitialArrowMiniTL = ArrowMiniTimeLimit = 4.00f;
	FastFallTimeLimit = 0.0225f;
	HorizontalTimeLimit = 0.225f;
	FastHorizTimeLimit = 0.015f;
	FallMultiplier = 0.050f;
	LandedMultiplier = 0.025f;
	ArrowMiniMultiplier = 0.1f;
	RowDestroyAnimTimeLimit = 0.75f;
	TetrominoOnGridTimer = 0.0f;
	ScoreBoxLocation = FVector(-2500.0f, 0.0f, 3750.0f);
	LevelUpgradeLocation = FVector(-2500.0f, 0.0f, 2750.0f);

	MainCamera->SetWorldLocation(FVector(1000.0f, 7000.0f, 3000.0f));
	MainCamera->SetWorldRotation(FRotator(0.0f, -90.0f, 0.0f));
	MainCamera->SetProjectionMode(ECameraProjectionMode::Orthographic);
	MainCamera->SetOrthoWidth(15000.0f);
	MainCamera->SetConstraintAspectRatio(true);
	MainCamera->SetPostProcessBlendWeight(0.0f);
	MainCamera->PostProcessSettings.bOverride_ScreenSpaceReflectionIntensity = 0.0f;

	UIColor = GetNewUIColor(1);
	bChangeUIColor = true;
}

void APossessor::BeginPlay()
{
	Super::BeginPlay();

	//create widget reference.

	DropSound->Stop();
	OneCorrectSound->Stop();
	AllCorrectSound->Stop();
	OneWrongSound->Stop();
	AllWrongSound->Stop();
	RotateSound->Stop();
	TetrineTheme->Stop();
	TickSound->Stop();
	ExplosionSound->Stop();
	LevelUpSound->Stop();
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
			if (NextTetrominos[0] != "none") 
			{ 
				CurrentTetromino->SetShapeName(NextTetrominos[0]);
				CurrentTetromino->SpawnShape();
				CurrentTetrominoName = CurrentTetromino->Shape;
				NextTetrominos[0] = NextTetrominos[1];
			}
			else 
			{ 
				CurrentTetromino->SetShapeName(GenerateRandomTetromino());
				CurrentTetromino->SpawnShape();
				CurrentTetrominoName = CurrentTetromino->Shape;
				NextTetrominos[0] = GenerateRandomTetromino();
			}
			NextTetrominos[1] = GenerateRandomTetromino();
			CurrentTetromino->MoveTetrominoOnGrid(FVector2D(0, 0), grid);
			bHasTetrominoLanded = false;
			bHasChangedPositions = true;
			if (bStartUILandedAnim || bStartUISavedAnim) { bStartUIAnimation = true; }
		}
		//if (bHasFinishedLandAnim)
		//{
		//	CurrentTetrominoName = CurrentTetromino->Shape;
		//}
		if (!bHasRowsToDelete)
		{
			TetrominoOnGridTimer += DeltaTime;
			if (bIsSaveTetroKeyHeld && !bhasSavedTetromino) { SaveTetromino(); CurrentTetromino->Destroy(); CurrentTetromino = nullptr; bhasSavedTetromino = bStartUISavedAnim = true; return; }
			if (CurrentHorizontalMove != 0.0f) { UpdateHorizontalElapsed(DeltaTime);  bHasChangedPositions = true; }
			UpdateFallElapsed(DeltaTime);
			if (bIsInstantDropped) { InstantDrop(); bIsInstantDropped = false; bHasTetrominoLanded = bHasStartedLandAnim = true; LandedTimeElapsed = LandedTimeLimit; }
			if (bIsRotating) { UpdateRotations(); bHasChangedPositions = true; }
			if (bHasChangedPositions) { UpdateGhostTetromino(); }
			if (bHasTetrominoLanded && UpdateLandedElapsed(DeltaTime)) { bHasRowsToDelete = bStartUILandedAnim = bIsKeyProcessed = true; CalculateArrowSequence(); }
		}
		else 
		{ 
			if ((CurrentWrongTries >= MaxWrongTries || bIsArrowMiniFinished) && bIsRowDestroyAnimFin) { UpdateRowDeletion(); TickedBlocksToColor = 0; }
			else if (!bIsArrowMiniFinished && CurrentWrongTries < MaxWrongTries) { bIsArrowMiniFinished = UpdateArrowMiniGame(DeltaTime); bIsRowDestroyAnimFin = false; }
			else if (!bIsRowDestroyAnimFin) { bIsRowDestroyAnimFin = IsRowDeletionAnimFin(DeltaTime); }
			else { bIsArrowMiniFinished = bIsRowDestroyAnimFin = false;  }
		}
	}
	else if (bIsGameOver)
	{
		TetrineTheme->Stop();
		//grid->Clear();
	}
	else
	{
		if (GetWorld() == nullptr) { UE_LOG(Possessor_log, Error, TEXT("GetWorld1 == nullptr")); return; }
		//grid = SpawnGrid();
		if (grid == nullptr) { UE_LOG(Possessor_log, Error, TEXT("Grid2 == nullptr")); return; }

		CurrentTetromino = SpawnTetromino();
		if (CurrentTetromino == nullptr) { UE_LOG(Possessor_log, Error, TEXT("Tetro2 == nullptr")); return; }
		CurrentTetromino->SetShapeName(GenerateRandomTetromino());
		CurrentTetromino->SpawnShape();
		CurrentTetromino->MoveTetrominoOnGrid(FVector2D(0, 0), grid);
		OldGhostPositions = CurrentTetromino->GetPositions();
		CurrentTetrominoName = CurrentTetromino->Shape;
		NextTetrominos[0] = GenerateRandomTetromino();
		NextTetrominos[1] = GenerateRandomTetromino();
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
		bHasReachedTimeLimit = HasReachedTimeLimit(FastFallTimeElapsed, FastFallTimeLimit);
	}
	else 
	{
		FallTimeElapsed += deltaTime;
		bHasReachedTimeLimit = HasReachedTimeLimit(FallTimeElapsed, FallTimeLimit);
	}

	if (bHasReachedTimeLimit)
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
	bHasReachedTimeLimit = HasReachedTimeLimit(LandedTimeElapsed, LandedTimeLimit);
	if (bHasReachedTimeLimit)
	{
		if (CurrentTetromino->DoesTetrominoCollide(FVector2D(0,-1),grid) == true)// block must be below in order to drop block																 
		{   
			if(FilterForDeletion(CurrentTetromino->GetTetrominoRows()).Num() != 0) 
			{ // checks if there is a row to delete, to which mini game is activated and deletion is deferred until after
				hasRowsToDelete = true;
			}
			else
			{
				LandedTetromino = CurrentTetromino->Shape;
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
	if (!bIsInstantDropKeyHeld && !bIsInstantDropped) { bIsInstantDropped = true;}
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
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->ChangeColor(0);
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->BlockHitBox->SetMobility(EComponentMobility::Stationary);
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->SetArrowVisibility(0);

		grid->GetBlock(OldGhostPositions[i])->SetBlockStatus(2);
		grid->GetBlock(OldGhostPositions[i])->SetBlockSprite(2);
		grid->GetBlock(OldGhostPositions[i])->SetArrowVisibility(0);

		CurrentTetromino->blocks[i]->SetPosition(OldGhostPositions[i]);
		CurrentTetromino->blocks[i]->SetActorLocation(CurrentTetromino->blocks[i]->GetDimensions().X * FVector(OldGhostPositions[i].X, 0, OldGhostPositions[i].Y));
	}
}

void APossessor::StartDeletionProcess(int8 extraRowsToDelete)
{
	TArray<int8> RowsToDelete = FilterForDeletion(CurrentTetromino->GetTetrominoRows());
	RowsToDelete = grid->GetExtraRows(RowsToDelete, extraRowsToDelete);
	//LandedTetromino = CurrentTetromino->Shape;
	CurrentTetromino->EndLife(grid);
	DeleteRows(RowsToDelete);
	grid->DropRows();
	CurrentTetromino = nullptr;
	Lines += RowsToDelete.Num();
	//add up new score
	if(bHasRowsToDelete){ CalculateMultiplier(); }
	int tempScore = (CalculateRowDropScore(RowsToDelete) + CalculateTetroDropScore())*GetMultiplier();
	if (tempScore != 0) { SpawnScoreBox(FString::FromInt(tempScore), ScoreBoxLocation); }
	AddToScore(tempScore);
	ChangeLevel();
	bStartUILandedAnim = true;
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
				AllCorrectSound->Play();
				return true;
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
		return false;
	}
	ArrowMiniTimeElapsed = 0.0f;
	CurrentWrongTries = 2;
	return true; // false == finished mini game
}

void APossessor::CalculateArrowSequence()
{
	ArrowSequence.Empty();
	ArrowSequencePosition.X = 0;
	TArray<int8> rowsToExtract = FilterForDeletion(CurrentTetromino->GetTetrominoRows());
	rowsToExtract.Sort();
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
	grid->SetRowColor("white",ArrowSequencePosition.Y);
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
		CurrentTetromino->blocks[i]->SetActorLocation(FVector(CurrentTetromino->blocks[i]->GetActorLocation().X, -50.0f, CurrentTetromino->blocks[i]->GetActorLocation().Z));
	}
}

void APossessor::UpdateArrowMiniTimerBar()
{
	int blocksToColor = ArrowMiniTimeElapsed / ArrowMiniTimeLimit*10;
	if (TickedBlocksToColor == blocksToColor) { return; }
	TickedBlocksToColor = blocksToColor;
	for (int i = 0; i < blocksToColor; ++i)
	{
		grid->GetBlock(FVector2D(i, ArrowSequencePosition.Y))->ChangeColor("grey");
		TickSound->Play();
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
	NextTetrominos[0] = SavedTetromino;
	NextTetrominos[1] = GenerateRandomTetromino();
	SavedTetromino = CurrentTetrominoName;
	for (int i = 0; i < 4; ++i)
	{
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->SetBlockStatus(0);
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->SetBlockSprite(0);
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->SetColor("empty");
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->ChangeColor(0);
		grid->GetBlock(CurrentTetromino->blocks[i]->GetPosition())->SetArrowVisibility(0);
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
	if (level > GetLevel()) 
	{ 
		FallTimeLimit = FMath::Clamp(InitialFallTL - (level*FallMultiplier), FinalFallTL, InitialFallTL);
		LandedTimeLimit = FMath::Clamp(InitialLandedTL - (level*LandedMultiplier), FinalLandedTL, InitialLandedTL);
		ArrowMiniTimeLimit = FMath::Clamp(InitialArrowMiniTL - (level*ArrowMiniMultiplier), FinalArrowMiniTL, InitialArrowMiniTL);
		SpawnScoreBox( FString::FromInt(level-GetLevel()), LevelUpgradeLocation);
		SetLevel(level);
		LevelUpSound->Play();

		bChangeUIColor = true;
		UIColor = GetNewUIColor(level);
	}
	
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
		result += temp;
	}
	return result;
}

int APossessor::CalculateTetroDropScore()
{
	int result = FMath::Clamp( int(((grid->GetHeight()/2) - TetrominoOnGridTimer) * 5),0,300);
	return result;
}

void APossessor::AddToScore(int score)
{
	Score += score;
}

void APossessor::SpawnScoreBox(FString score, FVector scoreBoxLocation)
{
	AScoreBox* newScore = GetWorld()->SpawnActor<AScoreBox>(AScoreBox::StaticClass());
	newScore->SetScore(score, scoreBoxLocation);
}

void APossessor::UpdateRowDeletion()
{
	StartDeletionProcess( (ExtraRowsToDelete-CurrentWrongTries) );
	UE_LOG(Possessor_log, Error, TEXT("Value of Row deletions is: %d"), ExtraRowsToDelete - CurrentWrongTries);
	bHasRowsToDelete = bHasTetrominoLanded = false;
	CurrentArrow = "none";
	ArrowMiniTimeElapsed = TetrominoOnGridTimer = 0.0f;
	CurrentWrongTries = 0;
	grid->SetRowArrowSprite(ArrowSprite, ArrowSequencePosition.Y);
	bIsGameOver = grid->IsBlockInDeadZone();
	bIsRowDestroyAnimFin = bhasSavedTetromino = bIsArrowMiniFinished = false;
}

bool APossessor::IsRowDeletionAnimFin(float deltaTime)
{
	if (RowDestroyAnimTimeElapsed == 0.0f) { RowDestroyAnimTimeLimit = SetUpRowsDestroyAnim( grid->GetExtraRows(FilterForDeletion(CurrentTetromino->GetTetrominoRows()),ExtraRowsToDelete-CurrentWrongTries)); }
	RowDestroyAnimTimeElapsed += deltaTime;
	if (!HasReachedTimeLimit(RowDestroyAnimTimeElapsed, RowDestroyAnimTimeLimit)) 
	{
		return false;
	}
	else
	{
		RowDestroyAnimTimeElapsed = 0.0f;
		SetDownRowsDestroyAnim(grid->GetExtraRows(FilterForDeletion(CurrentTetromino->GetTetrominoRows()), ExtraRowsToDelete-CurrentWrongTries));
		UE_LOG(Possessor_log, Error, TEXT("IsRowDeletionAnim: %d"), ExtraRowsToDelete - CurrentWrongTries);
		ExplosionSound->Play();
		return true;
	}
}


float APossessor::SetUpRowsDestroyAnim(TArray<int8> rowsToDestroy)
{
	for (int i = 0; i < rowsToDestroy.Num(); ++i)
	{
		for (int j = 0; j < grid->GetWidth(); ++j)
		{
			grid->GetBlock(FVector2D(j, rowsToDestroy[i]))->SetFlipbook(BlockDestroyAnim, 0);
			grid->GetBlock(FVector2D(j, rowsToDestroy[i]))->BlockDestroyAnim->AddWorldOffset(FVector(0.0f, 10.0f, 0.0f));
			grid->GetBlock(FVector2D(j, rowsToDestroy[i]))->BlockDestroyAnim->Stop();
			grid->GetBlock(FVector2D(j, rowsToDestroy[i]))->BlockDestroyAnim->PlayFromStart();
		}
	}
	return BlockDestroyAnim->GetTotalDuration(); 
}

void APossessor::SetDownRowsDestroyAnim(TArray<int8> rowsToDestroy)
{
	for (int i = 0; i < rowsToDestroy.Num(); ++i)
	{
		for (int j = 0; j < grid->GetWidth(); ++j)
		{
			grid->GetBlock(FVector2D(j, rowsToDestroy[i]))->SetFlipbook(BlockDestroyAnim, 1);
			grid->GetBlock(FVector2D(j, rowsToDestroy[i]))->BlockDestroyAnim->AddWorldOffset(FVector(0.0f, -10.0f, 0.0f));
		}
	}
}


FColor APossessor::GetNewUIColor(int8 level)
{
	if (level % 9 == 0) { return FColor::FromHex("#E14B32"); } // square // red
	else if (level % 9 == 1) { return FColor::FromHex("#3264E1"); } // t // blue
	else if (level % 9 == 2) { return  FColor::FromHex("#19C819"); } // z // green
	else if (level % 9 == 3) { return FColor::FromHex("#FA9619"); } // s //orange
	else if (level % 9 == 4) { return FColor::FromHex("#E1E119"); } // right bicep // yellow
	else if (level % 9 == 5) { return FColor::FromHex("#7D32FA"); } // flat // purple
	else if (level % 9 == 6) { return FColor::FromHex("#FA32C8"); } // left bicep // pink
	else if (level % 9 == 7) { return FColor::FromHex("#FFFFFF"); } // remove all colors
	else { return FColor::FromHex("#32E1FA"); } // turqouise-ish
}