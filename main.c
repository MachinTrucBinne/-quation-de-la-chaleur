/*
Noé Aubin-Cadot.
Codé en novembre 2008.
Adapté pour SDL2 en juin 2017.
Déposé sur GitHub en septembre 2019.
C'est une simulation de l'équation de la chaleur 2D où la hauteur de la surface est la température.
*/
// Compiler avec :
// gcc -o main main.c -framework SDL2 -framework SDL2_image -framework OpenGL -framework GLUT -Wno-deprecated
// ou encore :
// gcc $(sdl2-config --cflags) -Wall -o main  main.c $(sdl2-config --libs) -lSDL2_image -framework OpenGL -framework GLUT -Wno-deprecated

// Pour C :
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

// Pour SDL :
#include <SDL2/SDL.h>
//#include <SDL2_image/SDL_image.h>
#include <SDL2/SDL_image.h>

// Pour OpenGL :
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#include <GLUT/glut.h>

void Dessiner_sin(SDL_Window  *fenetre, double *EQonde);

double angleZ = 170;
double angleY = 0;
double angleX = 0;
double vueZ = 20;
double vueY = 20;
double vueX = 20;
double ROT = 15;
int L=50;
double SINGULARITE=10;

SDL_Window *fenetre;
SDL_GLContext contexte;

