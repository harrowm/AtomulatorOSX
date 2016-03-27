# Makefile for Atomulator - Linux and Windows

ifeq ($(OS), Windows_NT)
	ifeq ($(PROCESSOR_ARCHITECTURE), x86)
		TARGET=Atomulator-Win32

		# have to dynamically link OpenAL32
		# order of the libraries is important !!!

		# works !!
		#LIBS  = -Wl,-Bstatic -lallegro_monolith-static -lstdc++ -lgcc /mingw32/i686-w64-mingw32/lib/libwinpthread.a
		#LIBS += -ldumb -lflac -lfreetype -logg -lz -lpng -ljpeg -Wl,-Bdynamic -lopenal32 -Wl,-Bstatic
		#LIBS += -lgdiplus -luuid -lkernel32 -lwinmm -lpsapi -lopengl32 -lglu32 -luser32 -lcomdlg32 -lgdi32 -lshell32 -lole32 -ladvapi32 -lws2_32 -lshlwapi

		LIBS  = -Wl,--subsystem,windows -static -lallegro_monolith-static -lstdc++ -lgcc
		LIBS += -ldumb -lflac -lfreetype -logg -lz -lpng -ljpeg /mingw32/lib/libopenal32.a
		LIBS += -lgdiplus -luuid -lkernel32 -lwinmm -lpsapi -lopengl32 -lglu32 -luser32 -lcomdlg32 -lgdi32 -lshell32 -lole32 -ladvapi32 -lws2_32 -lshlwapi
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE), amd64)
		TARGET=Atomulator-Win64
	endif
else
	# known to work, but too many libraries
	# LIBS = -lstdc++ `pkg-config --static --libs allegro-static-5 allegro_main-static-5 allegro_memfile-static-5 allegro_color-static-5 allegro_physfs-static-5 allegro_primitives-static-5 allegro_image-static-5 allegro_ttf-static-5 allegro_dialog-static-5 allegro_audio-static-5 allegro_acodec-static-5 allegro_font-static-5`

	LIBS = -lstdc++ `pkg-config --static --libs allegro-static-5 allegro_main-static-5 allegro_primitives-static-5 allegro_image-static-5 allegro_ttf-static-5 allegro_dialog-static-5 allegro_audio-static-5 allegro_acodec-static-5 allegro_font-static-5`
	UNAME_S := $(shell uname -s)
	UNAME_P := $(shell getconf LONG_BIT)

	ifeq ($(UNAME_S), Linux)
		ifeq ($(UNAME_P), 64)
			TARGET=Atomulator-Linux-x86_64
		else
			TARGET=Atomulator-Linux-i686
		endif
	endif
endif


VPATH = . resid-fp atommc
CPP  = g++
CC   = gcc
CFLAGS = -O3 -ffast-math -fomit-frame-pointer -falign-loops -falign-jumps -falign-functions

OBJ = 6502.o 6522via.o 8255.o 8271.o atom.o config.o csw.o ddnoise.o debugger.o disc.o fdi.o fdi2raw.o sound.o ssd.o uef.o video.o main.o linux-keydefine.o gui.o resid.o string.o 1770.o
SIDOBJ = convolve-sse.o convolve.o envelope.o extfilt.o filter.o pot.o sid.o voice.o wave6581__ST.o wave6581_P_T.o wave6581_PS_.o wave6581_PST.o wave8580__ST.o wave8580_P_T.o wave8580_PS_.o wave8580_PST.o wave.o
MMCOBJ = atommc.o atmmc2core.o atmmc2wfn.o ff_emu.o ff_emudir.o wildcard.o


../$(TARGET): $(OBJ) $(SIDOBJ) $(MMCOBJ) $(BUILD_NUMBER_FILE)
	$(CC) $(OBJ) $(SIDOBJ) $(MMCOBJ) -o ../$(TARGET) $(LIBS)

all : ../$(TARGET)

clean :
	rm -f *.o
	rm -f ../$(TARGET)

%.o : %.c
	$(CC) $(CFLAGS) -c $<

%.o : %.cc
	$(CPP) $(CFLAGS) -c $<
