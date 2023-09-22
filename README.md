# Convolution sur une image

Dans le fichier
[`convolution.cpp`](https://github.com/calculquebec/cq-formation-convolution/blob/main/convolution.cpp),
une image est d’abord chargée en mémoire-vive.
Ensuite, le calcul de convolution a lieu à partir de la
[ligne 104](https://github.com/calculquebec/cq-formation-convolution/blob/main/convolution.cpp#L104).

Essentiellement, chaque nouveau pixel est calculé en fonction des
pixels voisins impliqués par un noyau de convolution de taille
`(2*lHalfK+1) x (2*lHalfK+1)`.
Pour générer les noyaux de convolution :
```
cd noyaux
source source.sh
make
cd -
```

Enfin, le nom d’un fichier d’image et le nom d’un fichier de noyau de
convolution sont donnés en argument lors du lancement du programme.
Par exemple :
```
./convolution exemple.png noyaux/flou_45
```

Un [fichier MD5](https://github.com/calculquebec/cq-formation-convolution/blob/main/solutions/md5/exemple_flou_45.md5)
est disponible pour la validation :
```
md5sum -c solutions/md5/exemple_flou_45.md5
```

---

# Image Convolution

In the file
[`convolution.cpp`](https://github.com/calculquebec/cq-formation-convolution/blob/main/convolution.cpp),
an image is first loaded into memory and then, starting on
[line 104](https://github.com/calculquebec/cq-formation-convolution/blob/main/convolution.cpp#L104),
its convolution is computed.

Essentially, each pixel of the new image is calculated as a function of the
neighbouring pixels in the original image, with the number of neighbouring
pixels used determined by the kernel of size `(2*lHalfK+1) x (2*lHalfK+1)`.
To generate the convolution kernels:
```
cd noyaux
source source.sh
make
cd -
```

Finally, the name of the image file and of the convolution kernel must be
given as arguments of the binary when running the program. For example:
```
./convolution exemple.png noyaux/flou_45
```

An [MD5 file](https://github.com/calculquebec/cq-formation-convolution/blob/main/solutions/md5/exemple_flou_45.md5)
is available for validation:
```
md5sum -c solutions/md5/exemple_flou_45.md5
```
