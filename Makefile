CC=g++
CCFLAGS= -std=c++17
LIBFLAGS= -static-libstdc++

OBJDIR=objects
BINDIR=bin

INC_REX=src
INC_FLAG=../flag/src/
INC_EVER=../ever/src
INC_WALK=../walk/src/

all: clean list merge shuf split

clean: cleanbin cleanobj

cleanbin:
	rm $(BINDIR)/*

cleanobj:
	rm $(OBJDIR)/*

mkall: mkobj mkbin

mkobj:
	mkdir -p $(OBJDIR)

mkbin:
	mkdir -p $(BINDIR)

list: mkall $(OBJDIR)/list.o $(OBJDIR)/util.o $(OBJDIR)/pth.o $(OBJDIR)/pdh.o $(OBJDIR)/hrdl.o \
		$(OBJDIR)/flag.o $(OBJDIR)/ever.o
	$(CC) $(CCFLAGS) $(LIBFLAGS) -o $(BINDIR)/rexlist $(OBJDIR)/list.o \
					$(OBJDIR)/ever.o $(OBJDIR)/flag.o $(OBJDIR)/util.o \
					$(OBJDIR)/pth.o $(OBJDIR)/pdh.o $(OBJDIR)/hrdl.o \
					-I $(INC_REX) -I $(INC_FLAG)/ -I $(INC_EVER) -I $(INC_WALK)

merge: mkall $(OBJDIR)/merge.o $(OBJDIR)/util.o $(OBJDIR)/pth.o $(OBJDIR)/pdh.o $(OBJDIR)/hrdl.o \
		$(OBJDIR)/flag.o $(OBJDIR)/ever.o
	$(CC) $(CCFLAGS) $(LIBFLAGS) -o $(BINDIR)/rexmerge $(OBJDIR)/merge.o \
					$(OBJDIR)/ever.o $(OBJDIR)/flag.o $(OBJDIR)/util.o \
					$(OBJDIR)/pth.o $(OBJDIR)/pdh.o $(OBJDIR)/hrdl.o \
					-I $(INC_REX) -I $(INC_FLAG)/ -I $(INC_EVER) -I $(INC_WALK)

shuf: mkall $(OBJDIR)/shuf.o $(OBJDIR)/rt.o $(OBJDIR)/flag.o $(OBJDIR)/ever.o
	$(CC) $(CCFLAGS) $(LIBFLAGS) -o $(BINDIR)/rexshuf $(OBJDIR)/shuf.o \
					$(OBJDIR)/ever.o $(OBJDIR)/flag.o $(OBJDIR)/rt.o \
					-I $(INC_REX) -I $(INC_WALK)/ -I $(INC_FLAG)/ -I $(INC_EVER)

split: mkall $(OBJDIR)/split.o $(OBJDIR)/rt.o $(OBJDIR)/flag.o $(OBJDIR)/ever.o
	$(CC) $(CCFLAGS) $(LIBFLAGS) -o $(BINDIR)/rexsplit $(OBJDIR)/split.o \
					$(OBJDIR)/ever.o $(OBJDIR)/flag.o $(OBJDIR)/rt.o \
					-I $(INC_REX) -I $(INC_WALK)/ -I $(INC_FLAG)/ -I $(INC_EVER)

$(OBJDIR)/merge.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/merge.o -c $(INC_REX)/client/merge.cc \
					-I $(INC_REX) -I $(INC_EVER) -I $(INC_FLAG)

$(OBJDIR)/list.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/list.o -c $(INC_REX)/client/list.cc \
					-I $(INC_REX) -I $(INC_EVER) -I $(INC_FLAG)

$(OBJDIR)/shuf.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/shuf.o -c $(INC_REX)/client/shuf.cc \
					-I $(INC_REX)  -I $(INC_EVER)  -I $(INC_FLAG)

$(OBJDIR)/split.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/split.o -c $(INC_REX)/client/split.cc \
					-I $(INC_REX)  -I $(INC_EVER)  -I $(INC_FLAG)

$(OBJDIR)/rt.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/rt.o -c $(INC_REX)/rt.cc \
					-I $(INC_REX) -I $(INC_WALK) -I $(INC_EVER)

$(OBJDIR)/flag.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/flag.o -c $(INC_FLAG)/flag.cpp -I $(INC_FLAG)

$(OBJDIR)/ever.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/ever.o -c $(INC_EVER)/ever.cpp -I $(INC_EVER)

$(OBJDIR)/util.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/util.o -c $(INC_REX)/util.cc -I $(INC_REX) -I $(INC_EVER)

$(OBJDIR)/pth.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/pth.o -c $(INC_REX)/pth.cc -I $(INC_REX) -I $(INC_EVER)

$(OBJDIR)/pdh.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/pdh.o -c $(INC_REX)/pdh.cc -I $(INC_REX) -I $(INC_EVER)

$(OBJDIR)/hrdl.o: mkobj
	$(CC) $(CCFLAGS) -o $(OBJDIR)/hrdl.o -c $(INC_REX)/hrdl.cc -I $(INC_REX) -I $(INC_EVER)
