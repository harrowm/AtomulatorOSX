/*Atomulator v1.0 by Tom Walker
   Main loop*/

#include <string.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include "atom.h"
#include "atommc.h"
#include "sid_atom.h" // avoid compiler warnings on OSX
#include "1770.h"

int palnotntsc = 0;
int colourboard = 1;
int sndddnoise = 1;

// SID
int sndatomsid=1;
int cursid=0;
int sidmethod=0;

// SP3 FOR JOYSTICK SUPPORT

int joyst = 0;

// END SP3

// SP10 FOR KEYBOARDJOYSTICK SUPPORT

int keyjoyst = 0;

// END SP10

// GDOS2015
int fdc1770 = 0;
int GD_bank		= 0;	// Which of the 16 banks is jumpered.
// end GDOS2015

// RAM config
int main_ramflag	= 3;	// 25K by default (when RAMROM *DISABLED!*).
int vid_ramflag		= 7;	// 8K by default
int vid_top;			//= ((vid_ramflag+1)*0x0400)+0x8000;	// Last video RAM address.
// RAM config


int ramrom_enable = 1;

int fasttape = 0;

FILE *rlog;
void rpclog(char *format, ...)
{
	char buf[MAXPATH];
    char buf2[MAXPATH];
    extern ALLEGRO_PATH *docpath;
    
   	al_set_path_filename(docpath, "rlog.txt");
   	strlcpy(buf2, al_path_cstr(docpath, ALLEGRO_NATIVE_PATH_SEP), MAXPATH-1);
   	al_set_path_filename(docpath, NULL);
    
	if (!rlog)
		rlog = fopen(buf2, "wt");

	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);

    fputs(buf, rlog);
	fflush(rlog);
    
    if (strncmp(buf, "ERROR:", 6) == 0)
    {
        strcat(buf, "\nAtomulator will quit. See error log at:\n");
        strcat(buf, buf2);
        al_show_native_message_box(NULL, "Error", "Atomulator will quit", buf, NULL, ALLEGRO_MESSAGEBOX_ERROR);
    }
}

/*SP7 CHANGES*/

FILE *plog;
void prtbuf(char *format, ...)
{
	char buf[MAXPATH];
	extern ALLEGRO_PATH *docpath;
	
	al_set_path_filename(docpath, "pbuf.txt");
	strlcpy(buf, al_path_cstr(docpath, ALLEGRO_NATIVE_PATH_SEP), MAXPATH - 1);
   	al_set_path_filename(docpath, NULL);
	
	if (!plog)
		plog = fopen(buf, "wt");

	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
	fputs(buf, plog);
	fflush(plog);
}

/*END SP7*/

extern int tapeon;
extern uint8_t *ram;

int drawscr = 0;
int ddframes = 0;
int debugcursor =0;

void scrupdate()
{
	ddframes++;
	drawscr++;
    debugcursor++;
}

void atom_reset(int power_on)
{
    extern ALLEGRO_PATH *docpath;
    extern ALLEGRO_PATH *exepath;
    
	debuglog("atom_reset(%d)\n",power_on);
//	memset(ram, 0, 0x10000);
	if(power_on)
	{
		ram[8] = rand();
		ram[9] = rand();
		ram[10] = rand();
		ram[11] = rand();
		
		// Clear BBC basic workspace.
		// if(bbcmode)
		//	memset(&ram[0], 0, 0x10000);
	}
	resetvia();
	sid_reset();
    
	debuglog("exedir=%s\n",al_path_cstr(exepath, ALLEGRO_NATIVE_PATH_SEP));
    debuglog("docdir=%s\n",al_path_cstr(docpath, ALLEGRO_NATIVE_PATH_SEP));
    
	InitMMC();
	
	if (fdc1770)
		reset1770();
	else
		reset8271();

	gfxmode=0;
	reset6502();
	debuglog("atom_reset():done\n");
}

void atom_init(int argc, char **argv)
{
	int c;
	int tapenext = 0, discnext = 0;

	for (c = 1; c < argc; c++)
	{
		if (!strcasecmp(argv[c], "--help"))
		{
			printf("%s command line options :\n\n",ATOMULATOR_VERSION);
			printf("-disc disc.ssd  - load disc.ssd into drives :0/:2\n");
			printf("-disc1 disc.ssd - load disc.ssd into drives :1/:3\n");
			printf("-tape tape.uef  - load tape.uef\n");
			printf("-fasttape       - set tape speed to fast\n");
			printf("-debug          - start debugger\n");
			exit(-1);
		}
		else
		if (!strcasecmp(argv[c], "-tape"))
		{
			tapenext = 2;
		}
		else if (!strcasecmp(argv[c], "-disc") || !strcasecmp(argv[c], "-disk"))
		{
			discnext = 1;
		}
		else if (!strcasecmp(argv[c], "-disc1"))
		{
			discnext = 2;
		}
		else if (!strcasecmp(argv[c], "-fasttape"))
		{
			fasttape = 1;
		}
		else if (!strcasecmp(argv[c], "-debug"))
		{
			debug = debugon = 1;
		}
		else if (tapenext)
			strlcpy(tapefn, argv[c], MAXPATH-1);
		else if (discnext)
		{
			strlcpy(discfns[discnext - 1], argv[c], MAXPATH-1);
			discnext = 0;
		}
		else
		{
			strlcpy(discfns[0], argv[c], MAXPATH-1);
			discnext = 0;
		}
		if (tapenext)
			tapenext--;
	}

	inituef();
	initmem();
	loadroms();
	reset6502();
	initvideo();
	init8255();
	disc_reset();
	reset8271();
	resetvia();

	sid_init();
	sid_settype(sidmethod, cursid);
	loaddiscsamps();
	loaddisc(0, discfns[0]);
	loaddisc(1, discfns[1]);
	atom_reset(1);
}

void atom_run()
{
    int lines;
    
    // if at the debug screen then just redraw the atom screen and return not running the Atom emulator
    if (debug)
    {
        for (lines = 0; lines < 193; lines++) {
            drawline(lines);
        }
        return;
    }
    
    if ((drawscr > 0) || (tapeon && fasttape))
	{
        if (palnotntsc)
            exec6502(312, 64);
        else
            exec6502(262, 64);
        
		if (tapeon && fasttape)
			drawscr = 0;
		else
			drawscr -= (palnotntsc) ? 6 : 5;
		
		if (drawscr > 25)
			drawscr = 0;
        
        if (ddframes >= 25)
        {
            ddframes -= 25;
            mixddnoise();
        }
    }
}

void atom_exit()
{
	saveconfig();
	closeddnoise();
	FinalizeMMC();
//        dumpregs();
//        dumpram();
}

