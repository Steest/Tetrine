// Fill out your copyright notice in the Description page of Project Settings.

#include "Tetrine.h"
#include "ScoreBox.h"
#include "Digit.h"
#include "PaperSpriteComponent.h"

DEFINE_LOG_CATEGORY(Scorebox_log);
AScoreBox::AScoreBox()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> PlusSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/gplus_Sprite.gplus_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> ZeroSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/gzero_Sprite.gzero_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> OneSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/gone_Sprite.gone_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> TwoSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/gtwo_Sprite.gtwo_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> ThreeSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/gthree_Sprite.gthree_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> FourSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/gfour_Sprite.gfour_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> FiveSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/gfive_Sprite.gfive_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> SixSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/gsix_Sprite.gsix_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> SevenSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/gseven_Sprite.gseven_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> EightSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/geight_Sprite.geight_Sprite'"));
	ConstructorHelpers::FObjectFinderOptional<UPaperSprite> NineSpriteAsset(TEXT("PaperSprite'/Game/Art/Digits/gnine_Sprite.gnine_Sprite'"));
	
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
	LifeTimeLimit = 1.0f;
	AnimateRate = 0.10f;
	Rate = 0.10f;
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
	else if (LifeTimeElapsed / LifeTimeLimit > AnimateRate) { AnimateScoreColor(); AnimateRate += Rate; }
	else { UpdatePositions(DeltaTime); }
}


void AScoreBox::SetScore(FString number, FVector scoreBoxLocation)
{
	if (Score.Num() == 0)
	{
		FVector temp = FVector(0.0f, 10.0f, 0.0f) + scoreBoxLocation;
		Score.Add(GetWorld()->SpawnActor<ADigit>(ADigit::StaticClass()));
		Score[0]->SetSprite(PlusSprite);
		Score[0]->AddActorWorldOffset(FVector(0.0f, 10.0f, 0.0f)+scoreBoxLocation);
		Score[0]->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f)*Scale);
		RandomlyColorDigit(Score[0]);
	}

	for (int i = 0; i < number.Len(); ++i)
	{
		Score.Add(GetWorld()->SpawnActor<ADigit>(ADigit::StaticClass()));
		Score[i+1]->SetSprite(GetSprite(number[i]));
		Score[i+1]->SetActorScale3D(FVector(1.0f, 1.0f, 1.0f)*Scale);
		Score[i+1]->AddActorWorldOffset(FVector( (i+1)*(Scale*(200.0f)) , 10.0f, 0.0f)+scoreBoxLocation);
		RandomlyColorDigit(Score[i+1]);
	}
	LifeTimeElapsed = 0.0f;
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
		Score[i]->AddActorWorldOffset(FVector(0.0f, 0.0f, 1000.0f*deltaTime));
		Score[i]->UpdateTransparency(1.0f - LifeTimeElapsed / LifeTimeLimit);
	}
}

void AScoreBox::RandomlyColorDigit(ADigit* digit)
{
	FColor newColor;
	float randy = FMath::RandRange(0.0f, 1.0f); 
	float percent = (1.0 / 7.0);

	if (randy <= percent) { newColor = FColor::FromHex("#E14B32"); } //red
	else if (randy <= percent*2) { newColor = FColor::FromHex("#3264E1"); } //blue
	else if (randy <= percent*3) { newColor = FColor::FromHex("#19C819"); } //green
	else if (randy <= percent*4) { newColor = FColor::FromHex("#FA9619"); } //orange
	else if (randy <= percent*5) { newColor = FColor::FromHex("#E1E119"); } //yellow
	else if (randy <= percent*6) { newColor = FColor::FromHex("#7D32FA"); }// purple
	else { newColor = FColor::FromHex("#FA32C8"); } // pink

	digit->sprite->SetSpriteColor(newColor);
}

void AScoreBox::AnimateScoreColor()
{
	FLinearColor lastColor = Score[Score.Num() - 1]->sprite->GetSpriteColor();
	UE_LOG(Scorebox_log, Error, TEXT("LENGTH IS: %d"), Score.Num());
	for (int i = Score.Num()-1; i > 0; --i)
	{
		Score[i]->sprite->SetSpriteColor(Score[i - 1]->sprite->GetSpriteColor());
	}
	Score[0]->sprite->SetSpriteColor(lastColor);
}