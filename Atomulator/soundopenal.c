/*Atomulator v1.0 by Tom Walker
   OpenAL interface*/

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <OpenAL/al.h>
//#include <OpenAL/alut.h> file not available under OSX, potential HACK
#include "atom.h"

#include "allegro_audio.h"

int samples = 0;
FILE *allog;

ALuint buffers[4];      // front and back buffers
ALuint source[2];       // audio source
ALuint buffersdd[4];    // front and back buffers
ALenum format;          // internal format

#define FREQ 31200
#define SNDBUFLEN (312*2*2)
#define BUFLEN (SNDBUFLEN * 4)

void closeal();

void check()
{
	ALenum error;

	if ((error = alGetError()) != AL_NO_ERROR)
	{
		rpclog("AL Error : %08X\n", error);
		//rpclog("Description : %s\n",alGetErrorString(error));
	}
/*        if ((error = alutGetError()) != ALUT_ERROR_NO_ERROR)
        {
                printf("ALut Error : %08X\n", error);
                printf("Description : %s\n",alutGetErrorString(error));
        }*/
}

void initalmain(int argc, char *argv[])
{
	alutInit(0, NULL); //&argc, argv);
	check();
	atexit(closeal);
//        printf("AlutInit\n");
}

void closeal()
{
	alutExit();
}

int16_t tempbuf[BUFLEN >> 1];
int16_t tempbufdd[4410 * 2];

// HACK

    // flag used to determine whether to run Allegro sound or OpenAL
    static bool useAllegroSound = false;

    ALLEGRO_SAMPLE *sam = NULL;
    void *buff = NULL;

// End of HACK


void inital()
{
	int c;
    
    if (useAllegroSound)
    {
        if (!al_reserve_samples(16))
            rpclog("Could not set up Allegro voice and mixer\n");
        
        

        
        unsigned int samples = SNDBUFLEN;
        unsigned long sample_size = al_get_channel_count(ALLEGRO_CHANNEL_CONF_1) * al_get_audio_depth_size(ALLEGRO_AUDIO_DEPTH_INT16);
        unsigned long bytes = samples * sample_size;
        
        printf("samples %d sample_size %d bytes %d\n", samples, sample_size, bytes);
        

        buff = al_malloc(bytes);
        
        sam = al_create_sample(buff, samples, FREQ, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1, false);
        if (sam == NULL)
        {
            rpclog("Cannot create Allegro sample\n");
            exit(1); // HACK
        }
        return;
    }
    
	format = AL_FORMAT_STEREO16;
	check();

	alGenBuffers(4, buffers);
	check();

	alGenSources(2, source);
	check();

	alSource3f(source[0], AL_POSITION,        0.0, 0.0, 0.0);
	alSource3f(source[0], AL_VELOCITY,        0.0, 0.0, 0.0);
	alSource3f(source[0], AL_DIRECTION,       0.0, 0.0, 0.0);
	alSourcef(source[0], AL_ROLLOFF_FACTOR,  0.0          );
	alSourcei(source[0], AL_SOURCE_RELATIVE, AL_TRUE      );
	check();

	memset(tempbuf, 0, BUFLEN);

	for (c = 0; c < 4; c++)
		alBufferData(buffers[c], AL_FORMAT_STEREO16, tempbuf, BUFLEN, 31200);
	alSourceQueueBuffers(source[0], 4, buffers);
	check();
	alSourcePlay(source[0]);
	check();
//        printf("InitAL\n");

	alGenBuffers(4, buffersdd);
	check();

	alSource3f(source[1], AL_POSITION,        0.0, 0.0, 0.0);
	alSource3f(source[1], AL_VELOCITY,        0.0, 0.0, 0.0);
	alSource3f(source[1], AL_DIRECTION,       0.0, 0.0, 0.0);
	alSourcef(source[1], AL_ROLLOFF_FACTOR,  0.0          );
	alSourcei(source[1], AL_SOURCE_RELATIVE, AL_TRUE      );
	check();

	memset(tempbufdd, 0, 4410 * 4);

	for (c = 0; c < 4; c++)
		alBufferData(buffersdd[c], AL_FORMAT_STEREO16, tempbufdd, 4410 * 4, 44100);
	alSourceQueueBuffers(source[1], 4, buffersdd);
	check();
	alSourcePlay(source[1]);
	check();
//        printf("InitAL\n");
}

