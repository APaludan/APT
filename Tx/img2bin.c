#include <stdio.h>
#include <conio.h>

//source: https://stackoverflow.com/questions/32527351/conversion-of-image-binary-image-using-c 
int main(int argc, char *argv[])
{
    //clrscr();
    FILE *fptr;
    FILE *txt;
    int c;

    fptr = fopen(argv[1], "rb");
    //fptr = fopen("out.wav", "rb");
    txt = fopen("imagebin.txt", "w");

    if (fptr == NULL)
    {
        printf("NOTHING In FILE");
        fclose(fptr);
        return 1;
    }
    else
    {
        while ((c = fgetc(fptr)) != EOF)
        {
            for (int i = 0; i <= 7; i++)
            {
                if (c & (1 << (7 - i)))
                {
                    fputc('1', txt);
                }
                else
                {
                    fputc('0', txt);
                }
            }
        }
    }

    fclose(fptr);
    fclose(txt);
    return 0;
}