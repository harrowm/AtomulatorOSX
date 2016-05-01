/*Atomulator v1.0 by Tom Walker
  cross platform main function for Allegro 5 */

#define ALLEGRO_STATICLINK

#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_native_dialog.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <allegro5/allegro_image.h> // fixed font file is in an image format

#include "atom.h"

// called in uef.c and csw.c but not used
void cataddname (char *s) { rpclog("%s\n", s); }

// used by the debugger to force Atomulator to exit
bool quited = false;

// Allegro variables used
ALLEGRO_KEYBOARD_STATE keybd;
ALLEGRO_JOYSTICK *joystick;
ALLEGRO_TIMER *timer;
ALLEGRO_TIMER *displaytimer;
ALLEGRO_TIMER *hudtimer;
ALLEGRO_EVENT_QUEUE *events;
ALLEGRO_EVENT event;
ALLEGRO_DISPLAY *display;
ALLEGRO_DISPLAY *memDisplay;

ALLEGRO_MENU *menu;
ALLEGRO_FONT *font;
ALLEGRO_AUDIO_STREAM *stream;
ALLEGRO_AUDIO_STREAM *ddstream;
ALLEGRO_PATH *exepath; // Path to root of resources directory (fonts, roms, ddnoise)
ALLEGRO_PATH *docpath; // Path to root of executable directory (mmc, rlog file, atom.cfg file)

ALLEGRO_COLOR whiteColour;
ALLEGRO_COLOR blackColour;

extern ALLEGRO_MENU_INFO menu_info[]; 

extern void update_gui();

// On Debian, adding the menus to the display reduces the window size.
// We have to look for a resize event and then resize the window on
// start up.  These variables manage that process.
int origwinsizex;
int origwinsizey;
int winMenuHeight = 0;
bool displayjustcreated = true;

bool initJoystick()
{
	int a, i;

    int num_joysticks = al_get_num_joysticks();
    
    if (num_joysticks == 0)
        rpclog("No joysticks installed\n");
    else
        rpclog("Number of joysticks: %d\n", num_joysticks);
    
    if (num_joysticks > 0) {
        joystick = al_get_joystick(0);
        rpclog("Joystick: '%s'\n", al_get_joystick_name(joystick));
        rpclog("  Buttons:");
        int n = al_get_joystick_num_buttons(joystick);
        for (i = 0; i < n; i++)
            rpclog(" '%s'", al_get_joystick_button_name(joystick, i));
        rpclog("\n");
            
        n = al_get_joystick_num_sticks(joystick);
        for (i = 0; i < n; i++)
        {
            rpclog("  Stick %d: '%s'\n", i, al_get_joystick_stick_name(joystick, i));
            for (a = 0; a < al_get_joystick_num_axes(joystick, i); a++)
                rpclog("    Axis %d: '%s'\n", a, al_get_joystick_axis_name(joystick, i, a));
        }
    }
    else
        joystick = NULL;
    
    return true;
}

bool allegro_init()
{
	// initialize allegro and required addons
    if (!(al_init() && al_init_image_addon() && al_init_primitives_addon() &&
		al_install_mouse() && al_install_keyboard() && al_install_joystick() &&
		al_init_font_addon() && al_init_acodec_addon() && al_install_audio() &&
        al_init_native_dialog_addon()))
    {
		rpclog("ERROR: Can't initialize Allegro - quiting!\n");
        return false;
    }
	
	// Paths
    // For OSX we want to store some files in the bundle rather than off the directory containing the executable
    // OSX: in ~/Documents/Atomulator/ - mmc, atom.cfg, rlog.txt
    // OSX: in executable directory (ie within the bundle): fonts, ddnoise, roms
    //
    // on Windows/Linux - all in executable directory
    
   	char fontpath[MAXPATH-1];

   	al_set_org_name("");
   	al_set_app_name("Atomulator");  // Explicitly set to remove .exe on Windows

#ifdef __APPLE__
   	docpath = al_get_standard_path(ALLEGRO_USER_DOCUMENTS_PATH);
	if (!docpath)
	{
		rpclog("ERROR: Cannot get path for resources - usually Documents in the home directory.\n");
		return false;
	}

    // Create the directory for mmc if it doesn't exist
	al_append_path_component(docpath, "Atomulator");
	al_append_path_component(docpath, "mmc");
   	al_make_directory(al_path_cstr(docpath, ALLEGRO_NATIVE_PATH_SEP));
   	al_drop_path_tail(docpath); //remove "mmc"
#else
    docpath = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
    if (!docpath)
    {
        rpclog("ERROR: Cannot get path to documents (ALLEGRO_RESOURCES_PATH).\n");
        return false;
    }
#endif
    
   	exepath = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
	if (!exepath)
	{
		rpclog("ERROR: Cannot get path to executable (ALLEGRO_RESOURCES_PATH).\n");
		return false;
	}

	// On MS Visual Studio, running in immediate mode from the compiler uses a "Debug" directory, lets remove
	// this to stop having to manually copy files around when coding. Similarly, if we are running the release version 
	// from the release directory we remove that
#ifdef _MSC_VER
	if ((strcmp(al_get_path_tail(exepath), "Debug") == 0) || (strcmp(al_get_path_tail(exepath), "Release") == 0))
    {
		al_drop_path_tail(exepath);	
        al_drop_path_tail(docpath);
    }
#endif

	// Get the font file
    al_append_path_component(exepath, "fonts");
   	al_set_path_filename(exepath, "fixed_font.tga");
   	strlcpy(fontpath, al_path_cstr(exepath, ALLEGRO_NATIVE_PATH_SEP), MAXPATH-1);
	al_drop_path_tail(exepath); //leave exepath correct for subsequent uses

    font = al_load_font(fontpath, 0, 0);
    if (!font)
    {
        rpclog("ERROR: Failed to load font from: %s.\n", fontpath);
        return false;
    }
    
    whiteColour = al_map_rgb(255, 255, 255);
    blackColour = al_map_rgb(0, 0, 0);
	
    return true;
}

