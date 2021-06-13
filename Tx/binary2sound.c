#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>


#define NUMBER_OF_BITS 3


//Modulation functions
int _mfsk();
int makeAudioBuffer(int16_t *buffer, char *binaryBytes, long int bitDuration, long int binaryFileLen, int N);
void addSeparatorTone(int16_t *buffer, long int *n, long int bitDuration, int16_t **samples, int isFirst);
void addEndTone(int16_t *buffer, long int *n, long int bitDuration, int16_t **samples);
void addBitstringTone(int16_t *buffer, long int *n, long int bitDuration, char *binaryBytes, long int j, int16_t **samples, long int binaryFileLen);
void makeAudio(int16_t *buffer, long int N);
int calcSamples(int16_t **samples, long int bitDuration, long int N, long int binaryFileLen);
int identifyBitCombination(char *bits);
int right(int i);
int left(int i);

//int _bfsk();

int main(void){
	//char *imageBitArray, c; disse variabler bliver ikke brugt, så tænker, de skal væk (c bliver brugt senere, men bliver også initialiseret senere)
	int modulationScheme = 0; //0 = mfsk, 1 = bfsk
 
	_mfsk();
	
  return 0;
}

int _mfsk()
{
  char *binaryBytes, c;
  int index = 0;
      //bitDuration is calculated by dividing the sample rate of the system with the framerate. eg: 44100 / 144 = 306.25 (INT 306) and multiplied by scalar (# of frames for each tone)
  long int bitDuration = 306*5; // duration of each bit (samples per bit)

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

  int16_t *buffer = malloc((3 * (N + 1) + 55) * sizeof(int16_t)); //buffer array
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

int makeAudioBuffer(int16_t *buffer, char *binaryBytes, long int bitDuration, long int binaryFileLen, int N){
  long int n = 0, j = 0; 
  int16_t *samples[16];

  if (calcSamples(samples, bitDuration, N, binaryFileLen)) //calculates matrix of samples
  {
    printf("Audio samples memory allocation error");
    return 1;
  }

  addSeparatorTone(buffer, &n, bitDuration, samples, 1); //writes wake up tone to buffer
  while (j < binaryFileLen){ //writes tones to the buffer depending on combinations of three bits at a time. Every other tone is a separation tone (to make rx easier)
    addBitstringTone(buffer, &n, bitDuration, binaryBytes, j, samples, binaryFileLen);
    addSeparatorTone(buffer, &n, bitDuration, samples, 0);
    j += 3;
  }
  addEndTone(buffer, &n, bitDuration, samples);

  return 0;
}

void addSeparatorTone(int16_t *buffer, long int *n, long int bitDuration, int16_t **samples, int isFirst){ //skriver en separator-tone (440 Hz) ind i buffer
  long int e; //is used in the for loop as the upper bound for n, as the number of samples for each bit sequence/separation tone should be equal to the bitDuration measured in samples
  int i;

  if (isFirst){ //hvis det er den første tone lægges der en kvart bølge ind i bufferen først, for at amplituden starter i 0
    for (i = 75; i < 100; i++, *n++){
      buffer[*n] = samples[1][i % bitDuration];
    }
  }

  e = (*n) + bitDuration;
  for (*n; *n < e; (*n)++){
    buffer[*n] = samples[1][(*n) % bitDuration]; //by doing n % bitDuration will always zero the value so
                                                    //even if n is higher than the array size
                                                    //it will never make an error.
                                                    //fx. n = 10000   -> 10000 % 10000 = 0 -> 19999 % 10000 = 9999
                                                    //fx. n = 30000   -> 30000 % 10000 = 0 -> 39999 % 10000 = 9999
        
  }
}

void addBitstringTone(int16_t *buffer, long int *n, long int bitDuration, char *binaryBytes, long int j, int16_t **samples, long int binaryFileLen){
    int i = 0, k = 0;

    char str[15] = "";
    long int e = (*n) + bitDuration;//is used in the for loop as the upper bound for n, as the number of samples for each bit sequence/separation tone should be equal to the bitDuration measured in samples

    for (k = 0; k < NUMBER_OF_BITS; ++k){
        if ((j + k) < binaryFileLen)  //tilføjet 17/5, ikke testet. Men burde forhindre out of bounds
          strncat(str, &binaryBytes[j+k], 1); //copies next 4 bits from binary img-file into a separate string
    }

    i = identifyBitCombination(str); //bruges til at finde ud af hvilken bitstreng det er, hvilket bestemmer hvilken tone der skal tilføjes

    for (*n; *n < e; (*n)++){
        buffer[*n] = samples[i+2][(*n) % bitDuration]; //samples[1] is the separation tone, meaning that the tones representing bit seqs have indices 2 through 15
    }
}

void addEndTone(int16_t *buffer, long int *n, long int bitDuration, int16_t **samples){ //skriver den sidste del af den sidste separator-tone (440 Hz) ind i buffer
//så den slutter med amplitude = 0
  int i;

  for (i = 0; i < 25; i++, *n++){
    buffer[*n] = samples[1][i % bitDuration]; //by doing n % bitDuration will always zero the value so
                                                    //even if n is higher than the array size
                                                    //it will never make an error.
                                                    //fx. n = 10000   -> 10000 % 10000 = 0 -> 19999 % 10000 = 9999
                                                    //fx. n = 30000   -> 30000 % 10000 = 0 -> 39999 % 10000 = 9999     
  }
}

int identifyBitCombination(char *bits){
  /* I denne funktion undersøges de 4 bits, der ligger i "str". Det gøres nærmest i et binary tree, hvor
  der gås til venstre, hvis en bit er 0 og højre hvis den er 1 */
    int res = 1, i; //res indeholder den plads, man ender på i det binary tree

    for (i = 0; i < NUMBER_OF_BITS; ++i){
        if (bits[i] == '0') //hvis karakter nr. i er et 0, bruges funktionen left til at beregne positionen i tree'et
            res = left(res);
        else
            res = right(res); //hvis karakter nr. i er et 1, går man videre med funktionen right
    }
    return res - pow(2,NUMBER_OF_BITS); //returnerer indices fra 0 til 2^(NUMBER_OF_BITS)-1
    //det nederste lag i tree'et har pladserne [2^NUMBER_OF_BITS;2^(NUMBER_OF_BITS+1)-1].
    //Jeg trækker 2^NUMBER_OF_BITS fra, fordi vi så får et indeks returneret som er 0, hvis bitstrengen kun 
    //indeholder 0'er, og ellers er det større end 0, men mindre end 2^NUMBER_OF_BITS
}

int left(int i){
    return 2*i;
}
int right(int i){
    return 2*i + 1;
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