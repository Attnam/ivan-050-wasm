# Iter Vehemens ad Necem makefile for DJGPP environment

# Copyrights (C) Timo Kiviluoto / IvanDev 2000-2002

AR       = ar rs
CC       = gcc -o
FeLibDIR = FeLib
FeLibOBJ = $(FeLibDIR)/Source/bitmap.o $(FeLibDIR)/Source/colorbit.o $(FeLibDIR)/Source/graphics.o\
           $(FeLibDIR)/Source/error.o $(FeLibDIR)/Source/femain.o $(FeLibDIR)/Source/save.o\
           $(FeLibDIR)/Source/feio.o $(FeLibDIR)/Source/felist.o $(FeLibDIR)/Source/hscore.o\
           $(FeLibDIR)/Source/femath.o $(FeLibDIR)/Source/whandler.o
FeLibASM = $(FeLibDIR)/Source/gccblit.o
IVANDIR  = Main
IVANBIN  = Ivan.exe
IVANOBJ  = $(IVANDIR)/Source/charba.o $(IVANDIR)/Source/area.o $(IVANDIR)/Source/team.o\
           $(IVANDIR)/Source/charde.o $(IVANDIR)/Source/cont.o $(IVANDIR)/Source/dungeon.o\
           $(IVANDIR)/Source/game.o $(IVANDIR)/Source/godba.o $(IVANDIR)/Source/godde.o\
           $(IVANDIR)/Source/itemba.o $(IVANDIR)/Source/itemde.o $(IVANDIR)/Source/level.o\
           $(IVANDIR)/Source/lsquare.o $(IVANDIR)/Source/lterraba.o $(IVANDIR)/Source/lterrade.o\
           $(IVANDIR)/Source/main.o $(IVANDIR)/Source/materba.o $(IVANDIR)/Source/materde.o\
           $(IVANDIR)/Source/message.o $(IVANDIR)/Source/object.o $(IVANDIR)/Source/pool.o\
           $(IVANDIR)/Source/proto.o $(IVANDIR)/Source/roomba.o  $(IVANDIR)/Source/roomde.o\
           $(IVANDIR)/Source/script.o $(IVANDIR)/Source/square.o $(IVANDIR)/Source/stack.o\
           $(IVANDIR)/Source/wsquare.o $(IVANDIR)/Source/type.o $(IVANDIR)/Source/worldmap.o\
           $(IVANDIR)/Source/wskill.o $(IVANDIR)/Source/wterraba.o $(IVANDIR)/Source/wterrade.o\
           $(IVANDIR)/Source/config.o $(IVANDIR)/Source/terra.o $(IVANDIR)/Source/entity.o\
           $(IVANDIR)/Source/fluid.o $(IVANDIR)/Source/unit.o $(IVANDIR)/Source/igraph.o\
           $(IVANDIR)/Source/identity.o $(IVANDIR)/Source/slot.o
FLAGS = -IInclude -I$(FeLibDIR)/Include -s -O3 -ffast-math -W -Wall
LIBS = -lstdcxx

all:	$(IVANBIN)

$(FeLibOBJ) : %.o : %.cpp
	@echo Compiling $@...
	@$(CC) $@ -c $< $(FLAGS)

$(FeLibASM) : %.o : %.s
	@echo Compiling $@...
	@$(CC) $@ -c $< $(FLAGS)

$(IVANOBJ) : %.o : %.cpp
	@echo Compiling $@...
	@$(CC) $@ -c $< $(FLAGS) -I$(IVANDIR)/Include

$(IVANBIN) : $(FeLibOBJ) $(FeLibASM) $(IVANOBJ)
	@echo Compiling $(IVANBIN)...
	@$(CC) $(IVANBIN) $(FeLibOBJ) $(FeLibASM) $(IVANOBJ) $(LIBS)
