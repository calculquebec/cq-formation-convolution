#!/usr/bin/env python

from mpi4py import MPI  # MPI.Init() implicite
from PIL import Image
import numpy as np
import sys


def charger_noyau(nom_fichier: str):
    """Chargement du noyau à partir du fichier texte de format :
        taille
        valeur_0_0 valeur_0_1 ... valeur_0_taille-1
        ...
        valeur_taille-1_0 ... valeur_taille-1_taille-1

    Retour:
    np.array(float): le noyau de convolution
    """

    with open(nom_fichier) as f:
        taille = int(f.readline().strip())

        assert 3 <= taille <= 255, \
            f'{nom_fichier} - taille de noyau invalide (<3 ou >255).'
        assert (taille % 2) == 1, \
            f'{nom_fichier} - taille de noyau invalide (mod 2 = 0).'

        noyau = np.loadtxt(f)

        assert noyau.shape == (taille, taille), \
            f'{nom_fichier} - il manque des valeurs dans le fichier.'

    return noyau


def prod_conv(image, filtre, comm, val_min: int = 0, val_max: int = 255):
    """Produit de convolution - écrase l'image originale

    Arguments:
        image - numpy.ndarray, dtype=float64, matrice 3D
        filtre - numpy.ndarray, dtype=float64, matrice carrée
        val_min - tronquer les valeurs faibles à val_min (0 par défaut)
        val_max - tronquer les valeurs élevées à val_max (255 par défaut)

    Référence:
    https://fr.wikipedia.org/wiki/Produit_de_convolution
    """

    rank, nranks = comm.Get_rank(), comm.Get_size()

    # Tourner le filtre pour que la convolution soit une corrélation croisée
    # https://en.wikipedia.org/wiki/Cross-correlation
    assert filtre.shape[0] == filtre.shape[1], "Le filtre n'est pas carré."
    filtre = filtre[::-1, ::-1].copy()

    # Calculer les marges autour de l'image
    taille_filtre = filtre.shape[0]
    marge = taille_filtre // 2  # Division entière

    # Créer une image temporaire avec les marges
    hauteur, largeur, canaux = image.shape
    im_temp = np.ndarray(
        (
            marge + hauteur + marge,
            marge + largeur + marge,
            canaux
        ),
        dtype=image.dtype
    )

    # Copier l'image originale en tenant compte des marges
    im_temp[marge:-marge, marge:-marge, :] = image

    # Remplir les marges du haut et du bas avec un effet miroir vertical
    im_temp[0:marge, marge:-marge, :] = image[0:marge, :, :][::-1, :, :]
    im_temp[-marge:, marge:-marge, :] = image[-marge:, :, :][::-1, :, :]

    # Remplir les marges de gauche et de droite avec l'effet miroir horizontal
    im_temp[:, 0:marge, :] = im_temp[:, marge:marge * 2, :][:, ::-1, :]
    im_temp[:, -marge:, :] = im_temp[:, -2*marge:-marge, :][:, ::-1, :]

    debut = rank * hauteur // nranks
    fin = (rank + 1) * hauteur // nranks

    # Prod_conv[i, j] = Sum(Im[marge + i ± marge, marge + j ± marge] * Filtre)
    for i in range(debut, fin):
        for j in range(largeur):
            ii, jj = (marge + i), (marge + j)  # Coordonnées im_temp

            for k in range(canaux):
                image[i, j, k] = np.vdot(
                    im_temp[ii-marge:ii+marge + 1, jj-marge:jj+marge + 1, k],
                    filtre
                )

    # Récupération des données
    if rank == 0:
        for r in range(1, nranks):
            debut = r * hauteur // nranks
            fin = (r + 1) * hauteur // nranks
            image[debut:fin, :, :] = comm.recv(source=r, tag=456)

        image.clip(val_min, val_max, out=image)
    else:
        comm.send(image[debut:fin, :, :], 0, 456)


def main():
    """
    Programme principal
    """

    comm = MPI.COMM_WORLD
    rank = comm.Get_rank()

    if len(sys.argv) < 3:
        if rank == 0:
            sys.exit(f'Utilisation: {sys.argv[0]}' +
                     ' image.png fichier_noyau [resultat.png]')
        else:
            sys.exit(None)

    fichier_image = sys.argv[1]
    fichier_noyau = sys.argv[2]
    fichier_resultat = sys.argv[3] if len(sys.argv) > 3 else 'resultat.png'

    try:
        # Charger l'image originale en RGB
        image = np.array(Image.open(fichier_image))[:, :, :3].astype(float)

        # Charger le noyau de convolution
        noyau = charger_noyau(fichier_noyau)
    except Exception as e:
        if rank == 0:
            sys.exit(f'Erreur: {e}')
        else:
            sys.exit(None)

    # Calcul principal
    if rank == 0:
        print(f"Dimensions de l'image: {image.shape[:2]}")
        print(f"Dimensions du noyau:   {noyau.shape}")
    prod_conv(image, noyau, comm)

    try:
        if rank == 0:
            # Enregistrer l'image résultante
            image_finale = Image.fromarray(image.astype(np.uint8), 'RGB')
            image_finale.save(fichier_resultat)
            print(f"Image enregistrée dans {fichier_resultat}")
    except Exception as e:
        if rank == 0:
            sys.exit(f'Erreur: {e}')
        else:
            sys.exit(None)


if __name__ == '__main__':
    main()
