// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPSBasePlayerCharacter.generated.h"

class AFPSBaseWeapon;
class UCameraComponent;
class USpringArmComponent;

UCLASS()
class HAPPYNEWYEAR_API AFPSBasePlayerCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AFPSBasePlayerCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	//Input Functions

	void MoveForward(float Value);

	void MoveRight(float Value);

	void Zoom();

	void StartFire();

	void StopFire();

	void OnReload();

	//Spawn Functions

	void SpawnWeapon();

	//Components

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UCameraComponent* CameraComponent;

	//UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	//USpringArmComponent* SpringArmComponent;

	UPROPERTY(VisibleDefaultsOnly, BlueprintReadOnly, Category = "Components")
	USkeletalMeshComponent* Mesh1P;

	//Weapon vars
	UPROPERTY(VisibleDefaultsOnly, Category = "Weapon")
	FName WeaponAttachementSocketName;

	UPROPERTY(EditDefaultsOnly, Category = "Weapon")
	TSubclassOf<AFPSBaseWeapon> StarterWeaponClass;

	AFPSBaseWeapon* CurrentWeapon;
	
	//Zoom vars

	//Set at begin play
	float DefaultFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player")
	float ZoomedFOV;

	UPROPERTY(EditDefaultsOnly, Category = "Player", meta = (ClampMin = 0.1, ClampMax = 100))
	float ZoomInterpSpeed;

	//State vars
	bool bWantsToZoom;

	bool bReloading;

	UPROPERTY(BlueprintReadOnly, Category = "Player")
	bool bDead;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	//virtual FVector GetPawnViewLocation() const override;

	//Setters
	void SetReloading(bool bNewReloading);

	UFUNCTION(BlueprintCallable)
	void SetWantsToZoom(bool bNewWantsToZoom);

	//Getters
	bool GetReloading() const;

	UFUNCTION(BlueprintCallable)
	AFPSBaseWeapon* GetCurrentWeapon() const;
	
};
