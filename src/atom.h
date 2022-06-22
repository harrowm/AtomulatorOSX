#ifndef _ATOM_H
#define _ATOM_H

#include <stdint.h>

#ifdef _MSC_VER
#define strcasecmp _stricmp
#define strncasecmp _strnicmp
#endif

// Include safe string functions for systems without them eg Windows
#ifndef HAVE_STRLCPY
//extern int strlcpy(char *dst, const char *src, int size);
#endif

#ifndef HAVE_STRLCAT
//extern int strlcpy(char *dst, const char *src, int size);
#endif

#define MAXPATH	512

// pollsound is called once per scan line (max 312), each time 2 samples
#define SNDBUFLEN 312*2

// ddnoise is generated every 1/12 second from a 44100 source = 3675 samples
#define DDSNDBUFLEN 3675

extern int vbl;
extern int gfxmode;
extern int css;
extern int speaker;
extern uint8_t lastdat;
extern int cswena;
extern int cswpoint;

//#define printf rpclog

extern int colourboard;
extern int palnotntsc;

// SP3 FOR JOYSTICK SUPPORT

extern int joyst;

// END SP3

// SP10 FOR KEYBOARDJOYSTICK SUPPORT

extern int keyjoyst;

// END SP10


typedef struct VIA
{
	uint8_t ora, orb, ira, irb;
	uint8_t ddra, ddrb;
	uint32_t t1l, t2l;
	int t1c, t2c;
	uint8_t acr, pcr, ifr, ier;
	int t1hit, t2hit;
	uint8_t porta, portb;
} VIA;


// From atom.c
extern int fasttape;
extern int ramrom_enable;
extern int overscan;

/* For 1770 based GDOS */
//#define WD1770 1
extern int fdc1770;
extern int GD_bank;
/* end */

// RAM Config
extern int main_ramflag;
extern int vid_ramflag;
extern int vid_top;
#define SET_VID_TOP()	{vid_top=((vid_ramflag+1)*0x0400)+0x8000;}	// Last video RAM address.
// end RAM Config

// From 6502.c
extern uint16_t pc;
extern uint8_t a, x, y, s;
typedef struct PROCFLAGS
{
	int c, z, i, d, v, n;
} PROCFLAGS;
extern PROCFLAGS p;

extern int nmi;
extern int debugon;
extern uint8_t opcode; // used in debugger.c
extern int interrupt;

extern int RR_jumpers;


// From debugger.c
extern int debug;
extern int fetchc[65536], readc[65536], writec[65536];
extern void calcMemScreen(void);

// From 8255.c
extern int spon, tpon;
//----



extern void (*fdccallback)(void);
extern void (*fdcdata)(uint8_t dat);
extern void (*fdcspindown)(void);
extern void (*fdcfinishread)(void);
extern void (*fdcnotfound)(void);
extern void (*fdcdatacrcerror)(void);
extern void (*fdcheadercrcerror)(void);
extern void (*fdcwriteprotect)(void);
extern int (*fdcgetdata)(int last);

extern int writeprot[2], fwriteprot[2];

void ssd_reset(void);
void ssd_load(int drive, char *fn);
void ssd_close(int drive);
void dsd_load(int drive, char *fn);
void ssd_seek(int drive, int track);
void ssd_readsector(int drive, int sector, int track, int side, int density);
void ssd_writesector(int drive, int sector, int track, int side, int density);
void ssd_readaddress(int drive, int sector, int side, int density);
void ssd_format(int drive, int sector, int side, int density);
void ssd_poll(void);

void fdi_reset(void);
void fdi_load(int drive, char *fn);
void fdi_close(int drive);
void fdi_seek(int drive, int track);
void fdi_readsector(int drive, int sector, int track, int side, int density);
void fdi_writesector(int drive, int sector, int track, int side, int density);
void fdi_readaddress(int drive, int sector, int side, int density);
void fdi_format(int drive, int sector, int side, int density);
void fdi_poll(void);

void loaddisc(int drive, char *fn);
void newdisc(int drive, char *fn);
void closedisc(int drive);
void disc_reset(void);
void disc_poll(void);
void disc_seek(int drive, int track);
void disc_readsector(int drive, int sector, int track, int side, int density);
void disc_writesector(int drive, int sector, int track, int side, int density);
void disc_readaddress(int drive, int track, int side, int density);
void disc_format(int drive, int track, int side, int density);
extern int defaultwriteprot;
extern char discfns[2][MAXPATH];


void setejecttext(int drive, char *fn);

extern int ddvol, ddtype;

extern int motorspin;
extern int fdctime;
extern int motoron;
extern int disctime;

typedef struct DRIVE
{
	void (*seek)(int drive, int track);
	void (*readsector)(int drive, int sector, int track, int side, int density);
	void (*writesector)(int drive, int sector, int track, int side, int density);
	void (*readaddress)(int drive, int track, int side, int density);
	void (*format)(int drive, int track, int side, int density);
	void (*poll)(void);
} DRIVE;

extern int curdrive;

