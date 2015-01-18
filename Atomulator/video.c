/*Atomulator v1.0 by Tom Walker
   6847 video emulation*/

#include <stdio.h>
#include <allegro.h>
#include <allegro_primitives.h>  // for drawing rectangles
#include <allegro5/allegro_font.h>

extern char* getPath();

#include "atom.h"

int fullscreen = 0;
int winsizex = 512, winsizey = 384;

uint8_t fontdata[] =
{
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x02, 0x1a, 0x2a, 0x2a, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x08, 0x14, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3c, 0x12, 0x12, 0x1c, 0x12, 0x12, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x20, 0x20, 0x20, 0x22, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3c, 0x12, 0x12, 0x12, 0x12, 0x12, 0x3c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3e, 0x20, 0x20, 0x3c, 0x20, 0x20, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3e, 0x20, 0x20, 0x3c, 0x20, 0x20, 0x20, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1e, 0x20, 0x20, 0x26, 0x22, 0x22, 0x1e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x3e, 0x22, 0x22, 0x22, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x02, 0x02, 0x02, 0x22, 0x22, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x24, 0x28, 0x30, 0x28, 0x24, 0x22, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x36, 0x2a, 0x2a, 0x22, 0x22, 0x22, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x32, 0x2a, 0x26, 0x22, 0x22, 0x22, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3e, 0x22, 0x22, 0x22, 0x22, 0x22, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x3c, 0x20, 0x20, 0x20, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x22, 0x2a, 0x24, 0x1a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3c, 0x22, 0x22, 0x3c, 0x28, 0x24, 0x22, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x10, 0x08, 0x04, 0x22, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3e, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x14, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x22, 0x22, 0x2a, 0x2a, 0x36, 0x22, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x14, 0x22, 0x22, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3e, 0x02, 0x04, 0x08, 0x10, 0x20, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x38, 0x20, 0x20, 0x20, 0x20, 0x20, 0x38, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x20, 0x20, 0x10, 0x08, 0x04, 0x02, 0x02, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0e, 0x02, 0x02, 0x02, 0x02, 0x02, 0x0e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x08, 0x1c, 0x2a, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x10, 0x3e, 0x10, 0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x14, 0x14, 0x36, 0x00, 0x36, 0x14, 0x14, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x08, 0x1e, 0x20, 0x1c, 0x02, 0x3c, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x32, 0x32, 0x04, 0x08, 0x10, 0x26, 0x26, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x10, 0x28, 0x28, 0x10, 0x2a, 0x24, 0x1a, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x08, 0x10, 0x20, 0x20, 0x20, 0x10, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x1c, 0x3e, 0x1c, 0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x3e, 0x08, 0x08, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x10, 0x20, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x30, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x02, 0x02, 0x04, 0x08, 0x10, 0x20, 0x20, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x18, 0x24, 0x24, 0x24, 0x24, 0x24, 0x18, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x08, 0x18, 0x08, 0x08, 0x08, 0x08, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x02, 0x1c, 0x20, 0x20, 0x3e, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x02, 0x0c, 0x02, 0x22, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x04, 0x0c, 0x14, 0x3e, 0x04, 0x04, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3e, 0x20, 0x3c, 0x02, 0x02, 0x22, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0x20, 0x20, 0x3c, 0x22, 0x22, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3e, 0x02, 0x04, 0x08, 0x10, 0x20, 0x20, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x1c, 0x22, 0x22, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x1c, 0x22, 0x22, 0x1e, 0x02, 0x02, 0x1c, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x18, 0x18, 0x00, 0x18, 0x18, 0x08, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x3e, 0x00, 0x3e, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x18, 0x24, 0x04, 0x08, 0x08, 0x00, 0x08, 0x00, 0x00,
};

