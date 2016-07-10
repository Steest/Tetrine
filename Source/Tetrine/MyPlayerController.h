// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "MyPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class TETRINE_API AMyPlayerController : public APlayerController
{
	GENERATED_BODY()

	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	//TSubclassOf<class UUserWidget> wSideBarDisplay;
	//UUserWidget* MySideBarDisplay;

	virtual void BeginPlay() override;
	
};
