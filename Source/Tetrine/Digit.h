// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Digit.generated.h"

UCLASS()
class TETRINE_API ADigit : public AActor
{
	GENERATED_BODY()

public:
	ADigit();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	class UPaperSpriteComponent* sprite;
	void SetSprite(class UPaperSprite* s);
	void UpdateTransparency(float transparencyAmount);
};


