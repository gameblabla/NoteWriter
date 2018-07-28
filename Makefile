PRGNAME     = notewrite.elf

# define regarding OS, which compiler to use
EXESUFFIX = 
TOOLCHAIN = 
CC          = nspire-gcc
CCP         = nspire-g++
LD          = nspire-gcc

# add SDL dependencies
SDL_LIB     = 
SDL_INCLUDE = 

# change compilation / linking flag options
F_OPTS		=
CC_OPTS		= -O2 -fomit-frame-pointer -fdata-sections -ffunction-sections $(F_OPTS)
CFLAGS		= -IWinbox -I.
CXXFLAGS	=$(CFLAGS) 
LDFLAGS     = -lSDL_gfx -lSDL -lm -Wl,--as-needed -Wl,--gc-sections -flto

# Files to be compiled
SRCDIR    = . ./Winbox
VPATH     = $(SRCDIR)
SRC_C   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.c))
SRC_CP   = $(foreach dir, $(SRCDIR), $(wildcard $(dir)/*.cpp))
OBJ_C   = $(notdir $(patsubst %.c, %.o, $(SRC_C)))
OBJ_CP   = $(notdir $(patsubst %.cpp, %.o, $(SRC_CP)))
OBJS     = $(OBJ_C) $(OBJ_CP)

# Rules to make executable
$(PRGNAME)$(EXESUFFIX): $(OBJS)  
	$(LD) $(CFLAGS) -o $(PRGNAME)$(EXESUFFIX) $^ $(LDFLAGS)

$(OBJ_C) : %.o : %.c
	$(CC) $(CFLAGS) -c -o $@ $<

$(OBJ_CP) : %.o : %.cpp
	$(CCP) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(PRGNAME) *.o
