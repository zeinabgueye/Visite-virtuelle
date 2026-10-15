##Robot permettant de guider une visite virtuelle.
Ce projet est un projet de groupe réalisé dans le cadre scolaire. Ma partie consiste à faire la télécommande permettant de digirer le robot et de réaliser la visite vituelle.

##Fonctionnement du projet:
-Panorama 360° de l'école.
-Navigation d'une piéce à une autre avec des fléches cliquables.
-Déplacement du robot dans l'ecole.
-Prise de photo des piéces par un téléphone relié au robot.

##Techonologies:
-Pannellum pour l'affichage des panorama 360°
-Logiciel python pour le stiching
-Serveur python (http) pour lancer le site en local
-ESPNOW
-HTML, CSS, JAVASCRIPT, Arduino IDE
-Arduino mega, esp32, esp8266, joystick, batteries...

##Comment ca fonctionne:
-Pour la visite:
Apres avoir réaliser le robot, on la relié à un telephone android qui a pris les photos et qui les a envoyer dans notre base de données ou sont stockés les photos brutes. Vient ensuite le traitement de ces photos avec un code python qui permet de faire l'assemblage des photos piéces par piéces et qui en fait un format 360°. Puis j'ai configuré Pannellum et les liens entres les scénes et j'ai  lancé le site avec python. 

-Pour la télecommande:
J'ai utilisé la methode ESPNOW qui consiste à relier de carte esp8266 et d'en faire un émetteur( sur la télécommande) et un récepteur(sur le robot). 


##Lancer le projet:
pour lancer le projet, il faut lancer le serveur python puis ouvrir l'adresse du serveur sur le navigateur. 

Zeinab Gueye Bachelor 2 CSI Epf Africa.
