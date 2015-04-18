/*Atomulator v1.0 by Tom Walker
   Disc drive noise*/

#include <allegro.h>
#include <allegro_audio.h>
#include <stdio.h>
#include <string.h>
#include "atom.h"

int ddvol = 3;
ALLEGRO_SAMPLE *seeksmp[4][2], *seek2smp, *seek3smp;
ALLEGRO_SAMPLE *stepsmp;
ALLEGRO_SAMPLE *motorsmp[3];

float ddnoise_mpos = 0;
int ddnoise_mstat = -1;
int oldmotoron = 0;

float ddnoise_spos = 0;
int ddnoise_sstat = -1;
int ddnoise_sdir = 0;
int ddtype = 0;

extern char* getPath();

ALLEGRO_SAMPLE* loadSampleFile(char* f)
{
    ALLEGRO_SAMPLE* s;

    if (f==NULL)
    {
        rpclog("Failed to load sample disc drive noise sample\n");
        return NULL;
    }

    s = al_load_sample(f);
    
    if (s == NULL)
        rpclog("Failed to load sample %s\n", f);
    return s;
}

void loaddiscsamps()
{
    if (ddtype)
    {
        seeksmp[0][0] = loadSampleFile(getPath("stepo.wav"));
        seeksmp[0][1] = loadSampleFile(getPath("stepi.wav"));
        seeksmp[1][0] = loadSampleFile(getPath("seek1o.wav"));
        seeksmp[1][1] = loadSampleFile(getPath("seek1i.wav"));
        seeksmp[2][0] = loadSampleFile(getPath("seek2o.wav"));
        seeksmp[2][1] = loadSampleFile(getPath("seek2i.wav"));
        seeksmp[3][0] = loadSampleFile(getPath("seek3o.wav"));
        seeksmp[3][1] = loadSampleFile(getPath("seek3i.wav"));
        
        motorsmp[0] = loadSampleFile(getPath("motoron35.wav"));
        motorsmp[1] = loadSampleFile(getPath("motor35.wav"));
        motorsmp[2] = loadSampleFile(getPath("motoroff35.wav"));
    }
    else
    {
        seeksmp[0][0] = loadSampleFile(getPath("step.wav"));
        seeksmp[0][1] = loadSampleFile(getPath("step.wav"));
        seeksmp[1][0] = loadSampleFile(getPath("seek.wav"));
        seeksmp[1][1] = loadSampleFile(getPath("seek.wav"));
        seeksmp[2][0] = loadSampleFile(getPath("seek3.wav"));
        seeksmp[2][1] = loadSampleFile(getPath("seek3.wav"));
        seeksmp[3][0] = loadSampleFile(getPath("seek2.wav"));
        seeksmp[3][1] = loadSampleFile(getPath("seek2.wav"));

        motorsmp[0] = loadSampleFile(getPath("motoron525.wav"));
        motorsmp[1] = loadSampleFile(getPath("motor525.wav"));
        motorsmp[2] = loadSampleFile(getPath("motoroff525.wav"));
	}
}

void closeddnoise()
{
	int c;

	for (c = 0; c < 4; c++)
	{
		if (seeksmp[c][0])
			al_destroy_sample(seeksmp[c][0]);
		if (seeksmp[c][1])
			al_destroy_sample(seeksmp[c][1]);
		seeksmp[c][0] = seeksmp[c][1] = NULL;
	}
	for (c = 0; c < 3; c++)
	{
		if (motorsmp[c])
			al_destroy_sample(motorsmp[c]);
		motorsmp[c] = NULL;
	}
}

int16_t ddbuffer[4410];

void ddnoise_seek(int len)
{
//        printf("Seek %i tracks\n",len);
	ddnoise_sdir = (len < 0) ? 1 : 0;

	if (len < 0)
		len = -len;

	ddnoise_spos = 0;

	if (len == 0)
	{
		ddnoise_sstat = -1; fdctime = 200;
	}
	else if (len == 1)
	{
		ddnoise_sstat = 0; fdctime = 140000;
	}
	else if (len < 7)
		ddnoise_sstat = 1;
	else if (len < 30)
		ddnoise_sstat = 2;
	else
		ddnoise_sstat = 3;

	if (!sndddnoise)
		fdctime = 200;
//        rpclog("Start seek!\n");
}

