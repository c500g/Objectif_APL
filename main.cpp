// https://www.sfml-dev.org/tutorials/2.6/start-linux-fr.php
// https://caiorss.github.io/C-Cpp-Notes/resources-executable.html#org8373d36

// instal SFML librairy: sudo apt-get install libsfml-dev
/* compilation: 
	g++ -c main.cpp
	g++ main.o -o objectif_APL -lsfml-graphics -lsfml-window -lsfml-system
	./objectif_APL.app
*/

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <string>
#include <cmath>
#include <iostream>

using namespace std;

sf::RenderWindow win(sf::VideoMode(1000, 600), "Objectif APL");

sf::View vueP;               // vue principale (scrolling captain)
sf::View vueT;               // vue tableau de bord (temps, vie, materiel...)
sf::RectangleShape rect;     //
sf::Texture texturePerso;
sf::Texture textureObject;
sf::Texture textureDecors;
sf::Texture texturePhoto;
sf::Text titre;
sf::Text titre2;
sf::Text texte;
sf::Text message;
sf::Font barber;
sf::Font arial;
sf::Sprite object;
sf::Sprite captain;
sf::Sprite garde;
sf::Sprite decors;
sf::Sprite photo;

int x=00, y=-500, n=0, xj=11880;   // x,y= coordonnees captain    n= numero de l'objet en cours  xj= x jeep
int vie=100, compt=0, comptGarde=0, comptGaz=0, comptEau=0, comptExplo=0, fin=0, light=0, comptPos=0;
int ga=0, garde_compt=0,  recharge=0, tir=0, garde_position=0, couteau=0;
int R=0, L=0, D=1,U=1; // vvariables d'autorisation de direction (mouvements)
int dir=0;               // direction left=2, right=3, up/down=1, accroupi=4
int t=0;               // numero d'objet'just pick-up
float img=0;
bool pos=0;
int SPD=5, DIF=2;    // vitesse de course et difficulte de jeu (nb de point vie perdu a chaque impact)

bool object_inv[12]= {1,0,0,0,0,0,0,0,0,0,1,0};
std::string object_name[12]={"knife","electric torch","gun","tool kit","first aid kit","access card","gas mask","door key","12v battery","explosives","knife","diving equipt"};
string mes="";
bool evenement[23];
int grd[22][5]={ {0,0,0,0,100 },{ 0,1300,500,0,100},
                 {0,3000,500,3,100},{0,3100,500,4,100},{0,3200,500,0,100},
                 {1,4440,150,0,100}, {1,6000,950,0,100},
                 {0,11000,500,8,100},{1,11100,500,9,100},{1,11200,500,0,100} ,
                 {3,16600,500,11,100},{3,16800,500,12,100},{3,17000,500,13,100}, {3,17200,500,14,100},{3,17400,500,15,100},{3,17600,500,0,100},
                 {0,21280,1000,0,100},{1,22500,200,0,150},
                 {1,24000,500,0,100},
                 {3,26000,500,20,150},{1,26075,500,21,150},{1,26150,500,0,150},};

void rectangle(int x, int y, int lx, int ly, int c);  // x,y= position depart  lx,ly= longueur, largeur c= code couleur
void afficheViewPrincipale();
void getObject();
void useObject();
void changeObject();
void afficheViewTableau();
void testCollision();
void initializeTexture();
void initializeView();
void afficheMessage(int t,int compt);
void echelle(int x, int yH, int yl,int yI1, int yI2);
void testEvenement();
void testClavier();
void afficheCaptain();
void afficheGarde();
void afficheGaz();
void explosion();
void intro();


int main()
{

 // evenement[4]=1;  evenement[15]=1;evenement[13]=1; evenement[18]=1;evenement[14]=1;evenement[12]=1;evenement[17]=1;
 // object_inv[2]=1; object_inv[9]=1; object_inv[7]=1;
// x=20500; y=1000;


  mes=" Hey Captain, your mission \n is to destroy the secret lab\n and escape from this place\nlet's go !"; compt=-450;
  win.setFramerateLimit(60);
 if (!texturePerso.loadFromFile("sources/image/captain2.png"))      return EXIT_FAILURE;
 if (!textureObject.loadFromFile("sources/image/object.jpg"))       return EXIT_FAILURE;
 if (!textureDecors.loadFromFile("sources/image/decors.png"))       return EXIT_FAILURE;
 if (!texturePhoto.loadFromFile("sources/image/Bivouac.jpg"))       return EXIT_FAILURE;
 if (!barber.loadFromFile("sources/font/Barber_shop.ttf"))          return EXIT_FAILURE;
 if (!arial.loadFromFile("sources/font/arial.ttf"))                 return EXIT_FAILURE;


initializeTexture();  // texture captain, object et decors + titre
initializeView();     // initialize les vue et le split-screen


	// Start the game loop
    while (win.isOpen()  )
        {
        
        sf::Event event;
        while (win.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                win.close();
        }
        testClavier();
        win.clear();
        testCollision();
        vueP.setCenter(sf::Vector2f(x+250, y-200));
        afficheViewPrincipale();  // dessine la vue principale + decors + echelle
        testEvenement();
        afficheCaptain();
        afficheGarde();
        afficheViewTableau();    // dessine la tableau de controle a droite (titre, life, object, )
        if (mes !="") { afficheMessage(t, compt);    compt++; }     else compt=0;
        win.display();
    }
    return 0; 
}

