// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "TT_PlayerGridCamera.generated.h"

class UCapsuleComponent;
class USpringArmComponent;
class UCameraComponent;
class ATT_GridManager;

UCLASS()
class TINYTOWN_API ATT_PlayerGridCamera : public APawn
{
	GENERATED_BODY()
	
public:	

/*---------- Functions -----------*/
	ATT_PlayerGridCamera();


protected:

/*---------- Components ----------*/
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCapsuleComponent* CapsuleComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		USpringArmComponent* SpringArmComp;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
		UCameraComponent* CameraComp;


/*---------- Functions -----------*/

	virtual void BeginPlay() override;

	void InputKeyboardMovements(float donotuse); // Handles camera movements via keyboard

	void InputCameraMovements(); // Handles camera movements via mouse/touch

	void InputCameraRotation(); //Handles camera rotation via mouse

	void InputCameraZoom(float value); // Handles zoom via mouse
	   
	void InputSelectButtonDown(); //Handles Select button input
	void InputSelectButtonUp();

	void InputRotationButtonDown(); //Handles Rotation button input
	void InputRotationButtonUp();

	void MouseTrace(); // Get hit results under cursor, handles tile detection

	void MoveCamera(float X, float Y, float sensitivity); // Moves the camera in XY direction multiplied by Sensitivity
	void RotateCamera(float X, float Y, float XSensit, float YSensit); // Rotates the camera around Y and Z axis multiplied by Sensitivity

	ATT_GridManager* GetGridManager(); // TActor iterator to find grid manager

 
/*---------- Variables -----------*/

	// Movement sensitivity for moving the camera with mouse or touch drag
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float cameraDragSensitivty;

	// Movement sensitivity for moving the camera with keyboard
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float cameraDragKeyboardSensitivty;

	// Y axis rotation sensitivity using mouse or touch 
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float cameraRotationYSensitivity;

	// X axis rotation sensitivity using mouse or touch 
	UPROPERTY(EditAnywhere, Category = "Input Settings")
	float cameraRotationXSensitivity;

	// Maximum length for the spring arm
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MaxSpringArmLength;

	// Minimum length for the spring arm
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	float MinSpringArmLength;

	// Defines how accurately the player can zoom (the higher, the slower the zoom)
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	int zoomStepAccuracy;

	// Defines how fast the camera lerps when zooming and de-zooming (the higher, the faster)
	UPROPERTY(EditAnywhere, Category = "Camera Settings")
	int zoomLerpSpeed;


	ATT_GridManager* GridManager; // Current grid manager

	int zoomCurrentStep; //Temp var to track zoom steps
	int zoomCoefficient; //Length of one zoom step
	float newTargetArmLength; //New arm length after input (used to lerp on tick)

	bool isMovementEnabled; // Dictates whether the player can move the camera or not

	bool isSelectButtonDown; // Indicates if the Select Button (Left click / touch) is down
	bool isRotationButtonDown; // Indicates if the Rotate button (middle mouse / 2 fingers) is down
	bool isMovingCamera; // Indicates if the camera is being moved
	bool isRotatingCamera; // Indicates if the camera is being rotated

	int32 currentLinetracedTile; // Tile ID of the last tile to be linetraced to

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