void logDisplay(ALLEGRO_DISPLAY *d)
{
    char displayDesc[ALLEGRO_NUM_PIXEL_FORMATS][50] = {
        "ALLEGRO_PIXEL_FORMAT_ANY",
        "ALLEGRO_PIXEL_FORMAT_ANY_NO_ALPHA",
        "ALLEGRO_PIXEL_FORMAT_ANY_WITH_ALPHA",
        "ALLEGRO_PIXEL_FORMAT_ANY_15_NO_ALPHA",
        "ALLEGRO_PIXEL_FORMAT_ANY_16_NO_ALPHA",
        "ALLEGRO_PIXEL_FORMAT_ANY_16_WITH_ALPHA",
        "ALLEGRO_PIXEL_FORMAT_ANY_24_NO_ALPHA",
        "ALLEGRO_PIXEL_FORMAT_ANY_32_NO_ALPHA",
        "ALLEGRO_PIXEL_FORMAT_ANY_32_WITH_ALPHA",
        "ALLEGRO_PIXEL_FORMAT_ARGB_8888",
        "ALLEGRO_PIXEL_FORMAT_RGBA_8888",
        "ALLEGRO_PIXEL_FORMAT_ARGB_4444",
        "ALLEGRO_PIXEL_FORMAT_RGB_888",
        "ALLEGRO_PIXEL_FORMAT_RGB_565",
        "ALLEGRO_PIXEL_FORMAT_RGB_555",
        "ALLEGRO_PIXEL_FORMAT_RGBA_5551",
        "ALLEGRO_PIXEL_FORMAT_ARGB_1555",
        "ALLEGRO_PIXEL_FORMAT_ABGR_8888",
        "ALLEGRO_PIXEL_FORMAT_XBGR_8888",
        "ALLEGRO_PIXEL_FORMAT_BGR_888",
        "ALLEGRO_PIXEL_FORMAT_BGR_565",
        "ALLEGRO_PIXEL_FORMAT_BGR_555",
        "ALLEGRO_PIXEL_FORMAT_RGBX_8888",
        "ALLEGRO_PIXEL_FORMAT_XRGB_8888",
        "ALLEGRO_PIXEL_FORMAT_ABGR_F32",
        "ALLEGRO_PIXEL_FORMAT_ABGR_8888_LE",
        "ALLEGRO_PIXEL_FORMAT_RGBA_4444",
        "ALLEGRO_PIXEL_FORMAT_SINGLE_CHANNEL_8",
        "ALLEGRO_PIXEL_FORMAT_COMPRESSED_RGBA_DXT1",
        "ALLEGRO_PIXEL_FORMAT_COMPRESSED_RGBA_DXT3",
        "ALLEGRO_PIXEL_FORMAT_COMPRESSED_RGBA_DXT5"
    };
    
    rpclog ("Allegro display is of format %s\n", displayDesc[al_get_display_format(d)]);
}
    
bool allegro_create_display_and_menus()
{
   	/* ALLEGRO_GTK_TOPLEVEL is necessary for menus with GTK - ie Debian */

#ifdef ALLEGRO_GTK_TOPLEVEL
   	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_NO_PRESERVE_TEXTURE | ALLEGRO_GTK_TOPLEVEL);
#else
	al_set_new_display_flags(ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_NO_PRESERVE_TEXTURE);
#endif

    al_set_new_bitmap_flags(ALLEGRO_NO_PRESERVE_TEXTURE);
    
	// The screen handling performance has been improved by directly manipulating the bitmap, code copied from Optima - many thanks hoglet.
	// The code requires a 32bit pixel size
	al_set_new_display_option(ALLEGRO_COLOR_SIZE, 32, ALLEGRO_REQUIRE);

    display = al_create_display(winsizex, winsizey);
    if (display == NULL) 
	{
        rpclog("ERROR: Error creating Allegro display (32bit pixel size required).\n");
        return false;
    }
    
    logDisplay(display);

	// On some flavours of Linux (eg Debian) adding in the menus changes the window size (!!)
	// Have to listen for the resize event and reset   
	// These variables track the initial size and initial creation
	origwinsizex = winsizex;
	origwinsizey = winsizey;
	displayjustcreated = true;

    menu = al_build_menu(menu_info);
    if (menu == NULL)
    {
        rpclog("ERROR: Error creating Allegro menu\n");
        return false;
    }

	al_set_window_title(display, ATOMULATOR_VERSION);
    return true;
}

