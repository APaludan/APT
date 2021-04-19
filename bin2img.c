#include <stdio.h>
#include <conio.h>

//The following function converts the ones and zeroes in the text file into a character.
//For example the text file may have the 8 consecutive characters '1', '0', '0', '0', '1', '0', '0', '0'.
//This converts it into the character equivalent of the binary value 10001000
<<<<<<< HEAD

//aloha :-)

char bytefromtext(char* text) 
=======
// hej cj
char bytefromtext(char *text)
>>>>>>> 28afb23f8a6a51e250b6e88bd719e42a38cb6276
{
  char result = 0;
  for (int i = 0; i < 8; i++)
  {
    if (text[i] == '1')
    {
      result |= (1 << (7 - i));
    }
  }
  return result;
}

void main()
{
  //clrscr();
  FILE *pfile;
  FILE *image;
  char buf[8];
  char c;
  int j = 0;

  image = fopen("output.jpg", "wb"); //open an empty .bmp file to
                                     //write characters from the source image file
  pfile = fopen("test1.txt", "r");

  if (pfile == NULL)
    printf("error");
  else
  {
    c = fgetc(pfile);

    while (c != EOF)
    {
      buf[j++] = c;
      if (j == 8)
      {
        fputc(bytefromtext(buf), image);
        j = 0;
      }
      c = fgetc(pfile);
    }

    fclose(pfile);
    fclose(image);
  }

  getch();
}