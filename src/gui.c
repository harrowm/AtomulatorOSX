/*Atomulator v1.0 by Tom Walker
   Linux GUI*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <stdio.h>
#include "atom.h"
#include "atommc.h"
#include "roms.h"
#include "resources.h"
#include "sidtypes.h"
#include "sid_atom.h"
#include "debugger.h"
#include "buildversion.h"

int timerspeeds[] 	= { 5, 12, 25, 38, 50, 75, 85, 100, 150, 200, 250 };
int frameskips[] = { 0,  0,  0,  0,  0,  0,  0,   1,   2,   3,   4 };
int emuspeed = 4;
int fskipmax = 0;

int atompause = 0;

char ejecttext[2][260] = { "Eject disc :0/2", "Eject disc :1/3" };

void setejecttext(int drive, char *fn)
{
	if (fn[0])
		sprintf(ejecttext[drive], "Eject drive :%i/%i - %s", drive, drive + 2, fn);
	else
		sprintf(ejecttext[drive], "Eject drive :%i/%i", drive, drive + 2);
    update_gui();
}

extern int fullscreen;
extern bool quited;
int windx = 512, windy = 384;
extern int dcol;
extern int ddtype, ddvol, sndddnoise;
int showspeed;

extern ALLEGRO_KEYBOARD_STATE keybd;
extern ALLEGRO_TIMER *timer;
extern ALLEGRO_TIMER *displaytimer;
extern ALLEGRO_TIMER *hudtimer;
extern ALLEGRO_EVENT_QUEUE *events;
extern ALLEGRO_EVENT event;
extern ALLEGRO_DISPLAY *display;
extern ALLEGRO_MENU *menu;
extern ALLEGRO_FONT *font;
extern ALLEGRO_AUDIO_STREAM *stream;
extern ALLEGRO_AUDIO_STREAM *ddstream;
extern ALLEGRO_PATH *docpath;

void gui_keydefine(void);

void setquit()
{
    quited = true;
}

void update_gui()
{
    al_set_menu_item_flags(menu, IDM_TAPES_NORMAL, (!fasttape) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_TAPES_FAST, (fasttape) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_caption(menu, IDM_DISC_EJECT_0, ejecttext[0]);
    al_set_menu_item_caption(menu, IDM_DISC_EJECT_1, ejecttext[1]);

    al_set_menu_item_flags(menu, IDM_DISC_WPROT_0, (writeprot[0]) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_DISC_WPROT_1, (writeprot[1]) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_DISC_WPROT_D, (defaultwriteprot) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

	al_set_menu_item_flags(menu, IDM_VID_SNOW, (snow) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_VID_FULLSCREEN, (fullscreen) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_VID_COLOUR, (colourboard) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_VID_PALNOTNTSC, (palnotntsc) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, IDM_HARD_BBC, (RR_jumpers & RAMROM_FLAG_BBCMODE) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	
    al_set_menu_item_flags(menu, IDM_RAMROM_ENABLE, (ramrom_enable) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_RAMROM_DISKROM, (RR_jumpers & RAMROM_FLAG_DISKROM) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

    al_set_menu_item_flags(menu, IDM_SOUND_ATOM, (spon) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_SOUND_TAPE, (tpon) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

    al_set_menu_item_flags(menu, IDM_SOUND_ATOMSID, (sndatomsid)  ? ALLEGRO_MENU_ITEM_CHECKED : 0);

	al_set_menu_item_flags(menu, IDM_SID_6581, (cursid == SID_MODEL_6581) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_8580, (cursid == SID_MODEL_8580) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_8580DB, (cursid == SID_MODEL_8580D) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_6581R4, (cursid == SID_MODEL_6581R4) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_6581R34885, (cursid == SID_MODEL_6581R3_4885) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_6581R30486S, (cursid == SID_MODEL_6581R3_0486S) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_6581R33984, (cursid == SID_MODEL_6581R3_3984) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_6581R4AR3789, (cursid == SID_MODEL_6581R4AR_3789) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_6581R34485, (cursid == SID_MODEL_6581R3_4485) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_6581R41986S, (cursid == SID_MODEL_6581R4_1986S) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_8085R53691, (cursid == SID_MODEL_8580R5_3691) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_8580R53691DB, (cursid == SID_MODEL_8580R5_3691D) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_8580R51489, (cursid == SID_MODEL_8580R5_1489) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SID_8580R51489DB, (cursid == SID_MODEL_8580R5_1489D) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

	al_set_menu_item_flags(menu, IDM_ROMBANK_00, (GD_bank == 0) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_01, (GD_bank == 1) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_02, (GD_bank == 2) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_03, (GD_bank == 3) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_04, (GD_bank == 4) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_05, (GD_bank == 5) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_06, (GD_bank == 6) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_07, (GD_bank == 7) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_08, (GD_bank == 8) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_09, (GD_bank == 9) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_10, (GD_bank == 10) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_11, (GD_bank == 11) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_12, (GD_bank == 12) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_13, (GD_bank == 13) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_14, (GD_bank == 14) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_ROMBANK_15, (GD_bank == 15) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

	al_set_menu_item_flags(menu, IDM_RAM_1K, (main_ramflag == 0) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_RAM_1K_PLUS_5K, (main_ramflag == 1) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_RAM_6K_PLUS_3K, (main_ramflag == 2) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_RAM_6K_PLUS_19K, (main_ramflag == 3) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_RAM_6K_PLUS_22K, (main_ramflag == 4) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_RAM_6K_PLUS_23K, (main_ramflag == 5) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	
	al_set_menu_item_flags(menu, IDM_VIDEORAM_1K, (vid_ramflag == 0) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_VIDEORAM_2K, (vid_ramflag == 1) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_VIDEORAM_3K, (vid_ramflag == 2) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_VIDEORAM_4K, (vid_ramflag == 3) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_VIDEORAM_5K, (vid_ramflag == 4) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_VIDEORAM_6K, (vid_ramflag == 5) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_VIDEORAM_7K, (vid_ramflag == 6) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_VIDEORAM_8K, (vid_ramflag == 7) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	
    al_set_menu_item_flags(menu, IDM_SOUND_DDNOISE, (sndddnoise) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

    al_set_menu_item_flags(menu, IDM_DDT_525, (!ddtype) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_DDT_35, (ddtype) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

    al_set_menu_item_flags(menu, IDM_DDV_33, (ddvol == 1) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_DDV_66, (ddvol == 2) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_DDV_100, (ddvol == 3) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

    al_set_menu_item_flags(menu, IDM_JOYSTICK, (joyst) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_KEYJOYSTICK, (keyjoyst) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

	al_set_menu_item_flags(menu, IDM_GDOS2015_ENABLE, (fdc1770) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

	al_set_menu_item_flags(menu, IDM_SPD_10, (emuspeed == 0) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_SPD_25, (emuspeed == 1) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_SPD_50, (emuspeed == 2) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_SPD_75, (emuspeed == 3) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_SPD_100, (emuspeed == 4) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_SPD_150, (emuspeed == 5) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_SPD_170, (emuspeed == 6) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_SPD_200, (emuspeed == 7) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, IDM_SPD_300, (emuspeed == 8) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SPD_400, (emuspeed == 9) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
	al_set_menu_item_flags(menu, IDM_SPD_500, (emuspeed == 10) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

    al_set_menu_item_flags(menu, IDM_SHOWSPEED, (showspeed) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
}


// The longer menu functions

char tapefn[MAXPATH];

static void gui_tape_load()
{
	char tempname[MAXPATH];
	ALLEGRO_FILECHOOSER *fc;

	strlcpy(tempname, tapefn, MAXPATH - 1);

	fc = al_create_native_file_dialog(al_path_cstr(docpath, ALLEGRO_NATIVE_PATH_SEP), "Please choose a tape image", "UEF;CSW", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
	if (al_show_native_file_dialog(display, fc))
	{
		closeuef();
		closecsw();
		if (al_get_native_file_dialog_count(fc) == 1)
		{
			strlcpy(tapefn, al_get_native_file_dialog_path(fc, 0), MAXPATH - 1);
			loadtape(tapefn);
			// HACK - need to implement the tape catalog window
			// clearcatwindow();
			// catupdatewindow();
			// tapeloaded=1;
		}
	}

	al_destroy_native_file_dialog(fc);
}

static void gui_disc_load(int d)
{
	char tempname[MAXPATH];

	ALLEGRO_FILECHOOSER *fc;

	strlcpy(tempname, discfns[d], MAXPATH - 1);

	fc = al_create_native_file_dialog(al_path_cstr(docpath, ALLEGRO_NATIVE_PATH_SEP), "Please choose a disc image", "DSK;SSD;DSD;IMG;FDI", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
	if (al_show_native_file_dialog(display, fc))
	{
		closedisc(d);

		if (al_get_native_file_dialog_count(fc) == 1)
		{
			strlcpy(discfns[d], al_get_native_file_dialog_path(fc, 0), MAXPATH - 1);
			loaddisc(d, discfns[d]);
            setejecttext(d, discfns[d]);
			if (defaultwriteprot)
				writeprot[d] = 1;
		}
	}
	al_destroy_native_file_dialog(fc);
}

// MH - moved from atom.c and changed to Allegro5
static void changetimerspeed(int i)
{
	al_destroy_timer(timer);
	timer = al_create_timer(1.0 / (i * 6));
	if (timer == NULL)
	{
		rpclog("ERROR: Error creating Allegro timer\n");
		return;
	}

	al_register_event_source(events, al_get_timer_event_source(timer));
	al_start_timer(timer);
}

static void gui_scrshot()
{
	ALLEGRO_FILECHOOSER *fc;

	fc = al_create_native_file_dialog(al_path_cstr(docpath, ALLEGRO_NATIVE_PATH_SEP), "Please enter a filename", "BMP", ALLEGRO_FILECHOOSER_SAVE);

	if (al_show_native_file_dialog(display, fc))
	{
		if (al_get_native_file_dialog_count(fc) == 1)
		{
			strlcpy(scrshotname, al_get_native_file_dialog_path(fc, 0), MAXPATH - 1);
			savescrshot = 1;

			char* ext = strrchr(scrshotname, '.');
			if (!ext && strlen(scrshotname) < (MAXPATH - 5))
				strcat(scrshotname, ".bmp");
		}
		rpclog("Screen shot %s\n", scrshotname);
	}
    al_destroy_native_file_dialog(fc);
}

static void gui_open_settings()
{
    ALLEGRO_FILECHOOSER *fc;
    
    fc = al_create_native_file_dialog(BaseMMCPath, "Select AtoMMC Directory", "", ALLEGRO_FILECHOOSER_FOLDER);
    
    if (al_show_native_file_dialog(display, fc))
    {
        if (al_get_native_file_dialog_count(fc) == 1)
        {
            strlcpy(BaseMMCPath, al_get_native_file_dialog_path(fc, 0), MAXPATH - 1);
            saveconfig();
            loadconfig();
            InitMMC();
        }
        rpclog("Updated AtoMMC Path to %s", BaseMMCPath);
    }
    al_destroy_native_file_dialog(fc);
}

static void about()
{
    extern ALLEGRO_DISPLAY *display;
    char version[40];
    getVersionString(version);
    
    char versioninfo[250];
    strcpy(versioninfo, "AtomulatorOSX Version: ");
    strcat(versioninfo, version);
    strcat(versioninfo, "\n\nAtomulatorOSX created by Malcolm Harrow, updated by Katherine Cramer.\n Based on work done by David Banks, Phill Harvey-Smith, and Kees van Oss.");
    
    
    al_show_native_message_box(display, "About AtomulatorOSX...", "About AtomulatorOSX", versioninfo, NULL, ALLEGRO_MESSAGEBOX_QUESTION);
    //al_destroy_display(display);
}

void processMenuOption(intptr_t option)
{
	int c;

	switch (option)
	{
		// File menu
        case IDM_ABOUT:
            about();
            break;
            
		case IDM_FILE_RESET:
			atom_reset(0);
			break;

		case IDM_FILE_EXIT:
			quited = true;
			break;
        
        case IDM_SETTINGS:
            gui_open_settings();
            break;

		// Tape Menu
		case IDM_TAPE_LOAD:
			gui_tape_load();
			break;

		case IDM_TAPE_EJECT:
			closeuef();
			closecsw();
			break;

		case IDM_TAPE_REW:
			closeuef();
			closecsw();
			loadtape(tapefn);
			break;

		case IDM_TAPE_CAT: // HACK COMPLETE ME
			break;

		case IDM_TAPES_NORMAL:
			fasttape = 0;
			break;

		case IDM_TAPES_FAST:
			fasttape = 1;
			break;

		// Disc menu
		case IDM_DISC_LOAD_0:
			gui_disc_load(0);
			break;

		case IDM_DISC_LOAD_1:
			gui_disc_load(1);
			break;

		case IDM_DISC_EJECT_0:
			closedisc(0);
			discfns[0][0] = 0;
			setejecttext(0, "");
			break;

		case IDM_DISC_EJECT_1:
			closedisc(1);
			discfns[1][0] = 0;
			setejecttext(1, "");
			break;

		case IDM_DISC_NEW_0: // HACK FIX ME
			break;

		case IDM_DISC_NEW_1: // HACK FIX ME
			break;

		case IDM_DISC_WPROT_0:
			writeprot[0] = !writeprot[0];
			if (fwriteprot[0])
				fwriteprot[0] = 1;
			break;

		case IDM_DISC_WPROT_1:
			writeprot[1] = !writeprot[1];
			if (fwriteprot[1])
				fwriteprot[1] = 1;
			break;

		case IDM_DISC_WPROT_D:
			defaultwriteprot = !defaultwriteprot;
			break;

		// Settings - Video menu
		case IDM_VID_SNOW:
			snow = !snow;
			break;

		case IDM_VID_FULLSCREEN:
			al_set_display_flag(display, ALLEGRO_FULLSCREEN_WINDOW, ((al_get_display_flags(display) & ALLEGRO_FULLSCREEN_WINDOW) ? false : true));
			break;

		case IDM_VID_COLOUR:
			colourboard = !colourboard;
			updatepal();
			break;

		case IDM_VID_PALNOTNTSC:	
			palnotntsc = !palnotntsc;
			updatepal(); // HACK Do we need this call ?
			break;

		// Settings - Hardware menu
		case IDM_HARD_BBC:
			atompause = 1;
			RR_jumpers ^= RAMROM_FLAG_BBCMODE;
			atom_reset(1);
			atompause = 0;
			break;

		// Settings - RAMROM menu
		case IDM_RAMROM_ENABLE:
			ramrom_enable = !ramrom_enable;
			atom_reset(1);
			break;

		case IDM_RAMROM_DISKROM:
			RR_jumpers ^= RAMROM_FLAG_DISKROM;
			atom_reset(1);
			break;
		
		// Settings - RAM menu


		case IDM_RAM_1K:
			main_ramflag = 0;
			break;		

		case IDM_RAM_1K_PLUS_5K:
			main_ramflag = 1;
			break;

		case IDM_RAM_6K_PLUS_3K:
			main_ramflag = 2;
			break;

		case IDM_RAM_6K_PLUS_19K:
			main_ramflag = 3;
			break;

		case IDM_RAM_6K_PLUS_22K:
			main_ramflag = 4;
			break;

		case IDM_RAM_6K_PLUS_23K:
			main_ramflag = 5;
			break;

		case IDM_VIDEORAM_1K:
			vid_ramflag = 0;
			SET_VID_TOP();
			break;

		case IDM_VIDEORAM_2K:
			vid_ramflag = 1;
			SET_VID_TOP();
			break;

		case IDM_VIDEORAM_3K:
			vid_ramflag = 2;
			SET_VID_TOP();
			break;

		case IDM_VIDEORAM_4K:
			vid_ramflag = 3;
			SET_VID_TOP();
			break;

		case IDM_VIDEORAM_5K:
			vid_ramflag = 4;
			SET_VID_TOP();
			break;

		case IDM_VIDEORAM_6K:
			vid_ramflag = 5;
			SET_VID_TOP();
			break;

		case IDM_VIDEORAM_7K:
			vid_ramflag = 6;
			SET_VID_TOP();
			break;

		case IDM_VIDEORAM_8K:
			vid_ramflag = 7;
			SET_VID_TOP();
			break;

		// Settings - Floppy menu

		case IDM_GDOS2015_ENABLE:
			fdc1770 = !fdc1770;
			break;

		case IDM_ROMBANK_00:
			GD_bank = 0;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_01:
			GD_bank = 1;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_02:
			GD_bank = 2;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_03:
			GD_bank = 3;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_04:
			GD_bank = 4;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_05:
			GD_bank = 5;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_06:
			GD_bank = 6;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_07:
			GD_bank = 7;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_08:
			GD_bank = 8;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_09:
			GD_bank = 9;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_10:
			GD_bank = 10;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_11:
			GD_bank = 11;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_12:
			GD_bank = 12;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_13:
			GD_bank = 13;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_14:
			GD_bank = 14;
			set_dosrom_ptr();
			break;

		case IDM_ROMBANK_15:
			GD_bank = 15;
			set_dosrom_ptr();
			break;

		// Settings - Sound menu
		case IDM_SOUND_ATOM:
			spon = !spon;
			break;

		case IDM_SOUND_ATOMSID:
			sndatomsid = !sndatomsid;
			break;

		case IDM_SOUND_TAPE:
			tpon = !tpon;
			break;

		case IDM_SOUND_DDNOISE:
			sndddnoise = !sndddnoise;
			break;

		case IDM_SID_6581:
			cursid = SID_MODEL_6581;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_8580:
			cursid = SID_MODEL_8580;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_8580DB:
			cursid = SID_MODEL_8580D;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_6581R4:
			cursid = SID_MODEL_6581R4;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_6581R34885:
			cursid = SID_MODEL_6581R3_4885;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_6581R30486S:
			cursid = SID_MODEL_6581R3_0486S;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_6581R33984:
			cursid = SID_MODEL_6581R3_3984;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_6581R4AR3789:
			cursid = SID_MODEL_6581R4AR_3789;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_6581R34485:
			cursid = SID_MODEL_6581R3_4485;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_6581R41986S:
			cursid = SID_MODEL_6581R4_1986S;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_8085R53691:
			cursid = SID_MODEL_8580R5_3691;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_8580R53691DB:
			cursid = SID_MODEL_8580R5_3691D;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_8580R51489:
			cursid = SID_MODEL_8580R5_1489;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_8580R51489DB:
			cursid = SID_MODEL_8580R5_1489D;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_INTERP:
			sidmethod = 0;
			sid_settype(sidmethod, cursid);
			break;

		case IDM_SID_RESAMP:
			sidmethod = 1;
			sid_settype(sidmethod, cursid);
			break;

		// Settings - disk drive menu
		case IDM_DDT_525:
			ddtype = 0;
			closeddnoise();
			loaddiscsamps();
			break;

		case IDM_DDT_35:
			ddtype = 1;
			closeddnoise();
			loaddiscsamps();
			break;

		case IDM_DDV_33:				
			ddvol = 1;
			break;

		case IDM_DDV_66:
			ddvol = 2;
			break;

		case IDM_DDV_100:
			ddvol = 3;
			break;

		// Settings - Keyboard menu
		case IDM_KEY_REDEFINE:
			// HACK - no keyboard redefine dialog yet
			break;

		case IDM_KEY_DEFAULT:
			for (c = 0; c < 128; c++)
				keylookup[c] = c;
			break;

		// Settings - Joystick menu
		case IDM_JOYSTICK:
			joyst = !joyst;
			break;

		case IDM_KEYJOYSTICK:
			keyjoyst = !keyjoyst;
			break;

		// Misc menu
		case IDM_SPD_10:
			emuspeed = 0;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SPD_25:
			emuspeed = 1;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SPD_50:
			emuspeed = 2;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SPD_75:
			emuspeed = 3;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SPD_100:
			emuspeed = 4;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SPD_150:
			emuspeed = 5;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SPD_170:
			emuspeed = 6;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SPD_200:
			emuspeed = 7;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SPD_300:
			emuspeed = 8;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SPD_400:
			emuspeed = 9;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SPD_500:
			emuspeed = 10;
			fskipmax = frameskips[emuspeed];
			changetimerspeed(timerspeeds[emuspeed]);
			break;

		case IDM_SHOWSPEED:
			showspeed = !showspeed;
			break;

		case IDM_MISC_SCRSHOT:
			gui_scrshot();
			break;

		case IDM_MISC_DEBUG:
			if (!debugon)
				startdebug();
			else
				enddebug();
			break;

		case IDM_MISC_DEBONBRK:
			debug_on_brk = !debug_on_brk;
			break;

		case IDM_MISC_BREAK:
            debug = 1; // at debug prompt
            if (!debugon)
                startdebug();
			break;
			
		default:
			rpclog("Unknown menu item %d\n", option);
			break;
	}

	update_gui();
}


ALLEGRO_MENU_INFO menu_info[] = {
    ALLEGRO_START_OF_MENU("File", (uint16_t)IDM_FILE_MENU),
        // This does NOT work due to some sort of bug in Allegro with destroying displays...need to fix this.
//        {"About AtomulatorOSX...", IDM_ABOUT, 0, NULL},
//        ALLEGRO_MENU_SEPARATOR,
        { "AtoMMC path...", IDM_SETTINGS, 0, NULL},
		{ "Reset",    IDM_FILE_RESET, 0, NULL },
		{ "Exit",     IDM_FILE_EXIT,  0, NULL },
    ALLEGRO_END_OF_MENU,

    ALLEGRO_START_OF_MENU("Tape", (uint16_t)IDM_TAPE_MENU),
        { "Load tape...",   IDM_TAPE_LOAD,     0, NULL },
		ALLEGRO_MENU_SEPARATOR,
        { "Eject tape",     IDM_TAPE_EJECT,    0, NULL },
		ALLEGRO_MENU_SEPARATOR,
		{ "Rewind tape",    IDM_TAPE_REW,    0, NULL },
		{ "Show tape catalogue",    IDM_TAPE_CAT,	0, NULL },
        ALLEGRO_START_OF_MENU("Tape speed", (uint16_t)IDM_TAPESPEED_MENU),
            { "Normal", IDM_TAPES_NORMAL,    ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "Fast",   IDM_TAPES_FAST,      ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
        ALLEGRO_END_OF_MENU,
    ALLEGRO_END_OF_MENU,

    ALLEGRO_START_OF_MENU("Disk", (uint16_t)IDM_DISC_MENU),
        { "Load disc :0/2...",         IDM_DISC_LOAD_0,  0,                             NULL },
        { "Load disc :1/3...",         IDM_DISC_LOAD_1,  0,                             NULL },
		ALLEGRO_MENU_SEPARATOR,
		{ "Eject disc :0/2",            IDM_DISC_EJECT_0, 0,                             NULL },
		{ "Eject disc :1/3",            IDM_DISC_EJECT_1, 0,                             NULL },
		ALLEGRO_MENU_SEPARATOR,
		{ "New disc :0/2",				IDM_DISC_NEW_0,	 0,                             NULL },
		{ "New disc :1/3",				IDM_DISC_NEW_1,	 0,                             NULL },
		ALLEGRO_MENU_SEPARATOR,
		{ "Write protect disc :0/2",    IDM_DISC_WPROT_0, ALLEGRO_MENU_ITEM_CHECKBOX,    NULL },
        { "Write protect disc :1/3",    IDM_DISC_WPROT_1, ALLEGRO_MENU_ITEM_CHECKBOX,    NULL },
        { "Default write protect",      IDM_DISC_WPROT_D, ALLEGRO_MENU_ITEM_CHECKBOX,    NULL },
    ALLEGRO_END_OF_MENU,

	ALLEGRO_START_OF_MENU("Settings", (uint16_t)IDM_SETTINGS_MENU),
        ALLEGRO_START_OF_MENU("Video", (uint16_t)IDM_VIDEO_MENU),
			{ "Snow",			IDM_VID_SNOW,          ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "Fullscreen",		IDM_VID_FULLSCREEN,    ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "Colour Board",	IDM_VID_COLOUR,        ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "PAL (50Hz)",		IDM_VID_PALNOTNTSC,	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
        ALLEGRO_END_OF_MENU,
  
		ALLEGRO_START_OF_MENU("Hardware", (uint16_t)IDM_HARDWARE_MENU),
            { "BBC BASIC",	   IDM_HARD_BBC,    ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
        ALLEGRO_END_OF_MENU,
  
		ALLEGRO_START_OF_MENU("RamRom", (uint16_t)IDM_RAMROM_MENU),
            { "RAM/ROM enabled",			IDM_RAMROM_ENABLE,		ALLEGRO_MENU_ITEM_CHECKBOX,	NULL},
            { "RAM/ROM disk rom enabled",	IDM_RAMROM_DISKROM,	ALLEGRO_MENU_ITEM_CHECKBOX,	NULL},
        ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("Ram", (uint16_t)IDM_RAM_MENU),
			ALLEGRO_START_OF_MENU("Main RAM",	(uint16_t)IDM_MAINRAM_MENU),
				{ "Minimum 1K in base",  									IDM_RAM_1K, 			ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Minimum 1K in base + 5K",								IDM_RAM_1K_PLUS_5K,		ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "6K on motherboard + 3K DOS",								IDM_RAM_6K_PLUS_3K,     ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "6K motherboard + 3K DOS + 16K expansion",				IDM_RAM_6K_PLUS_19K,    ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "6K motherboard + 3K DOS + 22K expansion (hole at A00)",	IDM_RAM_6K_PLUS_22K,    ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "6K motherboard + 3K DOS + 23K expansion",				IDM_RAM_6K_PLUS_23K,	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			ALLEGRO_END_OF_MENU,
			ALLEGRO_START_OF_MENU("Video RAM", (uint16_t)IDM_VIDEORAM_MENU),
				{ "1K Video RAM",  	IDM_VIDEORAM_1K, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "2K Video RAM",  	IDM_VIDEORAM_2K, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "3K Video RAM",  	IDM_VIDEORAM_3K, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "4K Video RAM",  	IDM_VIDEORAM_4K, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "5K Video RAM",  	IDM_VIDEORAM_5K, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "6K Video RAM",  	IDM_VIDEORAM_6K, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "7K Video RAM",  	IDM_VIDEORAM_7K, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "8K Video RAM",  	IDM_VIDEORAM_8K, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			ALLEGRO_END_OF_MENU,
		ALLEGRO_END_OF_MENU,

		ALLEGRO_START_OF_MENU("Floppy", (uint16_t)IDM_FLOPPY_MENU),
			{ "GDOS/ADOS 2015 (1770 based) Enabled",	IDM_GDOS2015_ENABLE,		ALLEGRO_MENU_ITEM_CHECKBOX,	NULL },
			ALLEGRO_START_OF_MENU("Rombank", (uint16_t)IDM_ROMBANK_MENU),
				{ "Bank 00",  	IDM_ROMBANK_00, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 01",  	IDM_ROMBANK_01, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 02",  	IDM_ROMBANK_02, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 03",  	IDM_ROMBANK_03, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 04",  	IDM_ROMBANK_04, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 05",  	IDM_ROMBANK_05, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 06",  	IDM_ROMBANK_06, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 07",  	IDM_ROMBANK_07, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 08",  	IDM_ROMBANK_08, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 09",  	IDM_ROMBANK_09, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 10",  	IDM_ROMBANK_10, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 11",  	IDM_ROMBANK_11, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 12",  	IDM_ROMBANK_12, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 13",  	IDM_ROMBANK_13, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 14",  	IDM_ROMBANK_14, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				{ "Bank 15",  	IDM_ROMBANK_15, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			ALLEGRO_END_OF_MENU,
		ALLEGRO_END_OF_MENU,

        ALLEGRO_START_OF_MENU("Sound", (uint16_t)IDM_SOUND_MENU),
            { "Internal speaker",  	IDM_SOUND_ATOM, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "AtomSID",            IDM_SOUND_ATOMSID,       ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "Tape noise",         IDM_SOUND_TAPE,		ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{"Disc drive noise",    IDM_SOUND_DDNOISE, ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
			ALLEGRO_START_OF_MENU("reSID configuration", (uint16_t)IDM_RESID_MENU),
				ALLEGRO_START_OF_MENU("Model", (uint16_t)IDM_SIDMODEL_MENU ),
					{ "6581",  						IDM_SID_6581, 			ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "8580",  						IDM_SID_8580, 			ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "8580 + digi boost",  		IDM_SID_8580DB, 		ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "6581R4",  					IDM_SID_6581R4, 		ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "6581R3 4885",  				IDM_SID_6581R34885, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "6581R3 0486S",  				IDM_SID_6581R30486S, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "6581R3 3984",  				IDM_SID_6581R33984, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "6581R4AR 3789",  			IDM_SID_6581R4AR3789, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "6581R3 4485",  				IDM_SID_6581R34485, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "6581R4 1986S",  				IDM_SID_6581R41986S, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "8580R5 3691",  				IDM_SID_8085R53691, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "8580R5 3691 + digi boost",  	IDM_SID_8580R53691DB, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "8580R5 1489",  				IDM_SID_8580R51489, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "8580R5 1489 + digi boost",	IDM_SID_8580R51489DB,	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				ALLEGRO_END_OF_MENU,
				ALLEGRO_START_OF_MENU("Sample method",  (uint16_t)IDM_SIDMETHOD_MENU),
					{ "Interpolating",  IDM_SID_INTERP, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
					{ "Resampling",		IDM_SID_RESAMP,	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
				ALLEGRO_END_OF_MENU,
			ALLEGRO_END_OF_MENU,
            ALLEGRO_START_OF_MENU("Disc drive type", (uint16_t)IDM_FLOPPY_MENU),
                { "5.25", IDM_DDT_525, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
                { "3.5",  IDM_DDT_35, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            ALLEGRO_END_OF_MENU,
            ALLEGRO_START_OF_MENU("Disc drive volume", (uint16_t)IDM_DDVOL_MENU),
                { "33%",  IDM_DDV_33, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
                { "66%",  IDM_DDV_66, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
                { "100%", IDM_DDV_100, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            ALLEGRO_END_OF_MENU,
        ALLEGRO_END_OF_MENU,
        ALLEGRO_START_OF_MENU("Keyboard", (uint16_t)IDM_KEYBOARD_MENU),
            { "Redefine keys...", IDM_KEY_REDEFINE, 0, NULL },
            { "Default mapping",   IDM_KEY_DEFAULT, 0, NULL },
        ALLEGRO_END_OF_MENU,
		ALLEGRO_START_OF_MENU("Joystick", (uint16_t)IDM_JOYSTICK_MENU),
			{ "PORTB", IDM_JOYSTICK, 0, NULL },
			{ "Keyboard matrix",   IDM_KEYJOYSTICK, 0, NULL },
		ALLEGRO_END_OF_MENU,
    ALLEGRO_END_OF_MENU,
	ALLEGRO_START_OF_MENU("&Misc", (uint16_t)IDM_MISC_MENU),
        ALLEGRO_START_OF_MENU("Speed", (uint16_t)IDM_SPEED_MENU),
            { "10%",  IDM_SPD_10, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "25%",  IDM_SPD_25, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "50%",  IDM_SPD_50, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "75%",  IDM_SPD_75, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "100%", IDM_SPD_100, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "150%", IDM_SPD_150, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "170%", IDM_SPD_170, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "200%", IDM_SPD_200, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "300%", IDM_SPD_300, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "400%", IDM_SPD_400, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
			{ "500%", IDM_SPD_500, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
		ALLEGRO_END_OF_MENU,
        { "Show emulator speed", IDM_SHOWSPEED, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
        { "Save screenshot", IDM_MISC_SCRSHOT, 0, NULL },
		ALLEGRO_MENU_SEPARATOR,
		{ "Debugger", IDM_MISC_DEBUG, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
		{ "Debug on BRK", IDM_MISC_DEBONBRK, 0, NULL },
		{ "Break", IDM_MISC_BREAK, 0, NULL },
    ALLEGRO_END_OF_MENU,
	ALLEGRO_END_OF_MENU
};
