#include <SDL2\SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>


Uint8 champs0u[307200];
float champs1f[307200];
float champs2f[307200];
Uint8 champs3u[307200]; //champs créateur

/******************************************************************************/
/**                             structure                                    **/
/******************************************************************************/

int SDL_Init_erreur(int erreur) ;

int SDL_Window_NULL(SDL_Window *window ) ;
int SDL_Renderer_NULL(SDL_Renderer *renderer ) ;
int SDL_Texture_NULL(SDL_Texture *texture);
int SDL_Surface_NULL(SDL_Surface *surface);

int SDL_CreateTextureFromSurface_erreur(SDL_Texture *texture);

/******************************************************************************/
/**                             Programme                                    **/
/******************************************************************************/

//https://zestedesavoir.com/tutoriels/1014/utiliser-la-sdl-en-langage-c/les-textures-et-les-images/
int main(int argc, char *argv[])
{
/**                                Initailisation                            **/
    fprintf(stderr, "Initialisation SDL :\n");
    if (SDL_Init_erreur(SDL_Init(SDL_INIT_VIDEO|SDL_INIT_EVENTS))) goto Quit;

    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;
    SDL_Texture *texture = NULL;
    SDL_Event event;

    window = SDL_CreateWindow("Diffusion", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_SHOWN);
    if (SDL_Window_NULL(window)) goto Quit;

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (SDL_Renderer_NULL(renderer)) goto Quit;

    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,  640, 480);
    if (SDL_Texture_NULL(texture)) goto Quit;

/**                                   creation                               **/
    fprintf(stderr, "initilisation des variables :");


    SDL_Rect rect = {50, 50, 100, 100};
    SDL_Rect dst  = {0 ,  0, 640, 480}; //correspond à l’endroit du renderer ou placer la texture
    /////
    SDL_RenderPresent(renderer);
    SDL_Delay(1000);
    Uint8 r;
    Uint8 g;
    Uint8 b;
    Uint8 t = 0;
    clock_t time_req;
    char cont = 1; /* Détermine si on continue la boucle principale */
    int i_f ;
    int j_f ;
    int i;
    int j;
    int boutonsourisLeft = 0 ;
    int boutonsourisRight = 0 ;
    float l =1./8. ;// constante de diffusion normalisé
    float sumchamp ;
    float sumchamp2 =0;
    // initiclisation du champs
    for (i = 0 ; i < 640 ; i++){
        for( j = 0 ; j < 480 ; j++){
            //champs1f[i*480+j]=(float) ((i+j)/255)%1 ;
            i_f = (float) i ;
            j_f = (float) j ;
            champs1f[i*480+j]= SDL_expf(-((i_f-320.)*(i_f-320.)+(j_f-240.)*(j_f-240.))/(10.*10.)) ;
            champs3u[i*480+j]=0 ;
            //fprintf(stderr,"(i=%d,j=%d) = %d, %f\n",i,j,r,champs1f[i*480+j]);
            /*if (i <320+10 && i >320-10 && j <240+10 && j >240-10){
                champs1f[i*480+j] = 1 ;
            }
            else  {
                champs1f[i*480+j] = 0 ;
            }*/
        }
    }
    //////////
    fprintf(stderr, "On rentre dans la boucle : \n");

