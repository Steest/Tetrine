// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetrine.h"
#include "Block.h"
#include "Grid.h"
#include "PaperSpriteComponent.h"
#include "PaperFlipbookComponent.h"

DEFINE_LOG_CATEGORY(MyLog);
ABlock::ABlock()
{
	PrimaryActorTick.bCanEverTick = false;

	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> EmptySpriteAsset(TEXT("PaperSprite'/Game/Art/empty_Sprite.empty_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> BlockSpriteAsset(TEXT("PaperSprite'/Game/Art/gray_box_Sprite.gray_box_Sprite'"));
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
	ArrowSprite->AddWorldOffset(FVector(0.0f, 1.0f, 0.0f));

	// anim destroy
	BlockDestroyAnim = CreateDefaultSubobject<UPaperFlipbookComponent>(TEXT("BlockDestroyAnim"));
	BlockDestroyAnim->AttachTo(RootComponent);
	BlockDestroyAnim->SetVisibility(false);

	BlockStatus = 0;
	Position.X = 0;
	Position.Y = 0;
	ArrowDirection = "none";
	Color = "none";
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

void ABlock::SetColor(FString color)
{
	Color = color;
}

FString ABlock::GetColor()
{
	return Color;
}

void ABlock::ChangeColor(FString color)
{
	FColor newColor;
	if (color == "default" || color == "") { color = GetColor(); }

	if (color == "red") { newColor = FColor::FromHex("#E14B32"); } // square
	else if (color == "blue") { newColor = FColor::FromHex("#3264E1"); } // t
	else if (color == "green") { newColor = FColor::FromHex("#19C819"); } // z
	else if (color == "orange") { newColor = FColor::FromHex("#FA9619"); } // s
	else if (color == "yellow") { newColor = FColor::FromHex("#E1E119"); } // right bicep
	else if (color == "purple") { newColor = FColor::FromHex("#7D32FA"); } // flat
	else if (color == "pink") { newColor = FColor::FromHex("#FA32C8"); } // left bicep
	else if (color == "grey") { newColor = FColor::FromHex("#676A6B"); } // used for arrow mini
	else if (color == "white" || color == "empty" || color == "none") { newColor = FColor::FromHex("#FFFFFF"); } // remove all colors
	else if (color == "black") { newColor = FColor::FromHex("#000000"); }
	else { newColor = FColor::FromHex("#32E1FA"); } // turqouise-ish
	//UE_LOG(MyLog, All,TEXT("My color is %s"),*newColor.ToString());
	RenderedColor = FLinearColor(newColor);
	BlockHitBox->SetSpriteColor(RenderedColor);
}

void ABlock::ChangeColor(int8 blockStatus)
{
	if (blockStatus == 0) { ChangeColor("empty"); }
	else if (blockStatus == 1 || blockStatus == 2) { ChangeColor("default"); }
	else { ChangeColor("none"); }
}

void ABlock::ChangeColorByShape(FString shapeName)
{
	FColor newColor;
	if (shapeName == "square") { newColor = FColor::FromHex("#E14B32"); } // red
	else if (shapeName == "t") { newColor = FColor::FromHex("#3264E1"); } // blue
	else if (shapeName == "z") { newColor = FColor::FromHex("#19C819"); } // green
	else if (shapeName == "s") { newColor = FColor::FromHex("#FA9619"); } // orange
	else if (shapeName == "right bicep") { newColor = FColor::FromHex("#E1E119"); } // yellow
	else if (shapeName == "flat") { newColor = FColor::FromHex("#7D32FA"); } // purple
	else if (shapeName == "left bicep") { newColor = FColor::FromHex("#FA32C8"); } // pink
	else { newColor = FColor::FromHex("#FFFFFF"); } // remove all colors

	RenderedColor = FLinearColor(newColor);
	BlockHitBox->SetSpriteColor(RenderedColor);
}

void ABlock::SetFlipbook(UPaperFlipbook* anim, int8 blockStatus)
{
	if (blockStatus == 0)
	{
		BlockDestroyAnim->SetFlipbook(anim);
		BlockDestroyAnim->SetVisibility(true);
	}
	else
	{
		BlockDestroyAnim->SetVisibility(false);
	}
}