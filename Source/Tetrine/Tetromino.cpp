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
		//
		// might need
		// to swap
		// Y and X
		// in above newLocation
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