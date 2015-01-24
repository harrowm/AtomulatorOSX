/*Atomulator v1.0 by Tom Walker
   Linux main*/

#ifndef WIN32

#include <stdio.h>
#include <allegro.h> // MH Even though we dont use any Allegro function in this file, not including the header file hangs in OSX, not sure why
#include "atom.h"

extern bool allegro_init();
extern bool allegro_create_display_and_menus();
extern bool allegro_create_timer_and_events();
extern void allegro_process_events();



int quited = 0;


#include <CoreFoundation/CoreFoundation.h>
char *getPath(char *file)
{
    // Search for the file in the main bundle - look in all directories
    CFURLRef fileURL = CFBundleCopyResourceURL(CFBundleGetMainBundle(), CFStringCreateWithCString(NULL, file, kCFStringEncodingUTF8), NULL, NULL);
    
    // return NULL if not found
    if (fileURL == NULL)
        return NULL;
    
    // Convert the string reference into a C string
    return (char*) CFStringGetCStringPtr(CFURLCopyFileSystemPath(fileURL, kCFURLPOSIXPathStyle), CFStringGetSystemEncoding());
}

void startblit()
{
}
void endblit()
{
}
void updatewindowsize(int x, int y)
{
}

int keylookup[128];

void cataddname(char *s)
{
}

int main(int argc, char **argv)
{
    if (!allegro_init())
        return 1;

    loadconfig();
    
    if (!allegro_create_display_and_menus())
        return 1;
	
    if (!allegro_create_timer_and_events())
        return 1;
    
    atom_init(argc, argv);
    
    while (!quited)
    {
        atom_run();
        allegro_process_events();
    }
    
	atom_exit();
    //allegro_exit();
	return 0;
}

#endif


