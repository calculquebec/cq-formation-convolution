#include <cstring>
#include <fstream>
#include <iostream>
#include <png.h>
#include <string>
#include <vector>


/**
 * Enregistrement de 4 octets, un par canal de pixel RGBA
 */
typedef struct {
    png_byte r;  // Rouge
    png_byte g;  // Vert
    png_byte b;  // Bleu
    png_byte a;  // Alpha
} png_rgba;


/**
 * Classe facilitant la lecture-écriture (Le) de fichiers PNG en RGBA
 * https://sourceforge.net/p/libpng/code/ci/master/tree/example.c
 * https://sourceforge.net/p/libpng/code/ci/master/tree/png.h
 */
class LePNG: public std::vector<png_rgba>
{
public:
    LePNG() {
        memset(&entete, 0, sizeof entete);

        entete.format = PNG_FORMAT_RGBA;
        entete.version = PNG_IMAGE_VERSION;
    }

    virtual ~LePNG() {
        png_image_free(&entete);
    }

    /**
     * Modifier les dimensions de l'image
     */
    void redimensionner(png_uint_32 largeur, png_uint_32 hauteur) {
        entete.width = largeur;
        entete.height = hauteur;

        resize(entete.width * entete.height);
    }

    /**
     * Charger une image d'un fichier PNG - 4 canaux (Red, Green, Blue, Alpha)
     */
    void charger(const std::string & nom_fichier) {
        if (!png_image_begin_read_from_file(&entete, nom_fichier.c_str()))
            throw nom_fichier + " - " + entete.message;

        resize(entete.width * entete.height);

        if (!png_image_finish_read(&entete, NULL, data(), 0, NULL))
            throw nom_fichier + " - " + entete.message;
    }

    /**
     * Enregistrer le résultat dans un fichier PNG
     */
    void enregistrer(const std::string & nom_fichier) {
        if (!png_image_write_to_file(
                &entete, nom_fichier.c_str(), 0, data(), 0, NULL)) {
            throw nom_fichier + " - " + entete.message;
        }
    }

    inline png_uint_32 largeur() const { return entete.width; }
    inline png_uint_32 hauteur() const { return entete.height; }

private:
    png_image entete;
};


/**
 * Classe facilitant la lecture d'un noyau de convolution (filtre) carré
 */
class Noyau: public std::vector<double>
{
public:
    Noyau(): taille(0) {}

    /**
     * Chargement du noyau à partir du fichier texte de format :
     *
     * taille
     * valeur_0_0 valeur_0_1 ... valeur_0_taille-1
     * ...
     * valeur_taille-1_0 ... valeur_taille-1_taille-1
     */
    void charger(const std::string & nom_fichier) {
        std::ifstream ifs;
        ifs.open(nom_fichier.c_str());

        if (!ifs.is_open())
            throw nom_fichier + " - n'a pas pu être ouvert.";

        ifs >> taille;

        if ((taille < 3) || (255 < taille))
            throw nom_fichier + " - taille de noyau invalide (<3 ou >255).";
        if ((taille & 1) == 0)
            throw nom_fichier + " - taille de noyau invalide (mod 2 = 0).";

        resize(taille * taille);
        auto itValeur = begin();

        do {
            ifs >> *itValeur++;
        } while (ifs.good() && (itValeur != end()));

        if (ifs.fail() || (itValeur != end()))
            throw nom_fichier + " - il manque des valeurs dans le fichier.";

        ifs.close();
    }

    inline size_type largeur() const { return taille; }

private:
    size_type taille;
};


/**
 * Produit de convolution - écrase l'image originale
 * https://fr.wikipedia.org/wiki/Produit_de_convolution
 */
