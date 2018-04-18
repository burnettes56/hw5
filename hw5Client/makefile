client: client.o SafeQueue.o
	g++ client.o SafeQueue.o -lpthread -o hw5Client
client.o: client.cc SafeQueue.h
	g++ -c client.cc
SafeQueue.o: SafeQueue.cc SafeQueue.h
	g++ -c SafeQueue.cc
clean:
	rm *.o
	rm -f hw5Client
cmake:
	make clean
	make
