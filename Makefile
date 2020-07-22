CC=g++
CCFLAGS= -std=c++17 -g
EXEFLAGS= -static-libstdc++

OBJDIR=objects
BINDIR=bin

REX=rex

INC_REX=src
INC_FLAG=../flag/src
INC_EVER=../ever/src
INC_WALK=../walk/src

all: clean list merge shuf split check

# ------------------------------------------------------------------------------
clean: cleanbin cleanobj

cleanbin:
	rm -f $(BINDIR)/*

cleanobj:
	rm -f $(OBJDIR)/*

mkall: mkobj mkbin

mkobj:
	mkdir -p $(OBJDIR)

mkbin:
	mkdir -p $(BINDIR)

# ------------------------------------------------------------------------------
list: mkall $(OBJDIR)/list.o $(OBJDIR)/util.o $(OBJDIR)/rt.o $(OBJDIR)/pth.o $(OBJDIR)/pdh.o $(OBJDIR)/hrdl.o \
		$(OBJDIR)/walk.o $(OBJDIR)/flag.o $(OBJDIR)/ever.o
	$(CC) $(CCFLAGS) $(EXEFLAGS) -o $(BINDIR)/$(REX)$@ $(OBJDIR)/list.o \
					$(OBJDIR)/ever.o $(OBJDIR)/flag.o $(OBJDIR)/util.o $(OBJDIR)/walk.o $(OBJDIR)/rt.o \
					$(OBJDIR)/pth.o $(OBJDIR)/pdh.o $(OBJDIR)/hrdl.o \
					-I $(INC_FLAG)/ -I $(INC_EVER) -I $(INC_WALK) -I $(INC_REX)

merge: mkall $(OBJDIR)/merge.o $(OBJDIR)/util.o $(OBJDIR)/rt.o $(OBJDIR)/pth.o $(OBJDIR)/pdh.o $(OBJDIR)/hrdl.o \
		$(OBJDIR)/walk.o $(OBJDIR)/flag.o $(OBJDIR)/ever.o
	$(CC) $(CCFLAGS) $(EXEFLAGS) -o $(BINDIR)/$(REX)$@ $(OBJDIR)/merge.o \
					$(OBJDIR)/ever.o $(OBJDIR)/flag.o $(OBJDIR)/util.o $(OBJDIR)/walk.o $(OBJDIR)/rt.o \
					$(OBJDIR)/pth.o $(OBJDIR)/pdh.o $(OBJDIR)/hrdl.o \
					-I $(INC_REX) -I $(INC_WALK) -I $(INC_FLAG)/ -I $(INC_EVER)

check: mkall $(OBJDIR)/check.o $(OBJDIR)/util.o $(OBJDIR)/rt.o $(OBJDIR)/pth.o $(OBJDIR)/pdh.o $(OBJDIR)/hrdl.o \
		$(OBJDIR)/walk.o $(OBJDIR)/flag.o $(OBJDIR)/ever.o
	$(CC) $(CCFLAGS) $(EXEFLAGS) -o $(BINDIR)/$(REX)$@ $(OBJDIR)/check.o \
					$(OBJDIR)/ever.o $(OBJDIR)/flag.o $(OBJDIR)/util.o $(OBJDIR)/walk.o $(OBJDIR)/rt.o \
					$(OBJDIR)/pth.o $(OBJDIR)/pdh.o $(OBJDIR)/hrdl.o \
					-I $(INC_REX) -I $(INC_FLAG)/ -I $(INC_EVER) -I $(INC_WALK)

shuf: mkall $(OBJDIR)/shuf.o $(OBJDIR)/rt.o $(OBJDIR)/flag.o $(OBJDIR)/ever.o $(OBJDIR)/walk.o
	$(CC) $(CCFLAGS) $(EXEFLAGS) -o $(BINDIR)/$(REX)$@ $(OBJDIR)/shuf.o \
					$(OBJDIR)/ever.o $(OBJDIR)/flag.o $(OBJDIR)/rt.o  $(OBJDIR)/walk.o\
					-I $(INC_REX) -I $(INC_WALK)/ -I $(INC_FLAG)/ -I $(INC_EVER)

split: mkall $(OBJDIR)/split.o $(OBJDIR)/rt.o $(OBJDIR)/flag.o $(OBJDIR)/ever.o $(OBJDIR)/walk.o
	$(CC) $(CCFLAGS) $(EXEFLAGS) -o $(BINDIR)/$(REX)$@ $(OBJDIR)/split.o \
					$(OBJDIR)/ever.o $(OBJDIR)/flag.o $(OBJDIR)/rt.o  $(OBJDIR)/walk.o\
					-I $(INC_REX) -I $(INC_WALK)/ -I $(INC_FLAG)/ -I $(INC_EVER)

# ------------------------------------------------------------------------------
$(OBJDIR)/check.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_REX)/client/check.cc \
					-I $(INC_REX) -I $(INC_EVER) -I $(INC_FLAG)

$(OBJDIR)/merge.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_REX)/client/merge.cc \
					-I $(INC_REX) -I $(INC_EVER) -I $(INC_FLAG)

$(OBJDIR)/list.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_REX)/client/list.cc \
					-I $(INC_REX) -I $(INC_EVER) -I $(INC_FLAG)

$(OBJDIR)/shuf.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_REX)/client/shuf.cc \
					-I $(INC_REX)  -I $(INC_EVER)  -I $(INC_FLAG)

$(OBJDIR)/split.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_REX)/client/split.cc \
					-I $(INC_REX)  -I $(INC_EVER)  -I $(INC_FLAG)

# ------------------------------------------------------------------------------
$(OBJDIR)/rt.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_REX)/rt.cc \
					-I $(INC_REX) -I $(INC_WALK) -I $(INC_EVER)

$(OBJDIR)/walk.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_WALK)/walk.cc -I $(INC_WALK)

$(OBJDIR)/flag.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_FLAG)/flag.cpp -I $(INC_FLAG)

$(OBJDIR)/ever.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_EVER)/ever.cpp -I $(INC_EVER)

$(OBJDIR)/util.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_REX)/util.cc -I $(INC_REX) -I $(INC_EVER)

$(OBJDIR)/pth.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_REX)/pth.cc \
						-I $(INC_REX) -I $(INC_EVER) -I $(INC_WALK)

$(OBJDIR)/pdh.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_REX)/pdh.cc -I $(INC_REX) -I $(INC_EVER)

$(OBJDIR)/hrdl.o: mkobj
	$(CC) $(CCFLAGS) -o $@ -c $(INC_REX)/hrdl.cc -I $(INC_REX) -I $(INC_EVER)