int main(int argc, char *argv[])
{
    // Initialisation de SDL
    SDL_Init(SDL_INIT_VIDEO);
    
    // Création d'une fenêtre
    fenetre = SDL_CreateWindow("Equation de la chaleur 2D (representation transversale)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 640, 480, SDL_WINDOW_OPENGL);
    contexte = SDL_GL_CreateContext(fenetre);
    SDL_SetWindowResizable(fenetre,SDL_TRUE);

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluPerspective(70,(double)640/480,1,1000);

    glEnable(GL_DEPTH_TEST);
	glEnable(GL_TEXTURE_2D);
    
    Uint32 last_time = SDL_GetTicks();
    Uint32 current_time,ellapsed_time;
    Uint32 start_time;
	
	int i,j,k;
	int M=L;
	int N=L;
	int TOURNE=0;
	int ONDEgo=0;
	double FORCE=1;
	double E11,E12,E22,E21;
	int inerationPARplot=100;
	double eps=0.05;
	double *EQonde;EQonde=(double *) malloc((M*N)*sizeof(double));
	double *EQonde1;EQonde1=(double *) malloc((M*N)*sizeof(double));
	double *EQonde2;EQonde2=(double *) malloc((M*N)*sizeof(double));
	
	/*
	// ONDE SINUSOIDALE
	for (i=0;i<M;i++)
	{
	for (j=0;j<N;j++)
		{
		EQonde[i*M+j]=sin(2*M_PI*i/(N-1))*sin(2*M_PI*j/(M-1));
		EQonde1[i*M+j]=sin(2*M_PI*i/(N-1))*sin(2*M_PI*j/(M-1));
		EQonde2[i*M+j]=sin(2*M_PI*i/(N-1))*sin(2*M_PI*j/(M-1));
		}
	}
	*/

	// ONDE AVEC UNE SINGULARITÉ
	for (i=0;i<M;i++)
	{
		for (j=0;j<N;j++)
		{
			EQonde[i*M+j]=0;
			EQonde1[i*M+j]=0;
			EQonde2[i*M+j]=0;
		}
	}
	// La singularité au centre :
	EQonde[N*M/2+N/2]=SINGULARITE;

    SDL_Event event;
    while(1)
    {
        start_time = SDL_GetTicks();
        while (SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
					exit(0);
					break;
				case SDL_KEYDOWN:
                switch(event.key.keysym.sym)
				{
                    case SDLK_ESCAPE: // Veut arrêter le jeu
                        exit(0);
						break;
					case SDLK_SPACE: // Stop ou arrête le fait que le plan d'onde tourne
                        {int EVENT=0;
						if (TOURNE)
							{TOURNE=0;EVENT=1;}
						if (!TOURNE && !EVENT)
							{TOURNE=1;}
						break;
						}
					case SDLK_RETURN: // Stop ou arrête le fait que l'onde évolue
                        {int EVENT=0;
						if (ONDEgo)
							{ONDEgo=0;EVENT=1;}
						if (!ONDEgo && !EVENT)
							{ONDEgo=1;}
						break;
						}
					case SDLK_UP:
						vueZ-=3;
						vueY-=3;
						vueX-=3;
						break;
					case SDLK_DOWN:
						vueZ+=3;
						vueY+=3;
						vueX+=3;
						break;
					case SDLK_RIGHT:
						angleZ-=30;
						break;
					case SDLK_LEFT:
						angleZ+=30;
						break;
					case SDLK_o:
						//FORCE--;
						FORCE-=0.1;
						break;
					case SDLK_p:
						//FORCE++;
						FORCE+=0.1;
						break;
					case SDLK_i:
						ROT+=2;
						break;
					case SDLK_k:
						ROT-=2;
						break;
				}
                break;
            }
        }

        current_time = SDL_GetTicks();
        ellapsed_time = current_time - last_time;
        last_time = current_time;
		
		if (TOURNE)
		{
			angleZ += 0.01 * ellapsed_time;
		}
		
        Dessiner_sin(fenetre, EQonde);
		if (ONDEgo)
		{
			for (k=0;k<inerationPARplot;k++)
			{
				for (i=1;i<M-1;i++)
				{
					for (j=1;j<N-1;j++)
					{
						EQonde2[i*M+j]=EQonde1[i*M+j];
						EQonde1[i*M+j]=EQonde[i*M+j];
					}
				}
				for (i=1;i<M-1;i++)
				{
					for (j=1;j<N-1;j++)
					{
						E11=EQonde1[(i+1)*M+j] - EQonde1[i*M+j];
						E12=EQonde1[(i-1)*M+j] - EQonde1[i*M+j];
						E22=EQonde1[i*M+j+1] - EQonde1[i*M+j];
						E21=EQonde1[i*M+j-1] - EQonde1[i*M+j];
						//EQonde[i*M+j]=EQonde1[i*M+j]+eps*(-4*EQonde1[i*M+j]+ EQonde1[(i+1)*M+j]+ EQonde1[(i-1)*M+j]+ EQonde1[i*M+j+1]+ EQonde1[i*M+j-1]); // c'est le cas FORCE=1... LE CAS DU LAPLACIEN STANDARD
						EQonde[i*M+j]=EQonde1[i*M+j]+eps*(
											 (E11) * pow(E11*E11+1,(FORCE-1)/2)
											+(E12) * pow(E12*E12+1,(FORCE-1)/2)
											+(E22) * pow(E22*E22+1,(FORCE-1)/2)
											+(E21) * pow(E21*E21+1,(FORCE-1)/2)
											);
						EQonde[N*M/2+N/2]=SINGULARITE;
					}
				}
			}
		}
        ellapsed_time = SDL_GetTicks() - start_time;
        if (ellapsed_time < 10)
        {
            SDL_Delay(10 - ellapsed_time);
        }
    }
    return 0;
}

void Dessiner_sin(SDL_Window  *fenetre, double *EQonde)
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity( );

	int M=L;
	int N=L;
    gluLookAt(vueX,vueY,vueZ,0,0,ROT,0,0,1);

    glRotated(angleZ,0,0,1);
    glRotated(angleY,0,1,0);
	glRotated(angleX,1,0,0);
	
    glBegin(GL_QUADS);
	int i,j;
	for (i=0;i<M-1;i++)
	{
		for (j=0;j<N-1;j++)
		{
			glColor3ub(255*(fabs(atan(EQonde[i*M+j]))),100*sin(M_PI*i/10)+40-N,255*sin(M_PI*i/10));
			
			glVertex3d(i,j,10*EQonde[i*M+j]);
			glVertex3d(i,j+1,10*EQonde[i*M+j+1]);
			glVertex3d(i+1,j+1,10*EQonde[(i+1)*M+j+1]);
			glVertex3d(i+1,j,10*EQonde[(i+1)*M+j]);
		}
	}
	glEnd();
    glFlush();
    SDL_GL_SwapWindow(fenetre);
}