ALLEGRO_COLOR convertcolor(int c)
{
    if (colourboard)
    {
        switch(c)
        {
            case 0:
                return al_map_rgb(0, 0, 0); /*Black*/
            case 1:
                return al_map_rgb(0, 63, 0); /*Green*/
            case 2:
                return al_map_rgb(63, 63, 0); /*Yellow*/
            case 3:
                return al_map_rgb(0, 0, 63); /*Blue*/
            case 4:
                return al_map_rgb(63, 0, 0); /*Red*/
            case 5:
                return al_map_rgb(63, 63, 63); /*Buff*/
            case 6:
                return al_map_rgb(0, 63, 63); /*Cyan*/
            case 7:
                return al_map_rgb(63, 0, 63); /*Magenta*/
            default:
                return al_map_rgb(63, 0, 0); /*Orange - actually red on the Atom*/

        }
    }
    else
    {
        switch(c)
        {
            case 0:
                return al_map_rgb(8, 8, 8); /*Black*/
            case 1:
                return al_map_rgb(55, 55, 55); /*Green*/
            case 2:
                return al_map_rgb(63, 63, 63); /*Yellow*/
            case 3:
                return al_map_rgb(32, 32, 32); /*Blue*/
            case 4:
                return al_map_rgb(32, 32, 32); /*Red*/
            case 5:
                return al_map_rgb(63, 63, 63); /*Buff*/
            case 6:
                return al_map_rgb(55, 55, 55); /*Cyan*/
            case 7:
                return al_map_rgb(55, 55, 55); /*Magenta*/
            default:
                return al_map_rgb(55, 55, 55); /*Orange - actually red on the Atom*/
        }

    }
}
                                  
ALLEGRO_COLOR textcol(int c)
{
    if (colourboard)
    {
        switch(c)
        {
            case 0:
                return al_map_rgb(0, 0, 0); /*Black*/
            case 1:
                return al_map_rgb(0, 63, 0); /*Green*/
            case 2:
                return al_map_rgb(0, 0, 0); /*Black*/
            default:
                return al_map_rgb(63, 0, 0); /*Orange - actually red on the Atom*/
        }
    }
    else
    {
        switch(c)
        {
            case 0:
                return al_map_rgb(0, 0, 0); /*Black*/
            case 1:
                return al_map_rgb(55, 55, 55); /*Green*/
            case 2:
                return al_map_rgb(0, 0, 0); /*Black*/
            default:
                return al_map_rgb(55, 55, 55); /*Orange - actually red on the Atom*/
        }
    }
}

ALLEGRO_COLOR grcol(int c)
{
    if (colourboard)
    {
        switch(c)
        {
            case 0:
                return al_map_rgb(0, 0, 0); /*Black*/
            case 1:
                return al_map_rgb(0, 63, 0); /*Green*/
            case 2:
                return al_map_rgb(0, 0, 0); /*Black*/
            default:
                return al_map_rgb(63, 63, 63); /*Buff*/
        }
    }
    else
    {
        switch(c)
        {
            case 0:
                return al_map_rgb(0, 0, 0); /*Black*/
            case 1:
                return al_map_rgb(55, 55, 55); /*Green*/
            case 2:
                return al_map_rgb(0, 0, 0); /*Black*/
            default:
                return al_map_rgb(63, 63, 63); /*Buff*/
        }
    }
}


ALLEGRO_BITMAP *b2;
ALLEGRO_STATE state;

#define ATOM_SCREEN_WIDTH 256.0
#define ATOM_SCREEN_HEIGHT 192.0

ALLEGRO_FONT *font;

