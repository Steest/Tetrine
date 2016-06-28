// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetrine.h"
#include "Block.h"
#include "Grid.h"
#include "PaperSpriteComponent.h"

ABlock::ABlock()
{
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> EmptySpriteAsset(TEXT("PaperSprite'/Game/Art/empty_Sprite.empty_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> BlockSpriteAsset(TEXT("PaperSprite'/Game/Art/salmon_block_Sprite.salmon_block_Sprite'"));
	ConstructorHelpers::FObjectFinder<UPaperSprite> ArrowSpriteAsset(TEXT("PaperSprite'/Game/Art/Arrow_Sprite.Arrow_Sprite'"));
	BlockSprite = BlockSpriteAsset.Get();
	EmptySprite = EmptySpriteAsset.Get();

	// main hitbox / sprite
	BlockHitBox = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("BlockHitBox"));
	BlockHitBox->SetSprite(EmptySpriteAsset.Get());
	RootComponent = BlockHitBox;

	// arrow sprite 
	ArrowSprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("ArrowSprite"));
	ArrowSprite->SetSprite(ArrowSpriteAsset.Object);
	ArrowSprite->AttachTo(BlockHitBox);
	ArrowSprite->SetVisibility(false);
	ArrowSprite->AddWorldOffset(FVector(0.0f, 2.0f, 0.0f));

	BlockStatus = 0;
	Position.X = 0;
	Position.Y = 0;
	ArrowDirection = "none";
}

void ABlock::BeginPlay()
{
	Super::BeginPlay();
}

void ABlock::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ABlock::DoesBlockCollide(FVector2D movement, AGrid* grid)
{
	FVector2D newPosition = GetPosition() + movement;
	if (newPosition.X < 0 || newPosition.Y < 0 ||
		newPosition.X >= grid->GetWidth() || newPosition.Y >= grid->GetHeight() ||
		grid->GetBlock(newPosition)->GetBlockStatus() == 1)
	{
		return true;
	}
	return false;
}

bool ABlock::SetBlockStatus(int8 blockStatus)
{
	if (blockStatus == 0 || blockStatus == 1 || blockStatus == 2)
	{
		BlockStatus = blockStatus;
		return true;
	}
	return false;
}

void ABlock::SetPosition(FVector2D position)
{
	Position = position;
}

int8 ABlock::GetBlockStatus()
{
	return BlockStatus;
}

void ABlock::SetArrowVisibility(int8 blockStatus)
{
	bool b = (blockStatus == 1 || blockStatus == 2) ? true : false;
	ArrowSprite->SetVisibility(b);
}

FVector2D ABlock::GetPosition()
{
	return Position;
}

FVector2D ABlock::GetDimensions()
{
	return BlockHitBox->GetSprite()->GetSourceSize();
}

void ABlock::SetBlockSprite(int8 blockStatus)
{
	if (blockStatus == 0) { BlockHitBox->SetSprite(EmptySprite); }
	else if (blockStatus == 1 || blockStatus == 2) { BlockHitBox->SetSprite(BlockSprite); }
}

FString ABlock::GetArrowDirection()
{
	return ArrowDirection;
}

void ABlock::SetArrowDirection(FString direction)
{
	if (direction == "up") { ArrowSprite->SetWorldRotation(FRotator(90.0f,0.0f,0.0f)); }
	else if (direction == "right") { ArrowSprite->SetWorldRotation(FRotator(0.0f,0.0f,0.0f)); }
	else if (direction == "down") { ArrowSprite->SetWorldRotation(FRotator(270.0f,0.0f,0.0f)); }
	else if (direction == "left") { ArrowSprite->SetWorldRotation(FRotator(180.0f,0.0f,0.0f)); }
	else { return; }
	ArrowDirection = direction;
}

