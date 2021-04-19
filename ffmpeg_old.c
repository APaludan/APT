#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

int main(void)
{
    float amp = 2000.0;         // amplitude. SKAL VÆRE 2000 ELLERS GÅR ALT I STYKKER
    float sf = 176000.0;        // sampling frequency
    float bd = 352 * 10.0;    // duration of each bit. MUST BE MULTIPLE OF 352!!!!!
    float freq = 2000.0;        //frequency of sine wave


    char input[1300000];
    FILE *fp = fopen("test1.txt", "r");
    if (fp == NULL)
    {
        printf("Could not open file");
        return 1;
    }
    fscanf(fp, "%s", input);
    fclose(fp);
    float bps = sf / bd;
    printf("bps: %f", bps);
    //scanf("%s", input);

    int N = strlen(input) * bd; //duration
    printf("N: %d\n", N);

    int16_t *buf = malloc(N * sizeof(int16_t)); //buffer
    int n;                                      //buffer index

    char *bit = input;
    int j = 0; //bit array index

    for (n = 0; n < N;)
    {
        if (bit[j] - '0' == 1)
        {
            int e = n + bd;
            for (n = n; n < e; n++)
            {
                //buf[n] = amp * sin(2.0 * M_PI * (freq) * (n / sf)); //fra wiki sine wave
                buf[n] = amp * sin(2.0 * M_PI * n * freq / sf);
            }
            j++;
        }
        else if (bit[j] - '0' == 0)
        {
            int e = n + bd;
            for (n = n; n < e; n++)
            {
                //buf[n] = amp * sin(2.0 * M_PI * (freq-500) * (n / sf)); // fra wiki sine wave
                buf[n] = amp * sin(2.0 * M_PI * n * (freq - 500) / sf);
            }
            j++;
        }
        else
        {
            continue;
        }
    }
    for (int i = 0; i < 3000; i++)
    {
        if (i % 352 == 0)
        {
            printf("i:%d,%d ", i, buf[i]);
        }
    }
    printf("\n");

    // Pipe the audio data to ffmpeg, which writes it to a wav file
    FILE *pipeout;
    pipeout = popen("ffmpeg -y -f s16le -ar 176000 -ac 1 -i - out.flac", "w");
    fwrite(buf, 2, N, pipeout);
    pclose(pipeout);
    printf("pt2 %d\n", buf[1285]);
    return 1;
}
