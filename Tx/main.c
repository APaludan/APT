#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>

void printInfo(long int N, int filelen);
int makeAudioBuffer(int16_t *buf, char *input, long int filelen);
void makeAudio(int16_t *buf, long int N);
void cleanUp(int mode);
void ccImage(void);
int calcSamples(int16_t **samples);

double amp = INT16_MAX - 1; // amplitude
double sf = 44000.0;        // sampling frequency
int bd = 100 * 100;         // duration of each bit (samples per bit)
double freq = 440.0;        // frequency of sine wave

int main(void)
{

    

    int mode = 0; // 0 = No webcam | 1 = webcam
    //cleanUp(mode);
    if (mode == 1)
    {
        system("ffmpeg.exe -hide_banner -loglevel error -f  dshow -y -i \"video=Lenovo EasyCamera\" -frames:v 1 underwater.png"); // capture picture with webcam
        Sleep(1000);                                                                                                              // wait for webcam capture
    }
    //ccImage(); // compress and convert image

    FILE *fp = fopen("imagebin.txt", "rb"); // open binary txt
    if (fp == NULL)
    {
        Sleep(1000); // if file doesnt exist, wait additional 1 sec. maybe pc is slow
        fp = fopen("imagebin.txt", "rb");
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

    long int N = 2 * filelen * bd / 2.95; // number of samples
    printInfo(N, filelen);

    int16_t *buf = malloc(3 * (N + 1) * sizeof(int16_t)); // buffer
    if (buf == NULL)
    {
        printf("Audio buffer memory allocation error");
        return 1;
    }
    clock_t begin = clock();
    if (makeAudioBuffer(buf, input, filelen) == 1)
        return 1;
    free(input);

    clock_t end = clock();
    double time_spend = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time to calculate audio samples: %f\n", time_spend);

    makeAudio(buf, N);
    clock_t end2 = clock();
    time_spend = (double)(end2 - end) / CLOCKS_PER_SEC;
    printf("Time to make audio file with FFmpeg: %f\n", time_spend);

    free(buf);
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
    system("ffmpeg.exe -hide_banner -loglevel error -i underwater.png -q:v 2 -vf scale=40:-1 compressed.jpeg"); // compress
    system("img2bin.exe compressed.jpeg");                                                                      // convert to bits
    printf("Image converted to bits.\n");
    return;
}

void makeAudio(int16_t *buf, long int N)
{
    // Pipe the audio data to ffmpeg, which writes it to a wav file
    FILE *pipeout = popen("ffmpeg.exe -hide_banner -loglevel error -y -f s16le -acodec pcm_s16le -vn -ar 44000 -ac 1 -i - out.flac", "w");
    fwrite(buf, 2, N, pipeout);
    pclose(pipeout);
}

int makeAudioBuffer(int16_t *buf, char *input, long int filelen)
{
    long int n = 0; // buffer index
    int j = 0;      // bit array index

    int e;

    int16_t *samples[11];
    if (calcSamples(samples) == 1) //calculates matrix of samples
    {
        printf("Audio samples memory allocation error");
        return 1;
    }

    e += bd;
    for (n; n < e; n++) // loop - audio buffer
    {
        buf[n] = samples[1][n % bd];
    }
    printf("Making audio buffer...");
    while (j < filelen) // loop - input
    {
        e += bd;
        // evt lav om til en switch. Men test lige performance før push til main!!!!
        if (input[j] - '0' == 1 && input[j + 1] - '0' == 1 && input[j + 2] - '0' == 1) // 111
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[10][n % bd];
            }
        }
        else if (input[j] - '0' == 0 && input[j + 1] - '0' == 1 && input[j + 2] - '0' == 1) // 011
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[9][n % bd];
            }
        }
        else if (input[j] - '0' == 1 && input[j + 1] - '0' == 0 && input[j + 2] - '0' == 1) // 101
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[7][n % bd];
            }
        }
        else if (input[j] - '0' == 1 && input[j + 1] - '0' == 1 && input[j + 2] - '0' == 0) // 110
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[6][n % bd];
            }
        }
        else if (input[j] - '0' == 0 && input[j + 1] - '0' == 0 && input[j + 2] - '0' == 1) // 001
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[5][n % bd];
            }
        }
        else if (input[j] - '0' == 1 && input[j + 1] - '0' == 0 && input[j + 2] - '0' == 0) // 100
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[4][n % bd];
            }
        }
        else if (input[j] - '0' == 0 && input[j + 1] - '0' == 0 && input[j + 2] - '0' == 0) // 000
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[3][n % bd];
            }
        }
        else if (input[j] - '0' == 0 && input[j + 1] - '0' == 1 && input[j + 2] - '0' == 0) // 010
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                buf[n] = samples[2][n % bd];
            }
        }
        else
            j++;

        e += bd;
        for (n; n < e; n++) // loop - audio buffer
        {
            buf[n] = samples[1][n % bd];
        }

        j += 3;
    }

    printf("Audio buffer finished\n");
    return 0;
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

int calcSamples(int16_t **samples)
{
    double p2sf = 2.0 * M_PI / sf;
    for (int i = 1; i <= 10; i++)
    {
        samples[i] = malloc(bd * sizeof(int16_t));
        if (samples[i] == NULL)
            return 1;

        for (int j = 0; j < bd; j++)
        {
            samples[i][j] = 16383.0 * sin(j * (freq * i) * p2sf);
        }
    }
    return 0;
}