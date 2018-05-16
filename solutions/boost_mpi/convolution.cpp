//
//  tp2.cpp
//  Exemple de convolution d'image avec lodepng
//
//  Créé par Julien-Charles Lévesque
//  Copyright 2015 Université Laval. Tous droits réservés.
//


#include "lodepng.h"
#include <iostream>
#include <stdlib.h>
#include <fstream>
#include <mpi.h>

#include "Chrono.hpp"
#include "PACC/Tokenizer.hpp"
#include <boost/mpi.hpp>

using namespace std;
using namespace boost::mpi;

//Aide pour le programme
void usage(char* inName) {
    cout << endl << "Utilisation> " << inName << " fichier_image fichier_noyau [fichier_sortie=output.png]" << endl;
    environment::abort(1);
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
    environment env(inArgc, inArgv);

    if(inArgc < 3 or inArgc > 4) usage(inArgv[0]);
    string lFilename = inArgv[1];
    string lOutFilename;
    if (inArgc == 4)
        lOutFilename = inArgv[3];
    else
        lOutFilename = "output.png";

    communicator world;
    int mpiRank = world.rank();
    int mpiSize = world.size();

    // Lire le noyau.
    ifstream lConfig;
    lConfig.open(inArgv[2]);
    if (!lConfig.is_open()) {
        cerr << "Le fichier noyau fourni (" << inArgv[2] << ") est invalide." << endl;
        environment::abort(1);
    }
    
    PACC::Tokenizer lTok(lConfig);
    lTok.setDelimiters(" \n","");
        
    string lToken;
    lTok.getNextToken(lToken);
    
    int lK = atoi(lToken.c_str());
    int lHalfK = lK/2;

    if (mpiRank == 0) {
        cout << "Taille du noyau: " <<  lK << endl;
    }
    
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
    
    
    //Appeler lodepng
    decode(lFilename.c_str(), lImage, lWidth, lHeight);
    outImage.resize((int)lWidth*(int)lHeight*4);

    // Limites du present processus
    int ymin = lHalfK + (lHeight - 2 * lHalfK) * (mpiRank + 0) / mpiSize;
    int ymax = lHalfK + (lHeight - 2 * lHalfK) * (mpiRank + 1) / mpiSize;
    
    //Variables contenant des indices
    int fy, fx;
    //Variables temporaires pour les canaux de l'image
    double lR, lG, lB;    
    for (int y = ymin; y < ymax; y++)
    {
        for (int x = lHalfK; x < (int)lWidth - lHalfK; x++)
        {
            lR = 0.;
            lG = 0.;
            lB = 0.;
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
    }

    // Copier la bordure gauche du morceau
    for (int y = ymin; y < ymax; y++)
    {
        for(int x = 0; x < lHalfK; x++)
        {
            outImage[y*lWidth*4 + x*4] = lImage[y*lWidth*4 + x*4];
            outImage[y*lWidth*4 + x*4 + 1] = lImage[y*lWidth*4 + x*4 + 1];
            outImage[y*lWidth*4 + x*4 + 2] = lImage[y*lWidth*4 + x*4 + 2];
            outImage[y*lWidth*4 + x*4 + 3] = lImage[y*lWidth*4 + x*4 + 3];
        }
    }

    // Copier la bordure droite du morceau
    for (int y = ymin; y < ymax; y++)
    {
        for(int x = (int)lWidth-lHalfK; x < (int)lWidth; x++)
        {
            outImage[y*lWidth*4 + x*4] = lImage[y*lWidth*4 + x*4];
            outImage[y*lWidth*4 + x*4 + 1] = lImage[y*lWidth*4 + x*4 + 1];
            outImage[y*lWidth*4 + x*4 + 2] = lImage[y*lWidth*4 + x*4 + 2];
            outImage[y*lWidth*4 + x*4 + 3] = lImage[y*lWidth*4 + x*4 + 3];
        }
    }

    if (mpiRank == 0) {
        // Copier toute la bordure du haut de l'image
        for (int y = 0; y < lHalfK; y++)
        {
            for(int x = 0; x < (int)lWidth; x++)
            {
                outImage[y*lWidth*4 + x*4] = lImage[y*lWidth*4 + x*4];
                outImage[y*lWidth*4 + x*4 + 1] = lImage[y*lWidth*4 + x*4 + 1];
                outImage[y*lWidth*4 + x*4 + 2] = lImage[y*lWidth*4 + x*4 + 2];
                outImage[y*lWidth*4 + x*4 + 3] = lImage[y*lWidth*4 + x*4 + 3];
            }
        }

        for (int rank = 1; rank < mpiSize; rank++)
        {
            // Limites du present processus
            ymin = lHalfK + (lHeight - 2 * lHalfK) * (rank + 0) / mpiSize;
            ymax = lHalfK + (lHeight - 2 * lHalfK) * (rank + 1) / mpiSize;
            world.recv(rank, MPI_ANY_TAG, &outImage[ymin * lWidth * 4], (ymax - ymin) * lWidth * 4);
        }

        // Copier toute la bordure du bas de l'image
        for (int y = (int)lHeight - lHalfK; y < (int)lHeight; y++)
        {
            for(int x = 0; x < (int)lWidth; x++)
            {
                outImage[y*lWidth*4 + x*4] = lImage[y*lWidth*4 + x*4];
                outImage[y*lWidth*4 + x*4 + 1] = lImage[y*lWidth*4 + x*4 + 1];
                outImage[y*lWidth*4 + x*4 + 2] = lImage[y*lWidth*4 + x*4 + 2];
                outImage[y*lWidth*4 + x*4 + 3] = lImage[y*lWidth*4 + x*4 + 3];
            }
        }
    }
    else
    {
        world.send(0,12345,&outImage[ymin * lWidth * 4], (ymax - ymin) * lWidth * 4);
    }
    
    //Sauvegarde de l'image dans un fichier sortie
    if (mpiRank == 0) {
        encode(lOutFilename.c_str(),  outImage, lWidth, lHeight);

        cout << "L'image a été filtrée et enregistrée dans " << lOutFilename << " avec succès!" << endl;
    }

    delete[] lFilter;

    return 0;
}

