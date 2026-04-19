// *******************************************************************************
// * Copyright 2023, NaturalPoint Inc.
// *******************************************************************************
// *
// * Description:
// *    Wrapper functions that connect Unreal Engine games to TrackIR automatically.
// *    This takes care of connecting, polling data, and disconnecting TrackIR.
// *
// *******************************************************************************

#include "TrackIR.h"
#include "NPClient.h"
#include "NPClientWraps.h"

// #include <Winreg.h>
#include "Core.h"
#include "Modules/ModuleManager.h"
#include "Interfaces/IPluginManager.h"
#include "Engine.h"

#if PLATFORM_WINDOWS
#include "Windows/AllowWindowsPlatformTypes.h"
#include <winreg.h>
#include "Windows/HideWindowsPlatformTypes.h"
#endif

#define LOCTEXT_NAMESPACE "FNP_TrackIR_Module"

#define NS_NATURALPOINT TrackIR
namespace NS_NATURALPOINT {

	// Natural Point Developer ID
	//	Developer ID 1000 is for test purposes only
	//	contact NaturalPoint for your own Developer ID

	unsigned short ProductID = 1000;

	TRACKIR_API void SetProductID( unsigned short prodID )
	{
		ProductID = prodID;
	}


	// last trackir dll error
	NPRESULT lastError = tagNPResult::NP_OK;
	TRACKIR_API NPRESULT LastError()
	{ 
		return lastError; 
	}


	bool GetShowDebugMessages()
	{
		return mShowDebugMessages;
	}

	void SetShowDebugMessages(bool showDebugMessages)
	{
		mShowDebugMessages = showDebugMessages;
	}

	/// <summary>
	/// Query the NaturalPoint software version. 
	/// The major version comes in on the high byte. 
	/// The minor version comes in on the low byte.
	/// </summary>
	FString NP_GetVersion()
	{
		unsigned short wNPClientVer;
		if (tagNPResult::NP_OK == NP_QueryVersion(&wNPClientVer))
		{
			return FString::Printf(TEXT("TrackIR software version is %d.%02d"), (wNPClientVer >> 8), (wNPClientVer & 0x00FF));
		}
		return FString(TEXT("Error querying TrackIR software version!!"));
	}

	LONG GetStringRegKey(const HKEY Key, const FString& ValueName, FString& Value)
	{
		WCHAR Buffer[512];
		DWORD BufferSize = sizeof(Buffer);
		const LONG Result = RegQueryValueExW(Key, *ValueName, nullptr, nullptr, reinterpret_cast<LPBYTE>(Buffer), &BufferSize);
		if (Result == ERROR_SUCCESS)
		{
			Value = Buffer;
		}
		return Result;
	}

	FString Get_TrackIR_Dll_Location()
	{
		//  Add on the relative location of the third party dll and load it
		FString dllpath = TEXT("Failed");

		//find path to NPClient.dll
		HKEY subkey;
		if (::RegOpenKeyEx(HKEY_CURRENT_USER, TEXT("Software\\NaturalPoint\\NATURALPOINT\\NPClient Location"),
			0,
			KEY_READ,
			&subkey) != ERROR_SUCCESS)
		{
			if (mShowDebugMessages && GEngine)
			{
				UE_LOG(LogTemp, Warning, TEXT("TrackIR not found."));
			}
		}
		else
		{

			FString keyValue( TEXT( "Path" ) );
			if (GetStringRegKey( subkey, keyValue, dllpath ) == ERROR_SUCCESS)
			{
				dllpath = FPaths::Combine( dllpath, TEXT( "NPClient64.dll" ) );
				if (mShowDebugMessages)
				{
					UE_LOG( LogTemp, Warning, TEXT( "TrackIR  %s" ), *dllpath );
				}
			}
			::RegCloseKey( subkey );

		}
		return dllpath;
	}


	/// <summary>
	/// Finds NPClient.dll, loads exported SDK functions, and initializes connections to TrackIR	
	/// </summary>
	/// <param name="productID">The developer ID provided by NaturalPoint.</param>
	/// <returns>NP_OK on success, otherwise, returns NP_ERR_DEVICE_NOT_FOUND.</returns>
	NPRESULT LoadTrackIRDll( unsigned short productID )
	{
		NPRESULT result = tagNPResult::NP_OK;

		if (g_pTrackIR_DLL_Handle)
		{
			return result;
		}

		// Display TrackIR 
		if (productID == 1000)
		{
			GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Red, *FString::Printf(TEXT("TrackIR Temp Developer ID 1000:")));
		}

		// Get the location of the TrackIR DLL 
		FString LibraryPath = Get_TrackIR_Dll_Location();

		// Make sure it exists
		bool bTrackIRDllExists = FPaths::FileExists(LibraryPath);
		if (!bTrackIRDllExists)
		{
			if (mShowDebugMessages)
			{
				UE_LOG( LogTemp, Warning, TEXT( "Failed to load TrackIR" ) );
			}
			return tagNPResult::NP_ERR_DLL_NOT_FOUND;
		}

