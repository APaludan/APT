#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>


int calcSamples(int16_t **sampledSineWaves, double f_s, int bitDuration, double f_c, double amp);
int _8fsk(char *imageBitArray, int16_t **sample, long int imageBitArrayLength, long *sampleLength);
int _bfsk(char *imageBitArray, int16_t **sample, long int imageBitArrayLength, long *sampleLength);

int main(int argc, char *argv[])
{
	char outputFileName[100], *imageBitArray, c;
	int modulationScheme = 0, //0 = 8fsk, 1 = bfsk
      index = 0; 
	int16_t *sample;
	long sampleLength = 0;

	//Checks the different input arguments
	//Argument example  -output filepath/name.fileType -modulationScheme 8fsk
	#pragma region argument handling

	if(argc == 3)
	{
		printf("Modulation Scheme is not given, defaulting to 8fsk");
		if(strcmp(argv[1], "-output") == 0)
    {
			strcpy(outputFileName, argv[2]);
			modulationScheme = 0;
		}
	}
	else if(argc > 4)
	{
		if(strcmp(argv[1], "-output") == 0)
    {
			strcpy(outputFileName, argv[2]);
		}
		if(strcmp(argv[3], "-modulationScheme") == 0)
    {
			if(strcmp(argv[4], "8fsk") == 0)
    	{
				modulationScheme = 0;
			}	
			else
			{
				modulationScheme = 1;
			}
		}
	}

	#pragma endregion

	#pragma region read binary file
	//Get binary file information
  FILE *binaryFilePtr = fopen("../tempFiles/tempBinary.txt", "r");
  if (binaryFilePtr == NULL){
    fclose(binaryFilePtr);
    return 1;
  }

  fseek(binaryFilePtr, 0, SEEK_END);
  long int binaryFileSize = ftell(binaryFilePtr); //Tells the position of the pointer
  fseek(binaryFilePtr, 0, SEEK_SET);

  imageBitArray = malloc(sizeof(char) * (binaryFileSize + 1));
  if(imageBitArray == NULL)
  {
    printf("Cant allocate memory for image bits\n");
    return 1;  
  }

  while((c = fgetc(binaryFilePtr)) != EOF){
    imageBitArray[index] = c;
    index++;
  }
	
	#pragma endregion

	if(modulationScheme == 0)
  {
		_8fsk(imageBitArray, &sample, binaryFileSize, &sampleLength);
	}
	else if (modulationScheme == 1)
	{
		//_bfsk();
	}

	#pragma region ceate audio file
	//pipe audio to file

  FILE *audioPtr;	
	char optionString[] = {"ffmpeg.exe -hide_banner -loglevel error -y -f s16le -acodec pcm_s16le -vn -ar 44000 -ac 1 -i - "};

	printf("\n%ld", sampleLength);
	printf("\n\n%s\n\n", strcat(optionString, outputFileName));
	  			 //= popen("ffmpeg.exe -hide_banner -loglevel error -y -f s16le -acodec pcm_s16le -vn -ar 44000 -ac 1 -i - out.flac", "w");
  audioPtr = popen("ffmpeg.exe -hide_banner -loglevel error -y -f s16le -acodec pcm_s16le -vn -ar 44000 -ac 1 -i - out.flac", "w");
  //audioPtr = popen(strcat(optionString, outputFileName), "w");
  if(audioPtr != NULL){
		long n = sizeof(sample)/sizeof(sample[0]);
		printf("%ld", n);

    fwrite(sample, 2, sampleLength, audioPtr);
    pclose(audioPtr);
  }
  else{
    printf("Could not open ffmpeg correct\n");
    pclose(audioPtr);
    return 1;
  }

	#pragma endregion

	fclose(binaryFilePtr);
	free(sample);
  free(imageBitArray);

  return 0;
}