static void prod_conv(LePNG & rgba, const Noyau & filtre)
{
    // Dimensions originales
    const int largeur = rgba.largeur();
    const int hauteur = rgba.hauteur();
    std::cout << "Dimensions de l'image originale : " << largeur
        << " x " << hauteur << std::endl;

    // Calculer la marge autour de l'image
    const int taille_filtre = filtre.largeur();
    const int marge = (int)taille_filtre / 2;  // Type int (signé) nécessaire
    std::cout << "Taille du filtre : " << taille_filtre << std::endl;
    std::cout << "  Marge réelle :  " << marge << std::endl;

    const int marge_gauche = (marge + 15) & ~15;  // Alignée sur 64o=16*4o
    const int stride = marge_gauche + ((largeur + marge + 15) & ~15);
    std::cout << "  Marge alignée : " << marge_gauche << std::endl;
    std::cout << "  Largeur totale alignée : " << stride
        << " (= " << marge_gauche << " + " << largeur << " + "
        << stride - (marge_gauche + largeur) << ")" << std::endl;

    LePNG im_temp;
    im_temp.redimensionner(stride, marge + hauteur + marge);

    // Remplir les marges du haut et du bas
    for (int i = 0; i < marge; ++i) {
        for (int j = 0; j < largeur; ++j) {
            im_temp[(marge - 1 - i) * stride + (marge_gauche + j)] =
                rgba[i * largeur + j];
            im_temp[(marge + hauteur + i) * stride + (marge_gauche + j)] =
                rgba[(hauteur - 1 - i) * largeur + j];
        }
    }

    // Copier l'image originale
    for (int i = 0; i < hauteur; ++i) {
        for (int j = 0; j < largeur; ++j) {
            im_temp[(marge + i) * stride + (marge_gauche + j)] =
                rgba[i * largeur + j];
        }
    }

    // Remplir les marges de gauche et de droite
    for (png_uint_32 i = 0; i < im_temp.hauteur(); ++i) {
        for (int j = 0; j < marge; ++j) {
            im_temp[i * stride + (marge_gauche - 1 - j)] =
                im_temp[i * stride + (marge_gauche + j)];
            im_temp[i * stride + (marge_gauche + largeur + j)] =
                im_temp[i * stride + (marge_gauche + largeur - 1 - j)];
        }
    }

    std::cout << "Filtrage en cours ..." << std::endl;

    // Prod_conv[i, j] = Sum_ii(Sum_jj(Im[i+ii, j+jj] * Filtre[-ii, -jj]))
    for (int i = 0; i < hauteur; ++i) {
        for (int j = 0; j < largeur; ++j) {
            double r = 0.;
            double g = 0.;
            double b = 0.;

            for (int ii = -marge; ii <= marge; ++ii) {
                for (int jj = -marge; jj <= marge; ++jj) {
                    const LePNG::size_type index_im =
                        (marge + i + ii) * stride + (marge_gauche + j + jj);
                    const Noyau::size_type index_filt =
                        (marge - ii) * taille_filtre + (marge - jj);

                    r += (double)im_temp[index_im].r * filtre[index_filt];
                    g += (double)im_temp[index_im].g * filtre[index_filt];
                    b += (double)im_temp[index_im].b * filtre[index_filt];
                }
            }

            // Protection contre la saturation
            if (r < 0.) { r = 0.; } if (r > 255.) { r = 255.; }
            if (g < 0.) { g = 0.; } if (g > 255.) { g = 255.; }
            if (b < 0.) { b = 0.; } if (b > 255.) { b = 255.; }

            // Placer le résultat dans l'image originale
            rgba[i * largeur + j].r = r;
            rgba[i * largeur + j].g = g;
            rgba[i * largeur + j].b = b;
        }
    }
}


/**
 * Programme principal
 */
int main(int argc, char *argv[])
{
    LePNG png;
    Noyau noyau;

    if (argc < 3) {
        std::cerr << "Utilisation: " << argv[0]
            << " image.png fichier_noyau [resultat.png]" << std::endl;
        return 1;
    }

    try {
        // Charger l'image originale
        std::string nom_fichier_png(argv[1]);
        png.charger(nom_fichier_png);
    }
    catch (const std::string message) {
        std::cerr << "Erreur: " << message << std::endl;
        return 2;
    }

    try {
        // Charger le noyau de convolution
        std::string nom_fichier_noyau(argv[2]);
        noyau.charger(nom_fichier_noyau);
    }
    catch (const std::string message) {
        std::cerr << "Erreur: " << message << std::endl;
        return 3;
    }

    // Calcul principal
    prod_conv(png, noyau);

    try {
        // Enregistrer le résultat
        std::string fichier_resultat = (argc >= 4) ? argv[3] : "resultat.png";
        png.enregistrer(fichier_resultat);

        std::cout << "L'image a été filtrée et enregistrée dans "
            << fichier_resultat << " avec succès!" << std::endl;
    }
    catch (const std::string message) {
        std::cerr << "Erreur: " << message << std::endl;
        return 4;
    }

    return 0;
}

