//
//  buildversion.h
//  AtomulatorOSX
//
//  Created by Katherine Cramer on 6/12/22.
//  Copyright © 2022 Katherine Cramer. All rights reserved.
//

#ifndef buildversion_h
#define buildversion_h

struct VERSION {
    int major;
    int minor;
    int build;
};

static struct VERSION getVersion() {
    struct VERSION v = {1, 30, 3};
    return v;
}

static void getVersionString(char * vers) {
    struct VERSION v = getVersion();
    char major[10];
    char minor[10];
    char build[20];
    
    sprintf(major, "%i.", v.major);
    sprintf(minor, "%i.", v.minor);
    sprintf(build, "%i", v.build);
    
    strcpy(vers, major);
    strcat(vers, minor);
    strcat(vers, build);
}

#endif /* buildversion_h */
