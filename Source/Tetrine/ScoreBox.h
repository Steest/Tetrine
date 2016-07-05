// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "ScoreBox.generated.h"

UCLASS()
class TETRINE_API AScoreBox : public AActor
{
	GENERATED_BODY()

public:
	AScoreBox();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSeconds) override;

	class UPaperSprite* PlusSprite;
	class UPaperSprite* ZeroSprite;
	class UPaperSprite* OneSprite;
	class UPaperSprite* TwoSprite;
	class UPaperSprite* ThreeSprite;
	class UPaperSprite* FourSprite;
	class UPaperSprite* FiveSprite;
	class UPaperSprite* SixSprite;
	class UPaperSprite* SevenSprite;
	class UPaperSprite* EightSprite;
	class UPaperSprite* NineSprite;

	UPROPERTY(EditAnywhere, Category = "Score")
	float LifeTimeLimit;
	UPROPERTY(EditAnywhere, Category = "Score")
	float LifeTimeElapsed;
	TArray<class ADigit*> Score;
	UPROPERTY(EditAnywhere, Category = "Score")
	float Scale;
	void SetScore(FString number,FVector scoreBoxLocation);

private:
	
	void DestroyAll();
	class UPaperSprite* GetSprite(char character);
	void UpdatePositions(float deltaTime);
};
