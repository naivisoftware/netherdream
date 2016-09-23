// Local Includes
#include <netherdream.h>

// std includes
#include <string>
#include <iostream>
#include <algorithm>

// Don't define min / max for windows
#define NOMINMAX

// windows includes
#if defined(_WIN32) || defined(_WIN64)
	#include <windows.h>
#endif // _WIN32 || #ifdef _WIN64


//////////////////////////////////////////////////////////////////////////
// Utilities
//////////////////////////////////////////////////////////////////////////
const int16_t	gEtherMinValue(std::numeric_limits<int16_t>::min());
const int16_t	gEtherMaxValue(std::numeric_limits<int16_t>::max());

const float	gEtherMinValueFloat((float)std::numeric_limits<int16_t>::min());
const float	gEtherMaxValueFloat((float)std::numeric_limits<int16_t>::max());


//////////////////////////////////////////////////////////////////////////
// ether dream Function typefs
//////////////////////////////////////////////////////////////////////////
typedef int  (__stdcall *pEtherDearmGetCardNum)();
typedef void (__stdcall *pEtherDreamGetDeviceName)(const int* CardNum, char* buf, int max);
typedef int  (__stdcall *pEtherDreamGetStatus)(const int* CardNum);
typedef bool (__stdcall *pEtherDreamOpenDevice)(const int* CardNum);
typedef bool (__stdcall *pEtherDreamWriteFrame)(const int *CardNum, const struct EAD_Pnt_s* data, int Bytes, uint16_t PPS, uint16_t Reps);
typedef bool (__stdcall *pEtherDreamStop)(const int *CardNum);
typedef bool (__stdcall *pEtherDreamCloseDevice)(const int *CardNum);
typedef bool (__stdcall *pEtherDreamClose)();



//////////////////////////////////////////////////////////////////////////
// Ether dream callables
//////////////////////////////////////////////////////////////////////////
static pEtherDearmGetCardNum		EtherDreamGetCardNum(nullptr);
static pEtherDreamGetDeviceName		EtherDreamGetDeviceName(nullptr);
static pEtherDreamGetStatus			EtherDreamGetStatus(nullptr);
static pEtherDreamOpenDevice		EtherDreamOpenDevice(nullptr);
static pEtherDreamClose				EtherDreamClose(nullptr);
static pEtherDreamCloseDevice		EtherDreamCloseDevice(nullptr);
static pEtherDreamStop				EtherDreamStop(nullptr);
static pEtherDreamWriteFrame		EtherDreamWriteFrame(nullptr);



//////////////////////////////////////////////////////////////////////////
// Statics
//////////////////////////////////////////////////////////////////////////
static bool					sIsLoaded(false);
static const std::string	sEtherDLL("EtherDream");

#if defined(_WIN32) || defined(_WIN64)
static HINSTANCE			EtherDreamDLL(nullptr);
#endif



//////////////////////////////////////////////////////////////////////////
// Loading and binding the Lib
//////////////////////////////////////////////////////////////////////////

/**
@brief Binds the loaded ether dream lib
**/
static bool BindEtherDreamLib()
{
#ifdef _WIN32
	// Extract function
	EtherDreamGetCardNum = (pEtherDearmGetCardNum)GetProcAddress(EtherDreamDLL, "EtherDreamGetCardNum");
	if(!EtherDreamGetCardNum)
		return false;

	EtherDreamGetDeviceName = (pEtherDreamGetDeviceName)GetProcAddress(EtherDreamDLL, "EtherDreamGetDeviceName");
	if(!EtherDreamGetDeviceName)
		return false;

	EtherDreamGetStatus = (pEtherDreamGetStatus)GetProcAddress(EtherDreamDLL, "EtherDreamGetStatus");
	if(!EtherDreamGetStatus)
		return false;

	EtherDreamOpenDevice = (pEtherDreamOpenDevice)GetProcAddress(EtherDreamDLL, "EtherDreamOpenDevice");
	if(!EtherDreamOpenDevice)
		return false;

	EtherDreamClose = (pEtherDreamClose)GetProcAddress(EtherDreamDLL, "EtherDreamClose");
	if(!EtherDreamClose)
		return false;

	EtherDreamCloseDevice = (pEtherDreamCloseDevice)GetProcAddress(EtherDreamDLL, "EtherDreamCloseDevice");
	if(!EtherDreamCloseDevice)
		return false;

	EtherDreamStop = (pEtherDreamStop)GetProcAddress(EtherDreamDLL, "EtherDreamStop");
	if(!EtherDreamStop)
		return false;

	EtherDreamWriteFrame = (pEtherDreamWriteFrame)GetProcAddress(EtherDreamDLL, "EtherDreamWriteFrame");
	if(!EtherDreamWriteFrame)
		return false;

	return true;
#else
	return false;
#endif

}

