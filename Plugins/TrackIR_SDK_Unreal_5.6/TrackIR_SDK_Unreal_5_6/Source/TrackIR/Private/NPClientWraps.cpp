// *******************************************************************************
// * Copyright 2023, NaturalPoint Inc.
// *******************************************************************************
// *
// * Description:
// *   This module implements the wrapper code for interfacing to the NaturalPoint
// *   Game Client API.  Developers of client apps can include this module into
// *   their projects to simplify communication with the NaturalPoint software.
// *
// *   This is necessary since the NPClient DLL is run-time linked rather than
// *   load-time linked, avoiding the need to link a static library into the
// *   client program (only this module is needed, and can be supplied in source
// *   form.)
// *
// *******************************************************************************

#include "..\public\NPClientWraps.h"

#define NS_NATURALPOINT TrackIR
namespace NS_NATURALPOINT {

	PF_NP_REGISTERWINDOWHANDLE       gpfNP_RegisterWindowHandle = nullptr;
	PF_NP_UNREGISTERWINDOWHANDLE     gpfNP_UnregisterWindowHandle = nullptr;
	PF_NP_REGISTERPROGRAMPROFILEID   gpfNP_RegisterProgramProfileID = nullptr;
	PF_NP_QUERYVERSION               gpfNP_QueryVersion = nullptr;
	PF_NP_REQUESTDATA                gpfNP_RequestData = nullptr;
	PF_NP_GETSIGNATURE               gpfNP_GetSignature = nullptr;
	PF_NP_GETDATA                    gpfNP_GetData = nullptr;
	PF_NP_GETDATAEX                  gpfNP_GetDataEX = nullptr;
	PF_NP_STARTCURSOR                gpfNP_StartCursor = nullptr;
	PF_NP_STOPCURSOR                 gpfNP_StopCursor = nullptr;
	PF_NP_RECENTER	                 gpfNP_ReCenter = nullptr;
	PF_NP_STARTDATATRANSMISSION      gpfNP_StartDataTransmission = nullptr;
	PF_NP_STOPDATATRANSMISSION       gpfNP_StopDataTransmission = nullptr;

	/// <summary>
	/// Initialize function pointers to exported DLL functions from NPClient.dll
	/// </summary>
	void NP_SetupClientAPIFunctionWrappers()
	{
		// Get addresses of all exported functions
		gpfNP_GetSignature = (PF_NP_GETSIGNATURE)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_GetSignature"));
		gpfNP_RegisterWindowHandle = (PF_NP_REGISTERWINDOWHANDLE)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_RegisterWindowHandle"));
		gpfNP_UnregisterWindowHandle = (PF_NP_UNREGISTERWINDOWHANDLE)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_UnregisterWindowHandle"));
		gpfNP_RegisterProgramProfileID = (PF_NP_REGISTERPROGRAMPROFILEID)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_RegisterProgramProfileID"));
		gpfNP_QueryVersion = (PF_NP_QUERYVERSION)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_QueryVersion"));
		gpfNP_RequestData = (PF_NP_REQUESTDATA)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_RequestData"));
		gpfNP_GetData = (PF_NP_GETDATA)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_GetData"));
		gpfNP_GetDataEX = (PF_NP_GETDATAEX)(void*)FPlatformProcess::GetDllExport( g_pTrackIR_DLL_Handle, TEXT( "NP_GetDataEX" ) );
		gpfNP_StartCursor = (PF_NP_STARTCURSOR)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_StartCursor"));
		gpfNP_StopCursor = (PF_NP_STOPCURSOR)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_StopCursor"));
		gpfNP_ReCenter = (PF_NP_RECENTER)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_ReCenter"));
		gpfNP_StartDataTransmission = (PF_NP_STARTDATATRANSMISSION)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_StartDataTransmission"));
		gpfNP_StopDataTransmission = (PF_NP_STOPDATATRANSMISSION)(void*)FPlatformProcess::GetDllExport(g_pTrackIR_DLL_Handle, TEXT("NP_StopDataTransmission"));
	}

	////////////////////////////////////////////////////
	// NaturalPoint Game Client API function wrappers /////////////////////////////
	////////////////////////////////////////////////////
	//

