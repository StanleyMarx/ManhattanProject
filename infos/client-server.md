# Communication client/serveur

*sorry pour l'absence d'accents*

## Liens utile

[Git de L. Apvrille](https://gitlab.eurecom.fr/ludovic.apvrille/OS_Robot_Project_Fall2017/tree/master)
Toutes les informations necessaires sont dans le README, le document ci-dessous n'est qu'un TLDR (too long, didn't read)

## En bref

### Setup du serveur

Tous les programmes necessaires au bon fonctionnement du serveur sont [ici](https://gitlab.eurecom.fr/ludovic.apvrille/OS_Robot_Project_Fall2017/tree/master/server).

1. Telecharger les fichier (git clone)
2. compiler le serveur avec le makefile fourni
3. *en cas d'erreur `bluetooth/bluetooth.h was not found`* installer `libbluetooth-dev`
4. *en cas d'autre erreur* installer les packages `libpng-dev`, `libsdl2-dev`, `libglew-dev`

### Setup du client

1. *si `blueman` n'est pas installe* [l'installer](http://www.ev3dev.org/docs/tutorials/connecting-to-the-internet-via-bluetooth/)
2. Allumer l'EV3
3. "unpair" l'EV3 sur le serveur
4. "pair" l'EV3 sur le serveur, en entrant le code PIN sur le serveur et l'EV3
5. Se connecter en ssh sur l'EV3
6. Sur le serveur et l'EV3, entrer la commande `hcitool scan`

### Communication et protocoles

L'envoi de paquets doit se soumettre aux protocoles decrits en detail dans le [readme du git](https://gitlab.eurecom.fr/ludovic.apvrille/OS_Robot_Project_Fall2017/tree/master), l'essentiel etant que:

+ Tous les nombres envoyes sont des `unsigned integer` au format [little endian](https://fr.wikipedia.org/wiki/Endianness#Little_endian)
+ Les paquets sont au format `HEADER|BODY`
+ Il n'y a qu'un seul format pour le header mais le body a un format different en fonction du type de message
+ Les paquets malformes sont jetes par le serveur

Les differents types de message:
1. **ACK**: accuse la reception des messages client → serveur
2. **START**: serveur → client. Marque le debut du jeu
3. **STOP**: serveur → client. Marque la fin du jeu
4. **KICK**: serveur → client. Le robot est exclu du jeu
5. **POSITION**: le robot doit communiquer sq position toutes les 2 secondes
6. **MAPDATA**: apres une cartographie complete, le robot envoie sa carte via ces paquets. Ces paquets envoient un carre de 5cm*5cm a la fois
7. **MAPDONE**: marque la fin de l'emission de la cartographie
8. **OBSTACLE**: signale au serveur qu'un obstacle a ete depose ou detecte
9. **CUSTOM**: il est possible de creer son propre type de message

---

*that's all folks*
