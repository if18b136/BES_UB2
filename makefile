all: gridserver vehicleclient griddisplay

gridserver: gridserver.cpp
	g++ gridserver.cpp -o gridserver

vehicleclient: vehicleclient.cpp
	g++ vehicleclient.cpp -o vehicleclient

griddisplay:
	g++ griddisplay.cpp -o griddisplay

clean: gridserver.cpp vehicleclient.cpp
			rm -f gridserver vehicleclient griddisplay
