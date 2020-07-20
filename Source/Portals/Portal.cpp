#include "Portal.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SceneCaptureComponent2D.h"
#include "GameFramework/PlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "Kismet/KismetRenderingLibrary.h"
#include "Engine/GameViewportClient.h"
#include "Materials/MaterialInstanceDynamic.h"

APortal::APortal()
{
	PrimaryActorTick.bCanEverTick = true;

	RenderMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Portal Mesh"));
	SetRootComponent(RenderMesh);

	FrameMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Frame Mesh"));
	FrameMesh->SetupAttachment(RenderMesh);

	CaptureComponent = CreateDefaultSubobject<USceneCaptureComponent2D>(TEXT("Capture Component"));
	CaptureComponent->SetupAttachment(RenderMesh);
}

void APortal::BeginPlay()
{
	Super::BeginPlay();

	Player = UGameplayStatics::GetPlayerController(this, 0);

	if (Player)
	{
		int32 ViewportX;
		int32 ViewportY;
		Player->GetViewportSize(ViewportX, ViewportY);
		RenderTarget = UKismetRenderingLibrary::CreateRenderTarget2D(this, ViewportX, ViewportY);
		CaptureComponent->TextureTarget = RenderTarget;
		CaptureComponent->HiddenActors.Add(TargetPortal);

		if (TargetPortal && PortalMaterial)
		{
			UMaterialInstanceDynamic* DynamicMaterial = UMaterialInstanceDynamic::Create(PortalMaterial, this);
			DynamicMaterial->SetTextureParameterValue(PortalTextureParameterName, RenderTarget);

			TargetPortal->GetStaticMesh()->SetMaterial(0, DynamicMaterial);
		}
	}

	CaptureComponent->bEnableClipPlane = true;
	FVector Forward = GetActorForwardVector();
	CaptureComponent->ClipPlaneBase = GetActorLocation() - Forward * 5.0f;
	CaptureComponent->ClipPlaneNormal = Forward;
}

void APortal::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (TargetPortal)
	{
		TargetPortal->UpdateCaptureComponent(GetPlayerRelativePosition(), GetPlayerRelativeRotation());
	}
}

UStaticMeshComponent* APortal::GetStaticMesh()
{
	return RenderMesh;
}

void APortal::UpdateCaptureComponent(FVector LocalPosition, FRotator LocalRotation)
{
	FVector NewPosition = ConvertToWorldFromLocal(LocalPosition);
	NewPosition.X *= -1;
	NewPosition.Y *= -1;

	CaptureComponent->SetWorldLocation(GetActorLocation() + NewPosition);

	FRotator NewRotation = ConvertToWorldFromLocal(LocalRotation);
	NewRotation.Yaw += 180.0f;
	CaptureComponent->SetWorldRotation(NewRotation);
}

FVector APortal::GetPlayerRelativePosition()
{
	if (!Player) return FVector(0.0f);

	FVector PositionDifference = Player->PlayerCameraManager->GetCameraLocation() - GetActorLocation();

	return FVector(
		FVector::DotProduct(GetActorForwardVector(), PositionDifference),
		FVector::DotProduct(GetActorRightVector(), PositionDifference),
		FVector::DotProduct(GetActorUpVector(), PositionDifference)
		);
}

FRotator APortal::GetPlayerRelativeRotation()
{
	if (!Player) return FRotator(0.0f);
	
	FRotator Rotation = Player->PlayerCameraManager->GetCameraRotation();
	FTransform Transform = GetActorTransform();

	FQuat QuatRotation = FQuat(Rotation);
	FQuat LocalQuat = Transform.GetRotation().Inverse() * QuatRotation;
	return LocalQuat.Rotator();
}

FVector APortal::ConvertToWorldFromLocal(FVector LocalPosition)
{
	return
		GetActorForwardVector() * LocalPosition.X + 
		GetActorRightVector() * LocalPosition.Y + 
		GetActorUpVector() * LocalPosition.Z;
}

FRotator APortal::ConvertToWorldFromLocal(FRotator LocalRotation)
{
	FTransform Transform = GetActorTransform();
	FQuat LocalQuat = FQuat(LocalRotation);

	FQuat NewQuat = Transform.GetRotation() * LocalQuat;
	
	return NewQuat.Rotator();
}
