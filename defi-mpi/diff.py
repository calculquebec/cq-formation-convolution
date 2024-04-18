#!/usr/bin/env python

from PIL import Image
import numpy as np
import sys


def main():
    """
    Programme principal
    """

    if len(sys.argv) < 2:
        sys.exit(f'Utilisation: {sys.argv[0]} image1.ext image2.ext')

    fichier_image1 = sys.argv[1]
    fichier_image2 = sys.argv[2]

    try:
        image1 = np.array(Image.open(fichier_image1))[:, :, :3]
        image2 = np.array(Image.open(fichier_image2))[:, :, :3]
    except Exception as e:
        sys.exit(f'Erreur: {e}')

    # Calcul principal
    diff_image = image2 - image1
    print('Somme des différences :', np.vdot(diff_image, diff_image))


if __name__ == '__main__':
    main()
