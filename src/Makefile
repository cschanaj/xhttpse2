CC=gcc
CFLAGS=-c -I. -I./modules/ -D_GNU_SOURCE
DFLAGS=-I. -D_GNU_SOURCE
SOURCES=HttpseVector.c HttpseRequest.c HttpseCode.c ./modules/StatusCode.c \
./modules/SecureFallback.c ./modules/DifferentContent.c \
./modules/MixedContent.c HttpseXData.c HttpseRuleset.c Main.c
OBJECTS=$(SOURCES:.c=.o)
C_LIB=-lcurl -lgumbo -lpthread
EXECUTABLE=xhttpse

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(DFLAGS) $(OBJECTS) $(C_LIB) -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm $(OBJECTS)

clean_all:
	rm $(OBJECTS) $(EXECUTABLE)
