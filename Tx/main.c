#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>

void printInfo(long int N, int filelen);
void makeAudioBuffer(int16_t *buf, char *input, long int filelen);
void makeAudio(int16_t *buf, long int N);
void cleanUp(int mode);
void ccImage(void);
void calcSamples(int16_t **samples);

double amp = INT16_MAX - 1; // amplitude
double sf = 88000.0;        // sampling frequency
int bd = 400 * 5;           // duration of each bit (samples per bit)
double freq = 880.0;        // frequency of sine wave

int main(void)
{
    int mode = 0; // 0 = No webcam | 1 = webcam
    cleanUp(mode);
    if (mode == 1)
    {
        system("ffmpeg.exe -hide_banner -loglevel error -f  dshow -y -i \"video=Lenovo EasyCamera\" -frames:v 1 underwater.png"); // capture picture with webcam
        Sleep(1000);                                                                                                              // wait for webcam capture
    }
    ccImage();

    FILE *fp = fopen("imagebin.txt", "rb"); // open binary txt
    if (fp == NULL)
    {
        Sleep(1000); // if file doesnt exist, wait additional 1 sec. maybe pc is slow
        if (fp == NULL)
        {
            printf("Could not open file.");
            return 1;
        }
    }
    fseek(fp, 0, SEEK_END);       // filepointer to end of file
    long int filelen = ftell(fp); // read file length
    rewind(fp);                   // fp back to beginning
    char *input = malloc(filelen * sizeof(char));
    if (input == NULL)
    {
        printf("Input memory allocation error");
        return 1;
    }
    fread(input, filelen, 1, fp); // read binary file
    fclose(fp);

    long int N = filelen * bd / 3.8; // number of samples
    printInfo(N, filelen);

    int16_t *buf = malloc((N + 1) * sizeof(int16_t)); // buffer
    if (buf == NULL)
    {
        printf("Audio buffer memory allocation error");
        return 1;
    }
    clock_t begin = clock();
    makeAudioBuffer(buf, input, filelen);
    free(input);
    clock_t end = clock();
    makeAudio(buf, N);
    free(buf);
    double time_spend = (double)(end - begin) / CLOCKS_PER_SEC; //
    printf("\nTime spend calculating audio samples: %f\n", time_spend);
    printf("Transmitting...");
    system("out.flac"); // play audio in system standard media player. must open in media player that close after play
    printf("Transmission finished\n");
    return 0;
}

void cleanUp(int mode) // remove old files
{
    if (mode == 1)
        remove("underwater.png");
    remove("compressed.jpeg");
    remove("imagebin.txt");
    remove("out.flac");
    return;
}

void ccImage(void) // compress and convert to bits
{
    system("ffmpeg.exe -hide_banner -loglevel error -i underwater.png -q:v 2 -vf scale=360:-1 compressed.jpeg"); // compress
    system("img2bin.exe compressed.jpeg");                                                                       // convert to bits
    printf("Image converted to bits.\n");
    return;
}

void makeAudio(int16_t *buf, long int N)
{
    // Pipe the audio data to ffmpeg, which writes it to a wav file
    FILE *pipeout = popen("ffmpeg.exe -hide_banner -loglevel error -y -f s16le -acodec pcm_s16le -vn -ar 88000 -ac 1 -i - out.flac", "w");
    fwrite(buf, 2, N, pipeout);
    pclose(pipeout);
}

void makeAudioBuffer(int16_t *buf, char *input, long int filelen)
{
    long int n = 0; // buffer index
    int j = 0;      // bit array index

    int e;
    int16_t *samples[10];
    calcSamples(samples);

    printf("Making audio buffer...");
    while (j < filelen) // loop - input
    {
        e = n + bd;
        // evt lav om til en switch. Men test lige performance før push til main!!!!
        if (input[j] - '0' == 1 && input[j + 1] - '0' == 1 && input[j + 2] - '0' == 1) // 111
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[9][n % bd];
            }
        }
        if (input[j] - '0' == 0 && input[j + 1] - '0' == 1 && input[j + 2] - '0' == 1) // 011
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[8][n % bd];
            }
        }
        if (input[j] - '0' == 1 && input[j + 1] - '0' == 0 && input[j + 2] - '0' == 1) // 101
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[7][n % bd];
            }
        }
        if (input[j] - '0' == 1 && input[j + 1] - '0' == 1 && input[j + 2] - '0' == 0) // 110
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[6][n % bd];
            }
        }
        if (input[j] - '0' == 0 && input[j + 1] - '0' == 0 && input[j + 2] - '0' == 1) // 001
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[5][n % bd];
            }
        }
        if (input[j] - '0' == 1 && input[j + 1] - '0' == 0 && input[j + 2] - '0' == 0) // 100
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[4][n % bd];
            }
        }
        if (input[j] - '0' == 0 && input[j + 1] - '0' == 0 && input[j + 2] - '0' == 0) // 000
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[3][n % bd];
            }
        }
        if (input[j] - '0' == 0 && input[j + 1] - '0' == 1 && input[j + 2] - '0' == 0) // 010
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[2][n % bd];
            }
        }
        else //if something is wrong just skip this character
        {
            j++;
        }
        j += 3;
    }

    printf("Audio buffer finished\n");
    return;
}

void printInfo(long int N, int filelen)
{
    float seconds = N / sf;
    float bps = filelen / seconds;
    printf("Input len: %d\n", filelen);
    printf("N: %ld\n", N);
    printf("Seconds: %.0f\n", seconds);
    printf("bps: %.0f\n", bps);
}

void calcSamples(int16_t **samples)
{
    double p2sf = 2.0 * M_PI / sf;
    for (int i = 1; i <= 9; i++)
    {
        samples[i] = malloc(bd * sizeof(int16_t));
        for (int j = 0; j < bd; j++)
        {
            samples[i][j] = 16383.0 * sin(j * (freq * i) * p2sf);
        }
    }
    return;
}