void initvideo()
{
    b2 = al_create_bitmap(ATOM_SCREEN_WIDTH, ATOM_SCREEN_HEIGHT);
    
    al_init_font_addon();
    
    font = al_load_font(getPath("fixed_font.tga"), 0, 0);
    if (!font)
    {
        printf("failed to load font.\n");
        return;
    }
    
    al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
    al_set_target_bitmap(b2);
    al_lock_bitmap(b2, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
}


uint8_t *ram;
int cy = 0, sy = 0;

int tapeon;
int frmcount;
int fskipcount = 0;

// For onscreen speed display
int totalframes = 0;
int old_totalframes = 0;
extern int totcyc; // from 6502.c
int old_totalcycles = 0;
double old_time = 0;
double new_time = 0;
char hudbuf[256];

char scrshotname[260];
int savescrshot = 0;

uint8_t fetcheddat[32];

void drawline(int line)
{
	int addr, chr, col;
    ALLEGRO_COLOR acol;
	int x, xx;
	uint8_t temp;
    
	if (!line)
		vbl = cy = sy = 0;
  
	if (line < 192)
	{
        //printf(" In draw with gfxmode %d\n", gfxmode);
        switch (gfxmode)
		{
		case 0: case 2: case 4: case 6:         /*Text mode*/
		case 8: case 10: case 12: case 14:
			addr = (cy << 5) + 0x8000;
                
			for (x = 0; x < 256; x += 8)
			{
				chr = fetcheddat[x >> 3];
				if (chr & 0x40)
				{
					temp = chr;
					chr <<= ((sy >> 2) << 1);
					chr = (chr >> 4) & 3;
					if (chr & 2)
						col = ((temp >> 6) | (css << 1))+1;
					else
						col = 0;
                    acol = convertcolor(col);
                    
                    al_put_pixel(x, line, acol);
                    al_put_pixel(x+1, line, acol);
                    al_put_pixel(x+2, line, acol);
                    al_put_pixel(x+3, line, acol);
                    
					if (chr & 1)
						col = ((temp >> 6) | (css << 1))+1;
					else
						col = 0;
                    acol = convertcolor(col);

                    al_put_pixel(x+4, line, acol);
                    al_put_pixel(x+5, line, acol);
                    al_put_pixel(x+6, line, acol);
                    al_put_pixel(x+7, line, acol);
                    
				}
				else
				{
					chr = ((chr & 0x3F) * 12) + sy;
					if (fetcheddat[x >> 3] & 0x80)
					{
						for (xx = 0; xx < 8; xx++)
						{
							al_put_pixel(x + xx, line, textcol((((fontdata[chr] >> (xx ^ 7)) & 1) ^ 1) | css));
						}
					}
					else
					{
						for (xx = 0; xx < 8; xx++)
						{
							al_put_pixel(x + xx, line, textcol(((fontdata[chr] >> (xx ^ 7)) & 1) | css));
						}
					}
				}
			}
			sy++;
			if (sy == 12)
			{
				sy = 0;
				cy++;
			}
			addr = (cy << 5) + 0x8000;
			for (x = 0; x < 32; x++)
				fetcheddat[x] = ram[addr++];
			break;
		
		/* Propper graphics modes */
		case 1:         /*64x64, 4 colours*/
			for (x = 0; x < 256; x += 16)
			{
				temp = fetcheddat[x >> 3];
				for (xx = 0; xx < 16; xx += 4)
				{
                    acol = convertcolor((temp >> 6) | (css << 1));
                    al_put_pixel(x+xx, line, acol);
                    al_put_pixel(x+xx+1, line, acol);
                    al_put_pixel(x+xx+2, line, acol);
                    al_put_pixel(x+xx+3, line, acol);
					temp <<= 2;
				}
			}
			
			addr = (((line + 1) / 3) << 4) | 0x8000;
			for (x = 0; x < 32; x++)
				fetcheddat[x] = ram[addr + (x >> 1)];
			
			break;
			
		case 3:         /*128x64, 2 colours*/
			for (x = 0; x < 256; x += 16)
			{
				temp = fetcheddat[x >> 3];
				for (xx = 0; xx < 16; xx += 2)
				{
                    acol = (temp & 0x80) ? grcol(css | 1) : grcol(css);
                    al_put_pixel(x+xx, line, acol);
                    al_put_pixel(x+xx+1, line, acol);

					temp <<= 1;
				}
			}
			
			addr = (((line + 1) / 3) << 4) | 0x8000;
			for (x = 0; x < 32; x++)
				fetcheddat[x] = ram[addr + (x >> 1)];
			
			break;

/* CHANGED FOR CORRECT CLEAR2a */

		case 5: /*128x64, 4 colours*/
			for (x = 0; x < 256; x += 8)
			{
				temp = fetcheddat[x >> 3];
                for (xx = 0; xx < 8; xx += 2)
				{
                    acol = convertcolor((temp >> 6) |(css << 1));
                    al_put_pixel(x+xx, line, acol);
                    al_put_pixel(x+xx+1, line, acol);
					temp <<= 2;
				}
			}

			addr = (((line + 1) / 3) << 5) | 0x8000;
			for (x = 0; x < 32; x++)
			fetcheddat[x] = ram[addr + x];
			break;

/* END CHANGES */

		case 7:         /*128x96, 2 colours*/
			for (x = 0; x < 256; x += 16)
			{
				temp = fetcheddat[x >> 3];
                for (xx = 0; xx < 16; xx += 2)
				{
                    acol = (temp & 0x80) ? grcol(css | 1) : grcol(css);
                    al_put_pixel(x+xx, line, acol);
                    al_put_pixel(x+xx+1, line, acol);
					temp <<= 1;
				}
			}

			addr = (((line + 1) >> 1) << 4) | 0x8000;
			for (x = 0; x < 32; x++)
				fetcheddat[x] = ram[addr + (x >> 1)];

			break;

		case 9:         /*128x96, 4 colours*/
			for (x = 0; x < 256; x += 8)
			{
				temp = fetcheddat[x >> 3];
				for (xx = 0; xx < 8; xx += 2)
				{
                    acol = convertcolor((temp >> 6) |(css << 1));
                    al_put_pixel(x+xx, line, acol);
                    al_put_pixel(x+xx+1, line, acol);
					temp <<= 2;
				}
			}

			addr = (((line + 1) >> 1) << 5) | 0x8000;

			for (x = 0; x < 32; x++)
				fetcheddat[x] = ram[addr + x];

			break;

		case 11:         /*128x192, 2 colours*/
			for (x = 0; x < 256; x += 16)
			{
				temp = fetcheddat[x >> 3];
				for (xx = 0; xx < 16; xx += 2)
				{
                    acol = (temp & 0x80) ? grcol(css | 1) : grcol(css);
                    al_put_pixel(x+xx, line, acol);
                    al_put_pixel(x+xx+1, line, acol);
					temp <<= 1;
				}
			}

			addr = ((line + 1) << 4) | 0x8000;
			for (x = 0; x < 32; x++)
				fetcheddat[x] = ram[addr + (x >> 1)];

			break;

		case 13:         /*128x192, 4 colours*/
			for (x = 0; x < 256; x += 8)
			{
				temp = fetcheddat[x >> 3];
                for (xx = 0; xx < 8; xx += 2)
				{
                    acol = convertcolor((temp >> 6) |(css << 1));
                    al_put_pixel(x+xx, line, acol);
                    al_put_pixel(x+xx+1, line, acol);
					temp <<= 2;
				}
			}

			addr = ((line + 1) << 5) | 0x8000;

			for (x = 0; x < 32; x++)
				fetcheddat[x] = ram[addr + x];

			break;

		case 15:         /*256x192, 2 colours*/
			for (x = 0; x < 256; x += 8)
			{
				temp = fetcheddat[x >> 3];
				for (xx = 0; xx < 8; xx++)
				{
                    acol = (temp & 0x80) ? grcol(css | 1) : grcol(css);
                    al_put_pixel(x+xx, line, acol);

					temp <<= 1;
				}
			}

			addr = ((line + 1) << 5) | 0x8000;
/*rpclog("addr=%04X\n",addr);*/
			for (x = 0; x < 32; x++)
				fetcheddat[x] = ram[addr + x];

			break;

//                        default:
//                        printf("Bad GFX mode %i\n",gfxmode);
//                        dumpregs();
//                        dumpram();
//                        exit(-1);
		}
	}
    
    
	if (line == 192)
	{
		startblit();
		frmcount++;
		fskipcount++;

		if (savescrshot)
		{
			savescrshot = 0;
			al_save_bitmap(scrshotname, b2);
        }

		if ((!(tapeon && fasttape) && fskipcount >= fskipmax) || frmcount == 60)
		{
            al_unlock_bitmap(b2);
            al_restore_state(&state);

			fskipcount = 0;
            
			if (tapeon)
            {
				al_draw_filled_rectangle(ATOM_SCREEN_WIDTH - 12, 0, ATOM_SCREEN_WIDTH, 4, al_map_rgb(255, 0, 0));
            }
            
            totalframes++;
            new_time = al_get_time();
            
            if (showspeed && ((new_time-old_time)>1.0))
            {
                sprintf(hudbuf,"MHz %2.2f FPS %3.1f", (totcyc-old_totalcycles)/((new_time - old_time)*1000000), (totalframes-old_totalframes)/(new_time - old_time));

                old_time = new_time;
                old_totalframes = totalframes;
                old_totalcycles = totcyc;
            }
                
            // draw a pattern on the screen and scale to max
            al_draw_scaled_bitmap(b2, 0, 0, ATOM_SCREEN_WIDTH, ATOM_SCREEN_HEIGHT, 0, 0, winsizex, winsizey, 0);

            if (showspeed) al_draw_text(font, al_map_rgb(255, 255, 255), 0.0, 0.0, 0, hudbuf);

            al_flip_display();
			frmcount = 0;
            
            al_store_state(&state, ALLEGRO_STATE_TARGET_BITMAP);
            al_set_target_bitmap(b2);
            al_lock_bitmap(b2, ALLEGRO_PIXEL_FORMAT_ANY, ALLEGRO_LOCK_WRITEONLY);
		}
		endblit();
	}

	if (line == 200)
		vbl = 1;

	if (line == 261)
		vbl = 0;

	if ((line == 261 && !colourboard) || line == 311)
	{
		switch (gfxmode)
		{

/* CHANGED FOR CORRECT CLEAR2a */

		case 0: case 2: case 4: case 6:         /*Text mode*/
		case 8: case 10: case 12: case 14:
		case 5: case 9: case 13: case 15:
			for (x = 0; x < 32; x++)
				fetcheddat[x] = ram[0x8000 + x];
			break;
/* END CHANGES */

		case 1: case 3: case 7: case 11:         /*16-byte per line*/
			for (x = 0; x < 32; x++)
				fetcheddat[x] = ram[0x8000 + (x >> 1)];
			break;

		}
	}

//        sndbuffer[line]=(speaker)?255:0;
}

/*void mixaudio(uint8_t *p)
   {
        memcpy(p,sndbuffer,262);
   }*/

void enterfullscreen()
{
/*	if (opengl)
        {
                rpclog("Enter fullscreen start\n");
                openglreinit();
                rpclog("Enter fullscreen end\n");
                return;
        }*/
    
    //HACK
//	#ifdef WIN32
//	destroy_bitmap(b2);
//	#endif
//
//	set_color_depth(depth);
//	set_gfx_mode(GFX_AUTODETECT_FULLSCREEN, 1024, 768, 0, 0);
//
//	#ifdef WIN32
//	b2 = create_video_bitmap(256, 192);
//	#endif
//
//	set_color_depth(8);
//	updatepal();
//
}

void leavefullscreen()
{
/*	if (opengl)
        {
                openglreinit();
                return;
        }*/

//HACK
//#ifdef WIN32
//	destroy_bitmap(b2);
//	#endif
//
//	set_color_depth(depth);
//
//#ifdef WIN32
//	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 2048, 2048, 0, 0);
//#else
//	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 512, 384, 0, 0);
//#endif
//
//	#ifdef WIN32
//	b2 = create_video_bitmap(256, 192);
//	#endif
//
//	set_color_depth(8);
//	updatepal();
//
//	updatewindowsize(512, 384);
}


