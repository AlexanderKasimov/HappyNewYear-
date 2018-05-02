// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSBaseWeapon.h"
#include "Components/SkeletalMeshComponent.h"  
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "PhysicalMaterials/PhysicalMaterial.h"
#include "TimerManager.h"
#include "FPSBasePlayerCharacter.h"
#include "HappyNewYear.h"
#include "Sound/SoundCue.h"
#include "DrawDebugHelpers.h"
#include "Camera/CameraComponent.h"

static int32 DebugWeaponDrawing = 0;

FAutoConsoleVariableRef CVARDebugWeaponDrawing(TEXT("FPS.DebugWeapons"), DebugWeaponDrawing, TEXT("Draw Debug Lines for Weapons"), ECVF_Cheat);

// Sets default values
AFPSBaseWeapon::AFPSBaseWeapon()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("MeshComponent"));
	RootComponent = MeshComponent;

	MuzzleSocketName = "MuzzleSocket";
	TracerTargetName = "BeamEnd";

	BaseDamage = 50.0f;
	BulletSpread = 1.0f;
	RateOfFire = 120;
	WeaponRange = 10000.0f;

	TotalAmmo = 120;
	CurrentAmmo = TotalAmmo;
	MagazineSize = 5;
	CurrentAmmoInMagazine = MagazineSize;
	ReloadTime = 2.3f;

}


// Called when the game starts or when spawned
void AFPSBaseWeapon::BeginPlay()
{
	Super::BeginPlay();

	TimeBetweenShots = 60 / RateOfFire;
	LastFireTime = -TimeBetweenShots;
}



// Called every frame
void AFPSBaseWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


//Input Functions
void AFPSBaseWeapon::StartFire()
{
	if (CurrentAmmoInMagazine > 0 && !OwningPlayer->GetReloading())
	{
		float FirstDelay = FMath::Max(LastFireTime + TimeBetweenShots - GetWorld()->TimeSeconds, 0.0f);

		GetWorldTimerManager().SetTimer(TimerHandle_TimeBetweenShots, this, &AFPSBaseWeapon::Fire, TimeBetweenShots, true, FirstDelay);
	}
	else if (CanReload())
	{
		StartReload();
	}

}


void AFPSBaseWeapon::StopFire()
{
	GetWorldTimerManager().ClearTimer(TimerHandle_TimeBetweenShots);
}


void AFPSBaseWeapon::StartReload()
{
	if (CanReload())
	{
		GetWorldTimerManager().SetTimer(TimerHandle_Reload, this, &AFPSBaseWeapon::Reload, ReloadTime, false, ReloadTime);
		OwningPlayer->SetReloading(true);
		UGameplayStatics::SpawnSoundAttached(ReloadSound, RootComponent);
	}
}


