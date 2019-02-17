// Fill out your copyright notice in the Description page of Project Settings.

#include "TT_PlayerGridCamera.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/CapsuleComponent.h"
#include "EngineUtils.h"
#include "TT_GridManager.h"
#include "TT_BlockManager.h"


/*---------- Primary functions ----------*/
ATT_PlayerGridCamera::ATT_PlayerGridCamera()
{	
/*---------- Components creation ----------*/
	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleComp"));
	RootComponent = CapsuleComp;

	SpringArmComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArmComp"));
	SpringArmComp->SetupAttachment(RootComponent);

	CameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("CameraComp"));
	CameraComp->SetupAttachment(SpringArmComp);

	
/*---------- Setting defaults ----------*/
	PrimaryActorTick.bCanEverTick = true;



}

void ATT_PlayerGridCamera::BeginPlay()
{
	Super::BeginPlay();

	GridManager = GetGridManager();
	isMovementEnabled = true;

	//Set camera zoom values
	zoomCurrentStep = zoomStepAccuracy / 2;
	zoomCoefficient = (MaxSpringArmLength - MinSpringArmLength) / zoomStepAccuracy;
	InputCameraZoom(1);
}

void ATT_PlayerGridCamera::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Update SpringArmLength with a lerp
	if (SpringArmComp->TargetArmLength != newTargetArmLength) 
	{
		SpringArmComp->TargetArmLength = FMath::Lerp(SpringArmComp->TargetArmLength, newTargetArmLength, zoomLerpSpeed * DeltaTime);
	}

	//Linetrace cursor to 3D world
	MouseTrace();

	InputCameraMovements();
	InputCameraRotation();
}

ATT_GridManager* ATT_PlayerGridCamera::GetGridManager()
{
	for (TActorIterator<ATT_GridManager> ActorItr(GetWorld()); ActorItr; ++ActorItr) {
		if (ActorItr->IsA(ATT_GridManager::StaticClass()))
		{
			return *ActorItr;
		}
	}
	UE_LOG(LogTemp, Log, TEXT("Couldn't get GridManager in PlayerGridCamera."))
	return nullptr;
}


/*---------- Player input functions ----------*/
void ATT_PlayerGridCamera::InputCameraZoom(float value)
{
	if (isMovementEnabled && value != 0) {

		if (value < -0.1f) {
			zoomCurrentStep = FMath::Clamp(zoomCurrentStep + 1 , 1, zoomStepAccuracy);

			newTargetArmLength = zoomCurrentStep * zoomCoefficient + MinSpringArmLength;
		}

		if (value > 0.1f){
			zoomCurrentStep = FMath::Clamp(zoomCurrentStep - 1, 1, zoomStepAccuracy);

			newTargetArmLength = zoomCurrentStep * zoomCoefficient + MinSpringArmLength;
		}

		UE_LOG(LogTemp, Log, TEXT("Zoom Step = %s"), *FString::FromInt(zoomCurrentStep));

	}
}

void ATT_PlayerGridCamera::InputKeyboardMovements(float donotuse) 
{
	if (isMovementEnabled) {
		isMovingCamera = true;

		MoveCamera(GetInputAxisValue("MoveRight"), GetInputAxisValue("MoveForward"), cameraDragKeyboardSensitivty);
	}
	else {
		isMovingCamera = false;
	}
} 

void ATT_PlayerGridCamera::InputCameraMovements()
{
	if (isMovementEnabled) {

		if (isMoveButtonDown) {

			isMovingCamera = true;

			MoveCamera(-GetInputAxisValue("MouseX"), -GetInputAxisValue("MouseY"), cameraDragSensitivty);
		}

		else {
			isMovingCamera = false;
		}
	}
}

void ATT_PlayerGridCamera::InputCameraRotation()
{
	if (isMovementEnabled) {

		if (isRotationButtonDown) {

			isRotatingCamera = true;

			RotateCamera(GetInputAxisValue("MouseX"), GetInputAxisValue("MouseY"), cameraRotationXSensitivity, cameraRotationYSensitivity);
		}

		else {
			isRotatingCamera = false;
		}
	}
}

void ATT_PlayerGridCamera::InputKeyboardRotation(float value)
{
	if (isMovementEnabled) {
		isRotatingCamera = true;
		RotateCamera(-value, 0.0f, cameraRotationKeyboardSensitivty, 0);
	}

	else {
		isRotatingCamera = false;
	}

}

void ATT_PlayerGridCamera::InputSelectButtonDown()
{
	isSelectButtonDown = true;

	if (currentLinetracedTile != -1) {
		GridManager->TileClicked(currentLinetracedTile);
	}

	if (isMovementEnabled) {

	}
}