/**                                    boucle                                **/
    while(t<1000){
        //(stderr, "Diffusion du champs : \n");

        sumchamp =0 ;
        //di = ((t%2))*2-1 ;
        //dj = ((t/2)%2)*2-1 ;
        // diffusion
        time_req = clock();
        for (i = 0 ; i < 640 ; i++){
            for( j = 0 ; j < 480 ; j++){
                champs2f[i*480+j]
                =      l*champs1f[((i+ 1+640)%640)*480+(j+ 1+480)%480]
                +      l*champs1f[((i+ 1+640)%640)*480+(j- 1+480)%480]
                +      l*champs1f[((i- 1+640)%640)*480+(j+ 1+480)%480]
                +      l*champs1f[((i- 1+640)%640)*480+(j- 1+480)%480]
                +(1-4*l)*champs1f[i*480+j] ;
                //+champs1f[((i+di+640)%640)*480+(j+dj+480)%480]%4 ;
                if (champs3u[i*480+j]==255)
                    champs2f[i*480+j] = 1 ;
                sumchamp= sumchamp+champs2f[i*480+j] ;
            }
        }
        time_req = clock() - time_req;
        printf("diff : %f s",(float)time_req / CLOCKS_PER_SEC);
        //fprintf(stderr, "Les evenements : \n");
        // evenemnt
        time_req = clock();

        while ( SDL_PollEvent(&event) )
        {
            /* Traitement de l'événement */
            switch (event.type) /* Quel événement avons-nous ? */
            {
                case SDL_KEYDOWN:
                    fprintf(stdout, "Un appuie sur une touche :\n");
                    fprintf(stdout, "\trépétition ? : %d\n",event.key.repeat);
                    fprintf(stdout, "\tscancode : %d\n",event.key.keysym.scancode);
                    fprintf(stdout, "\tkey : %d\n",event.key.keysym.sym);
                    if ( event.key.keysym.scancode == SDL_SCANCODE_ESCAPE )
                    {
                        goto Quit ;
                    }
                    break;

                case SDL_WINDOWEVENT: // clic sur la croix de la fenêtre
                    if (event.window.event == SDL_WINDOWEVENT_CLOSE) // Fermeture de la fenêtre
                    {
                        goto Quit ;
                    }
                    break;
                case SDL_MOUSEMOTION:
                    fprintf(stdout, "Un déplacement de la souris :\n");
                    fprintf(stdout, "\tfenêtre : %d\n",event.motion.windowID);
                    fprintf(stdout, "\tsouris : %d\n",event.motion.which);
                    fprintf(stdout, "\tposition : %d;%d\n",event.motion.x,event.motion.y);
                    fprintf(stdout, "\tdelta : %d;%d\n",event.motion.xrel,event.motion.yrel);
                    if (boutonsourisLeft ==1){
                        if ((0<event.motion.x) && (event.motion.x<640) && (0<event.motion.y) && (event.motion.y<480)){
                            champs2f[(event.button.x-1)*480+(event.button.y-1)] =1 ;
                            champs2f[(event.button.x-1)*480+(event.button.y  )] =1 ;
                            champs2f[(event.button.x-1)*480+(event.button.y+1)] =1 ;
                            champs2f[(event.button.x  )*480+(event.button.y-1)] =1 ;
                            champs2f[(event.button.x  )*480+(event.button.y  )] =1 ;
                            champs2f[(event.button.x  )*480+(event.button.y+1)] =1 ;
                            champs2f[(event.button.x+1)*480+(event.button.y-1)] =1 ;
                            champs2f[(event.button.x+1)*480+(event.button.y  )] =1 ;
                            champs2f[(event.button.x+1)*480+(event.button.y+1)] =1 ;
                            fprintf(stdout, "clique :\n");

                        }
                    }
                    if (boutonsourisRight ==1)
                        champs3u[event.button.x*480+event.button.y] =255 ;

                    break;
                case SDL_MOUSEBUTTONUP:
                    fprintf(stdout, "Un relachement d'un bouton de la souris :\n");
                    fprintf(stdout, "\tfenêtre : %d\n",event.button.windowID);
                    fprintf(stdout, "\tsouris : %d\n",event.button.which);
                    fprintf(stdout, "\tbouton : %d\n",event.button.button);
#if SDL_VERSION_ATLEAST(2,0,2)
                    fprintf(stdout, "\tclics : %d\n",event.button.clicks);
#endif
                    fprintf(stdout, "\tposition : %d;%d\n",event.button.x,event.button.y);
                    if (event.button.button ==SDL_BUTTON_LEFT )
                        boutonsourisLeft =0 ;
                    if (event.button.button ==SDL_BUTTON_RIGHT )
                        boutonsourisRight =0 ;
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    fprintf(stdout, "Un appuie sur un bouton de la souris :\n");
                    fprintf(stdout, "\tfenêtre : %d\n",event.button.windowID);
                    fprintf(stdout, "\tsouris : %d\n",event.button.which);
                    fprintf(stdout, "\tbouton : %d\n",event.button.button);
#if SDL_VERSION_ATLEAST(2,0,2)
                    fprintf(stdout, "\tclics : %d\n",event.button.clicks);
#endif
                    fprintf(stdout, "\tposition : %d;%d\n",event.button.x,event.button.y);
                    if (event.button.button ==SDL_BUTTON_LEFT )
                        boutonsourisLeft =1 ;
                    if (event.button.button ==SDL_BUTTON_RIGHT )
                        boutonsourisRight =1 ;
                    break;
                default:
                    fprintf(stdout, "Événement non traité : %d\n",event.type);
            }

            fprintf(stdout, "\n");
        }
        time_req = clock() - time_req;
        printf("even : %f s",(float)time_req / CLOCKS_PER_SEC);

        time_req = clock();
        for (i = 0 ; i < 640 ; i++){
            for( j = 0 ; j < 480 ; j++){
                champs1f[i*480+j] =champs2f[i*480+j] ;
            }
        }
        time_req = clock() - time_req;
        printf("redef : %f s",(float)time_req / CLOCKS_PER_SEC);

        time_req = clock();
        // mise a jour du graphique
        time_req = clock();
        for (i = 0 ; i < 640 ; i++){
            for( j = 0 ; j < 480 ; j++){
                r=(Uint8) (champs1f[i*480+j]*255) ;
                g=r ;
                b=r ;
                SDL_SetRenderDrawColor(renderer, r, g, b, 255);
                SDL_RenderDrawPoint(renderer, i, j);
                //fprintf(stderr,"(i=%d,j=%d) = %d, %f\n",i,j,r,champs1f[i*480+j]);
            }
        }
        time_req = clock() - time_req;
        printf("graph : %f s",(float)time_req / CLOCKS_PER_SEC);

        time_req = clock();
        SDL_RenderPresent(renderer);
        time_req = clock() - time_req;
        printf("render : %f s",(float)time_req / CLOCKS_PER_SEC);

        fprintf(stderr, "iterration de la boucle %d : champ %f \n",t,sumchamp) ;
        //fprintf(stderr,"di=%d,dj=%d\n",di,dj);
        sumchamp2 =sumchamp;
        t=t+1 ;
    }
    fprintf(stderr, "fin de la boucle :");

    SDL_Delay(1000);



    SDL_RenderFillRect(renderer, &rect);
    SDL_SetRenderTarget(renderer, NULL);

    SDL_RenderCopy(renderer, texture, NULL, &dst);

    SDL_RenderPresent(renderer);
    SDL_Delay(1000);
