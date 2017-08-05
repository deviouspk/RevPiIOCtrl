IDIR=./include
SRCDIR=./src
ODIR=./build
LIBS=-lrestclient-cpp -lonposix -lcurl
CC=g++
CFLAGS=-I$(IDIR) -L./libs -L/usr/local/lib

_DEPS = piControl.h piControlIf.hpp ThreadSynchronization.hpp HttpThread.hpp Authentication.hpp IOHandler.hpp CentralProcessing.hpp RevPiIOCtrl.hpp
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = jsoncpp.o piControlIf.o ThreadSynchronization.o HttpThread.o Authentication.o IOHandler.o CentralProcessing.o RevPiIOCtrl.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

$(ODIR)/%.o: $(SRCDIR)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

RevPiIOCtrl: $(OBJ)
	g++ -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o RevPiIOCtrl