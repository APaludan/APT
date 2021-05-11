#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>

//Modulation functions
int _8fsk();
int _bfsk();

int main(int argc, char *argv[])
{
	char outputFileName[100], *imageBitArray, c;
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
		//_8fsk();
	}
	else if (modulationScheme == 1)
	{
		_bfsk();
	}

	
  return 0;
}

int _8fsk()
{
	//Add old code(Andreas) and new code(Emilie) here.

  printf("8fsk is done creating the audio sample\n");
  return 0;
}


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
  //-y = overwrite already existing file.
  //-f = what format the data is in f.x (s16le / int16_t)
  //-ar = sampling frequency (f_s)
  //-ac = amount of channels 
  //-i = read input from standard input (in this case the pipe input)
  // tilføj evt: ' -acodec pcm_s16le -vn ' mellem s16le og -ar
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