void initializeTexture()
   {
    int lx, ly;  // juste pour test
     titre.setFont(barber);
     texte.setFont(arial); texte.setCharacterSize(20);
     message.setFont(arial); texte.setCharacterSize(30);
     captain.setTexture(texturePerso);
     garde.setTexture(texturePerso);
     object.setTexture(textureObject);
     decors.setTexture(textureDecors);
     photo.setTexture(texturePhoto);
   }

void initializeView()
   {
     vueP.reset(sf::FloatRect(0,0,	800,	600));
     vueT.reset(sf::FloatRect(0,0,	200,	600));
     vueP.setViewport(sf::FloatRect(0.f, 0.f, 0.8f, 1.f));
     vueT.setViewport(sf::FloatRect(0.8f, 0.f, 0.20f, 1.f));
   }

void testClavier()
      {

       if (ga>0) {L=0;}     // pendant un combat, interdiction de reculer !
       if (dir==2 || dir==3 || dir==4 ) { dir=0; }     // DIR= 0  : debout en attente
       if (fin==0)                                     // si ce n'est pas la fin
        {
        if (dir !=5)
            {
             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)     && U==1)  { y-=SPD; dir=1;  comptPos++; if (comptPos>7) {pos=!pos; comptPos=0;} }      // deplacement vertical
             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)   && D==1)  { y+=SPD; dir=1;  comptPos++; if (comptPos>7){ pos=!pos;comptPos=0;}}       // deplacement vertical
             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)   && D==0)  { dir=4; }               // DIR =4 : accroupi
             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)  && R==1 ) { x+=SPD; dir=3; }       // deplacement a droite
             if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)   && L==1)  { x-=SPD; dir=2; }       // deplacement a gauche
            }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && recharge==1 )     {   useObject();   recharge=0;  }
        if (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space))                    {   recharge=1;                 }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Tab) && n!=11)              {   changeObject();             }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Return) && compt==0)        {   getObject();                }
       // jeep
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && abs(x-xj)<30 && evenement[13]==1)    { dir=5; mes="you get in the jeep" ;}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && abs(x-11885)<30 && evenement[13]==0) { mes="the jeep doesn't start" ;}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)   && dir==5 )         {   dir=0; mes="you get off the jeep"; }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)  && dir==5 )         {   x=x+8; xj=xj+8; } // deplact a droite
        }
       if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))                     {   win.close(); }
        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::O))                          {   vueP.zoom(1.1f); }
        //if (sf::Keyboard::isKeyPressed(sf::Keyboard::P))                          {   vueP.zoom(0.9f); }
      }

void afficheCaptain()
  {
  captain.setPosition(x-10,y-125);
  if (img<7) { img=img+0.04*SPD ; }
  else img=0;
  if (dir==0)          { captain.setTextureRect(sf::IntRect( 50, 170, 100, 130));}
  if (dir==1)          { captain.setTextureRect(sf::IntRect( 200 + pos*150, 170, 100, 130));}
  if (dir==3)          { captain.setTextureRect(sf::IntRect( floor(img)*150+50, 5, 120, 130));}
  if (dir==2)          { captain.setTextureRect(sf::IntRect( floor(img)*150+50, 368, 120, 122));}
  if (dir==4)          { captain.setTextureRect(sf::IntRect( 1650, 170, 100, 130));}
  if (dir==0 && n==2)  { captain.setTextureRect(sf::IntRect( 650, 172, 120, 125));}     // position tir debout
  if (dir==4 && n==2)  { captain.setTextureRect(sf::IntRect( 500, 170, 120, 125));}    // position tir accroupi
  if (dir==0 && n==0 && couteau>0) { captain.setTextureRect(sf::IntRect( 1880, 368, 120, 125)); couteau--;}  // coup couteau
  if (dir==4 && n==0 && couteau>0) { captain.setTextureRect(sf::IntRect( 1720, 368, 120, 125)); couteau--;}  // coup couteau
  if (dir==5 )         { captain.setTextureRect(sf::IntRect( 1460, 172, 120, 125));}    // assis dans la jeep
  if (n==11 && evenement [22]==1) {captain.setTextureRect(sf::IntRect( 1725,5, 150, 130));}    // plongeur
  win.draw(captain);
  afficheGaz();
  }

