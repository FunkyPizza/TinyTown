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

	// Handles camera movements via keyboard
	void InputKeyboardMovements(float donotuse); 

	// Handles camera movements via mouse/touch
	void InputCameraMovements();

	//Handles camera rotation via mouse
	void InputCameraRotation(); 

	//Handles camera rotation via keyboard
	void InputKeyboardRotation(float value); 

	// Handles zoom via mouse
	void InputCameraZoom(float value); 

	//Handles Select button input
	void InputSelectButtonDown(); 
	void InputSelectButtonUp();

	//Handles Rotation button input
	void InputRotationButtonDown(); 
	void InputRotationButtonUp();

	//Handles Select button input
	void InputMoveButtonDown();
	void InputMoveButtonUp();

	// Get hit results under cursor, handles tile detection
	void MouseTrace(); 

	// Moves the camera in XY direction multiplied by Sensitivity
	void MoveCamera(float X, float Y, float sensitivity); 

	// Rotates the camera around Y and Z axis multiplied by Sensitivity
	void RotateCamera(float X, float Y, float XSensit, float YSensit); 

	//Prototype of a building mechanic
	void BuildUnderCursor(); 

	// TActor iterator to find grid manager
	ATT_GridManager* GetGridManager(); 

 
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

	// Movement sensitivity for moving the camera with keyboard
	UPROPERTY(EditAnywhere, Category = "Input Settings")
		float cameraRotationKeyboardSensitivty;

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

	// Current grid manager
	ATT_GridManager* GridManager; 

	//Zoom variables
	int zoomCurrentStep; 
	int zoomCoefficient;
	float newTargetArmLength;

	// Dictates whether the player can move the camera or not
	bool isMovementEnabled; 

	bool isSelectButtonDown; // Indicates if the Select Button (Left click / touch) is down
	bool isRotationButtonDown; // Indicates if the Rotate button (middle mouse / 2 fingers) is down
	bool isMoveButtonDown; // Indicates if the Move button (right click) is down
	bool isMovingCamera; // Indicates if the camera is being moved
	bool isRotatingCamera; // Indicates if the camera is being rotated

	int32 currentLinetracedTile; // Tile ID of the last tile to be linetraced to

public:	

	virtual void Tick(float DeltaTime) override;

	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

};
