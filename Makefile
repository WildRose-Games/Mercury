CC := clang
WARN := -Wall -Wextra -Wpedantic -std=c99
LIBS := -lm -ldl
PLAT := -lSDL2 -lGL
INC := -Ilib/cglm/include
SO := -fPIC -shared
EXPORT := -Wl,--export-dynamic

FLAGS := $(WARN) $(INC) $(LIBS)

HG := Hg
NAME := sandbox

DBG := -DHG_BUILD_DEBUG -O0 -g 

debug: gameDebug
	$(CC) $(FLAGS) $(PLAT) src/$(HG)/sdl2-$(HG).c $(DBG) -o bin/$(NAME)_dbg $(EXPORT)

gameDebug:
		$(CC) $(FLAGS) src/$(NAME).c $(DBG) -o bin/libGameDbg.so $(SO) 

release: 
	$(CC) $(FLAGS) $(PLAT) src/$(NAME).c src/$(HG)/sdl2-$(HG).c -O2 -o bin/$(NAME)

all: linuxDebug, linux

run: 
	( cd bin; ./$(NAME)_dbg )

runRelease:
	( cd bin; ./$(NAME) )

clean:
	rm -r bin

$(shell mkdir -p bin/res)
$(shell rm -r bin/res)
$(shell cp -r res/ bin/res/)