/////
/**                          liberation de la mémoire                        **/

 Quit:
    fprintf(stderr, "libération de la mémoire :");

    if(NULL != texture)
        SDL_DestroyTexture(texture);
    else
        fprintf(stderr,"erreur SDL_DestroyTexture: %s", SDL_GetError());
    if(NULL != renderer)
        SDL_DestroyRenderer(renderer);
    else
        fprintf(stderr,"erreur SDL_DestroyRenderer: %s", SDL_GetError());
    if(NULL != window)
        SDL_DestroyWindow(window);
    else
        fprintf(stderr,"erreur SDL_DestroyWindow: %s", SDL_GetError());
    SDL_Quit();
    fprintf(stderr, "fin programme");

    return EXIT_SUCCESS;
}

/******************************************************************************/
/**                             Fonction                                     **/
/******************************************************************************/
int SDL_Init_erreur(int erreur)
{
    if(erreur)
    {
        fprintf(stderr, "Erreur SDL_Init : %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int SDL_Window_NULL(SDL_Window *window )
{
    if(NULL == window)
    {
        fprintf(stderr, "Erreur SDL_CreateWindow : %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int SDL_Renderer_NULL(SDL_Renderer *renderer )
{
    if(NULL == renderer)
    {
        fprintf(stderr, "Erreur SDL_CreateRenderer : %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int SDL_Texture_NULL(SDL_Texture *texture)
{
    if(NULL == texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTexture : %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int SDL_Surface_NULL(SDL_Surface *surface)
{
    if(NULL == surface)
    {
        fprintf(stderr, "Erreur SDL_LoadBMP : %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

int SDL_CreateTextureFromSurface_erreur(SDL_Texture *texture)
{
    if(NULL == texture)
    {
        fprintf(stderr, "Erreur SDL_CreateTextureFromSurface : %s", SDL_GetError());
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}







