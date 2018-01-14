# Manhattan Project

## Ressources

+ [Site de l'équipe](http://ninofiliu.fr/OS_project/)
+ [OS - Projet automne 2017](http://soc.eurecom.fr/OS/projects_fall2017.html)
+ [Project repository](https://gitlab.eurecom.fr/ludovic.apvrille/OS_Robot_Project_Fall2017)
+ [Documentation de la librairie EV3DEV-C](http://in4lio.github.io/ev3dev-c/)
+ [Documentation sur les tachos](http://docs.ev3dev.org/projects/lego-linux-drivers/en/ev3dev-jessie/motors.html#tacho-motor-subsystem)

## final to-to list: 
- inclure message pour serveur dans prog
- tester asservissement angle 
- strat parcours un fois tour fait 
- le site :
Description of the architecture of the robot: sensors and actuators, etc. Pictures of your robot on the web site would be appreciated.

Algorithms used for the two most important points: cartography strategy, handling of obstacles, competition with other robots, etc. Don't provide source code here, just try to describe the algorithms using a pseudo C language. Also, do comment those algorithms, and explain why you think they are efficient.

Source code, and instructions on how to use that source code: how to compile it, how to download it on the robot, and how to start the robot. To protect you work, you can set a password to access to the code, but make sure to give us access in order to grade it (e.g., private git repository). You could even put a fake code on your website until the very last moment ;-). I strongly advise you to rely on a versioning system (svn, git, hg) to work on the code. Also, frequently backup your code.

Videos / pictures of your robot in action [we may provide you with cameras and video recorders if necessary].

How you have worked in the group, i.e., who made what. Be clear about that. Each member of groups will get an individual grade. All members of a team must contribute to the source code. You source code must clearly indicate, with comments, who programmed which function.


## To-do list

+ **Maîtriser les capteurs**: faire des programmes affichant la mesure effectuée par tous les capteurs 
+ **Idem pour les moteurs**: coder par exemple des fonctions *avancer(nb_cm)* ou *tourner(angle)*
+ **Maîtriser la communication client/serveur**
+ **Développer l'autonomie**: coder des programmes permettant au robot d'évoluer en effectuant des tâches simples comme se déplacer en évitant les obstacles
+ **Fixer une stratégie** pour la commplétion du projet final qui est celui de cartographier un espace
+ **Pour le 18 décembre** 
+ Test #1. Small stadium: be able to explore the stadium without bumping into obstacles.
+ Test #2. Small stadium: be able to start from the beginner position, and send to the BT server the position of the robot.
+ Test #3. Be able to make the difference between non-movable and movable objects.
+ Test #4. Be able to release an object.

## Lundi 11/12
PB : 
+ pb avec les .h : Makefile 
+ pb avec avancer (le moteur de la pelle bouge et pas les autres alors qu'on veut l'inverse)
+ comment trouver la position de départ? NON. task : le robot est déjà placé à la position de départ 
+ communication avec le serveur à tester 

## Jeudi 30 Novembre
Voici une liste de différents paliers à atteindre pour progresser sur le projet
 + **commander indépendament chaque élément**
	+  roues: avant/ arrière/ gauche/ droite/ vitesse/ tourner droite et gauche/ demi-tour 
	+  pelle: attraper/ garder/ relâcher 
	+  pointeur: réagir
	+  couleur: bien reconnaitre les couleurs/ poser la distance au sol et sa position/ réagir
	+ PAS DE CAPTEUR DE POSITION créer un historique des positions
	+  distance: réagir à une distance X d’un obstacle
	+  compas: orienter le robot dans une direction en particulier/ créer un historique des orientations

+ **poser l’interaction de plusieurs éléments en même temps**
	+  éviter tous les obstacles
	+  suivre une bande de couleur
	+  suivre un mur
	+  faire le tour d’un objet
	+  différencier un objet fixe d’un objet qu’on peut bouger
	+  suivre un chemin prédéfini
	+  revenir au point de départ par le chemin le plus rapide
	
## 4 dec
### to-do list
+ Communication client-serveur
+ Strategie d'exploration de la carte
+ Comprendre le code de la soeur d'Henri
### done list

+ Communication CS comprise, cf [infos/client-server.md](https://github.com/StanleyMarx/ManhattanProject/blob/master/infos/client-server.md): courte explication de la communication, et [utils/client.c](https://github.com/StanleyMarx/ManhattanProject/blob/master/utils/client.c): code du professeur compris et commenté qui implemente assez simplement l'envoi de paquets usuels
+ **creation MAIN.c** : initialisation, recherche sensors & moteurs A TESTER AVEC LE ROBOT
+ **tourner** : turns.c permet de tourner à droite ou à gauche (90°) A TESTER AVEC LE ROBOT
+ **Forward** : aller tout droit (pdt 9s) A TESTER AVEC LE ROBOT

## 16 dec
+ **Communication client/server établie** depuis mon dual boot Ubuntu, j'arrive à lancer `/server/server teams`, puis me SSH sur le robot et y lancer `/client/robotclient`. Le résultat est cohérent avec ce qui est censé se passer, cf le screen envoyé sur la conv fb. *-Nino*


## 12 Jan
### to-do list
+ Stopper le robot à une frontière non détectable pas le sonar
+ Connection serveur, envoyer messages
+ Décider d'une stratégie d'exploration
+ Re-tester que tous les programmes marchent bien


### done list
+ Obtenir la position en temps réel
+ Optention de la Map du terrain à partir des positions visitées
+ Mettre en place le capteur couleur pour une meilleure distinction movable_object/unmovable_object