		// get TrackIR DLL handle
		g_pTrackIR_DLL_Handle = !LibraryPath.IsEmpty() ? FPlatformProcess::GetDllHandle(*LibraryPath) : nullptr;
		if (!g_pTrackIR_DLL_Handle)
		{
			if (mShowDebugMessages)
			{
				UE_LOG( LogTemp, Warning, TEXT( "Failed to load TrackIR" ) );
			}
			return tagNPResult::NP_ERR_DEVICE_NOT_PRESENT;
		}
		UE_LOG(LogTemp, Warning, TEXT("%s"), *(NP_GetVersion()));

		NP_SetupClientAPIFunctionWrappers();

		// Register your applications Window Handle 
		result = NP_RegisterWindowHandle(GEngine->GameViewport->GetWindow()->GetNativeWindow()->GetOSWindowHandle());
		if (tagNPResult::NP_OK == result)
		{
			if (mShowDebugMessages)
			{
				UE_LOG( LogTemp, Warning, TEXT( "Window handle registration successful" ) );
			}
		}
		else
		{
			if (mShowDebugMessages)
			{
				UE_LOG( LogTemp, Warning, TEXT( "Error registering window handle!!" ) );
			}
			return tagNPResult::NP_ERR_INTERNAL_DATA;
		}


		// Select what kinds of data we want from TrackIR.
		// not necessary, but useful for bookkeeping for TrackIR.
		unsigned int DataFields = 0;
		DataFields |= NPPitch;
		DataFields |= NPYaw;
		DataFields |= NPRoll;
		DataFields |= NPX;
		DataFields |= NPY;
		DataFields |= NPZ;

		result = NP_RequestData(DataFields);
		if (tagNPResult::NP_OK == result)
		{
			if (mShowDebugMessages)
			{
				UE_LOG( LogTemp, Warning, TEXT( "data request set successful" ) );
			}
		}
		else
		{
			if (mShowDebugMessages)
			{
				UE_LOG( LogTemp, Warning, TEXT( "Error data request set failed!!" ) );
			}
			return tagNPResult::NP_ERR_NO_DATA;
		}

		// Register developer ID provided by NaturalPoint
		result = NP_RegisterProgramProfileID(productID);
		if (tagNPResult::NP_OK == result)
		{
			if (mShowDebugMessages)
			{
				UE_LOG( LogTemp, Warning, TEXT( "Register profile ID success" ) );
			}
		}
		else
		{
			if (mShowDebugMessages)
			{
				UE_LOG( LogTemp, Warning, TEXT( "Error register profile ID failed!!" ) );
			}
			return tagNPResult::NP_ERR_INVALID_ARG;
		}

		// Signal to TrackIR that we are ready to start recieving data.
		if (NP_StartDataTransmission() == tagNPResult::NP_OK)
		{
			if (mShowDebugMessages)
			{
				UE_LOG( LogTemp, Warning, TEXT( "Data transmission started" ) );
			}
		}
		else
		{
			if (mShowDebugMessages)
			{
				UE_LOG( LogTemp, Warning, TEXT( "Data transmission start Failed" ) );
			}
			return tagNPResult::NP_ERR_INTERNAL_DATA;
		}

