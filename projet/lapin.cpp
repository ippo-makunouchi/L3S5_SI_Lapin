/****************************************************************************************/
/*                     lapin.cpp                    */
/****************************************************************************************/
/*         Affiche a l'ecran un lapin en 3D         */
/****************************************************************************************/

/* inclusion des fichiers d'en-tete freeglut */

#ifdef __APPLE__
#include <GLUT/glut.h> /* Pour Mac OS X */
#else
#include <GL/glut.h>   /* Pour les autres systemes */
#endif
#include <math.h>
#include <cstdio>
#include <cstdlib>
#include <GL/freeglut.h>
#include <jpeglib.h>
#include <jerror.h>

//classe texture (les méthodes sont à la fin du code)
class Texture{
    public:
        Texture();
        Texture(char *fichier);
        void loadJpegImage(char *f);
        unsigned char texture[256][256][3];
        void activer();
        void desactiver();
};

char presse;
int anglex,angley,x,y,xold,yold;

const double PI = 3.14159265358979323846; //valeur "exacte" de pi

//lumières
int lumiere = 3;

//texture
Texture textures[3]; //tableau pour stocker nos textures

//dimensions buste
float largeurBuste = 4.0;
float longueurBuste = 6.0;
float hauteurBuste = 5.0;

//dimensions tête
float largeurTete = 3.5;
float longueurTete = 3.0;

//dimensions oreilles
float longueurOreille = 4.0;
float oreillesAngle = 0; //pour l'animation
float angleMax_Oreilles = 5;
float angleMin_Oreilles = -5;
float animSpeed_Oreilles = 0.3;

//dimensions jambes arrières
float largeurJambes = 3;

//dimensions queue
float animSpeed_Queue = 0.1;
float queue_posX = 0;
bool animationQueueActive = false;

//distance initiale de la caméra
float camDistance = 30.0;

/* Prototype des fonctions */
void affichage();
void clavier(unsigned char touche,int x,int y);
void clavierFleche(int touche, int x, int y);
void reshape(int x,int y);
void mouse(int bouton,int etat,int x,int y);
void mousemotion(int x,int y);
void anim();//animation automatique + animation avec l'appui sur la touche c
void init();

//toutes les fonctions pour dessiner le lapin
void lapin(); //méthode principale
void busteLapin();
void teteLapin();
void museau();
void moustache();
void joues();
void yeux();
void oreilles();
void pattesAvant();
void jambes();
void queueSphere(double r, int nm, int np);//primitive à partir de sa représentation paramétrique

void lumieres();

int main(int argc,char **argv)
{
  /* initialisation de glut et creation
     de la fenetre */
  glutInit(&argc,argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

  glutInitWindowPosition(200,100);
  glutInitWindowSize(500,500);

  glutCreateWindow("projet");

  /* Initialisation d'OpenGL */
  init();

  /* enregistrement des fonctions de rappel */
  glutDisplayFunc(affichage);
  glutKeyboardFunc(clavier);
  glutSpecialFunc(clavierFleche);
  glutReshapeFunc(reshape);
  glutMouseFunc(mouse);
  glutMotionFunc(mousemotion);
  glutIdleFunc(anim);

  /* Entree dans la boucle principale glut */
  glutMainLoop();
  return 0;
}

//Initialiser opengl
void init()
{
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glColor3f(1.0, 1.0, 1.0);
    glPointSize(2.0);

    glEnable(GL_DEPTH_TEST);

    glEnable(GL_NORMALIZE);
    glEnable(GL_LIGHTING);
    glEnable(GL_COLOR_MATERIAL);

    GLfloat ambient[] = {0.2, 0.2, 0.1, 1.0};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);

    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);

    GLfloat coul_diffuse[] = {1.0, 1.0, 0.0, 1.0};
    GLfloat dir_spot[] = { 0.0, 10.0, 0.0};

    glLightfv(GL_LIGHT1, GL_DIFFUSE, coul_diffuse);
    glLightfv(GL_LIGHT1, GL_SPOT_DIRECTION, dir_spot);

    textures[0] = Texture("./ventre_lapin.jpg");
    textures[1] = Texture("./oeil_lapin.jpg");
    textures[2] = Texture("./queue_lapin.jpg");

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_ADD);
}

