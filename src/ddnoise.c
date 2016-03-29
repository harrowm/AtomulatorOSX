/*Atomulator v1.0 by Tom Walker
   Disc drive noise*/

#include <allegro5/allegro.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <stdio.h>
#include <string.h>
#include "atom.h"

extern ALLEGRO_AUDIO_STREAM *ddstream;

int ddvol = 3;
ALLEGRO_SAMPLE *seeksmp[4][2], *seek2smp, *seek3smp;
ALLEGRO_SAMPLE *stepsmp;
ALLEGRO_SAMPLE *motorsmp[3];

static float ddnoise_mpos = 0.0f;
int ddnoise_mstat = -1;
int oldmotoron = 0;

static float ddnoise_spos = 0.0f;
int ddnoise_sstat = -1;
int ddnoise_sdir = 0;
int ddtype = 0;

static ALLEGRO_SAMPLE* loadSampleFile(char* f)
{
    ALLEGRO_SAMPLE* s;
	extern ALLEGRO_PATH *exepath;
	char ddnoisefile[MAXPATH];

    // Get the directory for the sound files in an OSX/Linux/Windows friendly manner
    al_append_path_component(exepath, "ddnoise");
	if (ddtype)
       	al_append_path_component(exepath, "35");
	else
       	al_append_path_component(exepath, "525");
			
    al_set_path_filename(exepath, f);
    strlcpy(ddnoisefile, al_path_cstr(exepath, ALLEGRO_NATIVE_PATH_SEP), MAXPATH-1);
	al_drop_path_tail(exepath); // leave clean for next use - remove drive type    
	al_drop_path_tail(exepath); // leave clean for next use - remove ddnoise directory
    al_set_path_filename(exepath, NULL); // Leave clean

    s = al_load_sample(ddnoisefile);

    if (s == NULL)
        rpclog("ERROR: Failed to load sample %s\n", ddnoisefile);

    return s;
}

void loaddiscsamps()
{
    if (ddtype)
    {
        seeksmp[0][0] = loadSampleFile("stepo.wav");
        seeksmp[0][1] = loadSampleFile("stepi.wav");
        seeksmp[1][0] = loadSampleFile("seek1o.wav");
        seeksmp[1][1] = loadSampleFile("seek1i.wav");
        seeksmp[2][0] = loadSampleFile("seek2o.wav");
        seeksmp[2][1] = loadSampleFile("seek2i.wav");
        seeksmp[3][0] = loadSampleFile("seek3o.wav");
        seeksmp[3][1] = loadSampleFile("seek3i.wav");

        motorsmp[0] = loadSampleFile("motoron.wav");
        motorsmp[1] = loadSampleFile("motor.wav");
        motorsmp[2] = loadSampleFile("motoroff.wav");
    }
    else
    {
        seeksmp[0][0] = loadSampleFile("step.wav");
        seeksmp[0][1] = loadSampleFile("step.wav");
        seeksmp[1][0] = loadSampleFile("seek.wav");
        seeksmp[1][1] = loadSampleFile("seek.wav");
        seeksmp[2][0] = loadSampleFile("seek3.wav");
        seeksmp[2][1] = loadSampleFile("seek3.wav");
        seeksmp[3][0] = loadSampleFile("seek2.wav");
        seeksmp[3][1] = loadSampleFile("seek2.wav");

        motorsmp[0] = loadSampleFile("motoron525.wav");
        motorsmp[1] = loadSampleFile("motor525.wav");
        motorsmp[2] = loadSampleFile("motoroff525.wav");
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

int16_t ddbuffer[DDSNDBUFLEN];

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

void mixddnoise()
{
	int c;
    float mpos_increment;
    unsigned int msample_length;
    float spos_increment;
    unsigned int ssample_length;
    
    memset(ddbuffer, 0, DDSNDBUFLEN * 2);

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

	if (sndddnoise && (ddnoise_mstat >= 0))
	{
        mpos_increment = (float)(al_get_sample_frequency(motorsmp[ddnoise_mstat]) / 44100.0f);
        msample_length = (float)al_get_sample_length(motorsmp[ddnoise_mstat]);

		for (c = 0; c <= DDSNDBUFLEN; c++)
		{
			if (ddnoise_mstat >= 0)
			{
                if (ddnoise_mpos >= msample_length)
				{
					ddnoise_mpos = 0;
					if (ddnoise_mstat != 1)
                    {
						ddnoise_mstat++;
                        if (ddnoise_mstat == 3)
                            ddnoise_mstat = -1;
                        else
                        {
                            mpos_increment = (float)(al_get_sample_frequency(motorsmp[ddnoise_mstat]) / 44100.0f);
                            msample_length = (float)al_get_sample_length(motorsmp[ddnoise_mstat]);
                        }
                    }
                }
				if (ddnoise_mstat != -1)
				{
                    ddbuffer[c] += ((int16_t)((int16_t*)al_get_sample_data(motorsmp[ddnoise_mstat]))[(int)ddnoise_mpos]);
                    ddnoise_mpos += mpos_increment;
				}
			}
		}

        if (ddnoise_sstat >= 0)
        {
            spos_increment = (float)(al_get_sample_frequency(seeksmp[ddnoise_sstat][ddnoise_sdir]) / 44100.0f);
            ssample_length = (float)al_get_sample_length(seeksmp[ddnoise_sstat][ddnoise_sdir]);
            for (c = 0; c < DDSNDBUFLEN; c++)
            {
				if (ddnoise_spos >= ssample_length)
				{
					if (ddnoise_sstat > 0)
					{
						fdctime = 100;
					}
					ddnoise_spos = 0;
					ddnoise_sstat = -1;
				}
				if (ddnoise_sstat != -1)
				{
                    ddbuffer[c] += ((int16_t)((int16_t*)al_get_sample_data(seeksmp[ddnoise_sstat][ddnoise_sdir]))[(int)ddnoise_spos]);
                    ddnoise_spos += spos_increment;
				}
                
                ddbuffer[c] = (ddbuffer[c] / 3) * ddvol;
            }
        }
	}

	giveSoundBuffer(ddstream, ddbuffer, DDSNDBUFLEN * 2);
	oldmotoron = motoron;
}
