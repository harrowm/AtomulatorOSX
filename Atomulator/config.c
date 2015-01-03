/*Atomulator v1.0 by Tom Walker
   Configuration handling*/

// Malcolm Harrow Dec 2014
// Extensive changes for Allegro 5 conversion
// Added ability to read default window size from the config file

#include <string.h>
#include <stdio.h>
#include <allegro.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include "atom.h"
#include "atommc.h"
#include "debugger.h"

#define LABEL_DISC0		"disc0"
#define LABEL_DISC1		"disc1"

#define LABEL_MMC_PATH	"mmc_path"

#define LABEL_COLOUR	"colourboard"
#define LABEL_BBCBASIC	"bbcbasic"
#define LABEL_SNOW		"snow"
#define LABEL_RAMROM	"ramrom_enable"
#define LABEL_RAMROMJMP	"ramrom_jumpers"

#define LABEL_FASTTAPE	"fasttape"

#define LABEL_DEF_WP 	"defaultwriteprot"
#define LABEL_DDVOL 	"ddvol"
#define LABEL_DDTYPE 	"ddtype"

#define LABEL_SND_INT 	"snd_internal"
#define LABEL_SND_TAPE 	"snd_tape"
#define LABEL_SND_DD 	"snd_ddnoise"

// Joystick support
#define LABEL_JOYSTICK  "joystick"

#define LABEL_KEY_DEF	"key_define_"
#define LABEL_USER_KBD	"user_keyboard"

#define LABEL_DEBUG_BRK	"debug_on_brk"

// initial screen window size
#define LABEL_SCREEN_X  "winsizex"
#define LABEL_SCREEN_Y  "winsizey"

int snow;
int defaultwriteprot;
char discfns[2][260];

struct stat	statbuf;

ALLEGRO_CONFIG *atom_config;

int dir_exists(char *path)
{
	int			result = 0;
	
	if (0==stat(path,&statbuf))
		if(S_ISDIR(statbuf.st_mode))
			result=1;
		
	return result;
}

int get_config_int(char *section, char *name, int def)
{
    const char *s;
    
    s = al_get_config_value(atom_config, section, name);
    if (s == NULL)
        return def;
    else
        return (int)strtol(s, &s, 10);
}


void set_config_int(char *section, char *key, int value)
{
    char s[256];
    
    sprintf(s, "%i", value);
    al_set_config_value(atom_config, section, key, s);
}

void load_config_string(char *label,
                        char *dest)
{
    const char *strtmp;
    
    strtmp = al_get_config_value(atom_config, NULL, LABEL_MMC_PATH);
    if (strtmp)
        strcpy(dest, strtmp);
    else
        dest[0] = 0;
}

void loadconfig()
{
	int c;
	char s[256];

	sprintf(s, "%satom.cfg", exedir);
	atom_config = al_load_config_file(s);
    
    if (atom_config == NULL)
    {
        rpclog("Failed to load atom config file from: %s\n", s);
        exit (EXIT_FAILURE);
    }
	
	load_config_string(LABEL_DISC0, discfns[0]);
    load_config_string(LABEL_DISC1, discfns[1]);
    load_config_string(LABEL_MMC_PATH, BaseMMCPath);

	// check to see if the mmc path is valid and exists, else set to
	// the default.
	if((0==strlen(BaseMMCPath)) || (!dir_exists(BaseMMCPath)))
		sprintf(BaseMMCPath,"%s%s",exedir,DEF_MMC_DIR);
	
	colourboard 	= get_config_int(NULL, LABEL_COLOUR, 1);
	bbcmode 		= get_config_int(NULL, LABEL_BBCBASIC, 0);
	snow 			= get_config_int(NULL, LABEL_SNOW, 0);
	ramrom_enable 	= get_config_int(NULL, LABEL_RAMROM, 0);
	RR_jumpers 		= get_config_int(NULL, LABEL_RAMROMJMP, 0);

	fasttape 		= get_config_int(NULL, LABEL_FASTTAPE, 0);

	defaultwriteprot = get_config_int(NULL, LABEL_DEF_WP, 1);
    
    // Malcolm Harrow Dec 2014 move this here from linux.c
    if (defaultwriteprot)
        writeprot[0] = writeprot[1] = 1;
    
	ddvol 			= get_config_int(NULL, LABEL_DDVOL, 2);
	ddtype 			= get_config_int(NULL, LABEL_DDTYPE, 0);

	spon 			= get_config_int(NULL, LABEL_SND_INT, 1);
	tpon 			= get_config_int(NULL, LABEL_SND_TAPE, 0);
	sndddnoise 		= get_config_int(NULL, LABEL_SND_DD, 1);

	// Joystick support
	joyst			= get_config_int(NULL, LABEL_JOYSTICK, 0);

	debug_on_brk	= get_config_int(NULL, LABEL_DEBUG_BRK, 0);
    
    // Set initial screen size, set default if not defined
    winsizex        = get_config_int(NULL, LABEL_SCREEN_X, 512);
    winsizey        = get_config_int(NULL, LABEL_SCREEN_Y, 384);
    
	for (c = 0; c < 128; c++)
	{
		sprintf(s, "%s%03i", LABEL_KEY_DEF, c);
		keylookup[c] = get_config_int(LABEL_USER_KBD, s, c);
	}
}

void saveconfig()
{
	int c;
	char s[256];


	al_set_config_value(atom_config, NULL, LABEL_DISC0, discfns[0]);
	al_set_config_value(atom_config, NULL, LABEL_DISC1, discfns[1]);
	al_set_config_value(atom_config, NULL, LABEL_MMC_PATH,BaseMMCPath);

    set_config_int(NULL, LABEL_COLOUR, colourboard);
    set_config_int(NULL, LABEL_BBCBASIC, bbcmode);
    set_config_int(NULL, LABEL_SNOW, snow);
    set_config_int(NULL, LABEL_RAMROM, ramrom_enable);
    set_config_int(NULL, LABEL_RAMROMJMP, RR_jumpers);
    
    
	// Joystick support
	set_config_int(NULL, LABEL_JOYSTICK, joyst);
	
	set_config_int(NULL, LABEL_FASTTAPE, fasttape);

	set_config_int(NULL, LABEL_DEF_WP, defaultwriteprot);

	set_config_int(NULL, LABEL_DDVOL, ddvol);
	set_config_int(NULL, LABEL_DDTYPE, ddtype);

	set_config_int(NULL, LABEL_SND_INT, spon);
	set_config_int(NULL, LABEL_SND_TAPE, tpon);
	set_config_int(NULL, LABEL_SND_DD, sndddnoise);

	set_config_int(NULL, LABEL_DEBUG_BRK, debug_on_brk);

    // Not going to save window size
    
	for (c = 0; c < 128; c++)
	{
		sprintf(s, "%s%03i", LABEL_KEY_DEF, c);
		set_config_int(LABEL_USER_KBD, s, keylookup[c]);
	}
    
    sprintf(s, "%satom.cfg", exedir);
    al_save_config_file(s, atom_config);
}
