#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

//RET VARIABEL NAVNE OG FILSTIER + FILNAVNE -> så det virker med resten af programmet 

int read_binary_data(){
  char *binary_bytes, c;
  int i, index = 0, bt_index = 0,
      binary_table[9] = {128, 64, 32, 16, 8, 4, 2, 1},
      decimal = 0;
  
  FILE *binary_file_ptr = fopen("../io_files/binary_image.txt", "r");
  FILE *output_image_file_ptr = fopen("../io_files/output_image.png", "wb");
  FILE *output_txt_file_ptr = fopen("../io_files/output_txt.txt", "w");
  

 if (binary_file_ptr == NULL || output_image_file_ptr == NULL){
    fclose(binary_file_ptr);
    fclose(output_image_file_ptr);

    return 1;
  }

  fseek(binary_file_ptr, 0, SEEK_END);
  long binary_file_size = ftell(binary_file_ptr); //Tells the position of the pointer
  fseek(binary_file_ptr, 0, SEEK_SET);
  
  binary_bytes = malloc(binary_file_size);

  //fgetc gets one character (char) and sets the pointer to the next character,
  //If EOF (end of file) the while loop breaks
  while((c = fgetc(binary_file_ptr)) != EOF){
    binary_bytes[index] = c;
    index++;
  }

  for(i = 0; i < binary_file_size; i++){
    //Using the "binary_table" integers can be added together to get a decimal
    //the decimal value can then be represented as a char and
    //be put into the new image file
    if(binary_bytes[i] == '1'){
      decimal += binary_table[bt_index];
    }
    bt_index++;
    
    if(bt_index == 8){
      fputc(decimal, output_image_file_ptr);
      //fputc(decimal, output_txt_file_ptr);
      decimal = 0;
      bt_index = 0;
    }  
  }

  fclose(binary_file_ptr);
  fclose(output_image_file_ptr);
  fclose(output_txt_file_ptr);

  return 0;   
}

int main(void){

  read_binary_data();

  return 0;
}