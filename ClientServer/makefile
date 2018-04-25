all: hw5Client hw5Server
hw5Client: hw5Client.o SafeQueue.o
	g++ hw5Client.o SafeQueue.o -lpthread -o hw5Client
hw5Client.o: hw5Client.cc SafeQueue.h
	g++ -c hw5Client.cc
hw5Server: hw5Server.o SetupData.o Log.o SafeQueue.o
	g++ hw5Server.o SetupData.o Log.o SafeQueue.o -lpthread -o hw5Server
hw5Server.o: hw5Server.cc
	g++ -c hw5Server.cc
SetupData.o: SetupData.cc SetupData.h
	g++ -c SetupData.cc
Log.o: Log.cc Log.h
	g++ -c Log.cc
SafeQueue.o: SafeQueue.cc SafeQueue.h
	g++ -c SafeQueue.cc
clean:
	rm *.o
	rm -f hw5Server
	rm -f hw5Client
cmake:
	make clean
	make
