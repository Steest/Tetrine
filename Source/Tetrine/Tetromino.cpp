// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetrine.h"
#include "Tetromino.h"
#include "Grid.h"
#include "Block.h"
#include "PaperSpriteComponent.h"

DEFINE_LOG_CATEGORY(Tetromino_log);
ATetromino::ATetromino()
{
	PrimaryActorTick.bCanEverTick = false;
	rotationMatrix.Emplace(FVector2D(0, 1));
	rotationMatrix.Emplace(FVector2D(-1, 0));
}

void ATetromino::BeginPlay() // we dont know which BeginPlay will fire first between this, possessor, tetromino, grid, blocks
{
	Super::BeginPlay();
	for (int i = 0; i < 4; ++i) // tetra == 4, always only 4 blocks in tetris
	{
		blocks.Add(SpawnBlock());
		blocks[i]->SetBlockStatus(2);
		blocks[i]->SetBlockSprite(2);
		// possessor places in the proper position
	}
}

void ATetromino::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATetromino::EndLife(AGrid* grid)
{
	for (int i = 0; i < 4; ++i)
	{
		blocks[i]->SetBlockStatus(1);
		grid->DropBlock(blocks[i]);
		blocks[i] = nullptr;
	}
	Destroy();
}

ABlock* ATetromino::SpawnBlock()
{
	return GetWorld()->SpawnActor<ABlock>(ABlock::StaticClass());
}

void ATetromino::SpawnShape(FString shape) // recall that the grid goes from bottom left to up right
{
	if (shape == "square")
	{
		blocks[0]->SetPosition(FVector2D(4, 18));
		blocks[1]->SetPosition(FVector2D(5, 18));
		blocks[2]->SetPosition(FVector2D(4, 19));
		blocks[3]->SetPosition(FVector2D(5, 19));
	}
	else if (shape == "t")
	{
		blocks[0]->SetPosition(FVector2D(4, 19));
		blocks[1]->SetPosition(FVector2D(3, 18));
		blocks[2]->SetPosition(FVector2D(4, 18));
		blocks[3]->SetPosition(FVector2D(5, 18));
	}
	else if (shape == "z")
	{
		blocks[0]->SetPosition(FVector2D(3, 19));
		blocks[1]->SetPosition(FVector2D(4, 19));
		blocks[2]->SetPosition(FVector2D(4, 18));
		blocks[3]->SetPosition(FVector2D(5, 18));
	}
	else if (shape == "right bicep")
	{
		blocks[0]->SetPosition(FVector2D(5, 19));
		blocks[1]->SetPosition(FVector2D(3, 18));
		blocks[2]->SetPosition(FVector2D(4, 18));
		blocks[3]->SetPosition(FVector2D(5, 18));
	}
	else if (shape == "left bicep")
	{
		blocks[0]->SetPosition(FVector2D(3, 19));
		blocks[1]->SetPosition(FVector2D(3, 18));
		blocks[2]->SetPosition(FVector2D(4, 18));
		blocks[3]->SetPosition(FVector2D(5, 18));
	}
	else if (shape == "flat")
	{
		blocks[0]->SetPosition(FVector2D(3, 19));
		blocks[1]->SetPosition(FVector2D(4, 19));
		blocks[2]->SetPosition(FVector2D(5, 19));
		blocks[3]->SetPosition(FVector2D(6, 19));
	}
	else if (shape == "s")
	{
		blocks[0]->SetPosition(FVector2D(4, 19));
		blocks[1]->SetPosition(FVector2D(5, 19));
		blocks[2]->SetPosition(FVector2D(3, 18));
		blocks[3]->SetPosition(FVector2D(4, 18));
	}
	else
	{
		UE_LOG(Tetromino_log, Log, TEXT("SpawnShape: String didn't match any existing shapes"));
	}
}

bool ATetromino::DoesTetrominoCollide(FVector2D movement, AGrid* grid)
{
	for (int i = 0; i < 4; ++i)
	{
		if (blocks[i]->DoesBlockCollide(movement, grid) == true) { return true; }
	}
	return false;
}

void ATetromino::MoveTetrominoOnGrid(FVector2D movement, AGrid* grid)
{
	for (int i = 0; i < 4; ++i) // remove old block image
	{
		FVector2D oldPosition = blocks[i]->GetPosition();
		grid->GetBlock(oldPosition)->SetBlockStatus(0); // remember grid has matrix, and tetromino has separate set of blocks
		grid->GetBlock(oldPosition)->SetBlockSprite(0);
	}

	for (int i = 0; i < 4; ++i) // then add new blocks
	{
		FVector2D newPosition = blocks[i]->GetPosition() + movement;
		blocks[i]->SetPosition(newPosition); // tetromino has right position now
		grid->GetBlock(newPosition)->SetBlockStatus(2); //grid has right collision matrix now
		grid->GetBlock(newPosition)->SetBlockSprite(2);

		FVector newLocation = blocks[i]->GetDimensions().X*FVector(blocks[i]->GetPosition().X, 0, blocks[i]->GetPosition().Y);
		blocks[i]->SetActorLocation(newLocation);
	}
}

void ATetromino::SetBlocksVisibility(bool b)
{
	if (blocks.Num() < 4)
	{
		UE_LOG(Tetromino_log, Log, TEXT("SetBlocksVisibility Failed: blocks < 4"));
		return;
	}
	blocks[0]->SetBlockVisibility(b);
	blocks[1]->SetBlockVisibility(b);
	blocks[2]->SetBlockVisibility(b);
	blocks[3]->SetBlockVisibility(b);
}

