// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameMode.h"
#include "TetrineGameMode.generated.h"

/**
 * 
 */
UCLASS()
class TETRINE_API ATetrineGameMode : public AGameMode
{
	GENERATED_BODY()

		virtual void StartPlay();

	// Reference UMG Asset in the Editor
	//UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	//	TSubclassOf<class UUserWidget> wSidebarDisplay;


//	class UUserWidget* MySidebarDisplay;
	
	
};
