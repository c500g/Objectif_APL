# Objectif_APL
litlle C++ game inspired from old vintage "Captain Goodnight" on Apple II

## install of smfl librairy for C++:
sudo apt-get install libsfml-dev

## compil/link program:
download main.cpp and sources directory (that includes image and font)  
compil : g++ -c main.cpp  
link and create executable : g++ main.o -o objectif_APL -lsfml-graphics -lsfml-window -lsfml-system

## usage: 
./objectif_APL.app

## synposis
You are a secret agent and you must blow-up a laboratory inside a secret base  
The adventure begins at the entry of the base. You'll have to explore, fight, shoot, look for object, find hidden passage, open doors, be careful of toxic gas, repair vehicule, put explosives in the lab and rush to escape before the big boom.  
You start with only a knife in your inventory

## how to play:
When you fight with the knife or shooting with the gun, the important rule is to in opposite position with your oponent in order to touch him and avoiding to be touched. This means you have to stand when he is kneeing and you have to be on your knee when he's standing (up and won arrow keys)  
  * arrow keys : move left, right, up, down  
  * return     : pick-up an object (if there is one !)  
  * space      : use the selected object  
  * tab        : choose/ select object in inventory
     
timing is not really an issue in the game (at least until you use explosive)
