// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSBaseWeapon.generated.h"

class USoundCue;

UCLASS()
class HAPPYNEWYEAR_API AFPSBaseWeapon : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSBaseWeapon();

	//Input Functions
	void StartFire();

	void StopFire();

	void StartReload();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Main Functions
	virtual void Fire();

	void Reload();

	//Control Functions
	bool CanReload() const;

	//FX Functions
	void PlayFireEffects(FVector TracerEndPoint);

	void PlayImpactEffects(EPhysicalSurface SurfaceType, FVector ImpactPoint);

	//Components
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Components")
	USkeletalMeshComponent* MeshComponent;

	//Weapon Stats vars
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	TSubclassOf<UDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float BaseDamage;

	//RPM
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float RateOfFire;

	/*BulletSpread in Degrees */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = 0.0f))
	float BulletSpread;
	
	//Range in CM - /100 to get Meters
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon", meta = (ClampMin = 0.0f))
	float WeaponRange;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 TotalAmmo;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 MagazineSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	int32 CurrentAmmoInMagazine;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Weapon")
	float ReloadTime;

	//Timers Handles
	FTimerHandle TimerHandle_TimeBetweenShots;
	FTimerHandle TimerHandle_Reload;

	//FX
	//Shot FX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem* MuzzleEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem* TracerEffect;

	//Impact FX
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem* DefaultImpactEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FX")
	UParticleSystem* FleshImpactEffect;

	//Sounds
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* ReloadSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* FireSound;

	//Impact Sounds
	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* DefaultHitSound;

	UPROPERTY(EditDefaultsOnly, Category = "Sounds")
	USoundCue* FleshHitSound;

	//Naming vars
	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName MuzzleSocketName;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Weapon")
	FName TracerTargetName;


	//UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	//TSubclassOf<UCameraShake> FireCamShake;

	//AutoFire control vars
	float LastFireTime;

	float TimeBetweenShots;

	//Helping vars
	class AFPSBasePlayerCharacter* OwningPlayer;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	//Setters
	void SetOwningPlayer(AFPSBasePlayerCharacter* NewOwningPlayer);


	
	
};