void ATT_PlayerGridCamera::InputSelectButtonUp()
{
	isSelectButtonDown = false;

	if (currentLinetracedTile != -1) {
		GridManager->TileClearState();
	}
}

void ATT_PlayerGridCamera::InputRotationButtonDown()
{
		isRotationButtonDown = true;
}

void ATT_PlayerGridCamera::InputRotationButtonUp()
{
	isRotationButtonDown = false;
}

void ATT_PlayerGridCamera::InputMoveButtonDown()
{
	isMoveButtonDown = true;
}

void ATT_PlayerGridCamera::InputMoveButtonUp()
{
	isMoveButtonDown = false;
}


/*---------- Camera movement functions ----------*/
void ATT_PlayerGridCamera::MoveCamera(float X, float Y, float sensitivity)
{
	//Avoids a speed increase when both X and Y axis = 1.
	float axisYNormalized = Y * FMath::Abs(FMath::Sin(FMath::Atan2(Y, X)));
	float axisXNormalized = X * FMath::Abs(FMath::Cos(FMath::Atan2(Y, X)));

	FVector movementVector = FVector(axisYNormalized * sensitivity, axisXNormalized * sensitivity, 0.0f);

	AddActorLocalOffset(movementVector);
}

void ATT_PlayerGridCamera::RotateCamera(float X, float Y, float XSensit, float YSensit)
{

	FRotator currentRotation = SpringArmComp->GetRelativeTransform().GetRotation().Rotator();

	if (currentRotation.Pitch >= -5.0f ){
		SpringArmComp->SetRelativeRotation(FRotator(-5.1f, 0, 0));
		return;
	}

	if (currentRotation.Pitch <= -85.0f) {
		SpringArmComp->SetRelativeRotation(FRotator(-84.9f, 0, 0));
		return;
	}

		FRotator newRotationY = FRotator(Y, 0, 0) * YSensit;
		SpringArmComp->AddLocalRotation(newRotationY);

		FRotator newRotationX = FRotator(0, X, 0) * XSensit;
		AddActorLocalRotation(newRotationX);
}


/*---------- Grid interaction functions ----------*/

void ATT_PlayerGridCamera::BuildUnderCursor()
{
	FHitResult Hit;
	if (GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, true, Hit))
	{
		if (Hit.GetActor()->IsA(ATT_GridManager::StaticClass())) {

				GridManager->BlockManager->CreateBuildingOnTile(Hit.Item);
		}
	}
}

void ATT_PlayerGridCamera::MouseTrace()
{
	if (GridManager && !isSelectButtonDown)
	{
		FHitResult Hit;
		if (GetWorld()->GetFirstPlayerController()->GetHitResultUnderCursor(ECollisionChannel::ECC_Camera, true, Hit))
		{
			if (Hit.Item != currentLinetracedTile) {

				currentLinetracedTile = Hit.Item;
				GridManager->TileHovered(currentLinetracedTile);
			}
		}

		else {
			GridManager->TileClearState();
			currentLinetracedTile = -1;
		}
	}
}


/*---------- Input binding ----------*/
void ATT_PlayerGridCamera::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	//Keyboard
	PlayerInputComponent->BindAxis("MoveForward", this, &ATT_PlayerGridCamera::InputKeyboardMovements);
	PlayerInputComponent->BindAxis("MoveRight", this, &ATT_PlayerGridCamera::InputKeyboardMovements);
	PlayerInputComponent->BindAxis("XRotation", this, &ATT_PlayerGridCamera::InputKeyboardRotation);
	PlayerInputComponent->BindAction("Build", IE_Pressed, this, &ATT_PlayerGridCamera::BuildUnderCursor);


	//Mouse
	PlayerInputComponent->BindAction("InputSelect", IE_Pressed, this, &ATT_PlayerGridCamera::InputSelectButtonDown);
	PlayerInputComponent->BindAction("InputSelect", IE_Released, this, &ATT_PlayerGridCamera::InputSelectButtonUp);
	PlayerInputComponent->BindAction("InputRotation", IE_Pressed, this, &ATT_PlayerGridCamera::InputRotationButtonDown);
	PlayerInputComponent->BindAction("InputRotation", IE_Released, this, &ATT_PlayerGridCamera::InputRotationButtonUp);
	PlayerInputComponent->BindAction("InputMovement", IE_Pressed, this, &ATT_PlayerGridCamera::InputMoveButtonDown);
	PlayerInputComponent->BindAction("InputMovement", IE_Released, this, &ATT_PlayerGridCamera::InputMoveButtonUp);
	PlayerInputComponent->BindAxis("ZoomIn", this, &ATT_PlayerGridCamera::InputCameraZoom);
	PlayerInputComponent->BindAxis("MouseY");
	PlayerInputComponent->BindAxis("MouseX");

}
