// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSBasePlayerCharacter.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "FPSBaseWeapon.h"

// Sets default values
AFPSBasePlayerCharacter::AFPSBasePlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComponent"));
	//SpringArmComponent->SetupAttachment(RootComponent);
	//SpringArmComponent->bUsePawnControlRotation = true;

	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComponent"));
	CameraComponent->SetupAttachment(RootComponent);

	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Mesh1P"));
	Mesh1P->SetupAttachment(CameraComponent);

	ZoomedFOV = 65.0f;
	ZoomInterpSpeed = 20;

	bWantsToZoom = false;
	bReloading = false;

	WeaponAttachementSocketName = "WeaponSocket";

}

// Called when the game starts or when spawned
void AFPSBasePlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	
	DefaultFOV = CameraComponent->FieldOfView;

	SpawnWeapon();
}


// Called every frame
void AFPSBasePlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	float TargetFOV = bWantsToZoom ? ZoomedFOV : DefaultFOV;

	float NewFOV = FMath::FInterpTo(CameraComponent->FieldOfView, TargetFOV, DeltaTime, ZoomInterpSpeed);
	CameraComponent->SetFieldOfView(NewFOV);
}


// Called to bind functionality to input
void AFPSBasePlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSBasePlayerCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSBasePlayerCharacter::MoveRight);
	PlayerInputComponent->BindAxis("LookUp", this, &AFPSBasePlayerCharacter::AddControllerPitchInput);
	PlayerInputComponent->BindAxis("LookRight", this, &AFPSBasePlayerCharacter::AddControllerYawInput);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AFPSBasePlayerCharacter::Jump);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSBasePlayerCharacter::StartFire);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AFPSBasePlayerCharacter::StopFire);
	PlayerInputComponent->BindAction("Reload", IE_Pressed, this, &AFPSBasePlayerCharacter::OnReload);
	//PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AFPSBasePlayerCharacter::Zoom);

}


//Input Functions
void AFPSBasePlayerCharacter::MoveForward(float Value)
{
	AddMovementInput(GetActorForwardVector() * Value);
}


void AFPSBasePlayerCharacter::MoveRight(float Value)
{
	AddMovementInput(GetActorRightVector() * Value);
}


void AFPSBasePlayerCharacter::Zoom()
{
	bWantsToZoom = !bWantsToZoom;
}


void AFPSBasePlayerCharacter::StartFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFire();
	}
}


void AFPSBasePlayerCharacter::StopFire()
{
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFire();
	}
}


void AFPSBasePlayerCharacter::OnReload()
{
	if (CurrentWeapon && !bReloading)
	{
		CurrentWeapon->StopFire();
		CurrentWeapon->StartReload();
	}
}


//Weapon Spawn
void AFPSBasePlayerCharacter::SpawnWeapon()
{
	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	CurrentWeapon = GetWorld()->SpawnActor<AFPSBaseWeapon>(StarterWeaponClass, SpawnParams);

	if (CurrentWeapon)
	{
		CurrentWeapon->SetOwner(this);
		CurrentWeapon->SetOwningPlayer(this);
		CurrentWeapon->Instigator = this;
		CurrentWeapon->AttachToComponent(Mesh1P, FAttachmentTransformRules::SnapToTargetNotIncludingScale, WeaponAttachementSocketName);
	}

}


//Setters
void AFPSBasePlayerCharacter::SetReloading(bool bNewReloading)
{
	bReloading = bNewReloading;
}


void AFPSBasePlayerCharacter::SetWantsToZoom(bool bNewWantsToZoom)
{
	bWantsToZoom = bNewWantsToZoom;
}


//Getters
bool AFPSBasePlayerCharacter::GetReloading() const
{
	return bReloading;
}


AFPSBaseWeapon * AFPSBasePlayerCharacter::GetCurrentWeapon() const
{
	if (CurrentWeapon)
	{
		return CurrentWeapon;
	}
	return nullptr;
}

//Override

//FVector AFPSBasePlayerCharacter::GetPawnViewLocation() const
//{
//	if (CameraComponent)
//	{
//		return CameraComponent->GetComponentLocation();
//	}
//	return Super::GetPawnViewLocation();
//}