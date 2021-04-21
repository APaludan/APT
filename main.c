#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

void printInfo(long long int N, float sf, char *input);
void makeAudioBuffer(int16_t *buf, char *input, long long int filelen);
void makeAudio(int16_t *buf, long long int N);

double amp = 32000;    // amplitude. skal være 2000 ellers går alt i stykker
double sf = 88000.0;  // sampling frequency
double bd = 200 * 100;  // duration of each bit
double freq = 440.0; // frequency of sine wave

int main(void)
{
    /*int mode = 0; // No webcam = 0 | webcam = 1
    if (mode == 1)
    {
        system("ffmpeg.exe -f dshow -y -i \"video=Lenovo EasyCamera\" -frames:v 1 img.bmp");
        sleep(3);
    }*/
    //img2bin();

    FILE *fp = fopen("test1.txt", "rb");
    if (fp == NULL)
    {
        printf("Could not open file.");
        return 1;
    }
    fseek(fp, 0, SEEK_END);
    long long int filelen = ftell(fp);
    rewind(fp);
    char *input = malloc(filelen * sizeof(char));
    if (input == NULL)
    {
        printf("Input memory allocation error");
        return 1;
    }
    fread(input, filelen, 1, fp);
    fclose(fp);

    long long int N = strlen(input) * bd - 1; // number of samples
    printInfo(N, sf, input);

    int16_t *buf = malloc(N * sizeof(int16_t)); // buffer
    if (buf == NULL)
    {
        printf("Audio buffer memory allocation error");
        return 1;
    }

    makeAudioBuffer(buf, input, filelen);
    makeAudio(buf, N);

    return 0;
}

void makeAudio(int16_t *buf, long long int N)
{
    // Pipe the audio data to ffmpeg, which writes it to a wav file
    FILE *pipeout = popen("ffmpeg.exe -y -f s16le -ar 44000 -ac 1 -i - out.wav", "w");
    fwrite(buf, 2, N, pipeout);
    pclose(pipeout);
    printf("pt2 %d\n", buf[1285]);
}

void makeAudioBuffer(int16_t *buf, char *input, long long int filelen)
{
    long long int n = 0; // buffer index
    int j = 0; // bit array index

    /*
    for (n = 0; n < sf*2; n++)
    {
        buf[n] = amp * sin(2 * M_PI * (n/sf * freq));
    }
    */

    while (j < filelen) // loop - input
    {
        if (input[j] - '0' == 1) 
        {
            int e = n + bd;
            for (n = n; n < e; n++) //loop - audio buffer 
            {
                double sample = amp * sin(2 * M_PI * (n/sf * (freq)));
                buf[n] = (int16_t) sample;
            }
            j++;
        }
        else if (input[j] - '0' == 0)
        {
            int e = n + bd;
            for (n = n; n < e; n++) //loop - audio buffer 
            {
                double sample = amp * sin(2 * M_PI * (n/sf * (freq*2)));
                buf[n] = (int16_t) sample;
            }
            j++;
        }
        else
        {
            j++;
        }
    }
    
    printf("Audio buffer finished");
    return;
}

void printInfo(long long int N, float sf, char *input)
{
    float seconds = N / sf;
    float bps = strlen(input) / seconds;
    printf("Input len: %d\n", strlen(input));
    printf("N: %d\n", N);
    printf("seconds: %f\n", seconds);
    printf("bps: %f\n", bps);
}