#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "omp.h"

#define NUM_THREADS 2

int main()
{
    omp_set_num_threads(NUM_THREADS);
    const double startTime = omp_get_wtime();
    
    FILE *image, *outputImage, *lecturas;
    image = fopen("imagenPrueba2.bmp","rb");          //Imagen original a transformar
    outputImage = fopen("imagenPrueba2_blur.bmp","wb");    //Imagen transformada
    int blur = 7;
    int w = (int)blur/2;
    printf("w = %d", w);
    long ancho;
    long alto;
    unsigned char r, g, b;               //Pixel
    unsigned char* ptr;
    unsigned char xx[54];
    int cuenta = 0;
    
    for(int i=0; i<54; i++) {
      xx[i] = fgetc(image);
      fputc(xx[i], outputImage);   //Copia cabecera a nueva imagen
    }

    ancho = (long)xx[20]*65536+(long)xx[19]*256+(long)xx[18];
    alto = (long)xx[24]*65536+(long)xx[23]*256+(long)xx[22];
    
    printf("largo img %li\n",alto);
    printf("ancho img %li\n",ancho);

    ptr = (unsigned char*)malloc(alto*ancho*3* sizeof(unsigned char));
    unsigned char img[alto][ancho];
    unsigned char imgBlur[alto][ancho];

    while(!feof(image)){
      b = fgetc(image);
      g = fgetc(image);
      r = fgetc(image);

      unsigned char pixel = 0.21*r+0.72*g+0.07*b;

      ptr[cuenta] = pixel; //b
      ptr[cuenta+1] = pixel; //g
      ptr[cuenta+2] = pixel; //r
      cuenta++;
    }
    //LLenar matriz bidimensional
    int m = 0;
    for (int i = 0; i < alto; i++){
      for (int j = 0; j < ancho; j++){
        img[i][j] = ptr[m];
        m+=1;
      }
    }

    for (int i=w; i<alto-w; i++)
      for (int j=w; j<ancho-w; j++){
         int sum = 0;
         for (int x=-w; x<=w; x++)
            for (int y=-w; y<=w; y++)
               sum += img[i+x][j+y];
         imgBlur[i][j] = (int) sum/(blur*blur);
       }

    m = 0;
    for (int i = 0; i < alto; i++){
      for (int j = 0; j < ancho; j++){
        ptr[m] = imgBlur[i][j];
        ptr[m+1] = imgBlur[i][j];
        ptr[m+2] = imgBlur[i][j];
        m+=1;
      }
    }

    
    //Grises
    #pragma omp parallel
    {
      #pragma omp for schedule(dynamic)
      for (int i = 0; i < alto*ancho*3; ++i) {
        fputc(ptr[i], outputImage);
        fputc(ptr[i+1], outputImage);
        fputc(ptr[i+2], outputImage);
      }
    }
    
    free(ptr);
    fclose(image);
    fclose(outputImage);
    const double endTime = omp_get_wtime();
    printf("tomo (%lf) segundos\n",(endTime - startTime));
    return 0;
}
