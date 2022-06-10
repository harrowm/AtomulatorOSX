/*Atomulator v1.0 by Tom Walker
   Configuration handling*/

#include <string.h>
#include <stdio.h>
#include <allegro5/allegro.h>
#include "atom.h"
#include "atommc.h"
#include "debugger.h"
#include "roms.h"

#define LABEL_DISC0		"disc0"
#define LABEL_DISC1		"disc1"

#define LABEL_MMC_PATH	"mmc_path"

#define LABEL_COLOUR	"colourboard"
#define LABEL_PALNOTNTSC "palnotntsc"
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

// SP3 JOYSTICK SUPPORT

#define LABEL_JOYSTICK  "joystick"

// END SP3

// SP10 KEYBOARDJOYSTICK SUPPORT

#define LABEL_KEYJOYSTICK  "keyjoystick"

// END SP10

// GDOS2015
#define LABEL_FDC1770	"fdc1770"
#define LABEL_GDBANK	"gdbank"
// END GDOS2015

// RAM config
#define LABEL_MAINRAM	"mainram"
#define LABEL_VIDRAM	"vidram"
// end RAM config

#define LABEL_KEY_DEF	"key_define_"
#define LABEL_USER_KBD	"user_keyboard"

#define LABEL_DEBUG_BRK	"debug_on_brk"

// initial screen window size
#define LABEL_SCREEN_X  "winsizex"
#define LABEL_SCREEN_Y  "winsizey"

// Show emulator speed
#define LABEL_SHOWSPEED "showspeed"

int snow;
int defaultwriteprot;
char discfns[2][MAXPATH];

ALLEGRO_CONFIG *atom_config;

static int get_config_int(char *section, char *name, int def)
{
    const char *s;

    s = al_get_config_value(atom_config, section, name);
    if (s == NULL)
    {
        return def;
    }
    else
    {
        return (int)atoi(s);
    }
}

static void set_config_int(char *section, char *key, int value)
{
    char s[256];

    sprintf(s, "%i", value);
    al_set_config_value(atom_config, section, key, s);
}

static void load_config_string(char *label,
					    char *dest)
{
	char *strtmp;

	strtmp = (char*)al_get_config_value(atom_config, NULL, label);
	if (strtmp)
		strlcpy(dest, strtmp, MAXPATH-1);
	else
		dest[0] = 0;
}

void loadconfig()
{
	int c;
	char s[MAXPATH];
	extern ALLEGRO_PATH *docpath;

	al_set_path_filename(docpath, "atom.cfg");
	strlcpy(s, al_path_cstr(docpath, ALLEGRO_NATIVE_PATH_SEP), MAXPATH - 1);
	al_set_path_filename(docpath, NULL);
	
	atom_config = al_load_config_file(s);
    
    if (!atom_config)
        atom_config = al_create_config();
    
	load_config_string(LABEL_DISC0,discfns[0]);
    if (!al_filename_exists(discfns[0]))
        discfns[0][0] = 0;
    
	load_config_string(LABEL_DISC1,discfns[1]);
    if (!al_filename_exists(discfns[1]))
        discfns[1][0] = 0;
    
    load_config_string(LABEL_MMC_PATH,BaseMMCPath);
    
    // On Windows al_filename_exists() returns true if passed a NULL string ..
	if (strlen(BaseMMCPath) == 0 || !al_filename_exists(BaseMMCPath))
	{
		rpclog("MMC Path given in atom.cfg is invalid - %s\n", BaseMMCPath);
		al_append_path_component(docpath, DEF_MMC_DIR);
		strlcpy(BaseMMCPath, al_path_cstr(docpath, ALLEGRO_NATIVE_PATH_SEP), MAXPATH - 1);
		al_drop_path_tail(docpath); // leave exepath correct for subsequent uses
		rpclog("MMC Path reset to - %s\n", BaseMMCPath);
	}
    
    rpclog("MMC path is %s\n", BaseMMCPath);

	palnotntsc      = get_config_int(NULL, LABEL_PALNOTNTSC, 0);
	colourboard 	= get_config_int(NULL, LABEL_COLOUR, 1);
    
    if (colourboard >1) colourboard = 1; // fix bug introduced where 2 was written to the atom.cfg file
    
	snow 			= get_config_int(NULL, LABEL_SNOW, 0);
	ramrom_enable 	= get_config_int(NULL, LABEL_RAMROM, 1);	// Default RAMROM enable
	RR_jumpers 		= get_config_int(NULL, LABEL_RAMROMJMP, RAMROM_FLAG_DISKROM);	// RAMROM diskrom enabled

	fasttape 		= get_config_int(NULL, LABEL_FASTTAPE, 0);

	defaultwriteprot = get_config_int(NULL, LABEL_DEF_WP, 1);
	if (defaultwriteprot)
		writeprot[0] = writeprot[1] = 1;

	ddvol 			= get_config_int(NULL, LABEL_DDVOL, 2);
	ddtype 			= get_config_int(NULL, LABEL_DDTYPE, 0);

	spon 			= get_config_int(NULL, LABEL_SND_INT, 1);
	tpon 			= get_config_int(NULL, LABEL_SND_TAPE, 0);
	sndddnoise 		= get_config_int(NULL, LABEL_SND_DD, 1);

// SP3 JOYSTICK SUPPORT

	joyst			= get_config_int(NULL, LABEL_JOYSTICK, 0);

// END SP3

// SP10 KEYBOARDJOYSTICK SUPPORT

	keyjoyst		= get_config_int(NULL, LABEL_KEYJOYSTICK, 0);

// END SP10

// GDOS2015
	fdc1770			= get_config_int(NULL, LABEL_FDC1770, 0);	// Default disabled
	GD_bank			= get_config_int(NULL, LABEL_GDBANK, 0);	// Default bank 0
// end GDOS2015

// RAM config
	main_ramflag	= get_config_int(NULL, LABEL_MAINRAM, 5);	// Default Max RAM
	vid_ramflag		= get_config_int(NULL, LABEL_VIDRAM, 7);	// Default max video RAM
	SET_VID_TOP();
// end RAM config


	debug_on_brk	= get_config_int(NULL, LABEL_DEBUG_BRK, 0);

    // Set initial screen size, set default if not defined
    winsizex        = get_config_int(NULL, LABEL_SCREEN_X, 512);
    winsizey        = get_config_int(NULL, LABEL_SCREEN_Y, 384);

    // Show emulator speed
    showspeed       = get_config_int(NULL, LABEL_SHOWSPEED, 0);

	for (c = 0; c < 128; c++)
	{
		sprintf(s, "%s%03i", LABEL_KEY_DEF, c);
		keylookup[c] = get_config_int(LABEL_USER_KBD, s, c);
	}
}

