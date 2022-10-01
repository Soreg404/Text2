CC=g++

BIN=bin/mingw/
INT=bin/mingw/int/

INCLD=include $(EXT_DRIVE)/lib/C++ $(EXT_DRIVE)/lib/C++/freetype
INCL=$(addprefix -I,$(INCLD))
HEADERS=include/*

LIB_PR= glew/lib/Release/x64 glfw/lib/x64/lib-mingw-w64 freetype/lib/release/x64/static
LIB=$(addprefix -L$(EXT_DRIVE)/lib/C++/,$(LIB_PR))
LIBS= $(EXT_DRIVE)/lib/C++/glew/lib/Release/x64/glew32s.lib -lfreetype -lglfw3 -lgdi32 -lopengl32

OBJ_N=main util text glsrv
OBJS=$(addprefix $(INT),$(addsuffix .o,$(OBJ_N)))

SRC=src/

$(BIN)main.exe: $(OBJS) $(HEADERS) include/util.h.gch
	$(CC) $(OBJS) $(LIB) $(LIBS) -o $@

./include/util.h.gch: ./include/util.h
	$(CC) $^

$(HEADERS):
	echo $@


$(INT)util.o: util.cpp
	$(CC) -c $^ $(INCL) -o $@
	
$(INT)main.o: main.cpp
	$(CC) -c $^ $(INCL) -o $@ 

$(INT)glsrv.o: $(SRC)glsrv.cpp
	$(CC) -c $^ $(INCL) -o $@

$(INT)text.o: $(SRC)text.cpp
	$(CC) -c $^ $(INCL) -o $@

clean:
	del $(subst /,\,$(INT)*) /Q