FILE *f1, *f2;
void mixddnoise()
{
	int c;


//        if (!f1) f1=fopen("f1.pcm","wb");
//        if (!f2) f2=fopen("f2.pcm","wb");

	memset(ddbuffer, 0, 4410 * 2);
//        fwrite(ddbuffer,4410*2,1,f1);
	if (motoron && !oldmotoron)
	{
		ddnoise_mstat = 0;
		ddnoise_mpos = 0;
	}
	if (!motoron && oldmotoron)
	{
		ddnoise_mstat = 2;
		ddnoise_mpos = 0;
	}

	if (sndddnoise)
	{

		for (c = 0; c < 4410; c++)
		{
			ddbuffer[c] = 0;
			if (ddnoise_mstat >= 0)
			{
				if (ddnoise_mpos >= al_get_sample_length(motorsmp[ddnoise_mstat]))
				{
					ddnoise_mpos = 0;
					if (ddnoise_mstat != 1)
						ddnoise_mstat++;
					if (ddnoise_mstat == 3)
						ddnoise_mstat = -1;
				}
				if (ddnoise_mstat != -1)
				{
//                                if (!c) rpclog("MixM!\n");
//					ddbuffer[c] += ((int16_t)((((int16_t*)motorsmp[ddnoise_mstat]->data)[(int)ddnoise_mpos]) ^ 0x8000) / 2);
//                    ddnoise_mpos += ((float)al_get_sample_frequency(motorsmp[ddnoise_mstat]) / 44100.0);
                    
                    unsigned int *ptr = (unsigned int *)al_get_sample_data(motorsmp[ddnoise_mstat]);
                    ptr += (int)ddnoise_mpos;
                    ddbuffer[c] += ((*ptr)^ 0x8000) / 2;
                    ddnoise_mpos += ((float)al_get_sample_frequency(motorsmp[ddnoise_mstat]) / 44100.0);
                    
//                    ddbuffer[c] += ((int16_t)((((int16_t*)motorsmp[ddnoise_mstat]->data)[(int)ddnoise_mpos]) ^ 0x8000) / 2);
//                    ddnoise_mpos += ((float)motorsmp[ddnoise_mstat]->freq / 44100.0);
				}
			}
		}

		for (c = 0; c < 4410; c++)
		{
			if (ddnoise_sstat >= 0)
			{
//                        rpclog("SSTAT %i %i\n",ddnoise_sstat,c);
				if (ddnoise_spos >= al_get_sample_length(seeksmp[ddnoise_sstat][ddnoise_sdir]))
				{
					if (ddnoise_sstat > 0)
					{
						fdctime = 100;
//                                        printf("FDC time\n");
					}
					ddnoise_spos = 0;
					ddnoise_sstat = -1;
//                                printf("Stat off\n");
//                                rpclog("Stat off!\n");
				}
				else
				{
//                                if (!c) rpclog("MixS!\n");
//HACK					ddbuffer[c] += ((int16_t)((((int16_t*)seeksmp[ddnoise_sstat][ddnoise_sdir]->data)[(int)ddnoise_spos]) ^ 0x8000) / 2);
                    // ddnoise_spos += ((float)al_get_sample_frequency(seeksmp[ddnoise_sstat][ddnoise_sdir]) / 44100.0);
                   
                    unsigned int *ptr = (unsigned int *)al_get_sample_data(seeksmp[ddnoise_sstat][ddnoise_sdir]);
                    ptr += (int)ddnoise_spos;
                    ddbuffer[c] += ((*ptr)^ 0x8000) / 2;
                    ddnoise_mpos += ((float)al_get_sample_frequency(seeksmp[ddnoise_sstat][ddnoise_sdir]) / 44100.0);
                    

				}
			}
			ddbuffer[c] = (ddbuffer[c] / 3) * ddvol;
		}
	}

//        mixtapenoise(ddbuffer);
//        fwrite(ddbuffer,4410*2,1,f2);
//rpclog("Give buffer... %i %i\n",ddnoise_mstat,ddnoise_sstat);
	givealbufferdd(ddbuffer);

	oldmotoron = motoron;
}
