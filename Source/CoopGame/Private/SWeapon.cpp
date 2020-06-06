// Fill out your copyright notice in the Description page of Project Settings.


#include "SWeapon.h"
#include "..\Public\SWeapon.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Particles/ParticleSystem.h"
#include "Components/SkeletalMeshComponent.h"
#include "Particles/ParticleSystemComponent.h"
#include "CoopGame.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "Net/UnrealNetwork.h"

static int32 DebugWeaponDrawing = 0;
FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("COOP.DebugWeapons"),
	DebugWeaponDrawing,
	TEXT("Draw Debug Lines for Weapons"),
	ECVF_Cheat);
// Sets default values
ASWeapon::ASWeapon()
{

	MeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	MuzzleSocketName = "MuzzleSocket";
	AltFireType = false;
	BaseDamage = 20;

	RateOfFire = 600;
	AltFireRate = 1;
	CurrentAmmo = ClipSize;

	//Causes it to Replicate from server to all of the clients
	SetReplicates(true);

}
void ASWeapon::BeginPlay()
{
	Super::BeginPlay();

	ShotGap = 60 / RateOfFire;
}
void ASWeapon::Fire()
{
	//If running function on client, tell server to fire
	if (Role < ROLE_Authority) {
		ServerFire();
	}
	//All this called by the server as a request
	// Trace world from pawn eyes to crosshair location
	if (CurrentAmmo > 0) {
		AActor* MyOwner = GetOwner();

		if (MyOwner) {
			FVector EyeLocation;
			FRotator EyeRotation;
			MyOwner->GetActorEyesViewPoint(EyeLocation, EyeRotation);

			FVector ShotDirection = EyeRotation.Vector();

			FVector TraceEnd = EyeLocation + (ShotDirection * 10000);

			FCollisionQueryParams QueryParams;
			QueryParams.AddIgnoredActor(MyOwner);
			QueryParams.AddIgnoredActor(this);
			QueryParams.bTraceComplex = true;
			QueryParams.bReturnPhysicalMaterial = true;

			//Particle "Target" Param
			FVector TracerEndPoint = TraceEnd;

			FHitResult Hit;
			if (GetWorld()->LineTraceSingleByChannel(Hit, EyeLocation, TraceEnd, COLLISION_WEAPON, QueryParams))
			{

				//Blocking Hit! Process damage
				AActor* HitActor = Hit.GetActor();



				UParticleSystem* SelectedEffect = nullptr;
				EPhysicalSurface SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

				float ActualDamage = BaseDamage;
				if (SurfaceType == SURFACE_FLESHVULNERABLE) {
					ActualDamage *= 2;
				}
				UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, MyOwner->GetInstigatorController(), this, DamageType);
				switch (SurfaceType)
				{
				case SURFACE_FLESHDEFAULT:
					SelectedEffect = SmallFleshImpactEffect;
					break;
				case SURFACE_FLESHVULNERABLE:
					SelectedEffect = FleshImpactEffect;
					break;
				default:
					SelectedEffect = DefaultImpactEffect;
					break;
				}
				if (SelectedEffect) {

					UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, Hit.ImpactPoint, Hit.ImpactNormal.Rotation());
				}
				TracerEndPoint = Hit.ImpactPoint;


			}
			if (DebugWeaponDrawing > 0) {
				DrawDebugLine(GetWorld(), EyeLocation, TraceEnd, FColor::Red, false, 1.0f, 0, 1.0f);
			}


			PlayFireEffects(TracerEndPoint);

			if (Role == ROLE_Authority) {
				HitScanTrace.TraceTo = TracerEndPoint;
			}
			LastFireTime = GetWorld()->TimeSeconds;

		}
		CurrentAmmo--;
	}
	
}

void ASWeapon::StartFire()
{
	
		if (AltFireType) {

			ShotGap = AltFireRate;
		}
		else {
			ShotGap = 60 / RateOfFire;
		}
		float FirstDelay = FMath::Max(LastFireTime + ShotGap - GetWorld()->TimeSeconds, 0.0f);


		GetWorldTimerManager().SetTimer(TimeBetweenShots, this, &ASWeapon::Fire, ShotGap, true, FirstDelay);
	
	
}

void ASWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimeBetweenShots);
}

void ASWeapon::Reload()
{
	if (TotalAmmo < (ClipSize - CurrentAmmo) && TotalAmmo > 0) 
	{
		CurrentAmmo = TotalAmmo;
		TotalAmmo = 0;
	}
	else if (TotalAmmo >= (ClipSize - CurrentAmmo)) 
	{
		TotalAmmo -= ClipSize - CurrentAmmo;
		CurrentAmmo = ClipSize;
	}
}

void ASWeapon::SwapFireType()
{
	AltFireType = !AltFireType;
}



void ASWeapon::PlayFireEffects(FVector TracerEndPoint)
{

	if (MuzzleEffect) {
		//attatched means it will follow along
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComp, MuzzleSocketName);
	}



	if (TracerEffect) {
		FVector MuzzleLocation = MeshComp->GetSocketLocation(MuzzleSocketName);

		UParticleSystemComponent* TracerComp = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComp) {
			TracerComp->SetVectorParameter("BeamEnd", TracerEndPoint);
		}
	}
}
void ASWeapon::OnRep_HitScanTrace()
{
	//Play cosmetic FX
	PlayFireEffects(HitScanTrace.TraceTo);
}
//Needs _Implementation since it is a server function
void ASWeapon::ServerFire_Implementation()
{
	ASWeapon::Fire();
}

//Needs _Validate since it is a server function with With Validation keyword
bool ASWeapon::ServerFire_Validate()
{
	//If anticheat mattered itd go here
	return true;
}

//This specifies what we want to replicate and how
void ASWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	//Doesnt replicate on owner
	DOREPLIFETIME_CONDITION(ASWeapon, HitScanTrace, COND_SkipOwner);
	
}

