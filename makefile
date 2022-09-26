CC=g++

BIN=bin/mingw/
INT=bin/mingw/int/

INCL=include $(EXT_DRIVE)/lib/C++ $(EXT_DRIVE)/lib/C++/freetype
HEADERS=include/*

LIBF= glew/lib/Release/x64 glfw/lib/x64/lib-mingw-w64 freetype/lib/release/x64/static
LIBS= $(EXT_DRIVE)/lib/C++/glew/lib/Release/x64/glew32s.lib -lfreetype -lglfw3 -lgdi32 -lopengl32

FILES=glsrv text
OBJS=$(addprefix $(INT),$(addsuffix .o,$(FILES)))

SRC=src/

$(BIN)main.exe: $(OBJS) $(HEADERS) $(INT)util.o $(INT)main.o
	$(CC) $(INT)* $(addprefix -L$(EXT_DRIVE)/lib/C++/,$(LIBF)) $(LIBS) -o $@

$(HEADERS):
	echo $@

$(INT)util.o: util.cpp
	$(CC) -c $^ -o $@ $(addprefix -I,$(INCL))
	
$(INT)main.o: main.cpp
	$(CC) -c $^ -o $@ $(addprefix -I,$(INCL))

$(OBJS):
	$(CC) -c $(SRC)$(basename $(notdir $@)).cpp $(addprefix -I,$(INCL)) -o $@
