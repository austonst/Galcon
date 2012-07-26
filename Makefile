CC=g++
CFLAGS=-g -c -Wall -std=c++0x
LDFLAGS=-Wall -lSDLmain -lSDL -lSDL_image -lSDL_ttf -std=c++0x
OUTPUT=-o galcon

all: galcon

galcon: building.o fleet.o planet.o rotationcache.o scale.o galcon.o projectile.o buildingInstance.o ai.o
	$(CC) building.o fleet.o planet.o rotationcache.o scale.o galcon.o projectile.o buildingInstance.o ai.o $(LDFLAGS) $(OUTPUT)

galcon.o: galcon.cpp planet.o fleet.o ai.o vec2f.h
	$(CC) galcon.cpp $(CFLAGS)

building.o: building.cpp building.h rotationcache.o vec2f.h
	$(CC) building.cpp $(CFLAGS)

fleet.o: fleet.cpp fleet.h planet.o vec2f.h
	$(CC) fleet.cpp $(CFLAGS)

planet.o: planet.cpp planet.h scale.o rotationcache.o buildingInstance.o vec2f.h
	$(CC) planet.cpp $(CFLAGS)

rotationcache.o: rotationcache.cpp rotationcache.h
	$(CC) rotationcache.cpp $(CFLAGS)

scale.o: scale.cpp scale.h
	$(CC) scale.cpp $(CFLAGS)

projectile.o: projectile.cpp projectile.h vec2f.h
	$(CC) projectile.cpp $(CFLAGS)

buildingInstance.o: buildingInstance.cpp buildingInstance.h building.o vec2f.h
	$(CC) buildingInstance.cpp $(CFLAGS)

ai.o: ai.cpp ai.h planet.h fleet.h
	$(CC) ai.cpp $(CFLAGS)
