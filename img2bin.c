#include <stdio.h>
#include <conio.h>

void img2bin()
{
  //clrscr();
  FILE *fptr;
  FILE *txt;
  int c;

  fptr = fopen("underwater-scene-1.jpg", "rb");
  txt = fopen("test1.txt", "w");

  if (fptr == NULL)
  {
    printf("NOTHING In FILE");
    fclose(fptr);
  }
  else
  {
    printf("success");

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
  return;
}