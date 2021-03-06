// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SWeapon.h"
#include "ProjectileWeapon.generated.h"

/**
 * 
 */
UCLASS()
class COOPGAME_API AProjectileWeapon : public ASWeapon
{
	GENERATED_BODY()
	

protected:
	virtual void Fire() override;


	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
	TSubclassOf<AActor> ProjectileClass;

	UPROPERTY(EditDefaultsOnly, Category = "ProjectileWeapon")
		TSubclassOf<AActor> AltProjectileClass;
};
