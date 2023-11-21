/****************************************************************************************/
/*                     Lapin.cpp                    */
/****************************************************************************************/
/*         Affiche a l'ecran un lapin en 3D         */
/****************************************************************************************/

#ifndef TEXTURE_H
#define TEXTURE_H

/* inclusion des fichiers d'en-tete freeglut */

#ifdef __APPLE__
#include <GLUT/glut.h> /* Pour Mac OS X */
#else
#include <GL/glut.h>   /* Pour les autres systemes */
#endif

class Texture
{
    public:
        Texture(char *fichier);
        unsigned char texture[256][256][3];
        void appliquerTexture();
        void desactiverTexture();
};

#endif // TEXTURE_H