void ATetromino::SetBlocksStatus(int8 blockStatus)
{
	if (blocks.Num() < 4)
	{
		UE_LOG(Tetromino_log, Log, TEXT("SetBlocksStatus Failed: blocks < 4"));
		return;
	}
	blocks[0]->SetBlockStatus(blockStatus);
	blocks[1]->SetBlockStatus(blockStatus);
	blocks[2]->SetBlockStatus(blockStatus);
	blocks[3]->SetBlockStatus(blockStatus);
}

TArray<int8> ATetromino::GetTetrominoRows()
{
	TArray<int8> resultingRows;
	for (int i = 0; i < 4; ++i)
	{
		resultingRows.AddUnique(blocks[i]->GetPosition().Y);
	}
	return resultingRows;
}

FVector2D ATetromino::GetPivotPosition()
{
	return blocks[2]->GetPosition();
}

void ATetromino::ShiftPositions(TArray<FVector2D> &positions, AGrid* grid)
{
	int8 MinColumn = 0;
	int8 MaxColumn = 0;
	for (int i = 0; i < positions.Num(); ++i)
	{
		if (positions[i].X >= grid->GetWidth() && positions[i].X > MaxColumn) { MaxColumn = positions[i].X; }
		else if (positions[i].X < MinColumn) { MinColumn = positions[i].X; }
	}

	if (MinColumn < 0)
	{
		for (int i = 0; i < positions.Num(); ++i)
		{
			positions[i] = (FVector2D(positions[i].X - MinColumn, positions[i].Y));
		}
	}
	else if (MaxColumn >= grid->GetWidth())
	{
		MaxColumn = MaxColumn - grid->GetWidth() + 1; // cant be equal to actual width
		for (int i = 0; i < positions.Num(); ++i)
		{
			positions[i] = (FVector2D(positions[i].X - MaxColumn , positions[i].Y));
		}
	}


	int8 MinRow = 0;
	int8 MaxRow = 0;
	for (int i = 0; i < positions.Num(); ++i)
	{
		if (positions[i].Y >= grid->GetHeight() && positions[i].Y > MaxRow) { MaxRow = positions[i].Y; }
		else if (positions[i].Y < MinRow) { MinRow = positions[i].Y; }
	}

	if (MinRow < 0)
	{
		for (int i = 0; i < positions.Num(); ++i)
		{
			positions[i] = (FVector2D(positions[i].X , positions[i].Y - MinRow));
		}
	}
	else if (MaxRow >= grid->GetHeight())
	{
		MaxRow = MaxRow - grid->GetHeight() + 1; // cant be equal to actual height (were subtracting)
		for (int i = 0; i < positions.Num(); ++i)
		{
			positions[i] = (FVector2D(positions[i].X , positions[i].Y - MaxRow));
		}
	}
}

bool ATetromino::CanShiftPositions(const TArray<FVector2D> &newPositions, AGrid* grid)
{
	for (int i = 0; i < newPositions.Num(); ++i)
	{
		if (grid->GetBlock(newPositions[i])->GetBlockStatus() == 1) { return false; }
	}
	return true;
}

void ATetromino::ApplyRotation(TArray<FVector2D> newPositions,AGrid* grid)
{
	for (int i = 0; i < 4; ++i) // remove old block image
	{
		FVector2D oldPosition = blocks[i]->GetPosition();
		grid->GetBlock(oldPosition)->SetBlockStatus(0); // remember grid has matrix, and tetromino has separate set of blocks
		grid->GetBlock(oldPosition)->SetBlockSprite(0);
	}

	for (int i = 0; i < 4; ++i)
	{
		grid->GetBlock(newPositions[i])->SetBlockStatus(2);
		grid->GetBlock(newPositions[i])->SetBlockSprite(2);
		blocks[i]->SetPosition(newPositions[i]);

		FVector rotatedPosition = blocks[i]->GetDimensions().X * FVector(newPositions[i].X, 0, newPositions[i].Y);
		blocks[i]->SetActorLocation(rotatedPosition);
	}
}

TArray<FVector2D> ATetromino::CalculateRotation()
{
	TArray<FVector2D> RotatedPositions;
	for (int i = 0; i < 4; ++i)
	{
		//step 1. subtract from pivot position to get block positions relative to origin
		FVector2D offsetPosition = blocks[i]->GetPosition() - GetPivotPosition();

		//step 2. multiply by rotation matrix to obtain rotation positions (relative to origin)
		//[0 -1] [ height ]    =   [newHeight = 0 * height + -1 * width]   = [height, width]
		//[1  0] [ width ]	   =   [newWidth = 1 * height + 0 * width ]
		float newRow = rotationMatrix[0].X * offsetPosition.X + rotationMatrix[0].Y * offsetPosition.Y;
		float newCol = rotationMatrix[1].X * offsetPosition.X + rotationMatrix[1].Y * offsetPosition.Y;

		//step 3. add pivot position to the product of each new block position to obtain final position relative to pivot
		RotatedPositions.Add( FVector2D(newRow, newCol) + GetPivotPosition() );
	}
	return RotatedPositions;
}

TArray<FVector2D> ATetromino::GetPositions()
{
	TArray<FVector2D> resultingPositions;
	for (int i = 0; i < 4; ++i)
	{
		resultingPositions.Add(blocks[i]->GetPosition());
	}
	return resultingPositions;
}