int16_t zbuf[16384];

void givealbuffer(int16_t *buf)
{
	int processed;
	int state;
	int c;

    if (useAllegroSound)
    {
        int16_t* ptr;
        
        ptr = (int16_t *) al_get_sample_data(sam);
        memcpy(ptr, buf, SNDBUFLEN*2);
        
     
        //    if (!allog) allog=fopen("/Users/malcolm/Desktop/al.pcm","wb");
        //    fwrite(ptr,SNDBUFLEN*2,1,allog);

        
        if (!allog) allog=fopen("/Users/malcolm/Desktop/goodbeep2.pcm","rb");
            fread(ptr,1,SNDBUFLEN*2,allog);
        //   al_rest(1);
        
        
        al_play_sample(sam, 1.0, 0, 1.0, ALLEGRO_PLAYMODE_ONCE, NULL);
        return;
    }

//        if (!sndinternal && !sndbeebsid) return;
//return;
	samples += 2000;

	alGetSourcei(source[0], AL_SOURCE_STATE, &state);

	if (state == 0x1014)
	{
		alSourcePlay(source[0]);
//                rpclog("Resetting sound\n");
	}
//        printf("State - %i %08X\n",state,state);
	alGetSourcei(source[0], AL_BUFFERS_PROCESSED, &processed);

//        printf("P ");
	check();
//        rpclog("Processed - %i\n",processed);

	if (processed >= 1)
	{
		ALuint buffer;
		//ALint temp;

		alSourceUnqueueBuffers(source[0], 1, &buffer);
//                printf("U ");
		check();

		for (int c = 0; c < (BUFLEN >> 1); c++)
        {
			zbuf[c] = buf[c >> 1];                //^0x8000;
            //if (zbuf[c] != 4095)
                //printf("%04x %d\n", zbuf[c], zbuf[c]);
            //if (!(c % 8)) printf("\n");
        }
		alBufferData(buffer, AL_FORMAT_STEREO16, zbuf, BUFLEN, FREQ);
//                printf("Passing %i bytes\n",BUFLEN);
//                printf("B ");
		check();

		alSourceQueueBuffers(source[0], 1, &buffer);
//                printf("Q ");
		check();

//                alGetBufferi(buffer,AL_FREQUENCY,&temp);
//                printf("Freq - %i\n",temp);

//                printf("\n");

                if (!allog) allog=fopen("/Users/malcolm/Desktop/al.pcm","wb");
                fwrite(zbuf,BUFLEN,1,allog);
	}
}

void givealbufferdd(int16_t *buf)
{
	int processed;
	int state;
	int c;

//        return;
//        rpclog("DDnoise1\n");

	if (!sndddnoise)
		return;
//        rpclog("DDnoise2\n");

//return;
	alGetSourcei(source[1], AL_SOURCE_STATE, &state);

	if (state == 0x1014)
	{
		alSourcePlay(source[1]);
//                printf("Resetting sounddd\n");
	}
	alGetSourcei(source[1], AL_BUFFERS_PROCESSED, &processed);
//rpclog("Get source\n");
	check();
//rpclog("Got source\n");
	if (processed >= 1)
	{
		ALuint buffer;
		//ALint temp;

//rpclog("Unqueue\n");
		alSourceUnqueueBuffers(source[1], 1, &buffer);
		check();

		for (c = 0; c < (4410 * 2); c++)
			zbuf[c] = buf[c >> 1];             //^0x8000;

//rpclog("BufferData\n");
		alBufferData(buffer, AL_FORMAT_STEREO16, zbuf, 4410 * 4, 44100);
		check();

//rpclog("Queue\n");
		alSourceQueueBuffers(source[1], 1, &buffer);
		check();
	}

//        rpclog("DDnoise3\n");
}

void mixtapenoise(int16_t *buf)
{
}
