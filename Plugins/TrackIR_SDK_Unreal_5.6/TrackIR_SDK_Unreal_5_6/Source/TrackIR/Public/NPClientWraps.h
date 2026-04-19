// *******************************************************************************
// * Copyright 2023, NaturalPoint Inc.
// *******************************************************************************
// *
// * Description:
// *   Header file for NPClientWraps.cpp module. Contains definitions for functions
// *   and function pointers for exported DLL functions.
// *
// *******************************************************************************


#pragma once

#include "..\public\NPClient.h"
#include "CoreMinimal.h"
#include "HAL/PlatformProcess.h"

#define NS_NATURALPOINT TrackIR
namespace NS_NATURALPOINT {

// global Handle to dynamically loaded TrackIR dll
void* g_pTrackIR_DLL_Handle = nullptr;

/////////////
// Defines ///////////////////////////////////////////////////////////////////////
/////////////
//

/// <summary>
/// Typedef for pointer to the notify callback function that is implemented within
/// the client -- this function receives head tracker reports from the game client API
/// </summary>
typedef NPRESULT(__stdcall* PF_NOTIFYCALLBACK)(unsigned short, unsigned short);

// Typedefs for game client API functions (useful for declaring pointers to these
// functions within the client for use during GetProcAddress() ops)
typedef NPRESULT(__stdcall* PF_NP_REGISTERWINDOWHANDLE)(void*);
typedef NPRESULT(__stdcall* PF_NP_UNREGISTERWINDOWHANDLE)(void);
typedef NPRESULT(__stdcall* PF_NP_REGISTERPROGRAMPROFILEID)(unsigned short);
typedef NPRESULT(__stdcall* PF_NP_QUERYVERSION)(unsigned short*);
typedef NPRESULT(__stdcall* PF_NP_REQUESTDATA)(unsigned short);
typedef NPRESULT(__stdcall* PF_NP_GETSIGNATURE)(LPTRACKIRSIGNATURE);
typedef NPRESULT(__stdcall* PF_NP_GETDATA)(LPTRACKIRDATA);
typedef NPRESULT( __stdcall* PF_NP_GETDATAEX )( LPTRACKIRDATA, long, long );
typedef NPRESULT(__stdcall* PF_NP_REGISTERNOTIFY)(PF_NOTIFYCALLBACK);
typedef NPRESULT(__stdcall* PF_NP_UNREGISTERNOTIFY)(void);
typedef NPRESULT(__stdcall* PF_NP_STARTCURSOR)(void);
typedef NPRESULT(__stdcall* PF_NP_STOPCURSOR)(void);
typedef NPRESULT(__stdcall* PF_NP_RECENTER)(void);
typedef NPRESULT(__stdcall* PF_NP_STARTDATATRANSMISSION)(void);
typedef NPRESULT(__stdcall* PF_NP_STOPDATATRANSMISSION)(void);

//// Function Prototypes ///////////////////////////////////////////////
//
// Functions exported from game client API DLL ( note __stdcall calling convention
// is used for ease of interface to clients of differing implementations including
// C, C++, Pascal (Delphi) and VB. )
//
extern PF_NP_REGISTERWINDOWHANDLE       gpfNP_RegisterWindowHandle;
extern PF_NP_UNREGISTERWINDOWHANDLE     gpfNP_UnregisterWindowHandle;
extern PF_NP_REGISTERPROGRAMPROFILEID   gpfNP_RegisterProgramProfileID;
extern PF_NP_QUERYVERSION               gpfNP_QueryVersion;
extern PF_NP_REQUESTDATA                gpfNP_RequestData;
extern PF_NP_GETSIGNATURE               gpfNP_GetSignature;
extern PF_NP_GETDATA                    gpfNP_GetData;
extern PF_NP_GETDATAEX					gpfNP_GetDataEX;
extern PF_NP_STARTCURSOR                gpfNP_StartCursor;
extern PF_NP_STOPCURSOR                 gpfNP_StopCursor;
extern PF_NP_RECENTER	                 gpfNP_ReCenter;
extern PF_NP_STARTDATATRANSMISSION      gpfNP_StartDataTransmission;
extern PF_NP_STOPDATATRANSMISSION       gpfNP_StopDataTransmission;


void NP_SetupClientAPIFunctionWrappers();

NPRESULT NP_RegisterWindowHandle(void*);
NPRESULT NP_UnregisterWindowHandle(void);
NPRESULT NP_RegisterProgramProfileID(unsigned short wPPID);
NPRESULT NP_QueryVersion(unsigned short* pwVersion);
NPRESULT NP_RequestData(unsigned short wDataReq);
NPRESULT NP_GetSignature(LPTRACKIRSIGNATURE pSignature);
NPRESULT NP_GetData(LPTRACKIRDATA pTID);
NPRESULT NP_GetDataEX(LPTRACKIRDATA pTID, long AppKeyHigh, long AppKeyLow);
NPRESULT NP_RegisterNotify(PF_NOTIFYCALLBACK pfNotify);
NPRESULT NP_UnregisterNotify(void);
NPRESULT NP_StartCursor(void);
NPRESULT NP_StopCursor(void);
NPRESULT NP_ReCenter(void);
NPRESULT NP_StartDataTransmission(void);
NPRESULT NP_StopDataTransmission(void);

} // end of NaturalPoint namespace