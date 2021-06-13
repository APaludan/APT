#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>

//gcc createSoundFromImage.c
//a.exe -webcam 0/1 -> 0 hvis man ik vil tage billede og 1 hvis man vil (webcam skal nok ændres afhængigt af computer)

int CaptureImage(char *argv[]);
int CompressImage();

//argc = amount(count) of arguments/variables
//argv = variables
int main(int argc, char *argv[])
{
	//Compile additional files
	system("gcc image2binary.c -o image2binary");
	system("gcc binary2sound.c -o binary2sound");

	//Capture image using webcam
	if(CaptureImage(argv)) return 1; //if CaptureImage returns 1 it failed (1 = true)

	//Compress image
	if(CompressImage()) return 1;

	//documentation for FFmpeg: https://ffmpeg.org/ffmpeg.html 

	//Converts image file to binary text file
	system("image2binary.exe ../tempFiles/blueshit.bmp ../tempFiles/tempBinary.txt");
	
	//Converts binary text file to sound
	system("binary2sound.exe -modulationScheme mfsk");

  return 0;
}

int CaptureImage(char *argv[])
{
	//------------------------------------
	//If in argument -webcam is 1
	//capture image using webcam
	//If capturing the image fails
	//the program returns 1. 
	//------------------------------------
	int imageCaptureRetryCount = 0, //image capture counter
	    capturingImage = 1; //webcam capture state 

	if(strcmp(argv[1], "-webcam") == 0)
  {
		if(strcmp(argv[2], "1") == 0)
    {
			while(capturingImage)
      {	
				//system returns either 0 (success) and 1 (failure)
				capturingImage = system("ffmpeg.exe -hide_banner -loglevel error -f  dshow -y -i \"video=USB2.0 HD UVC WebCam\" -frames:v 1 ../tempFiles/tempImage.png");
				
				if(imageCaptureRetryCount == 5)
        { 
          //If webcam cant capture a image for x amount of time, end program
					printf("Capture image failed\n");
					return 1;
				}
				imageCaptureRetryCount++;
			}
		}
		else
		{
			printf("Did not capture new image\n");
			return 0;
		}
	}
	else
	{
		printf("Could not recognize webcam command\n");
		return 0;
	}
	printf("Capture image succeeded\n");
	//if successful return 0
	return 0;
}

int CompressImage()
{
	//------------------------------------
	//Compression
	//Does compression and checks if
	//the compression is successful
	//------------------------------------

	int compressingImage = 1; //compressing image state 	

	compressingImage = system("ffmpeg.exe -hide_banner -loglevel error -y -i ../tempFiles/tempImage.png -q:v 2 -vf scale=1:-1 ../tempFiles/tempImage_compressed.jpeg"); 
  if(compressingImage == 0)
  {
    printf("Image has been compressed successfully\n");
		return 0;
  }
  else if(compressingImage == 1){
    printf("Compressing image failed\n");
		return 1;
  }

	return 0;
}