void afficheGarde()
  {
    for (int i=1; i<=21; i++)     // boucle affichage de tous les gardes en fonction de leur statut (stand-by, en marche, en tir debout, en tir assi, mort)
     {
     int xg=grd[i][1];   //std::cout<<" i=  "<< i<< "   xg= "<<xg<< "   grd[i][0]= "<< grd[i][0]<< "  actif= "<<garde_actif<<" compt= "<<garde_compt<<std::endl;
     int yg=grd[i][2];
     if (grd[i][0]==0) { garde.setPosition(xg,yg-125); garde.setTextureRect(sf::IntRect( 1100, 172, 150, 125)); } // garde stand-by droit
     if (grd[i][0]==1) { garde.setPosition(xg,yg-125); garde.setTextureRect(sf::IntRect( 780, 172, 150, 125));  }// garde stand-by gauche
     if (grd[i][0]==2) { garde.setPosition(xg,yg-100); garde.setTextureRect(sf::IntRect( floor(garde_position/20)*100, 00, 100, 100));  // en marche
     garde_position++; if (garde_position==100) {garde_position=0;}
     if (xg-x<400) { grd[i][0]==3;  garde_compt=0;}  }        // si le captain se rapproche a moins de 500 du garde, passage en mode tir
     if (grd[i][0]==3) { garde.setPosition(xg,yg-125); garde.setTextureRect(sf::IntRect( 780, 172, 150, 125)); } // garde en position tir debout
     if (grd[i][0]==4) { garde.setPosition(xg,yg-125); garde.setTextureRect(sf::IntRect( 940, 172, 150, 125));  }// garde en position tir assis
     if (grd[i][0]==5) { garde.setPosition(xg,yg-125); garde.setTextureRect(sf::IntRect( 1280, 172, 150, 125));    } // garde mort
     win.draw(garde);
     }
   // tir et test touche
   if (ga >0)  { comptGarde++;
                 if  (grd[ga][0]==0 && comptGarde>100) { grd[ga][0]=1; }
                 if  (grd[ga][0]==1 && comptGarde>30) { grd[ga][0]=3;comptGarde=0; }
                 if  (grd[ga][0]==3 && comptGarde>40) { grd[ga][0]=4; comptGarde=0; }
                 if  (grd[ga][0]==4 && comptGarde>40) { grd[ga][0]=3; comptGarde=0; }
                 int xg= grd[ga][1];
                 int s=  grd[ga][0] + dir;
                 if ((grd[ga][0]==3 || grd[ga][0]==4)  && comptGarde>35  )    // a compt=100 => le garde tire
                         {
                          rectangle(xg-600,y-70+(grd[ga][0]-3)*20,600,2,1);   // affiche du tir
                          if  (xg-x<600 && (s==3 || s==8) ) { vie=vie-DIF; }   // touche si garde debout et captain debout ou  garde assis et captain assis
                         }
                }


  if (vie<=0) { mes=" You have been killed !"; fin=1;  }
  }

void afficheMessage(int t, int compt)   // t= type de message. de 0 a 11, numero de l'objet. si >11 autre message
  {
  win.setView(vueP);
  rectangle(x+47,y-403,426,186,1);       // fond du message
  rectangle(x+50,y-400,420,180,4);      // fond du message
  message.setPosition(x+80,y-390);
  message.setString(mes);       win.draw(message);
   if (compt>100) { mes=""; }                           // compteur pour laisser le message qq secondes
  // if (t >0 && t<=11)  { object.setTextureRect(sf::IntRect( t*120, 0, 120, 105));  // si c'est un nouvel objet, on affiche son logo
      //                     object.setPosition(x+170,y-340); win.draw(object);}
  }

void getObject()    // retourne le numero de l'objet reccupere
  {
   int t=0; int xo,yo;
   xo=900; yo=200;      if (abs(x-xo) < 50 && y==yo )                    { t=3; }  // armoire 1
   xo=2500; yo=500;     if (abs(x-xo) < 50 && y==yo )                    { t=7; }  // boite a cle
   xo=1350; yo=500;     if (abs(x-xo) < 75 && y==yo )                    { t=2; }  // pistolet sur garde 1
   xo=6000; yo=950;     if (abs(x-xo) < 75 && y==yo )                    { t=6; }  // gas mask sur garde 6
   xo=6620; yo=2400;    if (abs(x-xo) < 75 && y==yo )                    { t=1; }  // au fond du couloir noir
   xo=9800; yo=200;     if (abs(x-xo) < 75 && y==yo && evenement[11]==0) { t=4; evenement[11]=1; }  // medikit armoire 2
   xo=11805; yo=500;    if (abs(x-xo) < 50 && y==yo )                    { t=9; }  // explosive dans jeep
   xo=12050; yo=500;    if (abs(x-xo) < 50 && y==yo )                    { t=8; }  // battery devant jeep
   xo=21280; yo=1000;   if (abs(x-xo) < 50 && y==yo )                    { t=5; }  // access card sur garde 16
   xo=22550; yo=200;    if (abs(x-xo) < 75 && y==yo )                    { t=2; }  // pistolet sur garde 17
   xo=24000; yo=500;    if (abs(x-xo) < 50 && y==yo && evenement[19]==0) { t=4; evenement[19]=1; }  // medikit sur garde 18
   xo=28100; yo=900;    if (abs(x-xo) < 50 && y==yo && evenement[22]==0) {  evenement[22]=1; }      // trappe remplissg eau
   xo=28015; yo=900;    if (abs(x-xo) < 50 && y==yo )                    { t=11; n=11; }            // equipement plongee
   if (t>0 && object_inv[t]==0)   { mes="You pick up a "+object_name[t]+" !"; if (t==5) { mes=mes+" \n but your gun is broken"; object_inv[2]=0;n=0;}
      object_inv[t]=1; object.setTextureRect(sf::IntRect( t*120, 0, 120, 105));  // si un nouvel objet, on affiche son logo
      object.setPosition(x+170,y-340); win.draw(object);  }
      else { mes="nothing to pick-up... "; t=0; }
  if (evenement[22]==1 ) {mes="you start filling \n the immersion chamber";}
  xo=27210; yo=900;  if (abs(x-xo) < 70 && y==yo ) { mes="this is the lab to blow up !"; }
 }