void affichage()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glShadeModel(GL_SMOOTH);
    glLoadIdentity();

    glTranslatef(0.0, 0.0, -camDistance);

      glRotatef(angley,1.0,0.0,0.0);
      glRotatef(anglex,0.0,1.0,0.0);

    lumieres();

      lapin();

    // Repère
    glBegin(GL_LINES);
    glColor3f(1.0, 0.0, 0.0); // axe x en rouge
    glVertex3f(0, 0, 0.0);
    glVertex3f(1, 0, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 1.0, 0.0); // axe des y en vert
    glVertex3f(0, 0, 0.0);
    glVertex3f(0, 1, 0.0);
    glEnd();

    glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 1.0); // axe des z en bleu
    glVertex3f(0, 0, 0.0);
    glVertex3f(0, 0, 1.0);
    glEnd();

    glutSwapBuffers();
}

void reshape(int w, int h)
{
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // Réinitialiser la matrice de modèle-vue
    gluPerspective(60, (float)w / h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void mouse(int button, int state,int x,int y)
{
  /* si on appuie sur le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
  {
    presse = 1; /* le booleen presse passe a 1 (vrai) */
    xold = x; /* on sauvegarde la position de la souris */
    yold=y;
  }
  /* si on relache le bouton gauche */
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
    presse=0; /* le booleen presse passe a 0 (faux) */
}

void mousemotion(int x,int y)
{
    if (presse) /* si le bouton gauche est presse */
    {
      /* on modifie les angles de rotation de l'objet
	 en fonction de la position actuelle de la souris et de la derniere
	 position sauvegardee */
      anglex=anglex+(x-xold);
      angley=angley+(y-yold);
      glutPostRedisplay(); /* on demande un rafraichissement de l'affichage */
    }

    xold=x; /* sauvegarde des valeurs courante de le position de la souris */
    yold=y;
}

void clavier(unsigned char touche,int x,int y)
{
  switch (touche)
    {
    case 'p': /* affichage du carre plein */
      glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
      glutPostRedisplay();
      break;
    case 'f': /* affichage en mode fil de fer */
      glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
      glutPostRedisplay();
      break;
    case 's' : /* Affichage en mode sommets seuls */
      glPolygonMode(GL_FRONT_AND_BACK,GL_POINT);
      glutPostRedisplay();
      break;
    case 'd':
      glEnable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'D':
      glDisable(GL_DEPTH_TEST);
      glutPostRedisplay();
      break;
    case 'a'://Les faces à l'envers s'affichent en fil de fer
      glPolygonMode(GL_FRONT,GL_FILL);
      glPolygonMode(GL_FRONT,GL_LINE);
      glutPostRedisplay();
    break;
    case 'q' : /*la touche 'q' permet de quitter le programme */
      exit(0);

    case 'Z':
        camDistance -= 1.0; // Zoom arrière (dézoom)
        if (camDistance < 8.0)
            camDistance = 8.0;
        glutPostRedisplay();
        break;

    case 'z':
        camDistance += 1.0; // Zoom avant
        if (camDistance > 30.0)
            camDistance = 30.0;
        glutPostRedisplay();
        break;

    //caresser le lapin (animation)
    case 'c':
        animationQueueActive = !animationQueueActive;
        if (animationQueueActive) {
            //printf("AnimAnimation de la queue activée.\n");
        } else {
            //printf("Animation de la queue désactivée.\n");
        }
        break;

    //lumières
    case 'l' :
        lumiere++;
            if(lumiere > 3) lumiere = 0;
        break;
    }
}

//Gère les événements des touches fléchées pour la rotation de la caméra.
void clavierFleche(int touche, int x, int y)
{
    switch (touche)
    {
        case GLUT_KEY_LEFT:
            //Rotation de la caméra vers la gauche
            anglex -= 2;
            break;
        case GLUT_KEY_RIGHT:
            //Rotation de la caméra vers la droite
            anglex += 2;
            break;
        case GLUT_KEY_DOWN:
            //Rotation de la caméra vers le haut
            angley += 2;
            break;
        case GLUT_KEY_UP:
            //Rotation de la caméra vers le bas
            angley -= 2;
            break;
    }

    glutPostRedisplay(); //Met à jour l'affichage après chaque cas
}

void lapin()
{
    glPushMatrix();
        busteLapin();
        teteLapin();
        museau();
        yeux();
        oreilles();
        pattesAvant();
        jambes();

        glPushMatrix();
            glTranslatef(queue_posX, largeurBuste - 1, -longueurBuste / 2);
            queueSphere(1, 50, 50);
        glPopMatrix();

    glPopMatrix();
}

void busteLapin()
{
    glPushMatrix();
        textures[0].activer();
        glBegin(GL_POLYGON);
            glColor3f(0.00, 0.00, 0.00);
            glTexCoord2f(0.0, 0.0); glVertex3f(-largeurBuste/2, hauteurBuste/2, longueurBuste/2);
            glTexCoord2f(0.0, 1.0); glVertex3f(-largeurBuste/2, -hauteurBuste/2, longueurBuste/2);
            glTexCoord2f(1.0, 1.0); glVertex3f(largeurBuste/2, -hauteurBuste/2, longueurBuste/2);
            glTexCoord2f(1.0, 0.0); glVertex3f(largeurBuste/2, hauteurBuste/2, longueurBuste/2);
        glEnd();
        textures[0].desactiver();

        glPushMatrix();
            glColor3f(0.82, 0.7, 0.5);
            glScalef(largeurBuste, hauteurBuste, longueurBuste);
            glutSolidCube(1);
        glPopMatrix();

    glPopMatrix();
}

void yeux()
{
    glPushMatrix();
        glTranslatef((largeurTete/3.5), ((hauteurBuste/2)+(longueurTete/2)+0.6), (longueurBuste/2)+longueurTete-0.7);
        textures[1].activer();
        glBegin(GL_POLYGON);
            glColor3f(0.00, 0.00, 0.00);
            glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, 0.5, 0.5);
            glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, -0.5, 0.5);
            glTexCoord2f(1.0, 1.0); glVertex3f(0.5, -0.5, 0.5);
            glTexCoord2f(1.0, 0.0); glVertex3f(0.5, 0.5, 0.5);
        glEnd();
        textures[1].desactiver();
    glPopMatrix();

    glPushMatrix();
        glTranslatef(-(largeurTete/3.5), ((hauteurBuste/2)+(longueurTete/2)+0.6), (longueurBuste/2)+longueurTete-0.7);
        textures[1].activer();
        glBegin(GL_POLYGON);
            glColor3f(0.00, 0.00, 0.00);
            glTexCoord2f(0.0, 0.0); glVertex3f(-0.5, 0.5, 0.5);
            glTexCoord2f(0.0, 1.0); glVertex3f(-0.5, -0.5, 0.5);
            glTexCoord2f(1.0, 1.0); glVertex3f(0.5, -0.5, 0.5);
            glTexCoord2f(1.0, 0.0); glVertex3f(0.5, 0.5, 0.5);
        glEnd();
        textures[1].desactiver();
    glPopMatrix();
}

