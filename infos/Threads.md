# Threads

*basé sur les recommandations de [Matteo](https://www.linkedin.com/in/matteo-bertolino-58859367/)*

## Motivations

Il faut envoyer la position du robot toutes les 2 secondes, et en même temps se diriger et cartographier la carte. Une manière simple d'implémenter ceci est d'avoir au moins deux threads:
+ Cartographie + mouvement
+ Communication de la position

## Implémentation - théorie

Il faut faire attention à la gestion de la mémoire partagée, en l'occurence ici, les variables de position. Si par exemple le robot bouge et que les variables évoluent ainsi `x:15→16,y:30→31` mais que la thread d'envoi de la position tombe au milieu de la double affectation, le serveur recevra `x=16,y=30` ce qui est n'est pas juste.

Ainsi il y a deux sections critiques:
1. SC1: Envoi de la position
2. SC2: Update du contenu des variables de position

Il faut donc les encadrer par des [MUTEX](http://www.thegeekstuff.com/2012/05/c-mutex-examples/). Ainsi, SC1 sera bloquée  si C2 est en execution et vice-versa.

Ça marche, jsuqu'au moment où SC2 prends beaucoup de temps. Il est alors difficile pour la thread d'envoi des positions de garder son rythme régulier. Ainsi il faut écourter SC2.

La solution proposée par Matteo est d'avoir 4 variables de position: `X, Y, Xdef, Ydef` (def pour definitif).  La thread d'envoi de position envoie `(Xdef,Ydef)`. La thread de cartographie/mouvement prends autant de temps qu'elle veut pour update `X,Y` mais sa section critique ne consiste qu'en `Xdef=X;Ydef=Y;`.

## Implémentation - exemple

```C
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int X=0, Y=0;
int Xdef=0,Ydef=0;
pthread_mutex_t lock;

void* display_entry(){
        /* affiche la position toutes les secondes */
        for (int i=0; i<5; i++){
                /* debut SC1 */
                pthread_mutex_lock(&lock);
                printf("x,y = %d,%d\n",Xdef,Ydef);
                pthread_mutex_unlock(&lock);
                /* fin SC1 */
                sleep(1);
        }
        return NULL;
}

int main(){
        pthread_t display;
        pthread_create(&display,NULL,display_entry,NULL);

        /* update de la position
           potentiellement complexe et asynchrone,
           donc X et Y sont passées dans Xdef et Ydef qu'une fois le travail fini */
        sleep(1);
        X+=10;
        sleep(2);
        Y+=10;

        /* debut SC2 */
        pthread_mutex_lock(&lock);
        Xdef=X;
        Ydef=Y;
        pthread_mutex_unlock(&lock);
        /* fin SC2 */

        pthread_join(display,NULL);
        pthread_mutex_destroy(&lock);
        return 0;
}
```
