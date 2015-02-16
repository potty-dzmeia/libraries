#pragma once

#include <windows.h>
#include <dsound.h>
#include <strsafe.h>


class WavFilePlayer
{
private:
	BYTE					*pbData;		//the sound data read from the file
	unsigned				iSizeSoundData; //The size of the sound data	
	
	LPDIRECTSOUND		 lpDS;				//pointer to the directsound object
	LPDIRECTSOUNDBUFFER  lpDsb;				//secondary buffer
	
	WAVEFORMATEX			 wfx;			//The format of the audio data

public:
	WavFilePlayer(void);
public:
	~WavFilePlayer(void);

/*If funcitons return 0 there is some error, 1 is OK*/
private:
	//Clears the memory  
	void ShutDown();


	
public:
	/*Function that loads all the data(into pbData) from the Sound File*/
	bool LoadWavFile(TCHAR *szFileName);

	/*Play the data streaming it*/
	bool Play(HWND);

	/*Stops playing the data*/
	bool Stop();

};
