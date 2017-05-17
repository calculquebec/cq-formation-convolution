//
//  tp2.cpp
//  Exemple de convolution d'image avec lodepng
//
//  Créé par Julien-Charles Lévesque
//  Copyright 2015 Université Laval. Tous droits réservés.
//

#define TILE_WIDTH 2

#include <time.h>
#include "lodepng.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>

#include "Chrono.hpp"
#include "PACC/Tokenizer.hpp"

void checkCUDAError(const char *msg)
{
    cudaError_t err = cudaGetLastError();
    if( cudaSuccess != err)
    {
        fprintf(stderr, "Cuda error: %s: %s.\n", msg, cudaGetErrorString( err) );
        exit(-1);
    }
}

__global__ void convolutionGPU2(int lK, int lWidth, unsigned char *lImage, double *lFilter, unsigned char *outImage)
{
    int fx, fy;
    double lR = 0.;
    double lG = 0.;
    double lB = 0.;
    int x,y;
    int lHalfK = lK/2;
    x = lHalfK + blockDim.x*blockIdx.x + threadIdx.x;
    y = lHalfK + blockDim.y*blockIdx.y + threadIdx.y;
    for (int j = -lHalfK; j <= lHalfK; j++) {
       fy = j + lHalfK;
       for (int i = -lHalfK; i <= lHalfK; i++) {
          fx = i + lHalfK;
          //R[x + i, y + j] = Im[x + i, y + j].R * Filter[i, j]
          lR += double(lImage[(y + j)*lWidth*4 + (x + i)*4    ]) * lFilter[fx + fy*lK];
          lG += double(lImage[(y + j)*lWidth*4 + (x + i)*4 + 1]) * lFilter[fx + fy*lK];
          lB += double(lImage[(y + j)*lWidth*4 + (x + i)*4 + 2]) * lFilter[fx + fy*lK];
       }
    }
    //protection contre la saturation
    if(lR<0.) lR=0.; if(lR>255.) lR=255.;
    if(lG<0.) lG=0.; if(lG>255.) lG=255.;
    if(lB<0.) lB=0.; if(lB>255.) lB=255.;
    //Placer le résultat dans l'image.
    outImage[y*lWidth*4 + x*4] = (unsigned char)lR;
    outImage[y*lWidth*4 + x*4 + 1] = (unsigned char)lG;
    outImage[y*lWidth*4 + x*4 + 2] = (unsigned char)lB;
    outImage[y*lWidth*4 + x*4 + 3] = lImage[y*lWidth*4 + x*4 + 3];
}

using namespace std;

//Aide pour le programme
void usage(char* inName) {
    cout << endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << endl;
    exit(1);
}

//Décoder à partir du disque dans un vecteur de pixels bruts en un seul appel de fonction
void decode(const char* inFilename,  vector<unsigned char>& outImage, unsigned int& outWidth, unsigned int& outHeight)
{
    //Décoder
    unsigned int lError = lodepng::decode(outImage, outWidth, outHeight, inFilename);

    //Montrer l'erreur s'il y en a une.
    if(lError) 
        cout << "Erreur de décodage " << lError << ": " << lodepng_error_text(lError) << endl;

    //Les pixels sont maintenant dans le vecteur outImage, 4 octets par pixel, organisés RGBARGBA...
}

//Encoder à partir de pixels bruts sur le disque en un seul appel de fonction
//L'argument inImage contient inWidth * inHeight pixels RGBA ou inWidth * inHeight * 4 octets
void encode(const char* inFilename, vector<unsigned char>& inImage, unsigned int inWidth, unsigned int inHeight)
{
    //Encoder l'image
    unsigned lError = lodepng::encode(inFilename, inImage, inWidth, inHeight);

    //Montrer l'erreur s'il y en a une.
    if(lError)
        cout << "Erreur d'encodage " << lError << ": "<< lodepng_error_text(lError) << endl;
}