void useObject()
  {
   if (n==1 ) { evenement[7]=!evenement[7]; mes="you switch your torch"; }
   if (n==6 ) { evenement[10]=!evenement[10]; mes="you put on/off your mask"; }
   if (n==4) { mes="you use your medikit..."; vie=100; object_inv[4]=0; n=0; }
   if (n==11 ) { mes="already in use..."; }
   int xd,yd;
   xd=710; yd=500;    if (abs(x-xd) < 40 && y==yd && n==3 && evenement[3]==0 ) { mes="You open ventilation panel"; evenement[3]=1; }       // ouverture ventilation 1
   xd=5310; yd=500;   if (abs(x-xd) < 40 && y==yd && n==3 && evenement[6]==0) { mes="You open ventilation panel"; evenement[6]=1; }      // ouverture ventilation 2
   xd=920; yd=500;    if (abs(x-xd) < 40 && y==yd && n==7 ) { mes="You open the staff room"; evenement[1]=1; }          // ouverture porte 1
   xd=9740; yd=500;   if (abs(x-xd) < 40 && y==yd && n==7 ) { mes="You open the corridor"; evenement[9]=1; }
   xd=11950; yd=500;  if (abs(x-xd) < 40 && y==yd && n==8 ) { mes="You repaired the jeep"; evenement[13]=1; object_inv[8]=0; n=0; }
   xd=20500; yd=500;  if (abs(x-xd) < 40 && y==yd && n==3 && evenement[15]==0) { mes="You open ventilation panel"; evenement[15]=1; }      // ouverture ventilation 3
   xd=23245; yd=500;   if (abs(x-xd) < 40 && y==yd && n==7 ) { mes="You open the control room"; evenement[16]=1; }
   xd=23800; yd=500;   if (abs(x-xd) < 40 && y==yd && n==3 && evenement[17]==0) { mes="You open ventilation panel"; evenement[17]=1; }      // ouverture ventilation 2
   xd=26920; yd=900;   if (abs(x-xd) < 40 && y==yd && n==5 && evenement[20]==0) { mes="You open the LABORATORY"; evenement[20]=1; }      // ouverture labo, c'est bientot la fin
   xd=27645; yd=900;   if (abs(x-xd) < 40 && y==yd && n==5 && evenement[21]==0) { mes="You open immersion chamber"; evenement[21]=1; }      // ouverture labo, c'est bientot la fin
   xd=27210; yd=900;   if (abs(x-xd) < 70 && y==yd && n==9 && evenement[23]==0) { mes="you put explosive ...\n hurry up before explosion !"; evenement[23]=1; }      // put explosive

  if (ga>0 && (abs(x-grd[ga][1]) <=60 && y==grd[ga][2]) && recharge==1 && n==0 )
            { couteau=10;
            if ((dir==0 && grd[ga][0]==4)|| (dir==4 && grd[ga][0]==3))  // si accroupi et le garde debout (ou contraire)
                    { grd[ga][4]=grd[ga][4]-5; // perte de 5 pt vie du g// combat couteau
                  if (grd[ga][4]<=0) { mes="You kill the guard"; grd[ga][0]=5; ga=grd[ga][3];  }
                     }
            }
   if (ga>0 && abs(x-grd[ga][1]) < 600 && y==grd[ga][2] && recharge==1 && n==2 )       // si on est en combat pistolet...
            { tir=1;                                                                        // on affiche le tir
              if ((dir==0 && grd[ga][0]==4)|| (dir==4 && grd[ga][0]==3))    // si accroupi et le garde debout (ou contraire)
                { grd[ga][4]=grd[ga][4]-25;                                     // on touche !
                  if (grd[ga][4]<=0) { mes="You kill the guard"; grd[ga][0]=5; ga=grd[ga][3]; grd[ga][0]=3;}
                }
            }
   if (mes =="") { mes="not possible to use "+ object_name[n]; t=0;  // aucun objet a utiliser
                   if (ga >0 && (n==0 || n==2) && mes!="You kill the guard") { mes=""; }  // ne pas afficher de message qunand on utilise un couteau ou pistolet pendant le combat
                 }
  }

void changeObject()
  {
   win.setView(vueT);
   do
       { for (int i=0 ; i<=120; i=i+25)
           {  object.setTextureRect(sf::IntRect( n*120+i, 0, 120, 105));
              win.draw(object);
              win.display(); }
       n++;
       }
   while (object_inv[n]==0) ;
   if (n==10) {n=0;}
   if (n!=1) { evenement[7]=0; } // si on change d'objet par autre chose que la lampe, celle-ci s'eteint
  }

