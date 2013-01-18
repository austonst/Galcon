Galcon-like Strategy Game
=========================

Galcon (working title) is a real time strategy game in which players fight over scattered planets and build fleets and stuff. While based off of games like Galcon and Eufloria, this game intends to introduce aspects such as building construction, varying ship types, and ship-to-ship interception.

I am currently working on this project by myself in my spare time, but I'd appreciate any playtesting, debugging, or development assistance. Feel free to write up an issue, make a pull request, or email me at austonst@gmail.com

Installation
------------

Galcon's dependencies should be fairly simple to get through most distribution's package manager. Ensure that you have installed:

* SDL
* SDL_image
* SDL_ttf

Currently, Galcon can be built using the provided makefile. It's designed for Linux systems using g++, though with a few adaptations, it can likely work on other platforms. Building is as simple as running:

    make

This will produce an executable binary in the same directory which can be run with:

    ./galcon