void teteLapin()
{
    glPushMatrix();
        glTranslatef(0.0, largeurBuste, (longueurBuste/2)+1);
        glColor3f(0.82, 0.7, 0.5);
        glScalef(largeurTete, longueurTete, largeurTete);
        glutSolidCube(1);
    glPopMatrix();
}

void museau()
{
    glPushMatrix();
    glTranslatef(0.0, (hauteurBuste/2)+0.75, ((longueurBuste/2)+(longueurTete)));
    glColor3f(0.9, 0.85, 0.79);
    glScalef(2, 1.5, 0.5);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.0, ((hauteurBuste/2)+1), ((longueurBuste/2)+(longueurTete)+0.3));
    glColor3f(1.0, 0.7, 0.8);
    glScalef(1.0, 1.0, 0.1);
    glutSolidCube(1);
    glPopMatrix();

    //dent
    glPushMatrix();
    glTranslatef(0.0, ((hauteurBuste/2)), ((longueurBuste/2)+(longueurTete)+0.3));
    glColor3f(1.0, 1.0, 1.0);
    glScalef(0.75, 0.75, 0.1);
    glutSolidCube(1);
    glPopMatrix();

    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(0.0, hauteurBuste/2, (longueurBuste/2+longueurTete)+0.4);
    glRotatef(90, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(-0.375, 0.0);
    glVertex2f(0.375, 0.0);
    glEnd();
    glPopMatrix();

    moustache();
    joues();
}

