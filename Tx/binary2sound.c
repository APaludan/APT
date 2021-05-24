#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>

//Modulation functions
int mfsk();
int makeAudioBuffer(int16_t *buffer, char *binaryBytes, long int bitDuration, long int binaryFileLen, long int N);
void addSeparatorTone(int16_t *buffer, long int *n, long int bitDuration, int16_t **samples);
void addBitstringTone(int16_t *buffer, long int *n, long int bitDuration, char *binaryBytes, long int j, int16_t **samples, long int binaryFileLen);
void makeAudio(int16_t *buffer, long int N);
int calcSamples(int16_t **samples, long int bitDuration, long int N, long int binaryFileLen);

//int _bfsk();

int main(int argc, char *argv[])
{
	//char *imageBitArray, c; disse variabler bliver ikke brugt, så tænker, de skal væk (c bliver brugt senere, men bliver også initialiseret senere)
	int modulationScheme = 0; //0 = mfsk, 1 = bfsk
 
	//Checks the different input arguments
	//Argument example  -modulationScheme bfsk
	if(strcmp(argv[1], "-modulationScheme") == 0)
  {
    if(strcmp(argv[2], "mfsk") == 0)
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
		mfsk();
	}
	else if (modulationScheme == 1)
	{
		//_bfsk();
	}
	
  return 0;
}

int mfsk()
{
  char *binaryBytes, c;
  int index = 0;
      //bitDuration is calculated by dividing the sample rate of the system with the framerate. eg: 44100 / 144 = 306.25 (INT 306) and multiplied by scalar (# of frames for each tone)
  long int bitDuration = 306*30; // duration of each bit (samples per bit)

  FILE *binaryFilePtr = fopen("../tempFiles/tempBinary.txt", "r");
  //  FILE *binaryFilePtr = fopen("../tempFiles/img.txt", "r"); //short binary file for test
  if (binaryFilePtr == NULL)
  {
    fclose(binaryFilePtr);
    printf("Could not open file");
    return 1;
  }

  printf("Bitduration: %ld\n", bitDuration);

  fseek(binaryFilePtr, 0, SEEK_END);
  long int binaryFileLen = ftell(binaryFilePtr); //Tells the position of the pointer
  fseek(binaryFilePtr, 0, SEEK_SET);

  printf("Binary file size: %ld\n", binaryFileLen);

  binaryBytes = malloc(sizeof(char) * (binaryFileLen + 1)); //The parameter to malloc is of type size_t, which is an unsigned type. If you pass an argument of any other integer type, it will be implicitly converted to size_t. (https://stackoverflow.com/questions/43453452/using-long-int-as-a-parameter-for-malloc)
  if (binaryBytes == NULL)
  {
    printf("Input memory allocation error");
    return 1;
  }

  //Reads the bytes from binaryFilePtr and puts it into binaryBytes
  fread(binaryBytes, binaryFileLen, 1, binaryFilePtr); 
  fclose(binaryFilePtr); //and closes file when done

  long int N = (long int)ceil((2 * binaryFileLen * bitDuration + bitDuration) / 3.); //number of samples including separation tones (er lige (14/5) rettet til 3, ret tilbage til 2.95 hvis det ikke virker)
  //the number of sep tones is equal to the number of bit tones + 1

  /*
  - Each 1/0 is represented by a sound, that sound consists of bitDuration samples (binaryfilelen * bitduration)
  - 3 bits pr sound therefore divided by 2.95, almost 3
  - We want a tone between every "bit tone" therefore 2*
  */

  int16_t *buffer = malloc(3 * (N + 1) * sizeof(int16_t)); //buffer array
  //kan vi gøre det mindre eller hvad?
  if (buffer == NULL)
  {
    printf("Audio buffer memory allocation error");
    return 1;
  }     

  //makeAudioBuffer skal returnere 1 ved fejl eller 0 ved succes
  if (makeAudioBuffer(buffer, binaryBytes, bitDuration, binaryFileLen, N))
    return 1;
    
  free(binaryBytes); //skal vi free'e før return 1; ? Og skal vi i så fald free'e alt inden de to return 1?

  makeAudio(buffer, N);
  free(buffer);

  /*
  printf("Transmitting...");
  system("../tempFiles/imageAudio.flac"); // play audio in system standard media player. must open in media player that close after play
  printf("Transmission finished\n");
  */

  printf("Mfsk is done creating the audio sample\n");
  return 0;
}

int makeAudioBuffer(int16_t *buffer, char *binaryBytes, long int bitDuration, long int binaryFileLen, long int N){
  long int n = 0, j = 0; 
  int16_t *samples[16];

  if (calcSamples(samples, bitDuration, N, binaryFileLen)) //calculates matrix of samples
  {
    printf("Audio samples memory allocation error");
    return 1;
  }

  addSeparatorTone(buffer, &n, bitDuration, samples); //writes wake up tone to buffer
  while (j < binaryFileLen){ //writes tones to the buffer depending on combinations of three bits at a time. Every other tone is a separation tone (to make rx easier)
    addBitstringTone(buffer, &n, bitDuration, binaryBytes, j, samples, binaryFileLen);
    addSeparatorTone(buffer, &n, bitDuration, samples);
    j += 3;
  }
  return 0;
}

