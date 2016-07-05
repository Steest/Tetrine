// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetrine.h"
#include "ScoreBox.h"
#include "Digit.h"
#include "PaperSpriteComponent.h"

// Sets default values
AScoreBox::AScoreBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> PlusSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/plus_Sprite.plus_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> ZeroSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/zero_Sprite.zero_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> OneSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/one_Sprite.one_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> TwoSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/two_Sprite.two_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> ThreeSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/three_Sprite.three_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> FourSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/four_Sprite.four_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> FiveSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/five_Sprite.five_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> SixSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/six_Sprite.six_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> SevenSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/seven_Sprite.seven_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> EightSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/eight_Sprite.eight_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> NineSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/nine_Sprite.nine_Sprite'"));


	PlusSprite = PlusSpriteAsset.Get();
	ZeroSprite = ZeroSpriteAsset.Get();
	OneSprite = OneSpriteAsset.Get();
	TwoSprite = TwoSpriteAsset.Get();
	ThreeSprite = ThreeSpriteAsset.Get();
	FourSprite = FourSpriteAsset.Get();
	FiveSprite = FiveSpriteAsset.Get();
	SixSprite = SixSpriteAsset.Get();
	SevenSprite = SevenSpriteAsset.Get();
	EightSprite = EightSpriteAsset.Get();
	NineSprite = NineSpriteAsset.Get();

	LifeTimeElapsed = 0.0f;
	LifeTimeLimit = 0.75f;
	Scale = 2.5f;
}

// Called when the game starts or when spawned
void AScoreBox::BeginPlay()
{
	Super::BeginPlay();
}

void AScoreBox::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	LifeTimeElapsed += DeltaTime;
	if (LifeTimeElapsed >= LifeTimeLimit) { DestroyAll(); }
	else { UpdatePositions(DeltaTime); }
}


void AScoreBox::SetScore(FString number, FVector scoreBoxLocation)
{
	if (Score.Num() == 0)
	{
		Score.Add(GetWorld()->SpawnActor<ADigit>(ADigit::StaticClass()));
		Score[0]->SetSprite(PlusSprite);
		Score[0]->AddActorLocalOffset(FVector(0.0f, 10.0f, 0.0f)+scoreBoxLocation);
		Score[0]->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f)*Scale);
	}

	for (int i = 0; i < number.Len(); ++i)
	{
		Score.Add(GetWorld()->SpawnActor<ADigit>(ADigit::StaticClass()));
		Score[i+1]->SetSprite(GetSprite(number[i]));
		Score[i+1]->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f)*Scale);
		Score[i+1]->AddActorLocalOffset(FVector( (i+1)*(Scale*(200.0f)) , 10.0f, 0.0f)+scoreBoxLocation);
	}
}

void AScoreBox::DestroyAll()
{
	for (int i = 0; i < Score.Num(); ++i)
	{
		Score[i]->Destroy();
		Score[i] = nullptr;
	}
	Destroy();
}

UPaperSprite* AScoreBox::GetSprite(char character)
{
	if (character == '0') { return ZeroSprite; }
	else if (character == '1') { return OneSprite; }
	else if (character == '2') { return TwoSprite; }
	else if (character == '3') { return ThreeSprite; }
	else if (character == '4') { return FourSprite; }
	else if (character == '5') { return FiveSprite; }
	else if (character == '6') { return SixSprite; }
	else if (character == '7') { return SevenSprite; }
	else if (character == '8') { return EightSprite; }
	else if (character == '9') { return NineSprite; }
	else { return nullptr; }
}

void AScoreBox::UpdatePositions(float deltaTime)
{
	for (int i = 0; i < Score.Num(); ++i)
	{
		Score[i]->AddActorWorldOffset(FVector(0.0f, 0.0f, 300.0f*deltaTime));
		Score[i]->UpdateTransparency(1.0f - LifeTimeElapsed / LifeTimeLimit);
	}
}