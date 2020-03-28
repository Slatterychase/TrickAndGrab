// Fill out your copyright notice in the Description page of Project Settings.


#include "SCharacter.h"
#include "..\Public\SCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/PawnMovementComponent.h"
#include "Particles/ParticleSystem.h"
#include "SWeapon.h"

// Sets default values
ASCharacter::ASCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	//Properly rotates camera based on view rotation
	SpringArmComp->bUsePawnControlRotation = true;
	SpringArmComp->SetupAttachment(RootComponent);

	//Needs to set crouch, even if not AI
	GetMovementComponent()->GetNavAgentPropertiesRef().bCanCrouch = true;

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	currentEquipmentSlot = 0;
	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	socketName = "Weapon Socket";
	
	
}

FVector ASCharacter::GetPawnViewLocation() const
{
	if (CameraComp) {
		return CameraComp->GetComponentLocation();
	}

	return Super::GetPawnViewLocation();
}

// Called when the game starts or when spawned
void ASCharacter::BeginPlay()
{
	Super::BeginPlay();
	/*for (int32 x = 0; x <= Weapons.Num(); x++) {
		gun = GetWorld()->SpawnActor<AActor>(Weapons[0], GetActorLocation(), FRotator::ZeroRotator);
		gun->SetOwner(GetOwner());
		socketName = "Weapon Socket";
		gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, socketName);
		currentGun = gun;
	}*/

	DefaultFOV = CameraComp->FieldOfView;
	//Spawn default weapon

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	CurrentWeapon = GetWorld()->SpawnActor<ASWeapon>(StarterWeaponClass, FVector::ZeroVector, FRotator::ZeroRotator, SpawnParams);

	if (CurrentWeapon) {
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, socketName);

	}

}

void ASCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}

void ASCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}

void ASCharacter::BeginCrouch()
{
	Crouch();
	
}

void ASCharacter::EndCrouch()
{
	UnCrouch();
}

void ASCharacter::BeginZoom()
{
	bWantsToZoom = true;
}

void ASCharacter::EndZoom()
{
	bWantsToZoom = false;
}

void ASCharacter::ScrollEquipmentUp()
{
	/*maxEquipmentSize = Weapons.Num() - 1;
	++currentEquipmentSlot;
	if (currentEquipmentSlot > maxEquipmentSize) {
		currentEquipmentSlot = 0;
	}
	gun->SetActorHiddenInGame = false;
	gun = GetWorld()->SpawnActor<AActor>(Weapons[0], GetActorLocation(), FRotator::ZeroRotator);
	gun->SetOwner(GetOwner());
	socketName = "Weapon Socket";
	gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, socketName);*/
	
	
	
	
}

void ASCharacter::ScrollEquipmentDown()
{	
	//maxEquipmentSize = Weapons.Num() - 1;
	//--currentEquipmentSlot;
	//if (currentEquipmentSlot < 0) {
	//	currentEquipmentSlot = maxEquipmentSize;
	//}
	//previousGun = gun;
	//gun = GetWorld()->SpawnActor<AActor>(Weapons[0], GetActorLocation(), FRotator::ZeroRotator);
	//gun->SetOwner(GetOwner());
	//socketName = "Weapon Socket";
	//gun->AttachToComponent(GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, socketName);
	//currentGun = gun;
}






void ASCharacter::Fire()
{
	if (CurrentWeapon) {
		CurrentWeapon->Fire();
	}
}

// Called every frame
void ASCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//true first, false second option
	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComp->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	CameraComp->SetFieldOfView(NewFOV);

}

// Called to bind functionality to input
void ASCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &ASCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &ASCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &ASCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("Turn", this, &ASCharacter::AddControllerYawInput);
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ASCharacter::Jump);

	PlayerInputComponent->BindAction("Crouch", IE_Pressed, this, &ASCharacter::BeginCrouch);
	PlayerInputComponent->BindAction("Crouch", IE_Released, this, &ASCharacter::EndCrouch);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &ASCharacter::BeginZoom);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &ASCharacter::EndZoom);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("InventoryUp", IE_Pressed, this, &ASCharacter::ScrollEquipmentUp);
	PlayerInputComponent->BindAction("InventoryDown", IE_Pressed, this, &ASCharacter::ScrollEquipmentDown);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &ASCharacter::Fire);
}

