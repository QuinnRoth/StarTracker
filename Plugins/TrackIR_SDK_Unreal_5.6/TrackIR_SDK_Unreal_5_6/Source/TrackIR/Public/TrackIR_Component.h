// *******************************************************************************
// * Copyright 2023, NaturalPoint Inc.
// *******************************************************************************
// *
// * Description:
// *   Header file for TrackIR_Component.cpp module. Contains properties and function
// *   definitions for the TrackIR component in Unreal Engine. 
// *   
// *   The component is an Actor Component and has exposed functions and properties 
// *   to interact with in Blueprints. All you have to do is add the component to an
// *   actor and select which camera component you want TrackIR to change.
// *
// *******************************************************************************

#pragma once

#include "CoreMinimal.h"
#include "Camera/CameraComponent.h"
#include "TrackIR_Component.generated.h"

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ))
class TRACKIR_API UTrackIR : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTrackIR();

public:

	/// <summary>
	/// Developer ID provided by NaturalPoint. Default to 1000 (TrackIR SDK dev ID).
	/// </summary>
	UPROPERTY( EditAnywhere, Category = "TrackIR Settings" )
		int DeveloperID{ 1000 };

	/// <summary>
	/// Enable or disable TrackIR tracking.
	/// </summary>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "TrackIR Settings" )
		bool TrackIREnabled{ true };

	/// <summary>
	/// Rate at which we poll TrackIR data
	/// </summary>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "TrackIR Settings" )
		float RefreshRate{ 0.001f };

	/// <summary>
	/// Automatically turn off pawn rotation control setting on selected camera. This will toggle back on after the camera is changed.
	/// </summary>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, AdvancedDisplay, SaveGame, Category = "TrackIR Settings" )
		bool AutoChangePawnRotation{ true };

	/// <summary>
	/// If set to true, red debug text will appear in top right corner, desplaying tracking information coming from TrackIR
	/// If false, then no text will appear.
	/// </summary>
	UPROPERTY( EditAnywhere, AdvancedDisplay, SaveGame, Category = "TrackIR Settings" )
		bool ShowDebugMesages{ true };

	/// <summary>
	/// Dropdown menu for selecting a camera by name for TrackIR to control
	/// </summary>
	UPROPERTY( EditAnywhere, Category = "TrackIR Settings", meta = ( GetOptions = "GetNameOptions", DisplayName = "Target Camera" ) )
		FName CameraNames;

	/// <summary>
	/// Updates camera list when drop down menu is pulled down.
	/// </summary>
	UFUNCTION( CallInEditor )
		TArray<FString> GetNameOptions() const;

	/// <summary>
	/// The camera which TrackIR controls. Editable in Blueprints.
	/// </summary>
	UPROPERTY( BlueprintReadWrite, SaveGame, Category = "TrackIR Settings" )
		UCameraComponent* SelectedCamera;

	/// <summary>
	/// Camera offset for TrackIR to use. Useful for thirdperson cameras for example. 
	/// TrackIR will change rotation and translation relative to this offset instead of the parent actor.
	/// </summary>
	UPROPERTY( EditAnywhere, BlueprintReadWrite, AdvancedDisplay, SaveGame, Category = "TrackIR Settings" )
		FVector Offset{ FVector(0.0f) };

	/// <summary>
	/// Blueprint function to expose TrackIR data. This will not poll for data or change the target camera.
	/// This only reads the latest data we have.
	/// </summary>
	/// <returns>Latest transform that we got from TrackIR.</returns>
	UFUNCTION( BlueprintCallable, Category = "TrackIR Functions" )
		FTransform GetTrackIRData();

	/// <summary>
	/// Blueprint function to poll for TrackIR data. This will update the transform of the selected camera.
	/// </summary>
	UFUNCTION( BlueprintCallable, Category = "TrackIR Functions" )
		void UpdateCameraPose();

public:
	/// <summary>
	/// Retrieve list of all camera components by name in parent actor.
	/// </summary>
	TArray<FString> GetCameraComponentNames() const;

	//function only available when compiled with editor
#if WITH_EDITOR
	virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;
#endif

	/// <summary>
	/// Gives us a camera component by its name. Used for changing the selected camera by the name we selected in the dropdown menu
	/// </summary>
	UCameraComponent* FindComponentByName( const FString& ComponentName ) const;

	// Called when the game starts
	virtual void BeginPlay() override;
	

	// keeps track of time between updates
	float mTimeTillNextUpdate = 0.0f;

public:
	// Called every frame
	virtual void TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;

private:
	bool mUsedPawnRotation{ false };
	UCameraComponent* mPreviousCamera = nullptr;

	/// <summary>
	/// Change back pawn control on selected camera if changed in the first place
	/// </summary>
	void revertUsePawnConstrolRotation();

	/// <summary>
	/// Turn pawn control off for selected camera if turned on
	/// </summary>
	void changeUsePawnConstrolRotation();

};
