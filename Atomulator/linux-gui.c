/*Atomulator v1.0 by Tom Walker
   Linux GUI*/

#ifndef WIN32
#include <string.h>

#include <stdio.h>
#include <stdbool.h>
#include <allegro.h>
#include <allegro_native_dialog.h>
#include <allegro_primitives.h>  // for drawing rectangles
#include <allegro_image.h>

#include "atom.h"
#include "roms.h"
#include "resources.h"
#include "sidtypes.h"
#include "sid_atom.h"

#undef printf


// MH
// define an array of functions for easy calling from linux.c

#define MENU_filemenu           0
#define MENU_gui_reset          1
#define MENU_gui_exit           2

#define MENU_tapemenu           3
#define MENU_gui_loadt          4
#define MENU_gui_ejectt         5
#define MENU_gui_rewind         6
#define MENU_tapespdmenu        7
#define MENU_gui_normal         8
#define MENU_gui_fast           9

#define MENU_diskmenu           10
#define MENU_gui_load0          11
#define MENU_gui_load1          12
#define MENU_gui_eject0         13
#define MENU_gui_eject1         14
#define MENU_gui_wprot0         15
#define MENU_gui_wprot1         16
#define MENU_gui_wprotd         17

#define MENU_settingsmenu       18
#define MENU_videomenu          19
#define MENU_gui_fullscreen     20
#define MENU_gui_snow           21
#define MENU_hardmenu           22
#define MENU_gui_colour         23
#define MENU_gui_bbc            24
#define MENU_ramrommenu         25
#define MENU_gui_ramrom_en      26
#define MENU_gui_ramromdsk_en   27

#define MENU_soundmenu          28
#define MENU_gui_internalsnd    29
#define MENU_gui_tnoise         30
#define MENU_gui_atomsid        31
#define MENU_residmenu          32
#define MENU_sidtypemenu        33
#define MENU_gui_sidtype1       34
#define MENU_gui_sidtype2       35
#define MENU_gui_sidtype3       36
#define MENU_gui_sidtype4       37
#define MENU_gui_sidtype5       38
#define MENU_gui_sidtype6       39
#define MENU_gui_sidtype7       40
#define MENU_gui_sidtype8       41
#define MENU_gui_sidtype9       42
#define MENU_gui_sidtype10      43
#define MENU_gui_sidtype11      44
#define MENU_gui_sidtype12      45
#define MENU_gui_sidtype13      46
#define MENU_gui_sidtype14      47
#define MENU_methodmenu         48
#define MENU_gui_method1        49
#define MENU_gui_method2        50

#define MENU_gui_ddnoise        51
#define MENU_ddtypemenu         52
#define MENU_gui_ddtype1        53
#define MENU_gui_ddtype2        54
#define MENU_ddvolmenu          55
#define MENU_gui_ddvol1         56
#define MENU_gui_ddvol2         57
#define MENU_gui_ddvol3         58
#define MENU_keymenu            59
#define MENU_gui_keydefine      60
#define MENU_gui_keydefault     61
#define MENU_gui_joystk_en      62

#define MENU_miscmenu           63
#define MENU_speedmenu          64
#define MENU_gui_speed1         65
#define MENU_gui_speed2         66
#define MENU_gui_speed3         67
#define MENU_gui_speed4         68
#define MENU_gui_speed5         69
#define MENU_gui_speed6         70
#define MENU_gui_speed7         71
#define MENU_gui_speed8         72
#define MENU_gui_speed9         73
#define MENU_gui_speed10        74
#define MENU_gui_scrshot        75

#define MENU_gui_showspeed      76

int timerspeeds[] 	= { 5, 12, 25, 38, 50, 75, 85, 100, 150, 200, 250 };
int frameskips[] 	= { 0,  0,  0,  0,  0,  0,  0,   1,   2,   3,   4 };
int emuspeed = 4;
int fskipmax = 0;

char ejecttext[2][260] = { "Eject disc :0/2", "Eject disc :1/3" };

void setejecttext(int drive, char *fn)
{
	if (fn[0])
		sprintf(ejecttext[drive], "Eject drive :%i/%i - %s", drive, drive + 2, al_get_app_name()); //HACK ??
	else
		sprintf(ejecttext[drive], "Eject drive :%i/%i", drive, drive + 2);
}

