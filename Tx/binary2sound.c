#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>

//Modulation functions
int _8fsk();
int makeAudioBuffer(int16_t *buffer, char *binaryBytes, long int binaryFileLen, int bitDuration);
void makeAudio(int16_t *buffer, long int N);
int calcSamples(int16_t **samples, int bitDuration);

//int _bfsk();

int main(int argc, char *argv[])
{
	char *imageBitArray, c;
	int modulationScheme = 0; //0 = 8fsk, 1 = bfsk
 
	//Checks the different input arguments
	//Argument example  -modulationScheme bfsk
	if(strcmp(argv[1], "-modulationScheme") == 0)
  {
    if(strcmp(argv[2], "8fsk") == 0)
    {
      modulationScheme = 0;
    }	
    else
    {
      modulationScheme = 1;
    }
  }

	if(modulationScheme == 0)
  {
		_8fsk();
	}
	else if (modulationScheme == 1)
	{
		//_bfsk();
	}
	
  return 0;
}

int _8fsk()
{
  char *binaryBytes, c;
  int index = 0, 
      bitDuration = 100 * 100; // duration of each bit (samples per bit)

  FILE *binaryFilePtr = fopen("../tempFiles/tempBinary.txt", "r");
  if (binaryFilePtr == NULL)
  {
    fclose(binaryFilePtr);
    printf("Could not open file");
    return 1;
  }

  fseek(binaryFilePtr, 0, SEEK_END);
  long binaryFileLen = ftell(binaryFilePtr); //Tells the position of the pointer
  fseek(binaryFilePtr, 0, SEEK_SET);

  printf("Binary file size: %ld\n", binaryFileLen);

  binaryBytes = malloc(sizeof(char) * (binaryFileLen + 1));
  if (binaryBytes == NULL)
  {
    printf("Input memory allocation error");
    return 1;
  }

  //Reads the bytes from binaryFilePtr and puts it into binaryBytes
  fread(binaryBytes, binaryFileLen, 1, binaryFilePtr); 
  fclose(binaryFilePtr); //and closes file when done

  long int N = 2 * binaryFileLen * bitDuration / 2.95; //number of samples
//why 2 og why 2.95?
//why 3?
  int16_t *buffer = malloc(3 * (N + 1) * sizeof(int16_t)); //buffer array
  if (buffer == NULL)
  {
    printf("Audio buffer memory allocation error");
    return 1;
  }     

  if (makeAudioBuffer(buffer, binaryBytes, binaryFileLen, bitDuration))
    return 1;
    free(binaryBytes);

  makeAudio(buffer, N);
  free(buffer);
  /*
  printf("Transmitting...");
  system("out.flac"); // play audio in system standard media player. must open in media player that close after play
  printf("Transmission finished\n");
  */

  printf("8fsk is done creating the audio sample\n");
  return 0;
}

int makeAudioBuffer(int16_t *buffer, char *binaryBytes, long int binaryFileLen, int bitDuration)
{
  long int n = 0; // buffer index
  int j = 0, e = bitDuration;    // bit array index

  int16_t *samples[11];
  if (calcSamples(samples, bitDuration)) //calculates matrix of samples
  {
    printf("Audio samples memory allocation error");
    return 1;
  }

  for (n; n < e; n++) // Wake-up tone, takes the first sine wave sample value
                      // and add them to the start of the buffer array
  {
    buffer[n] = samples[1][n % bitDuration];
  }
  printf("Making audio buffer...");
  while (j < binaryFileLen) 
  {
    e += bitDuration;
    //Checks for a certain combination of bits
    //if the combination is true *
    if (binaryBytes[j] - '0' == 1 && binaryBytes[j + 1] - '0' == 1 && binaryBytes[j + 2] - '0' == 1) // 111
    {
      //* go through the samples array, where the sine waves sample values are stored.
      //add those values to the buffer.
      for (n = n; n < e; n++) // loop - audio buffer
      {
        buffer[n] = samples[10][n % bitDuration]; //by doing n % bitDuration will always zero the value so
                                                  //even if n is higher than the array size
                                                  //it will never make an error.
                                                  //fx. n = 10000   -> 10000 % 10000 = 0 -> 19999 % 10000 = 9999
                                                  //fx. n = 30000   -> 30000 % 10000 = 0 -> 39999 % 10000 = 9999
      }
    }
    else if (binaryBytes[j] - '0' == 0 && binaryBytes[j + 1] - '0' == 1 && binaryBytes[j + 2] - '0' == 1) // 011
    {
      for (n = n; n < e; n++) 
      {
        buffer[n] = samples[9][n % bitDuration];
      }
    }
    else if (binaryBytes[j] - '0' == 1 && binaryBytes[j + 1] - '0' == 0 && binaryBytes[j + 2] - '0' == 1) // 101
    {
      for (n = n; n < e; n++) 
      {
        buffer[n] = samples[7][n % bitDuration];
      }
    }
    else if (binaryBytes[j] - '0' == 1 && binaryBytes[j + 1] - '0' == 1 && binaryBytes[j + 2] - '0' == 0) // 110
    {
      for (n = n; n < e; n++) 
      {
        buffer[n] = samples[6][n % bitDuration];
      }
    }
    else if (binaryBytes[j] - '0' == 0 && binaryBytes[j + 1] - '0' == 0 && binaryBytes[j + 2] - '0' == 1) // 001
    {
      for (n = n; n < e; n++) 
      {
        buffer[n] = samples[5][n % bitDuration];
      }
    }
    else if (binaryBytes[j] - '0' == 1 && binaryBytes[j + 1] - '0' == 0 && binaryBytes[j + 2] - '0' == 0) // 100
    {
      for (n = n; n < e; n++) 
      {
        buffer[n] = samples[4][n % bitDuration];
      }
    }
    else if (binaryBytes[j] - '0' == 0 && binaryBytes[j + 1] - '0' == 0 && binaryBytes[j + 2] - '0' == 0) // 000
    {
      for (n = n; n < e; n++) 
      {
        buffer[n] = samples[3][n % bitDuration];
      }
    }
    else if (binaryBytes[j] - '0' == 0 && binaryBytes[j + 1] - '0' == 1 && binaryBytes[j + 2] - '0' == 0) // 010
    {
      for (n = n; n < e; n++) 
      {
        buffer[n] = samples[2][n % bitDuration];
      }
    }
    else
      j++;

    e += bitDuration;
    for (n; n < e; n++) // end-tone, takes the first sine wave sample value
                        // and add them to the start of the buffer array
    {
      buffer[n] = samples[1][n % bitDuration];
    }
      j += 3;
  }

  printf("Audio buffer finished\n");
  return 0;
}