/**
@brief Loads the ether dream dll and binds it's functions to the members above

The dll it searches for is: sEtherDLL 'EtherDream.dll'. This dll should be somewhere
in the windows path environment.
**/
bool netherdream::LoadEtherDreamLib()
{
#ifdef _WIN32
	EtherDreamDLL = LoadLibrary(sEtherDLL.c_str());
#endif // _WIN32
	if(EtherDreamDLL == nullptr)
	{
		std::cout << "Unable to load dll: " << sEtherDLL << " ,is it in a PATH related directory?\n";
		sIsLoaded = false;
		return sIsLoaded;
	}

	// Bind functionality
	sIsLoaded = BindEtherDreamLib();
	if(!sIsLoaded)
	{
		std::cout << "Unable to bind dll: " << sEtherDLL << "\n";
		FreeLibrary(EtherDreamDLL);
	}
	return sIsLoaded;
}



/**
@brief Returns if the ether dream lib was loaded successfully
**/
bool netherdream::IsLoaded()
{
	return sIsLoaded;
}



/**
@brief GetNumberOfDacs

Returns the number of available ether dream dac controllers
**/
int netherdream::GetNumberOfDevices()
{
	return EtherDreamGetCardNum();
}



/**
@brief GetDeviceName

Returns the name of the ether dream controller based on the card number given (starts at 0)
**/
std::string netherdream::GetDeviceName(int inDacNum)
{
	char buffer[256];
	EtherDreamGetDeviceName(&inDacNum, buffer, 256);
	return std::string(buffer);
}



/**
@brief Returns the dac status (busy or ready)
**/
netherdream::Status netherdream::GetStatus(int inDacNum)
{
	return (netherdream::Status)EtherDreamGetStatus(&inDacNum);
}



/**
@brief Opens the dac for communcation
**/
bool netherdream::OpenDevice(int inDacNum)
{
	return EtherDreamOpenDevice(&inDacNum);
}



/**
@brief Closes the dac connection
**/
bool netherdream::CloseDevice(int inDacNum)
{
	return EtherDreamCloseDevice(&inDacNum);
}



/**
@brief Stops the output
**/
bool netherdream::Stop(int inDacNum)
{
	return EtherDreamStop(&inDacNum);
}


/**
@brief Shuts down ether dream signalling thread
**/
bool netherdream::Close()
{
	return EtherDreamClose();
}



/**
@brief Writes a frame to the ether dream device

@inNumer: The number of the dac controller to send data to
@inData:  Array of points to send over
@inBytes: Total size of array in bytes (ie: sizeof(EAD_Pnt_s) * size_array))
@inPPs:   Number of points per second to let the laser draw the points
@inReps:  How many times the buffer is drawn (-1 is infinite, 0 is invalid, 1 is default)

A value of -1 for inReps becomes max(uint_16_t)
**/
bool netherdream::WriteFrame(int inNumber, const struct EAD_Pnt_s* inData, int inBytes, uint16_t inPPS, uint16_t inReps)
{
	return EtherDreamWriteFrame(&inNumber, inData, inBytes, inPPS, inReps);
}

