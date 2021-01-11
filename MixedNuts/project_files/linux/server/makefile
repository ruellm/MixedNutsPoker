# source files.
SRC = OmahaHoldem.cpp \
      ServerLobby.cpp  \
	  TexasHoldem.cpp \
	  GameFactory.cpp  \
	  PacketSender.cpp   \
 	  ServerMain.cpp  \
   	  ClientSession.cpp \
	  HighLowBase.cpp \
	  PacketSenderGame.cpp \
	  SevenCardStud.cpp \
      DataManager.cpp \
	  IOServicePool.cpp \
	  PokerGameBase.cpp  \
 	  Shuffler.cpp  \
	  Deck.cpp \
	  Limit.cpp \
	  Razz.cpp \
	  SitAndGo.cpp \
  	  GameBase.cpp  \
	  OmahaHiLo.cpp  \
	  ServerControl.cpp \
	  StudHiLo.cpp \
	  ConfigManager.cpp

OBJ = $(SRC:.cpp=.o)

OUT = ../../Out/server

# include directories
INCLUDES = -I. -I/usr/local/include -I../../../boost/boost_1_42_0 -I../../system/include -I../../pokerlib/include -I../include/

# C++ compiler flags (-g -O2 -Wall)
CCFLAGS = -g

# compiler
CCC = g++

# library paths
LIBS = -L../ -L/usr/local/lib -lm -ldl -lpthread 

# compile flags
LDFLAGS = -g

# boost libraries
BOOSTLIB = ../../../boost/boost_1_42_0/stage/lib/libboost_system.a ../../../boost/boost_1_42_0/stage/lib/libboost_thread.a

# poker lib dependencies
POKERDEP = ../../Out/libpoker.a ../../Out/libsystem.a

.SUFFIXES: .cpp

.cpp.o:
	$(CCC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

$(OUT): $(OBJ)
	$(CCC) -o $(OUT) $(OBJ) $(INCLUDE) $(BOOSTLIB) $(POKERDEP) $(LIBS)

clean:
	rm -f $(OBJ) $(OUT) Makefile.bak 
