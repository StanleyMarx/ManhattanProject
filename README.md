# Manhattan Project

## Ressources

+ [Site de l'équipe](http://ninofiliu.fr/OS_project/)
+ [OS - Projet automne 2017](http://soc.eurecom.fr/OS/projects_fall2017.html)
+ [Project repository](https://gitlab.eurecom.fr/ludovic.apvrille/OS_Robot_Project_Fall2017)
+ [Documentation de la librairie EV3DEV-C](http://in4lio.github.io/ev3dev-c/)

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
	
## 4 dec - To-do list
+ Communication client-serveur
+ Strategie d'exploration de la carte
+ Comprendre le code de la soeur d'Henry