		return result;
	}


	// is TrackIR dll loaded and initialized?
	bool bTrackIRLoaded = false;

	/// <summary> 
	/// Checks if TrackIR is ready and if not initialize TrackIR for use
	/// </summary>
	/// <returns>
	/// Returns true if NPClient.dll was successfully loaded, otherwise returns NP_ERR_DLL_NOT_FOUND.
	/// </returns>
	bool TrackIR_Ready()
	{
		if (bTrackIRLoaded)
		{
			return true;
		}

		lastError = LoadTrackIRDll(ProductID);
		if (lastError == tagNPResult::NP_OK )
		{
			// verify signature to see if it is really TrackIR that we are connecting to.
			SIGNATUREDATA pSignature;
			SIGNATUREDATA verifySignature;
			
			strncpy_s(verifySignature.DllSignature, "precise head tracking\n put your head into the game\n now go look around\n\n Copyright EyeControl Technologies", sizeof(verifySignature.DllSignature));
			strncpy_s(verifySignature.AppSignature, "hardware camera\n software processing data\n track user movement\n\n Copyright EyeControl Technologies", sizeof(verifySignature.AppSignature));
			// query the dll and compare the results
			lastError = NP_GetSignature(&pSignature);
			if (lastError == tagNPResult::NP_OK)
			{
				if ((strcmp(verifySignature.DllSignature, pSignature.DllSignature) == 0)
					&& (strcmp(verifySignature.AppSignature, pSignature.AppSignature) == 0))
				{
					lastError = tagNPResult::NP_OK;
					// TrackIR successfully loaded
					bTrackIRLoaded = true;
				}
				else
				{
					lastError = tagNPResult::NP_ERR_DLL_NOT_FOUND;
				}
			}
			else
			{
				lastError = tagNPResult::NP_ERR_DLL_NOT_FOUND;
			}
		}

		return lastError == tagNPResult::NP_OK;
	} // Init_TrackIR()

	/// <summary>
	/// This function may be called during shutdown to clean up your module.  
	/// For modules that support dynamic reloading,
	/// we call this function before unloading the module.
	/// </summary>
	void Unload_TrackIR()
	{
		bTrackIRLoaded = false;

		// Free the dll handle
		if (g_pTrackIR_DLL_Handle)
		{
			// TrackIR shutdown process to safely stop and unregister from TrackIR.
			NP_StopDataTransmission();
			NP_UnregisterWindowHandle();
			FPlatformProcess::FreeDllHandle(g_pTrackIR_DLL_Handle);
		}
		g_pTrackIR_DLL_Handle = nullptr;
	}

	/// <summary>
	/// keep track of frame signatures so that 
	/// we know whether or not we got fresh data from TrackIR
	/// </summary>
	int mLatestFrameSignature;
	FTransform mLatestTransform;
	
	/// <summary>
	/// Exposes latest data received from TrackIR. Does not update TrackIR data on its own.
	/// </summary>
	FTransform GetTrackIRData()
	{
		return mLatestTransform;
	}
	
	/// <summary>
	/// Polls TrackIR for data when available.
	/// </summary>
	/// <param name="showDebugMessages">flag for outputting debug messages in game.</param>
	/// <returns>true if data successfully updated from TrackIR, otherwise, false.</returns>
	bool TrackIRDataUpdate( bool showDebugMessages )
	{

		TRACKIRDATA tid;
		NPRESULT result = NP_GetData(&tid);

		
		if (tagNPResult::NP_OK != result)
		{
			if (showDebugMessages)
			{
				GEngine->AddOnScreenDebugMessage( 2, 5.f, FColor::Red, *FString::Printf( TEXT( "TrackIR data Failure" ) ) );
			}
			return false;
		}

		// if data has not updated, don't do anything
		if (mLatestFrameSignature == tid.FrameSignature){
			if (showDebugMessages){
				GEngine->AddOnScreenDebugMessage( 2, 5.f, FColor::Red, *FString::Printf( TEXT( "No new data")));
			}
			return false;
		}


		mLatestFrameSignature = tid.FrameSignature;

		// We got fresh data, so update the latest cached/transformed pose.
		const float kEncodedRangeMinMax = 16383.0f;
		const float kDecodedTranslationMinMaxMeters = 0.5f; // +/- 50 cm
		const float kDecodedRotationMinMaxRadians = (float)PI; // +/- 180 deg

		// Negate right-hand rule rotations to be consistent with left-handed coordinate basis.
		// See remarks in the comments for the TrackIRData struct for more information.
		float rollRad = ( -tid.Roll / kEncodedRangeMinMax ) * kDecodedRotationMinMaxRadians;
		float pitchRad = ( -tid.Pitch / kEncodedRangeMinMax ) * kDecodedRotationMinMaxRadians;
		float yawRad = ( -tid.Yaw / kEncodedRangeMinMax ) * kDecodedRotationMinMaxRadians;
		float xMeters = ( tid.X / kEncodedRangeMinMax ) * kDecodedTranslationMinMaxMeters * 100;
		float yMeters = ( tid.Y / kEncodedRangeMinMax ) * kDecodedTranslationMinMaxMeters * 100;
		float zMeters = ( tid.Z / kEncodedRangeMinMax ) * kDecodedTranslationMinMaxMeters * 100;

		if (showDebugMessages){
			GEngine->AddOnScreenDebugMessage( 2, 5.f, FColor::Red, *FString::Printf( TEXT( "TrackIR Client: Yaw %04.02f, pitch %04.02f, Roll %04.02f, X %04.02f, Y %04.02f, Z %04.02f" ),
				-FMath::RadiansToDegrees(yawRad), -FMath::RadiansToDegrees( pitchRad ), -FMath::RadiansToDegrees(rollRad), xMeters, yMeters, zMeters ) );
		}


		/* Rotation
				
			Pitch 0 = Straight Ahead, +Up, -Down
			Roll  0 = Straight,       +Clockwise, -CCW.
			Yaw   0 = East,           +North, -South.
				
		*/

		FVector deltaPos = FVector( xMeters, yMeters, zMeters );

		FRotator deltaRot = FRotator( FMath::RadiansToDegrees( pitchRad ), FMath::RadiansToDegrees( yawRad), FMath::RadiansToDegrees( rollRad ) );
		FTransform finalTrans = FTransform( deltaRot, deltaPos );

		mLatestTransform = finalTrans;

		return true;
	}


	void FTrackIR_Module::StartupModule()
	{
#if PLATFORM_MAC
		checkf(false, "not IOS compatible");
#elif PLATFORM_LINUX
		checkf(false, "not Linux compatible");
#endif
	}

	void FTrackIR_Module::ShutdownModule()
	{
		Unload_TrackIR();
	}
} // end namespace NaturalPoint

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(TrackIR::FTrackIR_Module, TrackIR)