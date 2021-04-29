#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>

void printInfo(long long int N, char *input);
void makeAudioBuffer(int16_t *buf, char *input, long long int filelen, long long int N);
void makeAudio(int16_t *buf, long long int N);
void cleanUp(int mode);
void ccImage(void);

double amp = 32000;  // amplitude
double sf = 88000.0; // sampling frequency
double bd = 400 * 2; // duration of each bit (samples per bit)
double freq = 880.0; // frequency of sine wave

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
    fseek(fp, 0, SEEK_END);            // filepointer to end of file
    long long int filelen = ftell(fp); // read file length
    rewind(fp);                        // fp back to beginning
    char *input = malloc(filelen * sizeof(char));
    if (input == NULL)
    {
        printf("Input memory allocation error");
        return 1;
    }
    fread(input, filelen, 1, fp); // read binary file
    fclose(fp);

    long long int N = strlen(input) * bd; // number of samples
    printInfo(N, input);

    int16_t *buf = malloc(N * sizeof(int16_t)); // buffer
    if (buf == NULL)
    {
        printf("Audio buffer memory allocation error");
        return 1;
    }

    makeAudioBuffer(buf, input, filelen, N);
    makeAudio(buf, N);
    printf("Transmitting...");
    system("out.wav"); // play audio in system standard media player. must open in media player that closed after play
    printf("Transmission finished\n");
    return 0;
}

void cleanUp(int mode) // remove old files
{
    if (mode == 1)
        remove("underwater.png");
    remove("compressed.jpeg");
    remove("imagebin.txt");
    return;
}

void ccImage(void) // compress and convert to bits
{
    system("ffmpeg.exe -hide_banner -loglevel error -i underwater.png -q:v 5 -vf scale=360:-1 compressed.jpeg"); // compress
    system("img2bin.exe compressed.jpeg");                                                                     // convert to bits
    printf("Image converted to bits.\n");
    return;
}

void makeAudio(int16_t *buf, long long int N)
{
    // Pipe the audio data to ffmpeg, which writes it to a wav file
    FILE *pipeout = popen("ffmpeg.exe -hide_banner -loglevel error -y -f s16le -ar 44000 -ac 1 -i - out.wav", "w");
    fwrite(buf, 2, N, pipeout);
    pclose(pipeout);
}

void makeAudioBuffer(int16_t *buf, char *input, long long int filelen, long long int N)
{
    long long int n = 0; // buffer index
    int j = 0;           // bit array index
    
    printf("Making audio buffer...");
    while (j < filelen) // loop - input
    {
        int e = n + bd;
        if (input[j] - '0' == 1)
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                double sample = amp * sin(2 * M_PI * (n / sf * (freq)));
                buf[n] = (int16_t)sample;
            }
            j++;
        }
        else if (input[j] - '0' == 0)
        {
            for (n = n; n < e; n++) // loop - audio buffer
            {
                double sample = amp * sin(2 * M_PI * (n / sf * (freq + 220)));
                buf[n] = (int16_t)sample;
            }
            j++;
        }
        else //if something is wrong just skip this character
        {
            j++;
        }
    }
    printf("Audio buffer finished\n");
    return;
}

void printInfo(long long int N, char *input)
{
    float seconds = N / sf;
    float bps = strlen(input) / seconds;
    printf("Input len: %d\nN: %lld\nseconds: %.0f\nbps: %.0f\n", strlen(input), N, seconds, bps);
}