#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <windows.h>

void printInfo(int N, float sf, char *input);
void makeAudioBuffer(int16_t *buf, char *input);
void makeAudio(int16_t *buf, int N);

float amp = 2000;    // amplitude. skal være 2000 ellers går alt i stykker
float sf = 176000.0; // sampling frequency
float bd = 176 * 14; // duration of each bit. must be = period of sine wave
float freq = 2000.0; // frequency of sine wave

int main(void)
{
    int mode = 0; // No webcam = 0 | webcam = 1
    if (mode == 1)
    {
        system("ffmpeg.exe -f dshow -y -i \"video=Lenovo EasyCamera\" -frames:v 1 img.bmp");
        sleep(3);
        system("img2bin.exe");
        sleep(10);
    }

    char input[100000];
    FILE *fp = fopen("test1.txt", "r");
    if (fp == NULL)
    {
        printf("Could not open txt file.");
        return 1;
    }
    fscanf(fp, "%s", input);
    fclose(fp);

    int N = strlen(input) * bd - 1; // number of samples
    printInfo(N, sf, input);

    int16_t *buf = malloc(N * sizeof(int16_t)); // buffer
    makeAudioBuffer(buf, input);

    makeAudio(buf, N);

    return 0;
}

void makeAudio(int16_t *buf, int N)
{
    // Pipe the audio data to ffmpeg, which writes it to a wav file
    FILE *pipeout;
    pipeout = popen("ffmpeg.exe -y -f s16le -ar 176000 -ac 1 -i - out.wav", "w");
    fwrite(buf, 2, N, pipeout);
    pclose(pipeout);
    printf("pt2 %d\n", buf[1285]);
}

void makeAudioBuffer(int16_t *buf, char *input)
{
    int n = 0; // buffer index

    char *bit = input;
    int j = 0; // bit array index

    while (j < strlen(bit))
    {
        if (bit[j] - '0' == 1)
        {
            int e = n + bd;
            for (n = n; n < e; n++)
            {
                buf[n] = amp * sin((2.0 * M_PI * n * freq) / sf);
            }
            j++;
        }
        else if (bit[j] - '0' == 0)
        {
            int e = n + bd;
            for (n = n; n < e; n++)
            {
                buf[n] = amp * sin((2.0 * M_PI * n * (freq - 500)) / sf);
            }
            j++;
        }
        else
        {
            j++;
        }
    }
    return;
}

void printInfo(int N, float sf, char *input)
{
    float seconds = N / sf;
    float bps = strlen(input) / seconds;
    printf("N: %d\n", N);
    printf("seconds: %f\n", seconds);
    printf("bps: %f\n", bps);
}