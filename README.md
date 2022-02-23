# SFMLSoftBody

This is a soft body simulation based on the implementation described in this video - https://www.youtube.com/watch?v=kyQP4t_wOGI&t=2s

## Why I made this

Personaly I have always enjoyed the rather fun results one can get from physics simulations and with my A Level required practical being any project of choice I decided to give a 
second attempt at a soft body simulation. This newer version usesSFML to provide the graphical display which has been a much faster/simpler alternative to unity which I have used 
previously.

## What can it be used for

The simulation, whilst being fun to play with, can serve as a teaching tool by showing what the spring mass model between atoms really looks like.  

## How to run

Simply download the repository and move all of the .dll files in the lib folder into the main folder and launch main.exe. In reality main.exe only requires the .dll files and the 
font file, everything else is simply for building the project.

## How to use

Beware the program does not currently have a slick way of being closed, however simply pressing alt - f4 will close the window.
When the program is started you should be greeted by something which looks like this. 
![image](https://user-images.githubusercontent.com/95449040/155342601-1cdcbe69-e7fc-49e1-9818-5ddfc60135d4.png)
The window can be resized and moved to the desired location. The sliders will control properties of the simulation live, except for the size of the simulation which requires a sim
reset to take effect. The two numbers in red in the top right display two different fps counters. The left of the two, displays the number of visual frames being rendered per 
second, whereas the right shows the number of simulation frames per second, which is highly dependent on the number of mass points.

## Futher steps

The simulation sofware is currently limited to the predefined scene of polygons, which is fairly uninteresting, despite the software being capable of containing many many more.
