#pragma once

/**
@brief Low level c style interface for communicating with the EtherDream dac.

The EtherDream dac is an ILDA compatible laser controller.
For more information on the Ether Dream visit: http://ether-dream.com/

Use this library as a c++ interface to the by ether-dream supplied dll: EtherDream.dll
NOTE THAT THIS INTERFACE ONLY WORKS FOR WINDOWS
**/

//////////////////////////////////////////////////////////////////////////

// standard includes
#include <stdint.h>
#include <string>

// Struct for sending points
struct EAD_Pnt_s 
{
	int16_t X;					//< X Coordinate
	int16_t Y;					//< Y Coordinate
	int16_t R;					//< Red Color
	int16_t G;					//< Green Color
	int16_t B;					//< Blue Color
	int16_t I;					//< Alpha
	int16_t AL;					//< No clue
	int16_t AR;					//< No clue
};

// Utilities
extern const int16_t gEtherMaxValue;
extern const int16_t gEtherMinValue;

extern const float gEtherMinValueFloat;
extern const float gEtherMaxValueFloat;



//////////////////////////////////////////////////////////////////////////
// Ether dream functionality
//////////////////////////////////////////////////////////////////////////

namespace netherdream
{
	// Ether dream status
	enum class Status : int
	{
		Ready = 1,
		Busy  = 2
	};

	// DLL Load Functions
	bool			LoadEtherDreamLib();			//< Tries to load the ether dream dll and map it's functionality
	bool			IsLoaded();						//< Returns if the lib is currently loaded

	// Ether Dream Lib
	int				GetNumberOfDevices();			//< Returns the number of found ether dream devices
	std::string		GetDeviceName(int inDacNum);	//< Returns the ether dream name (based on the 0 indexed card number)
	Status			GetStatus(int inDacNum);		//< Returns the ether dream status
	bool 			OpenDevice(int inDacNum);		//< Opens the ether dream device
	bool			CloseDevice(int inDacNum);		//< Closes dac connection
	bool			Stop(int inDacNum);				//< Stop communication
	bool			Close();						//< Shuts down ether dream signalling thread
	bool			WriteFrame(int inNumber, 
						const struct EAD_Pnt_s* inData, 
						int inBytes, 
						uint16_t 
						inPPS, 
						uint16_t inReps);			//< Write a frame to the ether dream
}