int calcSamples(int16_t **samples, int bitDuration)
{
  double amp = 16383.0; // amplitude
  double f_s = 44000.0;        // sampling frequency
  double freq = 440.0;        // frequency of sine wave

  double p2sf = 2.0 * M_PI / f_s;
  for (int i = 1; i <= 10; i++)
  {
    samples[i] = malloc(bitDuration * sizeof(int16_t));
    if (samples[i] == NULL)
        return 1;

    for (int j = 0; j < bitDuration; j++)
    {
        samples[i][j] = amp * sin(j * (freq * i) * p2sf);
    }
  }
  return 0;
}

void makeAudio(int16_t *buffer, long int N)
{
  // Pipe the audio data to ffmpeg, which writes it to an audio file (wav/flac..)
  FILE *audioPtr = popen("ffmpeg.exe -hide_banner -loglevel error -y -f s16le -acodec pcm_s16le -vn -ar 44000 -ac 1 -i - ../tempFiles/imageAudio.flac", "w");
    fwrite(buffer, 2, N, audioPtr);
    pclose(audioPtr);

}

//Virker vidst ikke helt, men det er heller ikke nødvendigt jo haha
/*
int _bfsk(){
  char *binary_bytes, c;
  int index = 0;

  FILE *binary_file_ptr = fopen("../tempFiles/tempBinary.txt", "r");
  if (binary_file_ptr == NULL){
    fclose(binary_file_ptr);
    return 1;
  }

  fseek(binary_file_ptr, 0, SEEK_END);
  long binary_file_size = ftell(binary_file_ptr); //Tells the position of the pointer
  fseek(binary_file_ptr, 0, SEEK_SET);

  binary_bytes = malloc(binary_file_size);
  while((c = fgetc(binary_file_ptr)) != EOF){
    binary_bytes[index] = c;
    index++;
  }

  printf("Binary file size: %ld\n", binary_file_size);

  long sample_index = 0, //index in sample array
    binary_index = 0, //index in binary array
    amp = 16383,
    f_s = 10000, //sampling frequency (samples per second)
    f_c = 440,  //carrier frequency (Hz)
    f_c_modifier = 0, //Value for modifying the carrier frequency
    image_bitrate = 10, //image bits that will be transfered per second
    samples_per_bit = f_s / image_bitrate; //Amount of samples per bit
    
  long sample_duration = binary_file_size / image_bitrate, //sample time in seconds
    sample_size = f_s * sample_duration;
  
  
  printf("Bits send over sound per sec: %d\n", image_bitrate);
  printf("Samples per bit: %d\n", samples_per_bit);
  printf("Sample length (sec): %ld\n", sample_duration);
  printf("Sample size: %ld\n", sample_size);

  printf("Create sample array\n");
  int16_t *samples = malloc((sizeof(int16_t) * 2) * (sample_size + 1)); //int16_t/short can store a value between −32.768 and 32.767
  samples[0] = 0;
  
  printf("Add values sample array\n");

  if(binary_bytes[binary_index] == '1'){
    f_c_modifier = 2;
  }
  else{
    f_c_modifier = 1;
  }
  
  for(sample_index = 0; sample_index < sample_size; sample_index++){
    if(sample_index % samples_per_bit == 0 && sample_index != 0){
      binary_index++;

      if(binary_bytes[binary_index] == '1'){
        f_c_modifier = 2;
      }
      else{
        f_c_modifier = 1;
      }
    }
    samples[sample_index] = amp * sin(sample_index * (f_c * f_c_modifier) * 2.0 * M_PI / f_s);
  }

  printf("Convert values to audio file (ffmpeg.exe)\n");
  FILE *audio_ptr;

  audio_ptr = popen("ffmpeg.exe -hide_banner -loglevel error -y -f s16le -acodec pcm_s16le -vn -ar 88000 -ac 1 -i - ../tempFiles/sound.flac", "w");
  if(audio_ptr != NULL){
    fwrite(samples, 2, sample_size, audio_ptr);
    pclose(audio_ptr);
  }
  else{
    printf("Could not open ffmpeg correct\n");
    pclose(audio_ptr);
    return 1;
  }
  
  
  printf("Add values to csv file\n");
  FILE *csvfile;
  //csvfile = fopen("../io_files/samples.csv", "w");
  csvfile = fopen("../tempFiles/soundSamples.csv", "w");
  for (int n = 0 ; n < sample_size ; ++n){
    fprintf(csvfile, "%d\n", samples[n]);
    
    if(n > 100000){
      break;
    }
  } 
  fclose(csvfile);
  

  fclose(binary_file_ptr);
}
*/