void moustache()
{
    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(1, hauteurBuste/2+longueurTete/2-0.5, (longueurBuste/2+longueurTete)+0.3);
    glBegin(GL_LINES);
    glVertex2f(-0.5, 0.0);
    glVertex2f(0.5, 0.0);
    glEnd();
    glPopMatrix();

    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(0.9, hauteurBuste/2+longueurTete/2, (longueurBuste/2+longueurTete)+0.3);
    glRotatef(30, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(-0.5, 0.0);
    glVertex2f(0.5, 0.0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(0.9, hauteurBuste/2+longueurTete/2-1, (longueurBuste/2+longueurTete)+0.3);
    glRotatef(-30, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(-0.5, 0.0);
    glVertex2f(0.5, 0.0);
    glEnd();
    glPopMatrix();

    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(-1, hauteurBuste/2+longueurTete/2-0.5, (longueurBuste/2+longueurTete)+0.3);
    glBegin(GL_LINES);
    glVertex2f(-0.5, 0.0);
    glVertex2f(0.5, 0.0);
    glEnd();
    glPopMatrix();

    glColor3f(0.0, 0.0, 0.0);
    glPushMatrix();
    glTranslatef(-0.9, hauteurBuste/2+longueurTete/2, (longueurBuste/2+longueurTete)+0.3);
    glRotatef(150, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(-0.5, 0.0);
    glVertex2f(0.5, 0.0);
    glEnd();
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-0.9, hauteurBuste/2+longueurTete/2-1, (longueurBuste/2+longueurTete)+0.3);
    glRotatef(-150, 0.0, 0.0, 1.0);
    glBegin(GL_LINES);
    glVertex2f(-0.5, 0.0);
    glVertex2f(0.5, 0.0);
    glEnd();
    glPopMatrix();
}

void joues()
{
    glColor3f(0.82, 0.7, 0.5);
    glPushMatrix();
    glTranslatef((largeurBuste/2), ((hauteurBuste/2)+0.75), (longueurBuste/2)+1);
    glScalef(0.5, 1.5, 3.0);
    glutSolidCube(1);
    glPopMatrix();

    glPushMatrix();
    glTranslatef(-(largeurBuste/2), ((hauteurBuste/2)+0.75), (longueurBuste/2)+1);
    glScalef(0.5, 1.5, 3.0);
    glutSolidCube(1);
    glPopMatrix();
}

void oreilles()
{
    //Oreille droite
    glPushMatrix();

        glRotatef(oreillesAngle, 0, 1, 1); //Rotation sur l'axe Z (pour l'animation automatique)

        //Partie beige de l'oreille droite
        glPushMatrix();
            glColor3f(0.82, 0.7, 0.5);
            glTranslatef(largeurTete/4, ((hauteurBuste/2)+longueurTete+longueurOreille/2), (longueurBuste/2)+longueurTete/2);
            glScalef(1, longueurOreille, 1);
            glutSolidCube(1);
        glPopMatrix();

        //Partie rose de l'oreille droite
        glPushMatrix();
            glColor3f(1.0, 0.7, 0.8);
            glTranslatef(largeurTete/4, ((hauteurBuste/2)+longueurTete+longueurOreille/2), (longueurBuste/2)+longueurTete/2+0.6);
            glScalef(0.5, longueurOreille-1, 0.1);
            glutSolidCube(1);
        glPopMatrix();

    glPopMatrix();

    //Oreille gauche
    glPushMatrix();

        glRotatef(-oreillesAngle, 0, 1, 1); //Même rotation que l'oreille droite mais de l'autre sens

        //Partie beige de l'oreille gauche
        glPushMatrix();
            glColor3f(0.82, 0.7, 0.5);
            glTranslatef(-largeurTete/4, ((hauteurBuste/2)+longueurTete+longueurOreille/2), (longueurBuste/2)+longueurTete/2);
            glScalef(1, longueurOreille, 1);
            glutSolidCube(1);
        glPopMatrix();

        //Partie rose de l'oreille gauche
        glPushMatrix();
            glColor3f(1.0, 0.7, 0.8);
            glTranslatef(-largeurTete/4, ((hauteurBuste/2)+longueurTete+longueurOreille/2), (longueurBuste/2)+longueurTete/2+0.6);
            glScalef(0.5, longueurOreille-1, 0.1);
            glutSolidCube(1);
        glPopMatrix();

    glPopMatrix();
}

void pattesAvant()
{
    glColor3f(0.9, 0.8, 0.6);
    glPushMatrix();
    glTranslatef(largeurBuste/2-1, -(largeurBuste/2)-1.5, longueurBuste/2-1.5);
    glScalef(1.5, 2, 2);
    glutSolidCube(1);
    glPopMatrix();

    glColor3f(0.9, 0.8, 0.6);
    glPushMatrix();
    glTranslatef(-1, -(largeurBuste/2)-1.5, longueurBuste/2-1.5);
    glScalef(1.5, 2, 2);
    glutSolidCube(1);
    glPopMatrix();
}

void jambes()
{
    glColor3f(0.9, 0.8, 0.6);

    glPushMatrix();
        glTranslatef(0, hauteurBuste/2-3.5, -longueurBuste/2+1);

        //D
        glPushMatrix();
        //cuisse du lapin
        glTranslatef(largeurBuste/2+0.5, 0, 0);
        glRotatef(150, 1, 0, 0);
        glPushMatrix();
            glScalef(1, largeurJambes, 4);
            glutSolidCube(1);
        glPopMatrix();

        //pied
        glPushMatrix();
            glTranslatef(largeurBuste/2+(0.5), -largeurJambes+0.2, 0.5);
            glScalef(1,1,3);
            glutSolidCube(1);
        glPopMatrix();

        //G
        glPushMatrix();
        //cuisse du lapin
        glTranslatef(-(largeurBuste/2+0.5), 0, 0);
        glRotatef(150, 1, 0, 0);
        glPushMatrix();
            glScalef(1, largeurJambes, 4);
            glutSolidCube(1);
        glPopMatrix();

        //pied
        glPushMatrix();
            glTranslatef(-(largeurBuste/2+(0.5)), -largeurJambes+0.2, 0.5);
            glScalef(1,1,3);
            glutSolidCube(1);
        glPopMatrix();

    glPopMatrix();
}

void queueSphere(double r, int nm, int np)
{
    textures[2].activer();
    glColor3f(0.0, 0.0, 0.0);
    //Ce processus génère une série de polygones pour créer la sphère avec des méridiens et des parallèles.
    for (int i=0; i<nm; i++)
    {
        for (int j = 0; j<np; j++)
        {
            double p = (-PI/2) + (i*PI) / (nm-1); //Calcule la coordonnée p en fonction de i et du nombre de méridiens (nm).
            double t = (2*j*PI)/np; //Calcule la coordonnée t en fonction de j et du nombre de parallèles (np).
            double p2 = (-PI/2)+(((i+1)%nm)*PI)/(nm-1); //Calcule la coordonnée p2 pour le méridien suivant.
            double t2 = (2*((j+1)%np)*PI)/np; //Calcule la coordonnée t2 pour la parallèle suivante.

            glBegin(GL_POLYGON);

            //Ajoute les sommets du polygone en utilisant les coordonnées sphériques pour créer des faces.
            glTexCoord2f(1.0 * i / nm, 0.5 * j / np);
            glVertex3f(r * cos(p) * cos(t), r * cos(p) * sin(t), r * sin(p));

            glTexCoord2f(1.0 * (i + 1) / nm, 0.5 * j / np);
            glVertex3f(r * cos(p2) * cos(t), r * cos(p2) * sin(t), r * sin(p2));

            glTexCoord2f(1.0 * (i + 1) / nm, 0.5 * (j + 1) / np);
            glVertex3f(r * cos(p2) * cos(t2), r * cos(p2) * sin(t2), r * sin(p2));

            glTexCoord2f(1.0 * i / nm, 0.5 * (j + 1) / np);
            glVertex3f(r * cos(p) * cos(t2), r * cos(p) * sin(t2), r * sin(p));

            glEnd();
        }
    }
    textures[2].desactiver();
}

void anim()
{
    oreillesAngle += animSpeed_Oreilles; //Incrémente l'angle de rotation des oreilles
    if (oreillesAngle >= angleMax_Oreilles || oreillesAngle <= angleMin_Oreilles) //Vérifie si l'angle dépasse les limites
        animSpeed_Oreilles *= -1; //Inverse la direction de l'animation des oreilles

    if (animationQueueActive) //Si l'animation de la queue est activée
    {
        queue_posX += animSpeed_Queue; //Modifie la position de la queue
        if (queue_posX >= 0.3 || queue_posX <= -0.3) //Vérifie les limites de la position de la queue
            animSpeed_Queue *= -1; //Inverse la direction de l'animation de la queue
    }
    glutPostRedisplay(); //rafraîchissement de l'affichage
}

//Fonction pour configurer l'éclairage
void lumieres()
{
    //Position de la lumière ambiante (GL_LIGHT0)
    GLfloat light_position[] = {0.0, 1.0, 1.0, 0.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, light_position);

    //Position de la lumière de type spot (GL_LIGHT1)
    GLfloat l1_pos[] = {1.0, 5.0, -3.0, 1.0};
    glLightfv(GL_LIGHT1, GL_POSITION, l1_pos);

    //Contrôle de l'état des lumières en fonction de la variable lumiere
    if(lumiere == 0){
        //Désactiver la lumière ambiante (GL_LIGHT0) et la lumière de type spot (GL_LIGHT1)
        glDisable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
    }
    else if (lumiere == 1)
    {
        //Activer la lumière ambiante (GL_LIGHT0) et désactiver la lumière de type spot (GL_LIGHT1)
        glEnable(GL_LIGHT0);
        glDisable(GL_LIGHT1);
    }
    else if (lumiere == 2)
    {
        //Désactiver la lumière ambiante (GL_LIGHT0) et activer la lumière de type spot (GL_LIGHT1)
        glDisable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
    }
    else
    {
        //Activer la lumière ambiante (GL_LIGHT0) et la lumière de type spot (GL_LIGHT1)
        glEnable(GL_LIGHT0);
        glEnable(GL_LIGHT1);
    }
}


//classe texture
Texture::Texture()
{

}

Texture::Texture(char *f)
{
    loadJpegImage(f);
}

void Texture::activer()
{
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,256,256,0,GL_RGB,GL_UNSIGNED_BYTE,texture);
    glEnable(GL_TEXTURE_2D);
}

void Texture::desactiver()
{
    glDisable(GL_TEXTURE_2D);
}

void Texture::loadJpegImage(char *fichier)
{
    struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  FILE *file;
  unsigned char *ligne;

  cinfo.err = jpeg_std_error(&jerr);
  jpeg_create_decompress(&cinfo);
#ifdef __WIN32
  if (fopen_s(&file,fichier,"rb") != 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#elif __GNUC__
  if ((file = fopen(fichier,"rb")) == 0)
    {
      fprintf(stderr,"Erreur : impossible d'ouvrir le fichier texture.jpg\n");
      exit(1);
    }
#endif
  jpeg_stdio_src(&cinfo, file);
  jpeg_read_header(&cinfo, TRUE);

    unsigned char image[256 * 256 * 3];

    if(cinfo.image_width != 256 || cinfo.image_height != 256)
    {
        fprintf(stdout,"Erreur : l'image doit etre de dimensions 256x256\n");
        exit(1);
    }
  if (cinfo.jpeg_color_space==JCS_GRAYSCALE) {
    fprintf(stdout,"Erreur : l'image doit etre de type RGB\n");
    exit(1);
  }

  jpeg_start_decompress(&cinfo);
  ligne=image;
  while (cinfo.output_scanline<cinfo.output_height)
    {
      ligne=image+3*256*cinfo.output_scanline;
      jpeg_read_scanlines(&cinfo,&ligne,1);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);

  for(int i = 0; i < 256; i++)
    for(int j = 0; j < 256; j++)
    {
        texture[i][j][0] = image[i*256*3+j*3];
        texture[i][j][1] = image[i*256*3+j*3+1];
        texture[i][j][2] = image[i*256*3+j*3+2];
    }
}
