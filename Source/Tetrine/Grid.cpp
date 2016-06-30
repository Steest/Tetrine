// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetrine.h"
#include "Grid.h"
#include "Tetromino.h"
#include "Block.h"
#include "PaperSpriteComponent.h"

DEFINE_LOG_CATEGORY(Grid_log);
AGrid::AGrid()
{
	PrimaryActorTick.bCanEverTick = false;

	width = 10;
	height = 22;

	GridComponent = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("GridComponent"));
	RootComponent = GridComponent;
}

void AGrid::BeginPlay()
{
	Super::BeginPlay();
	GenerateMatrix(1);
}

void AGrid::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void AGrid::GenerateMatrix(int scale)
{
	for (int i = 0; i < width; ++i)
	{
		TArray<ABlock*> tempRow;
		for (int j = 0; j < height; ++j)
		{
			ABlock* tempBlock = SpawnBlock();
			tempBlock->SetActorLocation(FVector(i*tempBlock->GetDimensions().X, 0, j*tempBlock->GetDimensions().Y));
			tempBlock->SetPosition(FVector2D(i, j));
			tempBlock->SetBlockStatus(0);
			//FString tempStr = "Block[" + FString::FromInt(i) + "][" + FString::FromInt(j) + "]";
			//FName tempName = *tempStr;
			//	tempBlock->Rename(*tempStr);
			tempRow.Add(tempBlock);
		}
		matrix.Add(tempRow);
	}
}

bool AGrid::DropBlock(ABlock* block)
{
	if (block == nullptr)
	{
		UE_LOG(Grid_log, Error, TEXT("DropBlock Failed: block == nullptr"));
		return false;
	}
	GetBlock(block->GetPosition())->Destroy();
	SetBlock(block);
	return true;
}

int32 AGrid::GetHeight()
{
	return height-2;
}

int32 AGrid::GetWidth()
{
	return width;
}

ABlock* AGrid::GetBlock(FVector2D position)
{
	return matrix[position.X][position.Y];
}

void AGrid::SetBlock(ABlock* newBlock)
{
	matrix[newBlock->GetPosition().X][newBlock->GetPosition().Y] = newBlock;
	GetBlock(newBlock->GetPosition())->SetActorLocation(FVector(GetBlock(newBlock->GetPosition())->GetActorLocation().X, -1.0f, GetBlock(newBlock->GetPosition())->GetActorLocation().Z)); // fixes the ghost overlapping live tetromino
}

ABlock* AGrid::SpawnBlock()
{
	return GetWorld()->SpawnActor<ABlock>(ABlock::StaticClass());
}

void AGrid::DeleteRow(int8 row)
{
	for (int i = 0; i < GetWidth(); ++i)
	{
		matrix[i][row]->SetBlockSprite(0);
		matrix[i][row]->SetBlockStatus(0);
		matrix[i][row]->SetArrowVisibility(0);
	}
}

bool AGrid::ShouldDeleteRow(int8 row)
{
	for (int i = 0; i < GetWidth(); ++i)
	{
		if (matrix[i][row]->GetBlockStatus() == 0) { return false; }
	}
	return true;
}

void AGrid::DropRows()
{
	for (int i = 0; i < GetHeight(); ++i) 
	{
		if(IsRowEmpty(i))
		{
			int8 DropFromRow = i+1; // in case the row above is empty, we seek the first row (above this row) that has blocks to drop
			while (IsRowEmpty(DropFromRow) && DropFromRow < GetHeight()){ ++DropFromRow; }
			int8 j = i;
			while (DropFromRow < GetHeight() + 1)
			{ 
				TransferRow(DropFromRow,j);
				++DropFromRow;
				++j;
			}
		}
	}
}

bool AGrid::IsRowEmpty(int8 row)
{
	for (int i = 0; i < GetWidth(); ++i)
	{
		if (matrix[i][row]->GetBlockStatus() != 0) { return false; }
	}
	return true;
}

void AGrid::TransferRow(int8 fromRow, int8 toRow)
{
	for (int i = 0; i < GetWidth(); ++i)
	{
		matrix[i][toRow]->SetBlockStatus(matrix[i][fromRow]->GetBlockStatus());
		matrix[i][toRow]->SetBlockSprite(matrix[i][fromRow]->GetBlockStatus());
		matrix[i][toRow]->SetArrowDirection(matrix[i][fromRow]->GetArrowDirection());
		matrix[i][toRow]->SetArrowVisibility(matrix[i][fromRow]->GetBlockStatus());
		matrix[i][toRow]->SetColor(matrix[i][fromRow]->GetColor());
		matrix[i][toRow]->ChangeColor(matrix[i][fromRow]->GetBlockStatus());
	}
}

TArray<int8> AGrid::GetExtraRows(const TArray<int8>& rowsToAvoid, int8 numOfRows)
{
	TArray<int8> extraRows = rowsToAvoid;
	for (int i = 0; i < GetHeight() && numOfRows > 0; ++i)
	{
		if (!rowsToAvoid.Contains(i)) {	extraRows.Add(i); --numOfRows; }
	}
	extraRows.Sort();
	return extraRows;
}

void AGrid::SetArrowSprite(UPaperSprite* sprite, FVector2D position)
{
	GetBlock(position)->ArrowSprite->SetSprite(sprite);
}

void AGrid::SetRowArrowSprite(UPaperSprite* sprite, int8 row)
{
	for (int i = 0; i < GetWidth(); ++i)
	{
		SetArrowSprite(sprite, FVector2D(i, row));
	}
}

void AGrid::SetRowColor(FString color, int8 row)
{
	for (int i = 0; i < GetWidth(); ++i)
	{
		GetBlock(FVector2D(i, row))->ChangeColor(color);
	}
}