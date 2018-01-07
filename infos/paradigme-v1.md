# Paradigme v1.0

Le paradigme v1.0 est le schéma d'organisation du code de la première - et sans doute finale - version du code de Fred.

Il est composé de 3 fichiers, disponibles dans [le fichier de la v1.0](/v1):

+ main.c
+ myev3.h
+ main-prog.h

**main.c** est le fichier central. C'est lui qui initie la communication client/serveur, c'est lui qui affecte les tachos à leur *sn* respectif, c'est lui qui inclut les librairies nécessaires, etc. Notons que le contenu de main.c est **NON PROGRAMMABLE**, cf plus bas.

**myev3.h** est le fichier de librairies où sont codées les fonctions qui nous simplifient l'utilisation de Fred: *get_compass*, *move_forever*, etc. C'est aussi là que sont utilisées nos variables globales telles que le numéro des ports des tachos ou *max_speed*.

**main-prog.h** est le fichier où "tout se passe". Après l'initialisation de la communication CS et des tachos/capteurs, c'est là où est codé le comportement du robot.

## Pourquoi il ne faut pas toucher à main.c

**main.c** initie le robot et fait ensuite appel à la fonction *robot*. Cette fonction est contenue dans **main-prog.h** et c'est là où le comportement du robot est codé. La communication CS est complexe et peu compréhensible, donc la partie de main.c qui gère ça est copiée/collée depuis le code de Ludovic - y toucher aboutira sans doute à un bug. L'initialisation du robot a été faite par nous-même mais il n'y a pas de raison d'y toucher car cela aura une influence sur le comportement des fonctions de myev3.h.

Ce paradigme permet de séparer le code en une partie **"programmable"** (myev3.h, main-prog.h) et une partie **"non-programmable"**  (main.c) afin de simplifier l'écriture et de prévenir les bugs non-intentionnels.