int main(int inArgc, char *inArgv[])
{
    if(inArgc < 3 or inArgc > 4) usage(inArgv[0]);
    string lFilename = inArgv[1];
    string lOutFilename;
    if (inArgc == 4)
        lOutFilename = inArgv[3];
    else
        lOutFilename = "output.png";

    // Lire le noyau.
    ifstream lConfig;
    lConfig.open(inArgv[2]);
    if (!lConfig.is_open()) {
        cerr << "Le fichier noyau fourni (" << inArgv[2] << ") est invalide." << endl;
        exit(1);
    }
    
    PACC::Tokenizer lTok(lConfig);
    lTok.setDelimiters(" \n","");
        
    string lToken;
    lTok.getNextToken(lToken);
    
    int lK = atoi(lToken.c_str());
    int lHalfK = lK/2;
    
    cout << "Taille du noyau: " <<  lK << endl;
    
    //Lecture du filtre
    double* lFilter = new double[lK*lK];
        
    for (int i = 0; i < lK; i++) {
        for (int j = 0; j < lK; j++) {
            lTok.getNextToken(lToken);
            lFilter[i*lK+j] = atof(lToken.c_str());
        }
    }

    //Lecture de l'image
    //Variables à remplir
    unsigned int lWidth, lHeight; 
    vector<unsigned char> lImage;   //Les pixels bruts
    vector<unsigned char> outImage; //pixels de l'image apres le filtre

    // Variables sur GPU
    unsigned char *dev_lImage;
    double  *dev_lFilter;
    unsigned char *dev_outImage;
//    unsigned char *h_lImage;
//    double  *h_lFilter;
//    unsigned char *h_outImage;

    //Appeler lodepng
    decode(lFilename.c_str(), lImage, lWidth, lHeight);
    outImage.resize((int)lWidth*(int)lHeight*4);
    

    clock_t start, end;
     double cpu_time_used;
     
     start = clock();
    // Allocation de memoire GPU
    cudaMalloc((void**)&dev_lImage,sizeof(unsigned char)*lWidth*lHeight*4);
    cudaMalloc((void**)&dev_lFilter,sizeof(double)*lK*lK);
    cudaMalloc((void**)&dev_outImage,sizeof(unsigned char)*lWidth*lHeight*4);

    checkCUDAError("Malloc failed");

    // Copie de data ver GPU
    cudaMemcpy(dev_lImage,&lImage[0],sizeof(unsigned char)*lWidth*lHeight*4,cudaMemcpyHostToDevice);
    cudaMemcpy(dev_lFilter,lFilter,sizeof(double)*lK*lK,cudaMemcpyHostToDevice);
    checkCUDAError("Memcpy failed");
    int numThreadsX = 2;
    int numThreadsY = 15;
    int numBlocksX = ((int)lWidth-lK)/numThreadsX;
    int numBlocksY = ((int)lHeight-lK)/numThreadsY;
    dim3 dimGrid(numBlocksX,numBlocksY);
    dim3 dimBlock(numThreadsX,numThreadsY);

    printf("Lwidth-lK = %d\n",((int)lWidth-lK));
    printf("Lwidth-lK = %d\n",((int)lHeight-lK));
    printf("NumThreadsX= %d\n",numThreadsX);
    printf("NumThreadsY= %d\n",numThreadsY);
    printf("NumBlocksX= %d\n",numBlocksX);
    printf("NumBlocksY= %d\n",numBlocksY);

//    exit(0);
    convolutionGPU2 <<< dimGrid,dimBlock >>> (lK,(int)lWidth,dev_lImage,dev_lFilter,dev_outImage);
    checkCUDAError("convolutionGPU failed");
    
    cudaMemcpy(&outImage[0],dev_outImage,sizeof(unsigned char)*lWidth*lHeight*4,cudaMemcpyDeviceToHost);
    checkCUDAError("Memcpy back to CPU failed");
    end = clock();
     cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Exec time = %f\n",cpu_time_used);
 
    // The following part of the code can be parallelized the same way.
    //copie les bordures de l'image
    for(int x = 0; x < lHalfK; x++)
    {
        for (int y = 0; y < (int)lHeight; y++)
        {
            outImage[y*lWidth*4 + x*4] = lImage[y*lWidth*4 + x*4];
            outImage[y*lWidth*4 + x*4 + 1] = lImage[y*lWidth*4 + x*4 + 1];
            outImage[y*lWidth*4 + x*4 + 2] = lImage[y*lWidth*4 + x*4 + 2];
            outImage[y*lWidth*4 + x*4 + 3] = lImage[y*lWidth*4 + x*4 + 3];
        }
    }
    for(int x = (int)lWidth-lHalfK; x < (int)lWidth; x++)
    {
        for (int y = 0; y < (int)lHeight; y++)
        {
            outImage[y*lWidth*4 + x*4] = lImage[y*lWidth*4 + x*4];
            outImage[y*lWidth*4 + x*4 + 1] = lImage[y*lWidth*4 + x*4 + 1];
            outImage[y*lWidth*4 + x*4 + 2] = lImage[y*lWidth*4 + x*4 + 2];
            outImage[y*lWidth*4 + x*4 + 3] = lImage[y*lWidth*4 + x*4 + 3];
        }
    }
    for(int x = lHalfK; x < (int)lWidth - lHalfK; x++)
    {
        for (int y = 0; y < lHalfK; y++)
        {
            outImage[y*lWidth*4 + x*4] = lImage[y*lWidth*4 + x*4];
            outImage[y*lWidth*4 + x*4 + 1] = lImage[y*lWidth*4 + x*4 + 1];
            outImage[y*lWidth*4 + x*4 + 2] = lImage[y*lWidth*4 + x*4 + 2];
            outImage[y*lWidth*4 + x*4 + 3] = lImage[y*lWidth*4 + x*4 + 3];
        }
    }
    for(int x = lHalfK; x < (int)lWidth - lHalfK; x++)
    {
        for (int y = (int)lHeight - lHalfK; y < (int)lHeight; y++)
        {
            outImage[y*lWidth*4 + x*4] = lImage[y*lWidth*4 + x*4];
            outImage[y*lWidth*4 + x*4 + 1] = lImage[y*lWidth*4 + x*4 + 1];
            outImage[y*lWidth*4 + x*4 + 2] = lImage[y*lWidth*4 + x*4 + 2];
            outImage[y*lWidth*4 + x*4 + 3] = lImage[y*lWidth*4 + x*4 + 3];
        }
    }
    
    //Sauvegarde de l'image dans un fichier sortie
    encode(lOutFilename.c_str(),  outImage, lWidth, lHeight);

    cout << "L'image a été filtrée et enregistrée dans " << lOutFilename << " avec succès!" << endl;

    delete[] lFilter;
    return 0;
}

