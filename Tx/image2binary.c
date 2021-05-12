#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <windows.h>
#include <time.h>

int main(int argc, char *argv[])
{
  unsigned char *imageBytes; //char array for storing image bytes
  int i, imageIndex; 
  FILE *imagePtr, *txtPtr;

  #pragma region OpenFiles
  //Handles opening files

  if(argc > 1) //Checks if image path arguments are given
  {
     imagePtr = fopen(argv[1], "rb");  //argv[1] image filepath/filename
     if(argc > 2)
     {  
        txtPtr = fopen(argv[2], "w");  //argv[2] binary text (filepath/filename)
     }
     else
     {
      //if binary text file path is not given, return 1
      return 1;
     }
  }
  else
  {
    return 1;
  }

  //if one of the files cant be opened, return 1
  if (imagePtr == NULL || txtPtr == NULL)
  {
    fclose(imagePtr); 
    fclose(txtPtr); 
    return 1;
  }  

  #pragma endregion


  #pragma region Convert from image bytes to binary
  /*
    fseek moves the pointer in the file.
    if SEEK_END: moves pointer to end of file 
    if SEEK_SET: moves pointer to start of file 
    fseek(file pointer, offset from position, sets the pointers start position from where to read)
  */
  fseek(imagePtr, 0, SEEK_END);
  long imageFileSize = ftell(imagePtr); //Tells the position of the pointer
  fseek(imagePtr, 0, SEEK_SET);
  
  imageBytes = malloc(sizeof(char) * (imageFileSize + 1));
  if(imageBytes == NULL)
  {
    printf("Cant allocate memory for image bytes\n");
    return 1;  
  }

  //Reads the bytes from imagePtr and puts it into imageBytes
  fread(imageBytes, imageFileSize, 1, imagePtr);

  //loops over all the bytes (0000 0000, 8 bits) in the imageBytes array
  for(imageIndex = 0; imageIndex < imageFileSize; imageIndex++)
  {
    //for each byte, loop through all bits,
    //by using bitwise operators, we can get each single bit
    //and write it to a file. 
    for(i = 0; i <= 7; i++)
    {
      if(imageBytes[imageIndex] & (1 << (7 - i))) //*Explanation in bottom
      {
        fputc('1', txtPtr);
      }
      else{
        fputc('0', txtPtr);
      }
    }
  }

  #pragma endregion


  #pragma region clean up

  //Close files
  fclose(imagePtr);
  fclose(txtPtr);
  
  free(imageBytes);

  #pragma endregion
  return 0;
}

/*
char = 1byte = 8bits (0000 0000)

(char & (1 << (7 - i)))

char value 1  = 0000 0001
char value 85 = 0101 0101

The << (left shift) bit wise operater works by moving the bytes in << direction
left operand << right operand -> 
adding 0's to the left operand, from right to left based on the value of the right operand 

having byte 1 
(1000 0000) and shifting it 7 times results in a byte like this
(0100 0000) shifting it 6 times
(0010 0000) << 5 times
(0001 0000) << it 4 times
(0000 1000) << it 3 times
(0000 0100) << it 2 times
(0000 0010) << it 1 times
(0000 0001) << it 0 times

these 8 different bytes will then be compared to the char value (85)
  1 << 6 0100 0000
& 85     0101 0101
         0100 0000 this value != 0 therefore we can count it as a binary "1"

if fx. 
  1 << 5 0010 0000
& 85     0101 0101
         0000 0000 this values == 0 therefore we can count is as binary "0"
*/