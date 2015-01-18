/*Atomulator v1.0 by Tom Walker
   Main loop*/

// Malcolm Harrow Dec 2014
// Removed most of the Allegro calls from this file eg initialize keyboard and mouse
// Exception being rand()
// Moved F12 (restart) handling

#include <string.h>
#include <stdio.h>
#include <allegro.h>
#include "atom.h"
#include "atommc.h"

// Malcolm Harrow Dec 2014
// include sid_type.h to avoid compiler warnings on OSX
#include "sid_atom.h"

char exedir[MAXPATH+1];

int colourboard = 1;
int sndddnoise = 1;

// SID
int sndatomsid=1;
int cursid=0;
int sidmethod=0;

// Joystick support
unsigned char joyst = 0;

int ramrom_enable = 1;

int bbcmode = 0;
int fasttape = 0;
char discfn[260];

FILE *rlog;
void rpclog(char *format, ...)
{
	char buf[256];
    
//   return;
    
    sprintf(buf,"%s/rlog.txt", exedir);
    
	if (!rlog)
		rlog = fopen(buf, "wt");
//turn;
	va_list ap;
	va_start(ap, format);
	vsprintf(buf, format, ap);
	va_end(ap);
	fputs(buf, rlog);
	fflush(rlog);
}

int tapeon;
uint8_t *ram;
char filelist[256][17];
int filelistsize;
int filepos[256];

int drawscr;
int ddframes = 0;

void scrupdate()
{
	ddframes++;
	drawscr++;
}

void atom_reset(int power_on)
{
	debuglog("atom_reset(%d)\n",power_on);
//	memset(ram, 0, 0x10000);
	if(power_on)
	{
		ram[8] = rand();
		ram[9] = rand();
		ram[10] = rand();
		ram[11] = rand();
		
		// Clear BBC basic workspace.
		if(bbcmode)
			memset(&ram[0], 0, 0x10000);
	}
	resetvia();
	sid_reset();
	debuglog("exedir=%s\n",exedir);
	InitMMC();
	
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
			printf("Atomulator v1.10 command line options :\n\n");
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
			strcpy(tapefn, argv[c]);
		else if (discnext)
		{
			strcpy(discfns[discnext - 1], argv[c]);
			discnext = 0;
		}
		else
		{
			strcpy(discfns[0], argv[c]);
			discnext = 0;
		}
		if (tapenext)
			tapenext--;
	}
	initalmain(0, NULL);
	inituef();
	initmem();
	loadroms();
	reset6502();
	initvideo();
	init8255();
	disc_reset();
	reset8271();
	resetvia();

    inital();
	sid_init();
	sid_settype(sidmethod, cursid);
	loaddiscsamps();
	loaddisc(0, discfns[0]);
	loaddisc(1, discfns[1]);
	atom_reset(1);
}

void atom_run()
{
    if ((drawscr > 0) || (tapeon && fasttape))
    {
        if (colourboard)
            exec6502(312, 64);
        else
            exec6502(262, 64);
                
        if (tapeon && fasttape)
            drawscr = 0;
        else
            drawscr -= (colourboard) ? 6 : 5;
                        
        if (drawscr > 25)
            drawscr = 0;
                            
        if (ddframes >= 25)
        {
            ddframes -= 25;
            mixddnoise();
        }
    }
//    else
//        al_rest(0.000001);
}

void atom_exit()
{
	saveconfig();
	closeddnoise();
	FinalizeMMC();
//        dumpregs();
//        dumpram();
}

