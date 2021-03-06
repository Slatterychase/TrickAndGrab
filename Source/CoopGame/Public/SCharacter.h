// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "SWeapon.h"
#include "SCharacter.generated.h"


//Tells compiler it is a class
class UCameraComponent;
class USpringArmComponent;
class ASWeapon;
class USHealthComponent;

UCLASS()
class COOPGAME_API ASCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	ASCharacter();

	virtual FVector GetPawnViewLocation() const override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void MoveForward(float Value);

	void MoveRight(float Value);

	void BeginCrouch();

	void EndCrouch();

	void BeginZoom();

	void EndZoom();

	void ScrollEquipmentUp();

	void ScrollEquipmentDown();

	void EquipWeapon(ASWeapon* equippedWeapon);

	void ToggleWeapon(int32 equipSlot);

	void ToggleFireMode();

	void ReloadWeapon();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USpringArmComponent* SpringArmComp;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Replicated, Category = "Equipment")
	TArray<TSubclassOf<ASWeapon>> Weapons;
	/*UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Equipment", Replicated)
	ASWeapon* currentGun;*/
	
	ASWeapon* previousGun;
	int32 currentEquipmentSlot;

	UPROPERTY(Replicated)
	int32 maxEquipmentSize;

	ASWeapon* gun;
	
	bool bWantsToZoom;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	float DefaultFOV;

	UPROPERTY(VisibleDefaultsOnly, Category = "Player")
	FName socketName;

	UPROPERTY(Replicated)
	ASWeapon* CurrentWeapon;


	void StartFire();
	void StopFire();

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	TSubclassOf<ASWeapon> StarterWeaponClass;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USHealthComponent* HealthComponent;

	UFUNCTION()
	void OnHealthChanged(USHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDied;


public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
