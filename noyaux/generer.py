import math
import numpy as np
import sys


N_MIN, N_MAX = tuple([n * 2 + 1 for n in [1, 127]])


def generer_noyau_1d(N):
    """Génère un noyau de convolution basé sur le Blackman window :

        https://en.wikipedia.org/wiki/Window_function#Blackman_window

    N - un nombre impair de N_MIN à N_MAX

    Retour: un array Numpy à une dimension de longueur N, normalisé
    """

    assert isinstance(N, int), f"l'argument N ({N}) n'est pas de type int."
    assert N_MIN <= N <= N_MAX, \
        f"l'argument N ({N}) n'est pas de {N_MIN} à {N_MAX}."
    assert N % 2 == 1, f"l'argument N ({N}) n'est pas un nombre impair."

    n_s = (np.arange(N) + 0.5) / N

    noyau = 0.42 \
        - 0.50 * np.cos(2 * math.pi * n_s) \
        + 0.08 * np.cos(4 * math.pi * n_s)

    return noyau / noyau.sum()


def main():
    if len(sys.argv) <= 1:
        print(f'Exemple: {sys.argv[0]} N  # N impair de {N_MIN} à {N_MAX}')
        exit(1)

    N = int(sys.argv[1])

    noyau1d = generer_noyau_1d(N)
    noyau2d = (noyau1d.reshape((-1, 1)) @ noyau1d.reshape((1, -1)))

    print(N)
    for i in range(N):
        ligne = ''
        for j in range(N):
            ligne = ligne + f' {noyau2d[i, j]:.6g}'
        print(ligne)


if __name__ == '__main__':
    main()
