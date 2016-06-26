// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Grid.generated.h"

DECLARE_LOG_CATEGORY_EXTERN(Grid_log, Log, All);
UCLASS()
class TETRINE_API AGrid : public AActor
{
	GENERATED_BODY()

public:
	AGrid();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	// variables
	class UPaperSpriteComponent* GridComponent;

	int32 width;
	int32 height;
	TArray<TArray<class ABlock*>> matrix;

	// methods
	void GenerateMatrix(int scale);
	bool DropBlock(class ABlock* block);
	int32 UpdateMatrix();
	int32 GetWidth();
	int32 GetHeight();
	class ABlock* GetBlock(FVector2D position);
	void SetBlock(class ABlock* newBlock);
	class ABlock* SpawnBlock();
};