	/// <summary>
	/// Register the application window for liveness checks. This allows the TrackIR software to
	/// detect situations where e.g. your application crashes and fails to shut down cleanly.
	/// </summary>
	/// <remarks>
	/// TrackIR will only communicate with one application at a time.
	/// You must unregister you're window handle after you are done using TrackIR.
	/// </remarks>
	/// <param name="hWnd">Window Handle</param>
	/// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the window handle was successfully registered.
	/// </returns>
	NPRESULT NP_RegisterWindowHandle(void* hWnd)
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_RegisterWindowHandle)
			result = (*gpfNP_RegisterWindowHandle)(hWnd);

		return result;
	} // NP_RegisterWindowHandle()

	/// <summary>
	/// Unregisters a given window handle
	/// </summary>
	/// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the window handle was successfully unregistered.
	/// </returns>
	NPRESULT NP_UnregisterWindowHandle()
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr != gpfNP_UnregisterWindowHandle)
			result = (*gpfNP_UnregisterWindowHandle)();

		return result;
	} // NP_UnregisterWindowHandle()

	/// <summary>
	/// Registers a Profile ID to TrackIR so that the application can use it's assigned profile on TrackIR.
	/// </summary>
	/// <remarks>
	/// Every game/application has its own unique Profile ID so that TrackIR can use the tracking profile
	/// when your game is loaded up for the user.
	/// </remarks>
	/// <param name="wPPID">Profile ID</param>
	/// /// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the Profile ID was successfully registered.
	/// </returns>
	NPRESULT NP_RegisterProgramProfileID(unsigned short wPPID)
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_RegisterProgramProfileID)
			result = (*gpfNP_RegisterProgramProfileID)(wPPID);

		return result;
	} // NP_RegisterProgramProfileID()

	/// <summary>
	/// Gets the current TrackIR version.
	/// </summary>
	/// <param name="pwVersion">Reference to an unsigned short to store the version</param>
	/// /// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the version was successfully queried.
	/// </returns>
	NPRESULT NP_QueryVersion(unsigned short* pwVersion)
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_QueryVersion)
			result = (*gpfNP_QueryVersion)(pwVersion);

		return result;
	} // NP_QueryVersion()

	/// <summary>
	/// Tell TrackIR what kinds of data we are using. 
	/// </summary>
	/// <remarks>
	/// Historically this function was necessary to signal TrackIR to send different types of data. However, 
	/// This is not necessary but good practice and for bookkeeping on TrackIR. This function won't change anything TrackIR sends.
	/// TrackIR will send Roll, Pitch, Yaw, X, Y, Z regardless.
	/// </remarks>
	/// <param name="wDataReq">unsigned short whose bytes tell us what kind of data want</param>
	/// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the data was requested successfully.
	/// </returns>
	NPRESULT NP_RequestData(unsigned short wDataReq)
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_RequestData)
			result = (*gpfNP_RequestData)(wDataReq);

		return result;
	} // NP_RequestData()

	/// <summary>
	/// Gets TrackIR's signature from TrackIR.
	/// </summary>
	/// <remarks>
	/// Necessary step to verify that we are receiving data from TrackIR and that the DLL was loaded properly. 
	/// </remarks>
	/// <param name="pSignature">Reference to a TRACKIRSIGNATURE to store the signature given from TrackIR and the DLL</param>
	/// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the version was successfully queried.
	/// </returns>
	NPRESULT NP_GetSignature(LPTRACKIRSIGNATURE pSignature)
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_GetSignature)
			result = (*gpfNP_GetSignature)(pSignature);

		return result;
	} // NP_GetSignature()


	/// <summary>
	/// Gets tracking information sent from TrackIR.
	/// </summary>
	/// <param name="pTID">Reference to a TRACKIRDATA to store the tracking information given from TrackIR</param>
	/// <returns>	
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the tracking data was successfully gathered
	/// </returns>
	NPRESULT NP_GetData(LPTRACKIRDATA pTID)
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_GetData)
			result = (*gpfNP_GetData)(pTID);

		return result;
	} // NP_GetData()


	/// <summary>
	/// Gets tracking information sent from TrackIR. This function is only used if you were provided an app key from NaturalPoint.
	/// </summary>
	/// <remarks>
	/// Only a few games were given an app key for sending encrypted data over. It is very likely that you will not need to use this.
	/// </remarks>
	/// <param name="pTID">Reference to a TRACKIRDATA to store the tracking information given from TrackIR</param>
	/// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the tracking data was successfully gathered and decrypted.
	/// </returns>
	NPRESULT NP_GetDataEX( LPTRACKIRDATA pTID, long AppKeyHigh, long AppKeyLow )
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr != gpfNP_GetDataEX)
			result = ( *gpfNP_GetDataEX )( pTID, AppKeyHigh, AppKeyLow );

		return result;
	}


	/// <summary>
	/// Reports to TrackIR we have started tracking.
	/// </summary>
	/// <remarks>
	/// This function is only for book keeping purposes. This does not do anything on its own. 
	/// </remarks>
	/// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the tracking cursor was started.
	/// </returns>
	NPRESULT NP_StartCursor()
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_StartCursor)
			result = (*gpfNP_StartCursor)();

		return result;
	} // NP_StartCursor()

	/// <summary>
	/// Reports to TrackIR we have stops tracking.
	/// </summary>
	/// <remarks>
	/// This function is only for book keeping purposes. This does not do anything on its own. 
	/// </remarks>
	/// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the tracking cursor was stopped.
	/// </returns>
	NPRESULT NP_StopCursor()
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_StopCursor)
			result = (*gpfNP_StopCursor)();

		return result;
	} // NP_StopCursor()

	/// <summary>
	/// Recenters tracking data.
	/// </summary>
	/// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if the tracking data was successfully recentered.
	/// </returns>
	NPRESULT NP_ReCenter()
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_ReCenter)
			result = (*gpfNP_ReCenter)();

		return result;
	} // NP_ReCenter()

	/// <summary>
	/// Signals TrackIR to start sending data to the application.
	/// </summary>
	/// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if TrackIR started sending data to the window handle.
	/// </returns>
	NPRESULT NP_StartDataTransmission()
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_StartDataTransmission)
			result = (*gpfNP_StartDataTransmission)();

		return result;
	} // NP_StartDataTransmission()

	/// <summary>
	/// Tells TrackIR to stop sending data to the window handle.
	/// </summary>
	/// <returns>
	/// If function was not loaded from DLL, it returns NP_ERR_DLL_NOT_FOUND.
	/// Otherwise, returns NP_OK if TrackIR started sending data to the window handle.
	/// </returns>
	NPRESULT NP_StopDataTransmission()
	{
		NPRESULT result = tagNPResult::NP_ERR_DLL_NOT_FOUND;

		if (nullptr  != gpfNP_StopDataTransmission)
			result = (*gpfNP_StopDataTransmission)();

		return result;
	} // NP_StopDataTransmission()

} // end of NaturalPoint namespace
//////////////////////////////////////////////////////////////////////////////