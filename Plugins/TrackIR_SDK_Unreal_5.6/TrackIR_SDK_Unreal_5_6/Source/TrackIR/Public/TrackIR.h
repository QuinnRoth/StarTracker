// *******************************************************************************
// * Copyright 2023, NaturalPoint Inc.
// *******************************************************************************
// *
// * Description:
// *   Header file for TrackIR.h module. Contains function definitions for connecting,
// *   polling, and disconnection from TrackIR ready for Unreal Engine code.
// *
// *******************************************************************************

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

#include "Camera/CameraComponent.h"
#include "NPClientWraps.h"

class USkeletalMeshComponent;

#define NS_NATURALPOINT TrackIR
namespace NS_NATURALPOINT {

class TRACKIR_API FTrackIR_Module : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

static bool mShowDebugMessages{ true };

bool GetShowDebugMessages();

void SetShowDebugMessages( bool showDebugMessages );

/// <summary>
/// If not TrackIR not ready, initialize TrackIR and load client dll
/// returns if TrackIR is ready or not
/// </summary>
TRACKIR_API bool TrackIR_Ready();

/// <summary>
/// Free TrackIR Client dll
/// </summary>
TRACKIR_API void Unload_TrackIR();

/// <summary>
/// Update the characters camera Additive Offset
/// </summary>
bool TrackIRDataUpdate( bool showDebugmessages = false);

/// <summary>
/// Read current TrackIR data
/// </summary>
FTransform GetTrackIRData();

/// <summary>
/// return the last TrackIR error
/// </summary>
TRACKIR_API NPRESULT LastError();

/// <summary>
/// Set developers product ID
/// </summary>
TRACKIR_API void SetProductID(unsigned short prodID);



} // end of TrackIR namespace