void saveconfig()
{
	int c;
	char s[MAXPATH];
	extern ALLEGRO_PATH *docpath;

	al_set_config_value(atom_config, NULL, LABEL_DISC0, discfns[0]);
	al_set_config_value(atom_config, NULL, LABEL_DISC1, discfns[1]);
	al_set_config_value(atom_config, NULL, LABEL_MMC_PATH,BaseMMCPath);

	set_config_int(NULL, LABEL_PALNOTNTSC, palnotntsc);
	set_config_int(NULL, LABEL_COLOUR, colourboard);
	set_config_int(NULL, LABEL_SNOW, snow);
	set_config_int(NULL, LABEL_RAMROM,ramrom_enable);
	set_config_int(NULL, LABEL_RAMROMJMP,RR_jumpers);
	
// SP3 JOYSTICK SUPPORT

	set_config_int(NULL, LABEL_JOYSTICK, joyst);

// END SP3
	
// SP10 KEYBOARDJOYSTICK SUPPORT

	set_config_int(NULL, LABEL_KEYJOYSTICK, keyjoyst);

// END SP10
	
// GDOS2015
	set_config_int(NULL, LABEL_FDC1770, fdc1770);
	set_config_int(NULL, LABEL_GDBANK, GD_bank);
// end GDOS2015

// RAM config
	set_config_int(NULL, LABEL_MAINRAM, main_ramflag);
	set_config_int(NULL, LABEL_VIDRAM, vid_ramflag);
// end RAM config

	set_config_int(NULL, LABEL_FASTTAPE, fasttape);

	set_config_int(NULL, LABEL_DEF_WP, defaultwriteprot);

	set_config_int(NULL, LABEL_DDVOL, ddvol);
	set_config_int(NULL, LABEL_DDTYPE, ddtype);

	set_config_int(NULL, LABEL_SND_INT, spon);
	set_config_int(NULL, LABEL_SND_TAPE, tpon);
	set_config_int(NULL, LABEL_SND_DD, sndddnoise);

	set_config_int(NULL, LABEL_DEBUG_BRK, debug_on_brk);

    // Not going to save window size
    // but will save show emulator speed
    set_config_int(NULL, LABEL_SHOWSPEED, showspeed);

	for (c = 0; c < 128; c++)
	{
		sprintf(s, "%s%03i", LABEL_KEY_DEF, c);
		set_config_int(LABEL_USER_KBD, s, keylookup[c]);
	}

	al_set_path_filename(docpath, "atom.cfg");
	strlcpy(s, al_path_cstr(docpath, ALLEGRO_NATIVE_PATH_SEP), MAXPATH - 1);
	al_set_path_filename(docpath, NULL);
	al_save_config_file(s, atom_config);
}
