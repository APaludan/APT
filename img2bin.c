#include<stdio.h>
#include<conio.h>

void main()
{
  //clrscr();
  FILE *fptr;
  FILE *txt;
  int c;

  fptr=fopen("img.bmp","rb");
  txt=fopen("test1.txt","w");

  if(fptr==NULL)
  {
    printf("NOTHING In FILE");
    fclose(fptr);
  }
  else
  {
    printf("success");

    while ((c = fgetc (fptr)) != EOF)
    {
      for(int i=0;i<=7;i++)
      {
        if(c&(1<<(7-i)))
        {
          fputc('1',txt);
        }
        else
        {
          fputc('0',txt);
        }
      }
      // fprintf(txt,"\t");
    }

  }

  fclose(fptr);
  fclose(txt);
  return;
}