void testEvenement()
  {
    light=2;
    if ((evenement[0]==0 ) && x>250)    { evenement[0]=1; }   // point de non-retour debut de jeu; fermeture de l'entree...
    if (evenement[1]==1 && grd[1][0]!=5 )   { ga=1;  }  // quand on passe la porte 1, le garde 1 devient actif (si pas mort)
    if (x>2600 &&x<3000 && evenement[4]==0) { evenement[4]=1; ga=2;  }  // declenchement des 3 gardes du couloir ( 2, 3, 4)
    if (x>3830 && x<4000 && y==150 && grd[5][0]!=5)      {  ga=5;  }     // declenchement du garde entre ech 4 et 5
    if (x>5400 && x<5500 && y==950 && grd[6][0]!=5)      {  ga=6;  }     // declenchement du garde entre ech 4 et 5
   if (evenement[7]==0 && x>4700 && x<7000 && y>1800)    { light=0; }    // dans le noir si [pas de lampe !
   if (evenement[7]==1 && x>4700 && x<7000 && y>1800)    { light=1; }    // et on y voit mieux avec la lampe
   if (evenement[8]==0 && x>8100 && x<9700)    { evenement[8]=1; } // point de non-retour milieu de jeu et declenchament gaz
   if (x>9800 && x<10000)                      {evenement[8]=0; comptGaz=0;}  // a droite du gaz on enleve l'evenement declencheur du gaz
   if (evenement[10]==1)                       { SPD=2; } else { SPD=5; }
   if (x>10400 && evenement[12]==0)            { evenement[12]=1; ga=7;  }  // declenchement des 3 gardes du couloir (2,3,4)
   if (x>19430 && evenement[13]==1)                      { evenement[13]=0;  dir=0; mes="the tank of the jeep is empty..."; }
   if (x>15600 && x< 15800 && evenement[14]==0)          { evenement[14]=1; ga=10;  } // declenchement des 6 gardes (jeep)
   if (x>17600 && x< 18000 && evenement[14]==1)          { evenement[14]=0; ga=0;  }           // arret des 6 gardes (jeep)
   if (evenement[7]==0 && x>20500 && x<=22725 )          { light=0; }  // dans le noir si [pas de lampe !
   if (evenement[7]==1 && x>20500 && x<=22725 )          { light=1; }  // et on y voit mieux avec la lampe
   if (x>20700 && x<21200 && y==1000 && grd[16][0]!=5)   {  ga=16;  }           // declenchement du garde 1 souterrain 3
   if (x>21900 && x<22300 && y==200 && grd[17][0]!=5)    {  ga=17;  }           // declenchement du garde 2 souterrain 3
   if (x>23400 && x<24000 && y==500 && grd[18][0]!=5)    {  ga=18;  }           // declenchement du garde 2 souterrain 3
   if (x>25450 && evenement[18]==0)                      { evenement[18]=1; ga=19;  }
  if (evenement[22]==1) { rectangle (27760,915-comptEau,440,comptEau,6); evenement[21]=0;
                          if (comptEau<165) {comptEau++;}
                          else if (n!=11) { mes =" you died drown in water..."; sf::sleep(sf::seconds(5));fin=1;}
                          }
  if (evenement[23]==1 ) { comptExplo++;
                            if (comptExplo==800) {  explosion(); }
                             if (x>29000) { mes=" You manage to escape \n the secret base ! \n The lab is destroyed \n your mission is successful !"; fin=2;     }
                         }
  if (evenement[23]==0 && x>29000) { mes=" You manage to escape \n the secret base ! \n But the lab is not destroyed !\n your mission is a failure !"; fin=3;     }

  }

