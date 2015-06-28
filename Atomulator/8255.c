/*Atomulator v1.0 by Tom Walker
   8255 PIA + keyboard + sound emulation*/
#include <string.h>
#include <Allegro5/allegro.h>
#include "atom.h"
#include "sid_atom.h"

#include "Allegro5/allegro_audio.h"

extern ALLEGRO_KEYBOARD_STATE keybd; // used in the keyboard routines

#define SNDBUFLEN 312*2*2

int16_t sndbuffer[SNDBUFLEN];
int sndpos = 0;

/*SWARM - CTRL=LEFT, ADJ=RIGHT, REPT=FIRE*/
/*PINBALL - SHIFT,REPT*/
int output;
int tapecyc;
int inchunk;
int intone = 0, tapedat, hightone = 0;
int bytevalid = 0, bitvalid = 0;
int tapeon = 0;
uint16_t databyte;

uint16_t pc;

int keyl[128];
int keys[16][6] =
{
	{ 0,                        ALLEGRO_KEY_3,          ALLEGRO_KEY_MINUS,      ALLEGRO_KEY_G,          ALLEGRO_KEY_Q, ALLEGRO_KEY_ESCAPE },
	{ 0,                        ALLEGRO_KEY_2,          ALLEGRO_KEY_COMMA,      ALLEGRO_KEY_F,          ALLEGRO_KEY_P, ALLEGRO_KEY_Z   },
	{ ALLEGRO_KEY_UP,           ALLEGRO_KEY_1,          ALLEGRO_KEY_SEMICOLON,  ALLEGRO_KEY_E,          ALLEGRO_KEY_O, ALLEGRO_KEY_Y   },
	{ ALLEGRO_KEY_RIGHT,        ALLEGRO_KEY_0,          ALLEGRO_KEY_QUOTE,      ALLEGRO_KEY_D,          ALLEGRO_KEY_N, ALLEGRO_KEY_X   },
	{ ALLEGRO_KEY_CAPSLOCK,     ALLEGRO_KEY_BACKSPACE,  ALLEGRO_KEY_9,          ALLEGRO_KEY_C,          ALLEGRO_KEY_M, ALLEGRO_KEY_W   },
	{ ALLEGRO_KEY_TAB,          ALLEGRO_KEY_END,        ALLEGRO_KEY_8,          ALLEGRO_KEY_B,          ALLEGRO_KEY_L, ALLEGRO_KEY_V   },
	{ ALLEGRO_KEY_CLOSEBRACE,   ALLEGRO_KEY_ENTER,      ALLEGRO_KEY_7,          ALLEGRO_KEY_A,          ALLEGRO_KEY_K, ALLEGRO_KEY_U   },
	{ ALLEGRO_KEY_BACKSLASH,    0,                      ALLEGRO_KEY_6,          ALLEGRO_KEY_EQUALS,     ALLEGRO_KEY_J, ALLEGRO_KEY_T   },
	{ ALLEGRO_KEY_OPENBRACE,    0,                      ALLEGRO_KEY_5,          ALLEGRO_KEY_SLASH,      ALLEGRO_KEY_I, ALLEGRO_KEY_S   },
	{ ALLEGRO_KEY_SPACE,        0,                      ALLEGRO_KEY_4,          ALLEGRO_KEY_FULLSTOP,   ALLEGRO_KEY_H, ALLEGRO_KEY_R   }
};

void init8255()
{
	int c, d;

	memset(keyl, 0, sizeof(keyl));
	for (c = 0; c < 16; c++)
	{
		for (d = 0; d < 6; d++)
		{
			keyl[keys[c][d]] = c | (d << 4) | 0x80;
		}
	}
}

int keyrow;
void write8255(uint16_t addr, uint8_t val)
{
	switch (addr & 0xF)
	{
	case 0:
		keyrow = val & 0xF;
		gfxmode = (val >> 4) & 0x0F;
		break;
	case 2:
		css = (val & 8) >> 2;
		speaker = val & 4;
		break;
	case 3:
		switch (val & 0xE)
		{
		case 0x4: 
			speaker = val & 1;                 
			break;
			
		case 0x6: 
			css = (val & 1) ? 2 : 0; 
			break;
		}
		break;
	}
}

uint8_t read8255(uint16_t addr)
{
	uint8_t temp = 0xFF;
	int c;
    int *kptr;
    int i;

    al_get_keyboard_state(&keybd);
    
    switch (addr & 3)
	{
	case 0:
		return (keyrow & 0x0F) | ((gfxmode << 4) & 0xF0);
	case 1:
        kptr = keys[keyrow & 0xF];
        for (i = 0; i < 6; i++)
        {
            c = *kptr++;
            if (al_key_down(&keybd, c))
                temp &= ~(1 << ((keyl[keylookup[c]] & 0x70) >> 4));
        }

        if (al_key_down(&keybd, ALLEGRO_KEY_LCTRL))
            temp &= ~0x40;
        if (al_key_down(&keybd, ALLEGRO_KEY_LSHIFT) || al_key_down(&keybd, ALLEGRO_KEY_RSHIFT))
            temp &= ~0x80;

		return temp;
	case 2:
		if (vbl)
			temp &= ~0x80;
		if (al_key_down(&keybd, ALLEGRO_KEY_ALT) || al_key_down(&keybd, ALLEGRO_KEY_ALTGR))
			temp &= ~0x40;
		if (!css)
			temp &= ~8;
		if (!speaker)
			temp &= ~4;
		if (!intone)
			temp &= ~0x10;
		if (!tapedat)
			temp &= ~0x20;
		return temp;
	}
	return 0;
}

void polltape()
{
	if (cswena)
	{
		if (tapeon)
		{
			tapecyc += getcsw();
			tapedat = !tapedat;
		}
	}
	else
	{
		tapecyc += 794;
		intone ^= 0x10;
		if (tapeon)
		{
			if (hightone)
			{
				hightone--;
				tapedat = hightone & 1;
				inchunk = 0;
				if (!hightone)
					polluef();
			}
			else if (bytevalid)
			{
				if (databyte & 1)
					tapedat = bitvalid & 1;
				else
					tapedat = bitvalid & 2;
				bitvalid--;
				if (!bitvalid)
				{
					bytevalid--;
					databyte >>= 1;
					if (!bytevalid)
						polluef();
					else
					{
						bitvalid = 16;
					}
				}
			}
			else
				polluef();
		}
	}
}

void dcdlow()
{
	hightone = 0;
//        printf("High tone off\n");
}

void dcd()
{
	hightone = 15000;
//        printf("High tone on\n");
}

void receive(uint8_t dat)
{
	bytevalid = 10;
	bitvalid = 16;
	databyte = (dat << 1) | 0x200;
//        rpclog("Recieved byte %02X - first bit %i\n",dat,databyte&1);
	lastdat = dat;
}

extern void giveSoundBuffer();

void pollsound()
{
	int16_t temp = 0;

	if (sndatomsid)
        sid_fillbuf(&temp,1);

	if (spon)
        temp += (speaker) ? 4095 : -4096;
    
	if (tpon)
        temp += (tapedat) ? 2047 : -2048;

	if (temp!=0)
	{
		sndbuffer[sndpos++] = temp;
		sndbuffer[sndpos++] = temp;
	}

	if (sndpos >= SNDBUFLEN)
	{
        sndpos = 0;
        giveSoundBuffer(sndbuffer);
    }
}