void addSeparatorTone(int16_t *buffer, long int *n, long int bitDuration, int16_t **samples){ //skriver en separator-tone (440 Hz) ind i buffer
    long int e = (*n) + bitDuration; //is used in the for loop as the upper bound for n, as the number of samples for each bit sequence/separation tone should be equal to the bitDuration measured in samples
    
    for (*n; *n < e; (*n)++)
    {
        buffer[*n] = samples[1][(*n) % bitDuration]; //by doing n % bitDuration will always zero the value so
                                                    //even if n is higher than the array size
                                                    //it will never make an error.
                                                    //fx. n = 10000   -> 10000 % 10000 = 0 -> 19999 % 10000 = 9999
                                                    //fx. n = 30000   -> 30000 % 10000 = 0 -> 39999 % 10000 = 9999
        
    }
}

void addBitstringTone(int16_t *buffer, long int *n, long int bitDuration, char *binaryBytes, long int j, int16_t **samples, long int binaryFileLen){
    int i = 0, k = 0;
    char bitCombinations[14][4] = {"000", "001", "010", "100", "011", "101", "110", "111",
                                   "00", "01", "10", "11",
                                   "0", "1"
                                  };
    char str[4] = "";
    long int e = (*n) + bitDuration;//is used in the for loop as the upper bound for n, as the number of samples for each bit sequence/separation tone should be equal to the bitDuration measured in samples

    for (k = 0; k < 3; ++k){
        if ((j + k) < binaryFileLen)  //tilføjet 17/5, ikke testet. Men burde forhindre out of bounds
          strncat(str, &binaryBytes[j+k], 1); //copies next three bits from binary img-file into a separate string
    }

    for (i = 0; i < 14; ++i){
        if (!strcmp(str,bitCombinations[i])){ //compares string to the possible bit combinations to find the correct index
          break;
        }
    }

    for (*n; *n < e; (*n)++)
    {
        buffer[*n] = samples[i+2][(*n) % bitDuration]; //samples[1] is the separation tone, meaning that the tones representing bit seqs have indices 2 through 15
    }
}

// source: https://batchloaf.wordpress.com/2017/02/10/a-simple-way-to-read-and-write-audio-and-video-files-in-c-using-ffmpeg/
int calcSamples(int16_t **samples, long int bitDuration, long int N, long int binaryFileLen)
{
  double amp = 16383.0; // amplitude
  double f_s = 44000.0;        // sampling frequency 
  double freq = 440.0;        // frequency of sine wave
  int seconds = N / f_s;  //  second calculation used in calculation of bps (length of output sound file)
  int bps = binaryFileLen / seconds;  //  calculation for bps
  double p2f_s = 2.0 * M_PI / f_s;

  for (int i = 1; i <= 15; i++)
  {
    samples[i] = malloc(bitDuration * sizeof(int16_t));
    if (samples[i] == NULL)
        return 1;

    for (int j = 0; j < bitDuration; j++) //j er tid ud af x-aksen
    {
        samples[i][j] = amp * sin(j * (freq * i) * p2f_s - M_PI/2); //(- M_PI/2 -> phaseshifting)
        //vi får en y værdi som svarer til amplituden af kurven til tiden j
    }
  }

    //  prints the initial binary file length, duration of audio file and bit rate
    printf("Input length: %d\n", binaryFileLen);
    printf("Sample count: %ld\n", N);
    printf("Audio duration: %d\n", seconds);
    printf("Bitrate (/s): %d\n", bps);
  return 0;
}

// source: https://batchloaf.wordpress.com/2017/02/10/a-simple-way-to-read-and-write-audio-and-video-files-in-c-using-ffmpeg/
void makeAudio(int16_t *buffer, long int N)
{
  // Pipe the audio data to ffmpeg, which writes it to an audio file (wav/flac..)
  FILE *audioPtr = popen("ffmpeg.exe -hide_banner -loglevel error -y -f s16le -acodec pcm_s16le -vn -ar 44000 -ac 1 -i - ../tempFiles/imageAudio.wav", "w");
    fwrite(buffer, 2, N, audioPtr);
    pclose(audioPtr);
  
  printf("Adding values to .csv file...\n");
  FILE *csvfile;
  //csvfile = fopen("../io_files/samples.csv", "w");
  csvfile = fopen("buffer.csv", "w");
  for (int i = 0 ; i < N ; ++i){
    fprintf(csvfile, "%d\n", buffer[i]);
  } 
  fclose(csvfile);
  printf(".csv file saved\n");
}
 
/*int makeAudioBuffer(int16_t *buffer, char *binaryBytes, long int binaryFileLen, int bitDuration, int N)
{
  long int n = 0; // buffer index
  int j = 0,
       e = bitDuration;    // bit array index

  int16_t *samples[11];
  if (calcSamples(samples, bitDuration, N, binaryFileLen)) //calculates matrix of samples
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
                        // and add them to end of the buffer array
    {
      buffer[n] = samples[1][n % bitDuration];
    }
      j += 3;
  }

  printf("Audio buffer finished\n");
  return 0;
}*/

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