void testCollision()
  {
   L=1; R=1; U=0; D=0;       // cas normal= on peut aller a gauche et a droite
   if (y==500 && x<=-30)                      {L=0; }    // mur de gauche au pied de l'echelle 1
   if (y==500 && x<=100 && evenement[0]==1)   {L=0;}     // blocage point de non-retour
   if (y==200 && x<=470)                        {L=0; }    // mur gauche salle 1
   if (y==200 && x>=1010 && x<=1100)                        {R=0; }    // mur droite salle 1
   if (y==500 && x>=930 && x<=950 && evenement[1]==0)  { R=0;}      // porte 1 fermee a cle
   if (y==500 && x>=1000 && x<=1030 && evenement[1]==0)  { L=0;}    // porte 1 fermee a cle
   if (y==500 && x>=1630 && x<=1650 && evenement[2]==0)  { evenement [2]=1; }    // porte 2 fermee a par bouton
   if (y==500 && x>=1700 && x<=1730 && evenement[2]==0)  { L=0;}    // porte 2 fermee a par bouton
   if (y==1000 && x<=700)                        {L=0; }    // mur gauche souterrain 1
   if (y==1000 && x==2020 )                        {R=0; }   // mur droit souterrain 1
   if (y==500 && x>=3830 && x<=3900)             {R=0; }    // mur droite avant echelle 4
   if (y==500 && x>=4300 && x<=4500)             {L=0; }    // mur gauche avant echelle 5
   if (y==150 && x<=3700)                        {L=0; }    // mur gauche salle echelle 4
   if (y==150 && x>=4620)                        {R=0; }    // mur droite salle echelle 5
   if (y==950 && x<=5200)                        {L=0; }    // mur gauche souterrain 2 etage i1
   if (y==950 && x>=6120)                        {R=0; }    // mur droite souterrain 2 etage i1
   if (y==1500 && x<=5200)                        {L=0; }    // mur gauche souterrain 2 etage i2
   if (y==1500 && x>=7120)                        {R=0; }    // mur droite souterrain 2 etage i2
   if (y==2000 && x<=5000)                        {L=0; }    // mur gauche souterrain 2 bas
   if (y==2000 && x>=5915)                        {R=0; }    // mur droite souterrain 2 bas
   if (y==2400 && x<=5800)                        {L=0; }    // mur gauche souterrain 2 bas -1
   if (y==2400 && x>=6715)                        {R=0; }    // mur droite souterrain 2 bas -1
   if (y==500  && x>8000 && x<8050 && evenement[8]==1)  {L=0;}  // porte auto 1
   if (y==500 && x>=9750 && x<=9780 && evenement[9]==0)  { R=0;}   // porte 3 fermee a cle (apres porte auto milieu de jeu)
   if (y==200 && x==9750 )                         {L=0; }    // mur gauche salle medikit
   if (y==200 && x==10175 )                        {R=0; }    // mur droit salle medikit
   if (y==500 && x==20630 )                        {R=0; }   // mur droit couloir apres jeep
   if (y==1000 && x==20400 )                       {L=0; }    // mur gauche souterrain 3
   if (y==1000 && x==21520 )                       {R=0; }   // mur droit souterrain 3
   if (y==700 && x==21300 )                        {L=0; }    // mur gauche souterrain 3
   if (y==700 && x==22215 )                        {R=0; }   // mur droit souterrain 3
   if (y==200 && x==21300 )                        {L=0; }    // mur gauche souterrain 3
   if (y==200 && x==22725 )                        {R=0; }   // mur droit souterrain 3
   if (y==500 && x>22600 && x<=22680 )             {L=0; }   // mur gauche couloir
   if (y==500  && x>23245 && x<23290 && evenement[16]==0)  {R=0;}  // porte fermee a clef
   if (y==1600 && x==23760 )                        {L=0; }    // mur gauche souterrain 4
   if (y==1600 && x==24845 )                        {R=0; }   // mur droit souterrain 4
   if (y==500 && x<=24495 && x>24450)              {L=0; }    // mur gauche sortie souterrain 4
   if (y==500 && x==24115 )                        {R=0; }   // mur droit entree souterrain 4
   if (y==500 && x==26900 )                        {R=0; }   // mur droit fin tunnel
   if (y==900 && x<=26365 && x>26300 )              {L=0; }   // mur gauche fin tunnel
   if (y==900 && x>=28090 )                         {R=0; }   // mur droit levier sas
   if (y==900 && x==26940 && evenement[20]==0)  { R=0;}      // porte magnetique 3 fermee
   if (y==900 && x==26940 && evenement[20]==0)  { R=0;}      // porte magnetique 3 fermee
   if (y==900 && x==27040 )  { L=0; evenement[20]=0;}      // porte magnetique 3 se referme
   if (y==900 && x==27645 && evenement[21]==0)  { R=0;}      // porte magnetique sas fermee
   if (y==900 && x==27755 && evenement[21]==0)  { L=0;}      // porte magnetique sas fermee

   // dans eau immersion sas
   if (evenement[22]==1)
      { U=1; D=1; L=1; R=1;
        if (y==900 ) { D=0;}
        if (y==850 ) { U=0;}
        if (x==27755 ) { L=0;}
        if (x==28110 ) { R=0;}
        if (y==900 && abs(x-27910)<=10 )          { D=1; }  // en haut
        if (y>900 && y<=1310 && abs(x-27910)<=10) {U=1; D=1; L=0; R=0;}  // couloir vertical
        if (abs(y-1310)<20 && abs(x-27910)<=10)   {U=1; R=1; L=0; D=0;}  // au fond a gauche
        if (abs(y-1310)<20 && x>27910)            {U=0; R=1; D=0; L=1;}       // couloir horizontal
          }

   for (int i=0; i<20; i++) {  if (y==grd[i][2] && (x+45)>=grd[i][1] && (x-200)<grd[i][1] && grd[i][0]!=5)   { R=0;} }  // collision avec garde avant sa mort
   if (abs(y-950)<40 && dir!=1) { y=950; }    // recallage du y lors de sortie intermediaire d'echelle souterrain 2
   if (abs(y-1500)<40 && dir!=1) { y=1500; }  // recallage du y lors de sortie intermediaire d'echelle souterrain 2
   if (abs(y-700)<40 && dir!=1) { y=700; }    // recallage du y lors de sortie intermediaire d'echelle souterrain 3
   if (y==1601 || y==1602) { y=1600; }        // recallage echelle souterrain 4

   if (dir !=5 && evenement[10]==0 && (x/5)*5 !=x) {x=(x/5)*5;}       // recalage des x sur multiple de 5 (possible decalage a cause du SPD=2 en mode masque a gaz
   if (dir !=5 && evenement[10]==0 && (y/5)*5 !=y) {y=(y/5)*5;}       // recalage des y sur multiple de 5 (possible decalage a cause du SPD=2 en mode masque a gaz

  }

 void afficheGaz()
     {
     if (evenement[8]==1) { rectangle (8020,300,1800,200,9);
                            if (comptGaz<240) {  comptGaz++;  if (evenement[10]==0) {mes="you are suffocating...";}}
                            if (comptGaz==240 && evenement[10]==0 ) {mes="A lethal gas killed you..."; sf::sleep(sf::seconds(5));fin=1;}
                          }
     if (x>22000 && x<25000 && y>700) { rectangle (23765,700,150,900,9);rectangle (23915,1500,850,100,9);rectangle (24765,700,150,900,9);
                            if (comptGaz<240) {  comptGaz++;  if (evenement[10]==0) {mes="you are suffocating...";}}
                            if (comptGaz==240 && evenement[10]==0 ) {mes="A lethal gas killed you..."; sf::sleep(sf::seconds(5));fin=1;}
                          } //else {comptGaz=0; }


     }

