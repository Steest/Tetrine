// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetrine.h"
#include "Digit.h"
#include "PaperSpriteComponent.h"

// Sets default values
ADigit::ADigit()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	sprite = CreateDefaultSubobject<UPaperSpriteComponent>(TEXT("Digit"));
	RootComponent = sprite;
}

// Called when the game starts or when spawned
void ADigit::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void ADigit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ADigit::SetSprite(UPaperSprite* s)
{
	sprite->SetSprite(s);
}

void ADigit::UpdateTransparency(float transparencyAmount)
{
	FLinearColor tempColor = sprite->GetSpriteColor();
	tempColor.A = transparencyAmount;
	sprite->SetSpriteColor(tempColor);
}