//Main Functions
void AFPSBaseWeapon::Fire()
{
	if (CurrentAmmoInMagazine == 0)
	{
		StopFire();
		StartReload();
		return;
	}

	CurrentAmmoInMagazine--;

	AActor* Owner = GetOwner();
	if (Owner)
	{
		FVector CameraLocation;
		FRotator CameraRotation;		
		APlayerController* PC = Cast<APlayerController>(Instigator->Controller);
		if (PC)
		{			
			PC->GetPlayerViewPoint(CameraLocation, CameraRotation);
		}
		else
		{
			CameraRotation = FRotator::ZeroRotator;
			CameraLocation = FVector::ZeroVector;
		}

		FVector ShotDirection = CameraRotation.Vector();

		float HalfRad = FMath::DegreesToRadians(BulletSpread);
		ShotDirection = FMath::VRandCone(ShotDirection, HalfRad, HalfRad);
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		FVector TraceEnd = CameraLocation + (ShotDirection * WeaponRange);
		//UE_LOG(LogTemp, Log, TEXT("TraceEnd is %s"), *TraceEnd.ToString());
		if (DebugWeaponDrawing)
		{
			UE_LOG(LogTemp, Log, TEXT("MuzzleLocation is %s"), *MuzzleLocation.ToString());
			DrawDebugSphere(GetWorld(), MuzzleLocation, 10.0f, 12, FColor::Red, true, -1.0f, 0, 1.0f);
			UE_LOG(LogTemp, Log, TEXT("CameraLocation is %s"), *CameraLocation.ToString());
			DrawDebugSphere(GetWorld(), CameraLocation, 10.0f, 12, FColor::Yellow, true, -1.0f, 0, 1.0f);		
		}

		EPhysicalSurface SurfaceType = SurfaceType_Default;

		FCollisionQueryParams TraceParams;
		TraceParams.AddIgnoredActor(Owner);
		TraceParams.AddIgnoredActor(this);
		TraceParams.bTraceComplex = true;
		TraceParams.bReturnPhysicalMaterial = true;

		FHitResult Hit;
		//Tracer Particle "Target" Parameter
		FVector TracerEndPoint = TraceEnd;

		if (GetWorld()->LineTraceSingleByChannel(Hit, CameraLocation, TraceEnd, COLLISION_WEAPON, TraceParams))
		{
			//Processing Hit			
			AActor* HitActor = Hit.GetActor();

			SurfaceType = UPhysicalMaterial::DetermineSurfaceType(Hit.PhysMaterial.Get());

			float ActualDamage = BaseDamage;

			if (SurfaceType == SURFACE_FLESHVULNERABLE)
			{
				ActualDamage *= 4.0f;
			}

			UGameplayStatics::ApplyPointDamage(HitActor, ActualDamage, ShotDirection, Hit, Owner->GetInstigatorController(), Owner, DamageType);

			PlayImpactEffects(SurfaceType, Hit.ImpactPoint);
			TracerEndPoint = Hit.ImpactPoint;
		/*	UE_LOG(LogTemp, Log, TEXT("TracerEndPoint is %s"),
				*TracerEndPoint.ToString());*/
			if (DebugWeaponDrawing)
			{
				DrawDebugLine(GetWorld(), CameraLocation, TracerEndPoint, FColor::Yellow, true, -1.0f, 0, 2.0f);
			}
		
		}
		else 
		{
			if (DebugWeaponDrawing)
			{
				DrawDebugLine(GetWorld(), CameraLocation, TracerEndPoint, FColor::Green, true, -1.0f, 0, 2.0f);
			}

		}
			

		PlayFireEffects(TracerEndPoint);

		LastFireTime = GetWorld()->TimeSeconds;
		if (DebugWeaponDrawing)
		{
			if (GetWorld()->LineTraceSingleByChannel(Hit, MuzzleLocation, TraceEnd, COLLISION_WEAPON, TraceParams))
			{
				TracerEndPoint = Hit.ImpactPoint;
				DrawDebugLine(GetWorld(), MuzzleLocation, TracerEndPoint, FColor::Red, true, -1.0f, 0, 2.0f);
			}
		}



	}
	if (CurrentAmmoInMagazine == 0)
	{
		StopFire();
		StartReload();
		return;
	}

}


void AFPSBaseWeapon::Reload()
{
	int32 ReloadingAmount = FMath::Min(CurrentAmmo, MagazineSize - CurrentAmmoInMagazine);
	CurrentAmmo -= ReloadingAmount;
	CurrentAmmoInMagazine += ReloadingAmount;
	OwningPlayer->SetReloading(false);	
}


//Control Functions
bool AFPSBaseWeapon::CanReload() const
{
	bool bMagazineNotFull = CurrentAmmoInMagazine < MagazineSize;
	bool bHasAmmo = CurrentAmmo > 0;
	return (bMagazineNotFull && bHasAmmo && !OwningPlayer->GetReloading());
}


//FX Functions
void AFPSBaseWeapon::PlayFireEffects(FVector TracerEndPoint)
{
	if (MuzzleEffect)
	{
		UGameplayStatics::SpawnEmitterAttached(MuzzleEffect, MeshComponent, MuzzleSocketName);
	}

	if (TracerEffect)
	{
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		UParticleSystemComponent* TracerComponent = UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), TracerEffect, MuzzleLocation);
		if (TracerComponent)
		{
			TracerComponent->SetVectorParameter(TracerTargetName, TracerEndPoint);
		}
	}

	if (FireSound)
	{
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, MuzzleLocation);
	}
}


void AFPSBaseWeapon::PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint)
{
	UParticleSystem* SelectedEffect = nullptr;
	USoundCue* SelectedSound = nullptr;

	switch (SurfaceType)
	{
	case SURFACE_FLESHDEFAULT:
		SelectedEffect = FleshImpactEffect;
		SelectedSound = FleshHitSound;
		break;
	case SURFACE_FLESHVULNERABLE:
		SelectedEffect = FleshImpactEffect;
		SelectedSound = FleshHitSound;
		break;
	default:
		SelectedEffect = DefaultImpactEffect;
		SelectedSound = DefaultHitSound;
		break;
	}

	if (SelectedEffect)
	{
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		FVector ShotDirection = ImpactPoint - MuzzleLocation;
		ShotDirection.Normalize();

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), SelectedEffect, ImpactPoint, ShotDirection.Rotation());
	}
	if (SelectedSound)
	{
		FVector MuzzleLocation = MeshComponent->GetSocketLocation(MuzzleSocketName);
		UGameplayStatics::PlaySoundAtLocation(this, SelectedSound, MuzzleLocation);
	}
}


//Setters
void AFPSBaseWeapon::SetOwningPlayer(AFPSBasePlayerCharacter * NewOwningPlayer)
{
	OwningPlayer = NewOwningPlayer;
}