void afficheViewPrincipale()
  {
    win.setView(vueP);
    rectangle(-500,500,30000,3000,3);        // sol orange
    rectangle(-500,-500,30000,800,3);          // plafond orange
    rectangle(-500,300,600,200,3);          // mur a l'entree qui est refeme quand x> 300
    rectangle(500,50,600,150,0);          echelle(500,200,500,0,0);   // echelle 1 + salle toolkit
    rectangle(1000,300,50,50,3);     if (evenement[1]==0) { rectangle(1020,350,10,150,1); }   // porte 1 fermee a cle
    rectangle(1700,300,50,50,3);     if (evenement[2]==0) { rectangle(1720,350,10,150,1); }   // porte 2 fermee a par bouton
    rectangle(9800,300,50,50,3);     if (evenement[9]==0) { rectangle(9820,350,10,150,1); }   // porte 3 fermee a cle
    rectangle(800,850,1200,150,0);       echelle(2000,500,1000,0,0);   // souterrain 1 + echelle 3
    rectangle(3800,200,800,300,3);                                  // mur avant echelle 4;
    rectangle(3700,00,1000,150,0);       echelle(3800,150,500,0,0);    echelle(4500,150,500,0,0);  // echelle 4, 5 et salle
    rectangle(8000,300,50,50,3);     if (evenement[8]==1) { rectangle(8020,350,10,150,1);  }  // porte auto milieu de jeu
    rectangle(9750,50,500,150,0);     echelle(10100,200,500,0,0);                              // salle medikit
    rectangle(11600,196,8300,300,0);    // grand couloir jeep
    photo.setPosition(15000,200); win.draw(photo);  // photo bivouac TDM :-)
    rectangle(20700,300,2000,200,3);
    rectangle(23300,300,50,50,3);     if (evenement[16]==0) { rectangle(23320,350,10,150,1); }      // porte 1 fermee a cle
    rectangle(24200,300,300,200,3);  rectangle (27000,300,2000,200,3);
    rectangle(26400,700,1800,200,0);     echelle(26400,500,900,0,0);  // derniere echelle
    rectangle(27000,700,50,50,3);     if (evenement[20]==0) { rectangle(27020,750,10,150,1); }  // porte carte magnet (labo)
    rectangle(27700,700,1000,50,3);     if (evenement[21]==0) { rectangle(27725,750,10,150,1); 
    rectangle(27750,750,10,150,1);}      // porte a carte magnetique (labo)
    rectangle(27900,915,150,400,6);    rectangle(28000,1215,1000,100,6); rectangle(29000,-200,2000,3000,6); 
    if (evenement[22]==0) { rectangle(27900,900,100,15,1); }  // eau et trappe
    if (tir==1) { rectangle(x+70,y-70+dir*10,600,4,1); tir=0; } // affiche du tir

    // souterrain ventilation
    if (evenement[0]==0)   {  rectangle(-100,-700,250,200,4);     echelle(0,-500,500,0,0);   }     // souterrain 1
    if (evenement[3]==1)   {  echelle(710,500,1000,0,0); }
    if (evenement[6]==1)   {  rectangle(5200,800,1000,150,0); rectangle(5200,1350,2000,150,0);    // souterrain 2
                              rectangle(5000,1850,1000,150,0);  rectangle(5800,2250,1000,150,0);
                              echelle(5850,2000,2400,0,0); rectangle(6600,2350,50,50,1);
                              echelle(5310,500,2000,950,1500); echelle (7000,500,1500,0,0); }
    if (evenement[15]==1)   {  rectangle(20400,850,1200,150,0); rectangle(21300,50,1500,150,0); 
    			       rectangle(21300,600,1000,100,0); // souterrain 3
                               echelle(20500,500,1000,0,0);     echelle(21400,200,1000,700,0);  echelle(22700,200,500,0,0);}
    if (evenement[17]==1)   {  rectangle(23800,1500,1000,100,0); echelle(23800,500,1600,0,0);
    			       echelle(24800,500,1600,0,0); } // souterrain 4


  // affiche decors
     decors.setScale(0.6,0.6); decors.setPosition(195,350); decors.setTextureRect(sf::IntRect( 0, 250, 150, 100));  win.draw(decors);  // fleche decors
   decors.setScale(1,1);   object.setScale(1,1);
   decors.setPosition(3400,320); decors.setTextureRect(sf::IntRect( 200, 250, 100, 90));  win.draw(decors);  //  masque
   decors.setPosition(900,100); decors.setTextureRect(sf::IntRect( 0, 0, 100, 100));  win.draw(decors); // armoire salle 1
   decors.setPosition(700,375); decors.setTextureRect(sf::IntRect( 0, 110, 100, 90));  win.draw(decors);  // ventilation 1
   decors.setPosition(2500,330); decors.setTextureRect(sf::IntRect( 250, 10, 70, 80));  win.draw(decors);  // door key
   decors.setPosition(5300,375); decors.setTextureRect(sf::IntRect( 0, 110, 100, 90));  win.draw(decors);  // ventil 2
   decors.setPosition(9800,100); decors.setTextureRect(sf::IntRect( 100, 0, 100, 100));  win.draw(decors);  // medikit
   decors.setPosition(xj-80,400); decors.setTextureRect(sf::IntRect( 150,120, 220, 100));  win.draw(decors);  // jeep
   decors.setPosition(12800,200); decors.setTextureRect(sf::IntRect( 0, 110, 100, 90)); win.draw(decors);  // ventil decors
   decors.setPosition(13800,200); decors.setTextureRect(sf::IntRect( 0, 110, 100, 90));  win.draw(decors);  // ventil decors
   decors.setPosition(17500,250); decors.setTextureRect(sf::IntRect( 0, 250, 150, 100));  win.draw(decors); // fleche decors
   decors.setPosition(18500,200); decors.setTextureRect(sf::IntRect( 0, 110, 100, 90));  win.draw(decors);  // ventil decors
   decors.setPosition(19200,250); decors.setTextureRect(sf::IntRect( 0, 250, 150, 100));  win.draw(decors); // fleche decors
   decors.setPosition(19700,320); decors.setTextureRect(sf::IntRect( 200, 250, 100, 90));  win.draw(decors);  //  masque
   decors.setPosition(20500,375); decors.setTextureRect(sf::IntRect( 0, 110, 100, 90));  win.draw(decors);  // ventil 3
   decors.setPosition(23800,375); decors.setTextureRect(sf::IntRect( 0, 110, 100, 90));  win.draw(decors);  // ventilation 4
   decors.setPosition(26700,750); decors.setTextureRect(sf::IntRect( 205, 400, 80, 100));  win.draw(decors);  // biohazard
   decors.setPosition(27160,785); decors.setTextureRect(sf::IntRect( 0, 385, 175, 115));  win.draw(decors);  // labo
   decors.setPosition(27420,710); decors.setTextureRect(sf::IntRect( 500, 200, 290, 195)); decors.setScale(0.65,0.65); win.draw(decors);  // labo
     decors.setScale(1,1);
     if (n!=11) {object.setPosition(28020,838); object.setScale(0.6,0.6); object.setTextureRect(sf::IntRect( 1320, 0, 110, 100));  win.draw(object);}  // bouteille o2
     decors.setTextureRect(sf::IntRect( 350, 250+evenement[22]*43, 100, 50));   decors.setPosition(28137,800);  win.draw(decors);      // levier ouverture sas
     if (object_inv[8]==0 && evenement[13]==0) {object.setPosition(12050,466); object.setScale(0.3,0.3); object.setTextureRect(sf::IntRect( 960, 0, 120, 105));   win.draw(object); } // batterie
     object.setScale(1,1);
     if (evenement[10]==1) { object.setTextureRect(sf::IntRect( 720, 0, 120, 105));   object.setPosition(x-120,y-470);   win.draw(object);}  // affichage gas mask in use
     if (evenement[7]==1)  { object.setTextureRect(sf::IntRect( 120, 0, 120, 105));   object.setPosition(x-120,y-470);   win.draw(object);}  // affichage torch in use

    std::cout<<"  x= "<<x<<"    y= "<<y<<std::endl;
  }

