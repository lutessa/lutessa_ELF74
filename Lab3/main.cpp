#include "stdint.h"
#include "images.c"
#include <iostream>
using namespace std;
extern "C" uint16_t EightBitHistogram(uint16_t width,uint16_t height, const uint8_t *p_image,uint16_t *p_histogram);

int main()
{
  
  uint16_t p_histogram[256];

  //uint16_t x=EightBitHistogram(WIDTH0,HEIGTH0,p_start_image0,p_histogram);
  uint16_t x=EightBitHistogram(WIDTH1,HEIGTH1,p_start_image1,p_histogram);
  
  
  //Tamanho da Imagem
  cout<<"Tamanho "<<x<<endl;
  
  //Printa as cores e quantidades
  for (uint16_t i=0;i<256;i++){
      cout<<"COR "<<i<<" "<<p_histogram[i]<<endl;
  }
  
  return 0;
}
