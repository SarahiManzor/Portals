// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Portal.generated.h"

class USceneCaptureComponent2D;
class UStaticMeshComponent;
class UTextureRenderTarget2D;
class APlayerController;
class UMaterialInterface;

UCLASS()
class PORTALS_API APortal : public AActor
{
	GENERATED_BODY()
	
// ----------Base Functions----------
public:
	APortal();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

// ----------Variables----------
public:
protected:
private:
	// Config
	UPROPERTY(EditDefaultsOnly)
	UMaterialInterface* PortalMaterial;

	UPROPERTY(EditDefaultsOnly)
	FName PortalTextureParameterName;

	// Components
	UPROPERTY(VisibleAnywhere)
	USceneCaptureComponent2D* CaptureComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RenderMesh;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* FrameMesh;

	// References
	UPROPERTY(EditAnywhere)
	APortal* TargetPortal;

	UTextureRenderTarget2D* RenderTarget;

	APlayerController* Player;

	// State

// ----------Custom Functions----------
public:
	UStaticMeshComponent* GetStaticMesh();
	void UpdateCaptureComponent(FVector LocalPosition, FRotator LocalRotation);
protected:
private:
	FVector GetPlayerRelativePosition();
	FVector ConvertToWorldFromLocal(FVector LocalPosition);

	FRotator GetPlayerRelativeRotation();
	FRotator ConvertToWorldFromLocal(FRotator LocalRotation);
};