void afficheViewTableau()
  {
     win.setView(vueT);
     rectangle(0,0,200,600,1);                                                                 // carre blanc
     rectangle(5,5,190,590,0);                                                                // carre noir sur le blanc
     titre.setString("Objectif");   titre.setPosition(20,30); titre.setCharacterSize(30);     win.draw(titre);
     titre.setString("APL");   titre.setPosition(40,75); titre.setCharacterSize(45);          win.draw(titre);
     texte.setString("LIFE:");  texte.setPosition(20,200);                                    win.draw(texte);
     texte.setString("OBJECT:");  texte.setPosition(20,400);                                  win.draw(texte);
     object.setTextureRect(sf::IntRect( n*120, 0, 120, 105));
     object.setScale(1,1); object.setPosition(30,450);                                                           win.draw(object);
     rectangle( 30,250,30+vie,10,1);  // affiche barre de vie
  }

void echelle (int xEch,int yH, int yL, int yI1, int yI2)
   {
    //rectangle(400,500,300,5,1);
    rectangle((xEch-35),yH-150,150,(yL-yH+150),0);   // rectangle noir pour le fond
    for (int i=0; i<=50; i+=50) { rectangle (xEch+10+i,yH-50,4,yL-yH+50,1) ; }  // montants de l'echelle
    for (int i=(yH-20); i<=(yL-10); i+=20) { rectangle (xEch+10,i,50,4,1) ; }   // barreaux de l'echelle

  //testColisionEchelle
   if (x>=xEch-20 && x<=xEch+10  && y>yH && y<yL)                    { R=0; L=0; U=1; D=1; }            // sur l'echelle
   if (x>=xEch-20 && x<=xEch+10 && y==yL)      { D=0; U=1; R= 1; L=1; }          // au sol mais au pied de l'echelle
   if (x>=xEch-20 && x<=xEch+10 && y==yH )       { U=0; D=1; R= 1; L=1; }        // en haut echelle 1
   if (yI1!=0 && x>=xEch-20 && x<=xEch+10 && abs(y-yI1)<40)    {U=1; D=1; R= 1; L=1;       }
   if (yI2!=0 && x>=xEch-20 && x<=xEch+10 && abs(y-yI2)<40)    { U=1; D=1; R= 1; L=1;      }   // rectangle  pour le fond
   if (y==-500) { L=0; R=0; }
}

void rectangle(int x,int y, int lx, int ly, int c  )
  {
    int r, g, b,a;
    switch(light) { case 0: a=5; break;
                    case 1: a=100; break;
                    default: a=255; break;}
    switch(c)
    {
      case 1: r=255; g=255; b=255; break;   // blanc
      case 2: r=121; g=200; b=0; break;
      case 3: r=255; g=121; b=0; break;     // orange
      case 4: r=230; g=81; b=0; break;      // orange fonce
      case 6 : r=0; g=0; b=200; break;
      case 9: r=128; g=150; b=43; a=comptGaz;  break;      // couleur de masquage pas de lumiere
      default: r=0; g=0; b=0; break;        // noir

    }
    rect.setFillColor(sf::Color(r,g,b,a));
    rect.setPosition( x, y);
    rect.setSize(sf::Vector2f(lx,ly));
     win.draw(rect);
   }

void explosion()
   { int xx=15,yy=5;
     for (int i=1; i<50; i++)
       {
         win.clear();
         xx=-xx; yy=-yy;
         sf::sleep(sf::milliseconds(50));
         vueP.setCenter(sf::Vector2f(x+250+xx, y-200+yy));
         afficheViewPrincipale();
         afficheViewTableau();
         win.display();
        }
   if (evenement[22]==0 || x<27750) { mes="you died in the explosion..."; afficheMessage(t,compt); sf::sleep(sf::seconds(5));fin=1; }
   }


