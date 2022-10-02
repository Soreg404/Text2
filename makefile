CC=g++

SELF_DIR=$(dir $(abspath $(lastword $(MAKEFILE_LIST))))

SHELL=cmd

BIN=bin/mingw/
INT=bin/mingw/int/

PROJ=Text2/
SRC=$(PROJ)src/

SDKP=$(EXT_DRIVE)/lib/C++

INCLUDE_PATHS=$(PROJ)include $(SDKP) $(SDKP)/freetype
INCL=$(addprefix -I,$(INCLUDE_PATHS))
HEADERS=$(PROJ)include/text.hpp $(SRC)glsrv.hpp

LIBRARY_PATHS=glfw/lib/x64/lib-mingw-w64 freetype/lib/release/x64/static
LIB=$(addprefix -L$(SDKP)/,$(LIBRARY_PATHS))
LIBS= $(SDKP)/glew/lib/Release/x64/glew32s.lib -lglfw3 -lfreetype -lgdi32 -lopengl32

MAIN_OBJ=$(INT)main.o
OBJ_NAMES=util text glsrv
OBJS=$(addprefix $(INT),$(addsuffix .o,$(OBJ_NAMES)))



$(BIN)main.exe: $(OBJS) $(MAIN_OBJ) $(HEADERS) $(PROJ)include/util.h.gch
	@echo --- building program ---
	$(CC) $(OBJS) $(MAIN_OBJ) $(LIB) $(LIBS) -o $@

CMP=$(CC) -c $^ $(INCL) -o $@

$(OBJS): $(INT)%.o : $(SRC)%.cpp
	@echo --- compiling $(notdir $^) ---
	$(CMP)

$(MAIN_OBJ): $(PROJ)test.cpp
	@echo --- compiling $(notdir $^) to main.o ---
	$(CMP)

$(HEADERS):
	@echo header: $@

$(PROJ)include/util.h.gch: $(PROJ)include/util.h
	@echo --- precompiled headers ---
	$(CC) $^

clean:
	@echo -- cleaning --
	@del $(subst /,\,$(INT)*) /Q
