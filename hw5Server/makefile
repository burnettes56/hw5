client: client.o SafeQueue.o
	g++ client.o SafeQueue.o -lpthread -o hw5Client
client.o: client.cc SafeQueue.h
	g++ -c client.cc
server: server.o SetupData.o Log.o SafeQueue.o
	g++ server.o SetupData.o Log.o SafeQueue.o -lpthread -o hw5Server
server.o: server.cc
	g++ -c server.cc
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