int _8fsk(char *imageBitArray, int16_t **sample, long int imageBitArrayLength, long *sampleLength)
{
	long int n = 0; // sample array index
	int j = 0,     // image bit array index
      i = 0;

	double amp = (INT16_MAX / 2.0) - 10.0; // amplitude - int16_max/2 because a value in a sound array can be negative and positive
	double f_s = 44000.0;        			// sampling frequency (samples per second)
	int bitDuration = 100 * 100;      // duration of each bit (samples per bit)
	double f_c = 440.0;        				// frequency of sine wave (carrier frequency)
	
	long tmpSampleLength = ((imageBitArrayLength * bitDuration)) * 2;
	*sample = malloc(sizeof(int16_t) * (tmpSampleLength + 1));
	*sampleLength = tmpSampleLength;
  //før 2 * filelen * bd / 2.95;
	if(*sample == NULL){
		printf("Audio samples memory allocation error");
		return 1;
	}

	int16_t *sampledSineWaves[11];
	if (calcSamples(sampledSineWaves, f_s, bitDuration, f_c, amp) == 1) 		//calculates matrix of samples
	{
			printf("Audio samples memory allocation error");
			return 1;
	}
		int e = bitDuration; //Sets e (end of bit duration) to size of one bitDuration. Used to determine how many values 
											//that should be put into the samples array
    for (n; n < e; n++) // wake-up tone
    {
        *sample[n] = sampledSineWaves[1][n];
    }

    printf("creating audio sample");
    while (j < imageBitArrayLength) //Loop through all bits in the image bit array
    {
        e += bitDuration;
        
				//In the if statement blocks, the current and the two next bits will be read and a 3bit combination
				//can be found. An already sampled sine wave will be added to the samples array (output) 
				//according to the 3bit combination. 
				//ultimalety creating a large sample with sine waves with different frequencies. 
        if (imageBitArray[j] - '0' == 1 && imageBitArray[j + 1] - '0' == 1 && imageBitArray[j + 2] - '0' == 1) // 111
        {
            for (n = n; n < e; n++) 
            {
								//In this case it takes the sinewaves stored at index 10. 
								//and adds its values to the samples array. 
                *sample[n] = sampledSineWaves[10][n % bitDuration]; 
								//by doing n % bitDuration - the index will always be from 0 - the size of the array, and never throw index out of bounce error. 
								//fx n = 19999 % 10000 = 9999. 

            }
        }
        else if (imageBitArray[j] - '0' == 0 && imageBitArray[j + 1] - '0' == 1 && imageBitArray[j + 2] - '0' == 1) // 011
        {
            for (n = n; n < e; n++) 
            {
                *sample[n] = sampledSineWaves[9][n % bitDuration];
            }
        }
        else if (imageBitArray[j] - '0' == 1 && imageBitArray[j + 1] - '0' == 0 && imageBitArray[j + 2] - '0' == 1) // 101
        {
            for (n = n; n < e; n++) 
            {
                *sample[n] = sampledSineWaves[7][n % bitDuration];
            }
        }
        else if (imageBitArray[j] - '0' == 1 && imageBitArray[j + 1] - '0' == 1 && imageBitArray[j + 2] - '0' == 0) // 110
        {
            for (n = n; n < e; n++) 
            {
                *sample[n] = sampledSineWaves[6][n % bitDuration];
            }
        }
        else if (imageBitArray[j] - '0' == 0 && imageBitArray[j + 1] - '0' == 0 && imageBitArray[j + 2] - '0' == 1) // 001
        {
            for (n = n; n < e; n++) 
            {
                *sample[n] = sampledSineWaves[5][n % bitDuration];
            }
        }
        else if (imageBitArray[j] - '0' == 1 && imageBitArray[j + 1] - '0' == 0 && imageBitArray[j + 2] - '0' == 0) // 100
        {
            for (n = n; n < e; n++) 
            {
                *sample[n] = sampledSineWaves[4][n % bitDuration];
            }
        }
        else if (imageBitArray[j] - '0' == 0 && imageBitArray[j + 1] - '0' == 0 && imageBitArray[j + 2] - '0' == 0) // 000
        {
            for (n = n; n < e; n++) 
            {
                *sample[n] = sampledSineWaves[3][n % bitDuration];
            }
        }
        else if (imageBitArray[j] - '0' == 0 && imageBitArray[j + 1] - '0' == 1 && imageBitArray[j + 2] - '0' == 0) // 010
        {
            for (n = n; n < e; n++)
            {
                *sample[n] = sampledSineWaves[2][n % bitDuration];
            }
        }
        else
            j++;

        e += bitDuration;

				//if no combination can be found, add a specified sine wave. 
        for (n; n < e; n++) 
        {
            *sample[n] = sampledSineWaves[1][n % bitDuration];
        }

        j += 3;
    }

		printf("8fsk is done creating the audio sample\n");
    return 0;
}


int calcSamples(int16_t **sampledSineWaves, double f_s, int bitDuration, double f_c, double amp)
{
    double p2f_s = 2.0 * M_PI / f_s;

    //j = rows, i = columns
    for (int i = 1; i <= 10; i++)
    {
        sampledSineWaves[i] = malloc((bitDuration +1 )* sizeof(int16_t)); //allocates memory for a new sineWave sample
        if (sampledSineWaves[i] == NULL)
            return 1;

        for (int j = 0; j < bitDuration; j++) //add sineWave sample values to sineWave sample array
        {	
            sampledSineWaves[i][j] = amp * sin(j * (f_c * i) * p2f_s); //y = amp * sin(t * (f_c) * (2*pi / f_s))
        }
    }
    return 0;
}

int _bfsk(char *imageBitArray, int16_t **sample, long int imageBitArrayLength, long int *sampleLength)
{

}