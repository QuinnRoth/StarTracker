// *******************************************************************************
// * Copyright 2023, NaturalPoint Inc.
// *******************************************************************************
// *
// * Description:
// *   Function implementations for TrackIR Component functions defined in TrackIR_component.h.
// *
// *   Once added to an Actor, it will automatically update tracking data at a given rate.
// *   Be sure to either let this component turn off auto pawn control for the camera 
// *   (which will be turned back on after this component switches cameras), or turn it off
// *   manually. 
// *
// *******************************************************************************

#include "TrackIR_Component.h"
#include "TrackIR.h"

#include "GameFramework/Actor.h"

UTrackIR::UTrackIR()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features	SetMobility( EComponentMobility::Movable );
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

}


// Called when the game starts
void UTrackIR::BeginPlay()
{
	Super::BeginPlay();
	// default to true, but an example of where you would set this
	TrackIR::SetShowDebugMessages( ShowDebugMesages );
	TrackIR::SetProductID( DeveloperID );

}

/// <summary>
/// Retrieves a list of all the camera's of the attached actor
///  for the camera select drop down menu.
/// </summary>
TArray<FString> UTrackIR::GetCameraComponentNames() const
{
	TArray<FString> ComponentNames;
	if (AActor* Owner = GetOwner())
	{
		// Get all the components attached to the owner
		TArray<UCameraComponent*> OwnerComponents;
		Owner->GetComponents<UCameraComponent>( OwnerComponents );

		// Iterate through the components and extract their names
		for (UCameraComponent* Component : OwnerComponents)
		{
			ComponentNames.Add( Component->GetName() );
		}
	}

	return ComponentNames;
}

/// <summary>
/// callback function for selecting dropdown menu to select which component for TrackIR to interact with
///</summary>
TArray<FString> UTrackIR::GetNameOptions() const
{
	return GetCameraComponentNames();
}

// function only available when compiled with editor
#if WITH_EDITOR
/// <summary>
/// Called when selection changed on dropdown menu.
/// Update the selected camera for TrackIR to interact with based on the selection in the dropdown menu
/// </summary>
void UTrackIR::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
	Super::PostEditChangeProperty( PropertyChangedEvent );

	FName PropertyName = ( PropertyChangedEvent.Property != nullptr ) ? PropertyChangedEvent.Property->GetFName() : NAME_None;

	if (PropertyName == GET_MEMBER_NAME_CHECKED( UTrackIR, DeveloperID ))
	{
		TrackIR::SetProductID( DeveloperID );
	}
	// if the changed property is the CameraName dropdown menu then update the selected camera
	if (PropertyName == GET_MEMBER_NAME_CHECKED( UTrackIR, CameraNames ))
	{
		void revertUsePawnConstrolRotation();
		SelectedCamera = FindComponentByName( CameraNames.ToString() );
		void changeUsePawnConstrolRotation();
	}

	
}
#endif

/// <summary>
/// Finds camera component based on given string
/// </summary>
UCameraComponent* UTrackIR::FindComponentByName( const FString& ComponentName ) const
{
	
	AActor* OwnerActor = GetOwner();
	if (!OwnerActor)
	{
		return nullptr;
	}

	// get all the components attached to the owner of this component
	TSet<UActorComponent*> ComponentsSet = OwnerActor->GetComponents();

	// Of all the camera component, if we find a name match, return that camera
	for (UActorComponent* Component : ComponentsSet)
	{
		UCameraComponent* CameraComponent = Cast<UCameraComponent>( Component );
		if (CameraComponent && CameraComponent->GetName().Equals( ComponentName, ESearchCase::IgnoreCase ))
		{

			return CameraComponent;
		}
	}

	// otherwise return null
	return nullptr;
}


/// <summary>
/// Wrapper function to expose data coming from TrackIR. This will not update the data before 
/// returning it. It will only return the latest data we have.
/// <summary>
FTransform UTrackIR::GetTrackIRData()
{
	return TrackIR::GetTrackIRData();
}

///<summary>
/// Update TrackIR module with latest TrackIR update 
/// and update the camera's transform
///</summary>
void UTrackIR::UpdateCameraPose()
{

	if (TrackIR::TrackIRDataUpdate( ShowDebugMesages ))
	{
		FTransform newTrans = GetTrackIRData();
		SelectedCamera->SetRelativeLocationAndRotation( newTrans.GetLocation() + Offset, newTrans.GetRotation() );
	}
};

/// <summary>
/// Called every frame. Updates TrackIR tracking data every time a frame is available.
/// </summary>
void UTrackIR::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (SelectedCamera != mPreviousCamera)
	{
		CameraNames = (FName) SelectedCamera->GetName();
	}

	mPreviousCamera = SelectedCamera;

	// check if TrackIR is ready to go, and user enabled it
	if (TrackIREnabled && TrackIR::TrackIR_Ready())
	{
		
		// only update with respect to the refresh rate property
		mTimeTillNextUpdate -= DeltaTime;
		if (mTimeTillNextUpdate <= RefreshRate)
		{
			mTimeTillNextUpdate = RefreshRate;

			// if we have selected a valid camera, then update the camera's position and rotation
			if(SelectedCamera)
			{
				UpdateCameraPose();
			}
		}
	}

}



/// <summary>
/// Change camera back to using pawn control for rotation if it was set to true before
/// </summary>
void UTrackIR::revertUsePawnConstrolRotation()
{
	// user can disable this functionality in "advanced" under "TrackIR Settings"
	// or through changing the "AutoChangePawnRotation" property for the TrackIR Component.
	if (!AutoChangePawnRotation)
	{
		return;
	}

	// if we are changing from a camera and changed the usePawnControlRotation
	// change it back
	if (SelectedCamera)
	{
		if (mUsedPawnRotation)
		{
			SelectedCamera->bUsePawnControlRotation = true;
			mUsedPawnRotation = false;
		}
	}
}

/// <summary>
/// if a camera is selected for trackIR, then change the 
/// "Use pawn control for rotation" to false
/// </summary>
void UTrackIR::changeUsePawnConstrolRotation()
{
	// user can disable this functionality in "advanced" under "TrackIR Settings"
	// or through changing the "AutoChangePawnRotation" property for the TrackIR Component.
	if (!AutoChangePawnRotation)
	{
		return;
	}

	// if a camera has usePawnControlRotation Enabled
	// change it to false so TrackIR can change it
	if (SelectedCamera)
	{
		if (SelectedCamera->bUsePawnControlRotation)
		{
			SelectedCamera->bUsePawnControlRotation = false;
			mUsedPawnRotation = true;
		}
	}
}