void setquit()
{
}

extern int fullscreen;
extern int quited;
int windx = 512, windy = 384;
extern int dcol;
extern int ddtype, ddvol, sndddnoise;
extern int showspeed;

ALLEGRO_KEYBOARD_STATE keybd;
ALLEGRO_TIMER *timer;
ALLEGRO_TIMER *displaytimer;
ALLEGRO_EVENT_QUEUE *events;
ALLEGRO_EVENT event;
ALLEGRO_DISPLAY *display;
ALLEGRO_MENU *menu;

extern void scrupdate();

void gui_keydefine();

void updatelinuxgui()
{
    al_set_menu_item_flags(menu, MENU_gui_normal, (!fasttape) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_fast, (fasttape) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_wprot0, (writeprot[0]) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_wprot1, (writeprot[1]) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_wprotd, (defaultwriteprot) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_fullscreen, (fullscreen) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_snow, (snow) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_colour, (colourboard) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_bbc, (bbcmode) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_ramrom_en, (ramrom_enable) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_ramromdsk_en, (RR_jumpers & RAMROM_FLAG_DISKROM) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_internalsnd, (spon) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_tnoise, (tpon) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_atomsid, (sndatomsid) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_sidtype1, (cursid == SID_MODEL_6581) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype2, (cursid == SID_MODEL_8580) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype3, (cursid == SID_MODEL_8580D) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype4, (cursid == SID_MODEL_6581R4) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype5, (cursid == SID_MODEL_6581R3_4885) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype6, (cursid == SID_MODEL_6581R3_0486S) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype7, (cursid == SID_MODEL_6581R3_3984) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype8, (cursid == SID_MODEL_6581R4AR_3789) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype9, (cursid == SID_MODEL_6581R3_4485) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype10, (cursid == SID_MODEL_6581R4_1986S) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype11, (cursid == SID_MODEL_8580R5_3691) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype12, (cursid == SID_MODEL_8580R5_3691D) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype13, (cursid == SID_MODEL_8580R5_1489) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_sidtype14, (cursid == SID_MODEL_8580R5_1489D) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_method1, (sidmethod == 0) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_method2, (sidmethod == 1) ? ALLEGRO_MENU_ITEM_CHECKED : 0);    
    
    al_set_menu_item_flags(menu, MENU_gui_ddnoise, (sndddnoise) ? ALLEGRO_MENU_ITEM_CHECKED : 0);

    al_set_menu_item_flags(menu, MENU_gui_ddtype1, (!ddtype) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_ddtype2, (ddtype) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_ddvol1, (ddvol == 1) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_ddvol2, (ddvol == 2) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_ddvol3, (ddvol == 3) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_joystk_en, (joyst) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_speed1, (emuspeed == 0) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_speed2, (emuspeed == 1) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_speed3, (emuspeed == 2) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_speed4, (emuspeed == 3) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_speed5, (emuspeed == 4) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_speed6, (emuspeed == 5) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_speed7, (emuspeed == 6) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_speed8, (emuspeed == 7) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_speed9, (emuspeed == 8) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    al_set_menu_item_flags(menu, MENU_gui_speed10, (emuspeed == 9) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
    
    al_set_menu_item_flags(menu, MENU_gui_showspeed, (showspeed) ? ALLEGRO_MENU_ITEM_CHECKED : 0);
}

// Cannot hash define this function because needs to be a member of the array of functions
void gui_dummy()
{}

// File menu functions
void gui_reset()
{
    atom_reset(0);
}

void gui_exit()
{
    quited = 1;
}

// Tape menu functions

char tapefn[260];

void gui_loadt()
{
    char tempname[260];
    ALLEGRO_FILECHOOSER *fc;
    
    memcpy(tempname, tapefn, 260);
    
    fc = al_create_native_file_dialog(exedir, "Please choose a tape image", "UEF;CSW", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
    if (al_show_native_file_dialog(display, fc))
    {
        closeuef();
        closecsw();
        if (al_get_native_file_dialog_count(fc) == 1)
        {
            memcpy(tapefn, al_get_native_file_dialog_path(fc, 0), 260);
            loadtape(tapefn);
            // tapeloaded=1;
        }
    }
    al_destroy_native_file_dialog(fc);
}

void gui_ejectt()
{
    closeuef();
    closecsw();
    //        tapeloaded=0;
}

void gui_rewind()
{
    closeuef();
    closecsw();
    loadtape(tapefn);
}

void gui_normal()
{
    fasttape = 0;
    updatelinuxgui();
}

void gui_fast()
{
    fasttape = 1;
    updatelinuxgui();
}

// Disc menu functions

void gui_load0()
{
    char tempname[260];
    
    ALLEGRO_FILECHOOSER *fc;
    
    memcpy(tempname, discfns[0], 260);
   
    fc = al_create_native_file_dialog(exedir, "Please choose a disc image", "DSK;SSD;DSD;IMG;FDI", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);
    if (al_show_native_file_dialog(display, fc))
    {
        closedisc(0);
        
        if (al_get_native_file_dialog_count(fc) == 1)
        {
            memcpy(discfns[0], al_get_native_file_dialog_path(fc,0), 260);
            loaddisc(0, discfns[0]);
            if (defaultwriteprot)
                writeprot[0] = 1;
        }
    }
    updatelinuxgui();
    al_destroy_native_file_dialog(fc);
}

void gui_load1()
{
    char tempname[260];

    ALLEGRO_FILECHOOSER *fc;
    
    memcpy(tempname, discfns[1], 260);
    
    fc = al_create_native_file_dialog(exedir, "Please choose a disc image", "DSK;SSD;DSD;IMG;FDI", ALLEGRO_FILECHOOSER_FILE_MUST_EXIST);

    if (al_show_native_file_dialog(display, fc))
    {
        closedisc(1);
        
        if (al_get_native_file_dialog_count(fc) == 1)
        {
            memcpy(discfns[1], al_get_native_file_dialog_path(fc,0), 260);
            loaddisc(1, discfns[1]);
            if (defaultwriteprot)
                writeprot[1] = 1;
        }
    }
    updatelinuxgui();
    al_destroy_native_file_dialog(fc);
}

void gui_eject0()
{
    closedisc(0);
    discfns[0][0] = 0;
}
void gui_eject1()
{
    closedisc(1);
    discfns[1][0] = 0;
}

void gui_wprot0()
{
    writeprot[0] = !writeprot[0];
    if (fwriteprot[0])
        fwriteprot[0] = 1;
    updatelinuxgui();
}

void gui_wprot1()
{
    writeprot[1] = !writeprot[1];
    if (fwriteprot[1])
        fwriteprot[1] = 1;
    updatelinuxgui();
}

void gui_wprotd()
{
    defaultwriteprot = !defaultwriteprot;
    updatelinuxgui();
}


// Settings menu functions

void gui_fullscreen()
{
    if (fullscreen)
    {
        fullscreen = 0;
        leavefullscreen();
    }
    else
    {
        fullscreen = 1;
        enterfullscreen();
    }
}

void gui_snow()
{
    snow = !snow;
    updatelinuxgui();
}

void gui_colour()
{
    colourboard = !colourboard;
    updatepal();
}

void gui_bbc()
{
    bbcmode = !bbcmode;
    atom_reset(0);
}

void gui_ramrom_en()
{
    ramrom_enable = !ramrom_enable;
    atom_reset(0);
}

void gui_ramromdsk_en()
{
    RR_jumpers ^= RAMROM_FLAG_DISKROM;
    atom_reset(0);
}

// Sound menu functions
void gui_internalsnd()
{
    spon = !spon;
    updatelinuxgui();
}

void gui_tnoise()
{
    tpon = !tpon;
    updatelinuxgui();
}

void gui_atomsid()
{
    sndatomsid=!sndatomsid;
    updatelinuxgui();
}

void gui_sidtype1()
{
    cursid = SID_MODEL_6581;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype2()
{
    cursid = SID_MODEL_8580;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype3()
{
    cursid = SID_MODEL_8580D;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype4()
{
    cursid = SID_MODEL_6581R4;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype5()
{
    cursid = SID_MODEL_6581R3_4885;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype6()
{
    cursid = SID_MODEL_6581R3_0486S;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype7()
{
    cursid = SID_MODEL_6581R3_3984;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype8()
{
    cursid = SID_MODEL_6581R4AR_3789;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype9()
{
    cursid = SID_MODEL_6581R3_4485;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype10()
{
    cursid = SID_MODEL_6581R4_1986S;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype11()
{
    cursid = SID_MODEL_8580R5_3691;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}
void gui_sidtype12()
{
    cursid = SID_MODEL_8580R5_3691D;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}
void gui_sidtype13()
{
    cursid = SID_MODEL_8580R5_1489;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_sidtype14()
{
    cursid = SID_MODEL_8580R5_1489D;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_method1()
{
    sidmethod = 0;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_method2()
{
    sidmethod = 1;
    sid_settype(sidmethod, cursid);
    updatelinuxgui();
}

void gui_ddnoise()
{
    sndddnoise = !sndddnoise;
    updatelinuxgui();
}

void gui_ddtype1()
{
    ddtype = 0;
    closeddnoise();
    loaddiscsamps();
    updatelinuxgui();
}

void gui_ddtype2()
{
    ddtype = 1;
    closeddnoise();
    loaddiscsamps();
    updatelinuxgui();
}

void gui_ddvol1()
{
    ddvol = 1;
    updatelinuxgui();
}

void gui_ddvol2()
{
    ddvol = 2;
    updatelinuxgui();
}

void gui_ddvol3()
{
    ddvol = 3;
    updatelinuxgui();
}

// MH gui_keydefine in another source file

void gui_keydefault()
{
    int c;
    
    for (c = 0; c < 128; c++)
        keylookup[c] = c;
}

void gui_joystk_en()
{
    joyst = !joyst;
    updatelinuxgui();
}

// Misc menu functions

// MH - moved from atom.c and changed to Allegro5
// cannot get al_set_timer_speed to work reliably HACK

void changetimerspeed(int i)
{
    al_destroy_timer(timer);
    timer = al_create_timer(1.0/(i*6));
    if (timer == NULL)
    {
        rpclog("Error creating Allegro timer\n");
        return ;
    }
    al_register_event_source(events, al_get_timer_event_source(timer));
    al_start_timer(timer);
    fskipmax = frameskips[emuspeed];
    updatelinuxgui();
}

void gui_speed1()
{
    emuspeed = 0;
    changetimerspeed(timerspeeds[emuspeed]);
}

void gui_speed2()
{
    emuspeed = 1;
    changetimerspeed(timerspeeds[emuspeed]);
}

void gui_speed3()
{
    emuspeed = 2;
    changetimerspeed(timerspeeds[emuspeed]);
}

void gui_speed4()
{
    emuspeed = 3;
    changetimerspeed(timerspeeds[emuspeed]);
}

void gui_speed5()
{
    emuspeed = 4;
    changetimerspeed(timerspeeds[emuspeed]);
}

void gui_speed6()
{
    emuspeed = 5;
    changetimerspeed(timerspeeds[emuspeed]);
}

void gui_speed7()
{
    emuspeed = 6;
    changetimerspeed(timerspeeds[emuspeed]);
}

void gui_speed8()
{
    emuspeed = 7;
    changetimerspeed(timerspeeds[emuspeed]);
}

void gui_speed9()
{
    emuspeed = 8;
    changetimerspeed(timerspeeds[emuspeed]);
}

void gui_speed10()
{
    emuspeed = 9;
    changetimerspeed(timerspeeds[emuspeed]);
}

void gui_showspeed()
{
    showspeed = !showspeed;
    updatelinuxgui();
}

void gui_scrshot()
{
    ALLEGRO_FILECHOOSER *fc;
    
    fc = al_create_native_file_dialog(exedir, "Please enter a filename", "BMP", ALLEGRO_FILECHOOSER_SAVE);

    if (al_show_native_file_dialog(display, fc))
    {
        if (al_get_native_file_dialog_count(fc) == 1)
        {
            memcpy(scrshotname, al_get_native_file_dialog_path(fc, 0), 260);
            savescrshot = 1;
            
            char* ext = strrchr(scrshotname, '.');
            if (!ext)
                strcat(scrshotname, ".bmp");
            
        }
        printf("screen shot %s\n", scrshotname);
    }
}

// Array of menu pointers

typedef void (*func)(void);
func MENU_fn[] =
{
    gui_dummy,          // MENU_filemenu
    gui_reset,          // MENU_gui_reset
    gui_exit,           // MENU_gui_exit
    
    gui_dummy,          // MENU_tapemenu
    gui_loadt,          // MENU_gui_loadt
    gui_ejectt,         // MENU_gui_ejectt
    gui_rewind,         // MENU_gui_rewind
    gui_dummy,          // MENU_tapespdmenu
    gui_normal,         // MENU_gui_normal
    gui_fast,           // MENU_gui_fast
 
    gui_dummy,          // MENU_diskmenu
    gui_load0,          // MENU_gui_load0
    gui_load1,          // MENU_gui_load1
    gui_eject0,         // MENU_gui_eject0
    gui_eject1,         // MENU_gui_eject1
    gui_wprot0,         // MENU_gui_wprot0
    gui_wprot1,         // MENU_gui_wprot1
    gui_wprotd,         // MENU_gui_wprotd
    
    gui_dummy,          // MENU_settingsmenu
    gui_dummy,          // MENU_videomenu
    gui_fullscreen,     // MENU_gui_fullscreen
    gui_snow,           // MENU_gui_snow
    gui_dummy,          // hardmenu
    gui_colour,         // MENU_gui_colour
    gui_bbc,            // MENU_gui_bbc
    gui_dummy,          // MENU_ramrommenu
    gui_ramrom_en,      // MENU_gui_ramrom_en
    gui_ramromdsk_en,   // MENU_gui_ramromdsk_en
    
    
    gui_dummy,          // MENU_soundmenu
    gui_internalsnd,    // MENU_gui_internalsnd
    gui_tnoise,         // MENU_gui_tnoise
    gui_atomsid,        // MENU_gui_atomsid
    gui_dummy,          // MENU_residmenu
    gui_dummy,          // sidtypemenu
    gui_sidtype1,       // MENU_sidtypemenu1
    gui_sidtype2,       // MENU_sidtypemenu2
    gui_sidtype3,       // MENU_sidtypemenu3
    gui_sidtype4,       // MENU_sidtypemenu4
    gui_sidtype5,       // MENU_sidtypemenu5
    gui_sidtype6,       // MENU_sidtypemenu6
    gui_sidtype7,       // MENU_sidtypemenu7
    gui_sidtype8,       // MENU_sidtypemenu8
    gui_sidtype9,       // MENU_sidtypemenu9
    gui_sidtype10,      // MENU_sidtypemenu10
    gui_sidtype11,      // MENU_sidtypemenu11
    gui_sidtype12,      // MENU_sidtypemenu12
    gui_sidtype13,      // MENU_sidtypemenu13
    gui_sidtype14,      // MENU_sidtypemenu14
    gui_dummy,          // MENU_methodmenu
    gui_method1,        // MENU_gui_method1
    gui_method2,        // MENU_gui_method2
    
    gui_ddnoise,        // MENU_gui_ddnoise
    gui_dummy,          // MENU_ddtypemenu
    gui_ddtype1,        // MENU_gui_ddtype1
    gui_ddtype2,        // MENU_gui_ddtype2
    gui_dummy,          // MENU_ddvolmenu
    gui_ddvol1,         // MENU_gui_ddvol1
    gui_ddvol2,         // MENU_gui_ddvol2
    gui_ddvol3,         // MENU_gui_ddvol3
    gui_dummy,          // MENU_keymenu
    gui_keydefine,      // MENU_gui_keydefine
    gui_keydefault,     // MENU_gui_keydefault
    gui_joystk_en,      // MENU_gui_joystk_en
    gui_dummy,          // MENU_miscmenu
    gui_dummy,          // MENU_speedmenu
    gui_speed1,         // MENU_gui_speed1
    gui_speed2,         // MENU_gui_speed2
    gui_speed3,         // MENU_gui_speed3
    gui_speed4,         // MENU_gui_speed4
    gui_speed5,         // MENU_gui_speed5
    gui_speed6,         // MENU_gui_speed6
    gui_speed7,         // MENU_gui_speed7
    gui_speed8,         // MENU_gui_speed8
    gui_speed9,         // MENU_gui_speed9
    gui_speed10,        // MENU_gui_speed10
    gui_scrshot,        // MENU_gui_scrshot
    gui_showspeed       // MENU_gui_showspeed
};

ALLEGRO_MENU_INFO menu_info[] = {
    ALLEGRO_START_OF_MENU("&File", MENU_filemenu),
    { "&Hard reset",    MENU_gui_reset, 0, NULL },
    { "&Exit",          MENU_gui_exit,  0, NULL },
    ALLEGRO_END_OF_MENU,

    ALLEGRO_START_OF_MENU("&Tape", MENU_tapemenu),
        { "Load tape...",   MENU_gui_loadt,     0, NULL },
        { "Eject tape",     MENU_gui_ejectt,    0, NULL },
        { "Rewind tape",    MENU_gui_rewind,    0, NULL },
        ALLEGRO_START_OF_MENU("Tape speed", MENU_tapespdmenu),
            { "Normal", MENU_gui_normal,    ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "Fast",   MENU_gui_fast,      ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
        ALLEGRO_END_OF_MENU,
    ALLEGRO_END_OF_MENU,

    ALLEGRO_START_OF_MENU("&Disk", MENU_diskmenu),
        { "Load disc :&0/2...",         MENU_gui_load0,  0,                             NULL },
        { "Load disc :&1/3...",         MENU_gui_load1,  0,                             NULL },
        { "Eject disc :0/2",            MENU_gui_eject0, 0,                             NULL },
        { "Eject disc :1/3",            MENU_gui_eject1, 0,                             NULL },
        { "Write protect disc :0/2",    MENU_gui_wprot0, ALLEGRO_MENU_ITEM_CHECKBOX,    NULL },
        { "Write protect disc :1/3",    MENU_gui_wprot1, ALLEGRO_MENU_ITEM_CHECKBOX,    NULL },
        { "Default write protect",      MENU_gui_wprotd, ALLEGRO_MENU_ITEM_CHECKBOX,    NULL },
    ALLEGRO_END_OF_MENU,


    ALLEGRO_START_OF_MENU("&Settings", MENU_settingsmenu),
        ALLEGRO_START_OF_MENU("&Video", MENU_videomenu),
            { "Fullscreen", MENU_gui_fullscreen,    ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "Snow",       MENU_gui_snow,          ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
        ALLEGRO_END_OF_MENU,
        ALLEGRO_START_OF_MENU("&Hardware", MENU_hardmenu),
            { "&Colour board", MENU_gui_colour, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "&BBC BASIC",	   MENU_gui_bbc,    ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
        ALLEGRO_END_OF_MENU,
        ALLEGRO_START_OF_MENU("&RamRom", MENU_ramrommenu),
            { "RAM/ROM enabled",			MENU_gui_ramrom_en,		ALLEGRO_MENU_ITEM_CHECKBOX,	NULL},
            { "RAM/ROM disk rom enabled",	MENU_gui_ramromdsk_en,	ALLEGRO_MENU_ITEM_CHECKBOX,	NULL},
        ALLEGRO_END_OF_MENU,

        ALLEGRO_START_OF_MENU("&Sound", MENU_soundmenu),
            { "Internal speaker",  	MENU_gui_internalsnd, 	ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "Tape noise",         MENU_gui_tnoise,		ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "AtomSID",            MENU_gui_atomsid,		ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
            ALLEGRO_START_OF_MENU("reSID configuration", MENU_residmenu),
                ALLEGRO_START_OF_MENU("Model", MENU_sidtypemenu),
                    {"6581",                    MENU_gui_sidtype1,  ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"8580",                    MENU_gui_sidtype2,  ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"8580 + digi boost",       MENU_gui_sidtype3,  ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"6581R4",                  MENU_gui_sidtype4,  ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"6581R3 4885",             MENU_gui_sidtype5,  ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"6581R3 0486S",            MENU_gui_sidtype6,  ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"6581R3 3984",             MENU_gui_sidtype7,  ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"6581R4AR 3789",           MENU_gui_sidtype8,  ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"6581R3 4485",             MENU_gui_sidtype9,  ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"6581R4 1986S",            MENU_gui_sidtype10, ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"8580R5 3691",             MENU_gui_sidtype11, ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"8580R5 3691 + digi boost",MENU_gui_sidtype12, ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"8580R5 1489",             MENU_gui_sidtype13, ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"8580R5 1489 + digi boost",MENU_gui_sidtype14, ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                ALLEGRO_END_OF_MENU,
                ALLEGRO_START_OF_MENU("Sample method", MENU_methodmenu),
                    {"Interpolating", MENU_gui_method1, ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                    {"Resampling",    MENU_gui_method2, ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
                ALLEGRO_END_OF_MENU,
            ALLEGRO_END_OF_MENU,
            {"Disc drive noise",    MENU_gui_ddnoise, ALLEGRO_MENU_ITEM_CHECKBOX, NULL},
            ALLEGRO_START_OF_MENU("Disc drive type", MENU_ddtypemenu),
                { "5.25", MENU_gui_ddtype1, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
                { "3.5",  MENU_gui_ddtype2, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            ALLEGRO_END_OF_MENU,
            ALLEGRO_START_OF_MENU("Disc drive volume", MENU_ddvolmenu),
                { "33%",  MENU_gui_ddvol1, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
                { "66%",  MENU_gui_ddvol2, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
                { "100%", MENU_gui_ddvol3, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            ALLEGRO_END_OF_MENU,
        ALLEGRO_END_OF_MENU,
        ALLEGRO_START_OF_MENU("&Keyboard", MENU_keymenu),
            { "Redefine keyboard", MENU_gui_keydefine, 0, NULL },
            { "Default mapping",   MENU_gui_keydefault, 0, NULL },
        ALLEGRO_END_OF_MENU,
        { "Joystick PORTB", MENU_gui_joystk_en, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
    ALLEGRO_END_OF_MENU,

    ALLEGRO_START_OF_MENU("&Misc", MENU_miscmenu),
        ALLEGRO_START_OF_MENU("&Speed", MENU_speedmenu),
            { "10%",  MENU_gui_speed1, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "25%",  MENU_gui_speed2, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "50%",  MENU_gui_speed3, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "75%",  MENU_gui_speed4, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "100%", MENU_gui_speed5, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "150%", MENU_gui_speed6, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "200%", MENU_gui_speed7, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "300%", MENU_gui_speed8, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "400%", MENU_gui_speed9, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
            { "500%", MENU_gui_speed10, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
        ALLEGRO_END_OF_MENU,
        { "Show emulator speed", MENU_gui_showspeed, ALLEGRO_MENU_ITEM_CHECKBOX, NULL },
        { "Save screenshot", MENU_gui_scrshot, 0, NULL },
    ALLEGRO_END_OF_MENU,
    ALLEGRO_END_OF_MENU
};

extern char* CreatePathByExpandingTildePath();
# include <sys/stat.h> // mkdir

bool allegro_init()
{
    // initialize allegro and required addons -
    if (!(al_init() && al_init_image_addon() && al_init_primitives_addon() && al_install_mouse() && al_install_keyboard() && al_install_joystick()))
    {
        printf("Can't initialize Allegro - quiting!\n");
        return false;
    }
    
    
    if (CreatePathByExpandingTildePath("~/Documents/Atomulator/") == NULL)
    {
        char t[256];
        
        strcpy(t, CreatePathByExpandingTildePath("~"));
        strcat(t, "/Documents");
        (void)mkdir(t, 0755);
        
        strcat(t, "/Atomulator");
        (void)mkdir(t, 0755);
        
        strcpy(exedir, t);
        
        rpclog("Created default Atomulator log file directory at %s\n", t);
    }

    
    
    strcpy(exedir, CreatePathByExpandingTildePath("~/Documents/Atomulator/"));

    return true;
}

bool allegro_create_display_and_menus()
{
    al_set_new_display_flags(ALLEGRO_WINDOWED|ALLEGRO_RESIZABLE);
    display = al_create_display(winsizex, winsizey);
    if (!display) {
        rpclog("Error creating Allegro display.\n");
        return false;
    }
    
    menu = al_build_menu(menu_info);
    if (menu == NULL)
    {
        rpclog("Error creating Allegro menu\n");
        return false;
    }
    
    if (!al_set_display_menu(display, menu))
    {
        rpclog("Error associating Allegro menu with Allegro display\n");
        return false;
    }
    
    updatelinuxgui();

    return true;
}


bool allegro_create_timer_and_events()
{
    timer = al_create_timer(1.0/300);
    if (timer == NULL)
    {
        rpclog("Error creating Allegro timer\n");
        return false;
    }

    displaytimer = al_create_timer(1.0/60);
    if (displaytimer == NULL)
    {
        rpclog("Error creating Allegro display timer\n");
        return false;
    }
    
    events = al_create_event_queue();
    if (events == NULL)
    {
        rpclog("Error creating Allegro event queue\n");
        return false;
    }
    
    al_register_event_source(events, al_get_keyboard_event_source());
    al_register_event_source(events, al_get_timer_event_source(timer));
    al_register_event_source(events, al_get_timer_event_source(displaytimer));
    al_register_event_source(events, al_get_display_event_source(display));
    al_register_event_source(events, al_get_default_menu_event_source());
    al_start_timer(timer);
    al_start_timer(displaytimer);
    return true;
}

void allegro_process_events()
{
    al_wait_for_event(events, &event);
    switch (event.type)
    {
        case ALLEGRO_EVENT_TIMER:
            if(event.timer.source == timer)
                scrupdate();
            else
                drawAtomScreen();
            break;
            
        case ALLEGRO_EVENT_KEY_DOWN:
            if (event.keyboard.keycode == ALLEGRO_KEY_F12)
                atom_reset(0);
            break;
            
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            quited = true;
            break;
            
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            winsizex = al_get_display_width(event.display.source);
            winsizey = al_get_display_height(event.display.source);
            al_acknowledge_resize(event.display.source);
            break;
            
        case ALLEGRO_EVENT_MENU_CLICK:
            (*MENU_fn[event.user.data1])();
            break;
    }
}



//HACK
//DIALOG bemgui[] =
//{
//	{ d_ctext_proc, 200, 260, 0,   0,  15, 0, 0, 0, 0,     0, "Atomulator V1.01" },
//	{ d_menu_proc,	0,   0,	  0,   0,  15, 0, 0, 0, 0,     0, mainmenu	    },
//	{ d_yield_proc },
//	{ 0,		0,   0,	  0,   0,  0,  0, 0, 0, 0,     0, NULL, NULL, NULL  }
//};

//BITMAP *mouse, *_mouse_sprite;
//
//void entergui()
//{
//	int x = 1;
//	DIALOG_PLAYER *dp;
//

//
//	while (keypressed())
//		readkey();
////        while (al_key_down(&keybd, ALLEGRO_KEY_F11)) al_rest(100);
//
//	updatelinuxgui();
//
//	install_mouse();
//
//
//	set_color_depth(desktop_color_depth());
//	show_mouse(screen);
//	if (fullscreen)
//	{
//		bemgui[0].x = (1024 / 2) - 36;
//		bemgui[0].y = 768 - 8;
//	}
//	else
//	{
//		bemgui[0].x = (windx / 2) - 36;
//		bemgui[0].y = windy - 8;
//	}
//	bemgui[0].fg = makecol(255, 255, 255);
//	dp = init_dialog(bemgui, 0);
//	while (x && !(al_key_down(&keybd, ALLEGRO_KEY_F11) && !oldf11) && !al_key_down(&keybd, ALLEGRO_KEY_ESCAPE))
//	{
//		oldf11 = al_key_down(&keybd, ALLEGRO_KEY_F11);
//		x = update_dialog(dp);
//	}
//	shutdown_dialog(dp);
//	show_mouse(NULL);
//	set_color_depth(8);
//
//	remove_mouse();
//
//
//
//	clearscreen();

//

//}

// MH
// define an array of functions for easy calling from linux.c


#endif
