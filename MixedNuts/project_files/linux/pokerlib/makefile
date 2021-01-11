# source files.
SRC = NetPacketGame.cpp \
      Player.cpp \
      Card.cpp \
      NetPacketLobbyMsg.cpp \
      PokerHandsLogic.cpp \
	  NetPacketFactory.cpp \
      PacketParser.cpp  \
	  Table.cpp

OBJ = $(SRC:.cpp=.o)

OUT = ../../Out/libpoker.a

# include directories
INCLUDES = -I. -I/usr/local/include -I../../../boost/boost_1_42_0 -I../../system/include -I../include/

# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -g

# compiler
CCC = g++

# library paths
LIBS = -L../ -L/usr/local/lib -lm -l../../Out

# compile flags
LDFLAGS = -g

.SUFFIXES: .cpp

.cpp.o:
	$(CCC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	ar rcs $(OUT) $(OBJ)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak 
