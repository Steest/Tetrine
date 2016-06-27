// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Block.generated.h"

UCLASS()
class TETRINE_API ABlock : public AActor
{
	GENERATED_BODY()

public:
	ABlock();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// variables
	UPROPERTY(EditAnywhere, Category = "Anim")
	class UPaperSpriteComponent* BlockHitBox;
	UPROPERTY(EditAnywhere, Category = "Anim")
	class UPaperSpriteComponent* ArrowSprite;
	UPROPERTY(EditAnywhere, Category = "Anim")
	class UPaperSprite* EmptySprite;
	UPROPERTY(EditAnywhere, Category = "Anim")
	class UPaperSprite* BlockSprite;
	UPROPERTY(VisibleAnywhere, Category = "Stats")
		int8 BlockStatus; // 0=empty,1=taken(static),2=taken(moving)
	UPROPERTY(VisibleAnywhere, Category = "Stats")
		FVector2D Position; // position on the grid

							// methods
	bool DoesBlockCollide(FVector2D movement, class AGrid* grid);
	bool SetBlockStatus(int8 blockType);
	int8 GetBlockStatus();
	void SetPosition(FVector2D position);
	FVector2D GetPosition();
	void SetBlockVisibility(bool b);
	FVector2D GetDimensions();
	void SetBlockSprite(int8 blockStatus);
};