extern int sndddnoise;
extern int sndatomsid;
extern int cursid;
extern int sidmethod;

#include <allegro5/allegro_audio.h>
/* sound.c */
extern bool initSound(void);
extern void giveSoundBuffer(ALLEGRO_AUDIO_STREAM *stream, int16_t *soundBuf, int16_t lenBufInBytes);

/* csw.c */
extern void opencsw(char *fn);
extern void closecsw(void);
extern int getcsw(void);
extern void findfilenamescsw(void);
//extern uint8_t findcswbyte(void);

/* uef.c */
extern void inituef(void);
extern void polluef(void);
extern void openuef(char *fn);
extern void closeuef(void);
// extern void rewindit(void);
// extern int getftell(void);
// extern void findfilenamesuef(void);
//extern int ueffileopen(void);
//extern void receiveuef(uint8_t val);

/* 6502.c */
extern void reset6502(void);
extern void exec6502(int linenum, int cpl);
extern void dumpregs(void);

extern void initmem(void);
extern void loadroms(void);
extern void dumpram(void);

extern void set_dosrom_ptr(void);
extern uint8_t readmeml(uint16_t addr); // called from debugger.c
extern void writememl(uint16_t addr, uint8_t val); // called from debugger.c

// -- 6502.c internal
// static void load_rom(char *Name, int Size, int Offset);
// static void set_rr_ptrs(void);
// static int RamEnabled(uint16_t addr);


// video.c

extern void initvideo(void);
extern void drawline(int line);
extern void updatepal(void);
extern void drawAtomScreen(void);
// static void hudupdate(void);
// static void lockAtomScreen(void);
// static void unlockAtomScreen(void);

/* 8271.c */
extern void reset8271(void);
extern uint8_t read8271(uint16_t addr);
extern void write8271(uint16_t addr, uint8_t val);

// static void spinup8271(void);
// static void setspindown8271(void);
// static int getparams8271(void);
// static void seek8271(void);

//extern void spindown8271(void);
//extern void callback8271(void);
//extern void data8271(uint8_t dat);
//extern void finishread8271(void);
//extern void notfound8271(void);
//extern void datacrcerror8271(void);
//extern void headercrcerror8271(void);
//extern int getdata8271(int last);
//extern void writeprotect8271(void);

/* ddnoise.c */
//static ALLEGRO_SAMPLE *loadSampleFile(char *f);
extern void loaddiscsamps(void);
extern void closeddnoise(void);
extern void ddnoise_seek(int len);
extern void mixddnoise(void);

// From 8255.c
extern void pollsound(void);
extern void polltape(void);

extern void init8255(void);
extern void write8255(uint16_t addr, uint8_t val);
extern uint8_t read8255(uint16_t addr);
extern void dcd(int cycles);
extern void dcdlow(void);
extern void receive(uint8_t dat);

// From 6522via.c

extern void writevia(uint16_t addr, uint8_t val);
extern void dumpvia(void);
extern uint8_t readvia(uint16_t addr);
extern void resetvia(void);
extern void updatetimers(void);
// static void updateIFR(void);
// static void dumpvia(void);

/* debugger.c */
extern void startdebug(void);
extern void enddebug(void);
extern void debugread(uint16_t addr);
extern void debugwrite(uint16_t addr, uint8_t val);
extern void dodebugger(int linenum);
extern void debuglog(char *format, ...);
extern void drawDebugInputScreen(void);
extern void drawDebugMemScreen(void);
extern void handleDebuggerInput(int keycode, int inputChar);

// static void lockMemScreen(void);
// static void unlockMemScreen(void);
// static void initMemVideo(void);
// static void debugout(char *s);
// static uint8_t dreadmem(uint16_t addr);
// static void debugdisassemble(void);

/* atom.c */
extern void rpclog(char *format, ...);
extern void prtbuf(char *format, ...);
extern void scrupdate(void);
extern void atom_reset(int power_on);
extern void atom_init(int argc, char **argv);
extern void atom_run(void);
extern void atom_exit(void);

/* config.c */
extern void loadconfig(void);
extern void saveconfig(void);
// static int get_config_int(char *section, char *name, int def);
// static  void set_config_int(char *section, char *key, int value);
// static  void load_config_string(char *label, char *dest);

/* gui.c */
extern void setejecttext(int drive, char *fn);
extern void setquit(void);
extern void update_gui(void);
extern void processMenuOption(intptr_t option);
//extern void gui_tape_load(void);
//extern void gui_disc_load(int d);
//extern void changetimerspeed(int i);
//extern void gui_scrshot(void);



//---
void cataddname(char *s);

extern int winsizex, winsizey;

// Flag to say whether we want to show the emulator speed onscreen
extern int showspeed;

extern int keylookup[128];


extern int snow;
extern int fullscreen;

//void updatewindowsize(int x, int y);
void loadtape(char *fn);

extern char tapefn[MAXPATH];
extern int emuspeed, fskipmax;
extern char scrshotname[MAXPATH];
extern int savescrshot;


#endif
