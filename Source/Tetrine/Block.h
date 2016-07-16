// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Block.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(MyLog, Log, All);
UCLASS()
class TETRINE_API ABlock : public AActor
{
	GENERATED_BODY()

public:
	ABlock();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// variables
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Anim")
	class UPaperSpriteComponent* BlockHitBox;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Anim")
	class UPaperSpriteComponent* ArrowSprite;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Anim")
	class UPaperSprite* EmptySprite;
	UPROPERTY(EditAnywhere,BlueprintReadWrite, Category = "Anim")
	class UPaperSprite* BlockSprite;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	class UPaperFlipbookComponent* BlockDestroyAnim;
	UPROPERTY(VisibleAnywhere, Category = "Stats")
	int8 BlockStatus; // 0=empty,1=taken(static),2=taken(moving)
	UPROPERTY(VisibleAnywhere, Category = "Stats")
	FVector2D Position; // position on the grid
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Stats")
	FString ArrowDirection;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FString Color;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Anim")
	FLinearColor RenderedColor;

	// methods
	bool DoesBlockCollide(FVector2D movement, class AGrid* grid);
	bool SetBlockStatus(int8 blockType);
	int8 GetBlockStatus();
	void SetPosition(FVector2D position);
	FVector2D GetPosition();
	void SetArrowVisibility(int8 blockStatus);
	FVector2D GetDimensions();
	void SetBlockSprite(int8 blockStatus);
	FString GetArrowDirection();
	void SetArrowDirection(FString direction);
	void SetColor(FString color);
	FString GetColor();
	void ChangeColor(FString color);
	void ChangeColor(int8 blockStatus);
	void ChangeColorByShape(FString shape);
	void SetFlipbook(class UPaperFlipbook* anim, int8 blockStatus);
};
