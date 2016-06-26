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
	height = 20;

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

	//temp
	/*
	matrix[0][7]->SetBlockVisibility(1);
	matrix[0][7]->SetBlockSprite(1);
	matrix[0][7]->SetBlockStatus(1);

	matrix[5][0]->SetBlockVisibility(1);
	matrix[5][0]->SetBlockSprite(1);
	matrix[5][0]->SetBlockStatus(1);
	*/
}

bool AGrid::DropBlock(ABlock* block)
{
	if (block == nullptr)
	{
		UE_LOG(Grid_log, Log, TEXT("DropBlock Failed: block == nullptr"));
		return false;
	}
	GetBlock(block->GetPosition())->Destroy();
	SetBlock(block);
	return true;
}

int32 AGrid::GetHeight()
{
	return height;
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

void AGrid::DropRows(int8 rowsToDrop)
{

}