bool allegro_create_timer_and_events()
{
    timer = al_create_timer(1.0/300);
    if (timer == NULL)
    {
        rpclog("ERROR: Error creating Allegro timer\n");
        return false;
    }

    events = al_create_event_queue();
    if (events == NULL)
    {
        rpclog("ERROR: Error creating Allegro event queue\n");
        return false;
    }

    al_register_event_source(events, al_get_keyboard_event_source());
    al_register_event_source(events, al_get_mouse_event_source());
    al_register_event_source(events, al_get_timer_event_source(timer));
    al_register_event_source(events, al_get_display_event_source(display));
    al_register_event_source(events, al_get_default_menu_event_source());
    al_register_event_source(events, al_get_joystick_event_source());
    
    al_start_timer(timer);

    return true;
}

void allegro_process_events()
{
    al_wait_for_event(events, &event);
    switch (event.type)
    {
		// Place the user events first so response is faster
	
        case ALLEGRO_EVENT_DISPLAY_CLOSE:
            quited = true;
            al_flush_event_queue(events);
            break;

        case ALLEGRO_EVENT_MENU_CLICK:
			processMenuOption(event.user.data1);
            break;
        
        case ALLEGRO_EVENT_MOUSE_BUTTON_DOWN:
            if (event.mouse.y > (winsizey+winMenuHeight))
                debug = 1;
            else
                debug = 0;
            break;
            
        case ALLEGRO_EVENT_KEY_CHAR:
            if (debug == 0)
            {
                if (event.keyboard.keycode == ALLEGRO_KEY_F12)
                    atom_reset(0);
            }
            else // check to see if the user is typing in the debugger console
            {
                handleDebuggerInput(event.keyboard.keycode, event.keyboard.unichar);
			}
            break;
            
        case ALLEGRO_EVENT_DISPLAY_RESIZE:
            al_acknowledge_resize(event.display.source);
            
            // Allegro doesn't make this easy ..
            // If we are on a GUI where the menus are kept in the Window (eg Windows, Debian) rather than at the top of
            // the screen MacOS, Ubuntu then when the Menus are added, the deisplay gets smaller and we have to catch the event
            // and redraw.

            if (displayjustcreated == true)
            {
#if (defined _MSC_VER || defined ALLEGRO_GTK_TOPLEVEL)
                // Switching on the dubgger also forces a screen resize .. if the system isnt running on Debian then
                // the first resize event if on the debugger starting, so dont resize the screen to the original size
                if (!debugon)
                {
                    winMenuHeight = origwinsizey - al_get_display_height(display);
                    al_resize_display(display, origwinsizex, origwinsizey + winMenuHeight);
                }
#endif
                displayjustcreated = false;
            }
            else
            {
                // force the display's aspect ratio to 4/3
                if (debugon)
                {
                    winsizex = ((al_get_display_width(display) - 266) + 3) & ~0x3;  // round to a multiple of 4
                    winsizey = winsizex * 3 / 4;
                    al_resize_display(display, winsizex + 266.0, winsizey + winMenuHeight + al_get_font_line_height(font) * 5); // Allow for the debug input window
                }
                else
                {
                    winsizex = (al_get_display_width(display) + 3) & ~0x3;  // round to a multiple of 4
                    winsizey = winsizex * 3 / 4;
                    al_resize_display(display, winsizex, winsizey + winMenuHeight);
                }
            }
            al_acknowledge_resize(event.display.source);
            break;

        case ALLEGRO_EVENT_TIMER:
            scrupdate();
            break;
    }
}

void allegro_exit()
{
    al_uninstall_joystick();
	al_destroy_path(exepath);
	al_destroy_font(font);
	al_destroy_audio_stream(stream);
	al_destroy_audio_stream(ddstream);

	al_destroy_timer(timer);

    al_set_display_menu(display, NULL);

//    al_destroy_menu(menu); crashes on OSX

	al_destroy_event_queue(events);

	al_set_target_bitmap(NULL);

	// MH - al_destroy_display() hangs .. FIXME
	//al_destroy_display(display);
}

extern int ddframes;

int main(int argc, char **argv)
{
    if (!allegro_init())
        return 1;

    loadconfig();

    if (!allegro_create_display_and_menus())
        return 1;

    if (!allegro_create_timer_and_events())
        return 1;

	// On Linux systems e.g. Debian adding in the menu resizes the window
	// You must start the timers/events first and then catch the system
	// thrown resize event.

    if (!al_set_display_menu(display, menu))
    {
        rpclog("ERROR: Error associating Allegro menu with Allegro display\n");
        return 1;
    }

	if (!initSound())
		return 1;
    
    if (!initJoystick())
        return 1;
	
	update_gui();
    atom_init(argc, argv);

    while (!quited)
    {
        atom_run();
        allegro_process_events();
    }

	atom_exit();
    allegro_exit();
	return 0;
}
