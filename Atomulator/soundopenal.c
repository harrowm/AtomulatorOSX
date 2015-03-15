/*Atomulator v1.0 by Tom Walker
   port to Allegro 5 sound by Malcolm Harrow */

#include "atom.h"

#include "allegro_audio.h"

#define FREQ 31200
#define SNDBUFLEN (312*2*2)

extern ALLEGRO_AUDIO_STREAM *stream;

void inital()
{
    if (!al_reserve_samples(0))
        rpclog("Could not set up Allegro voice and mixer\n");
    
    // How many biffer fragments to allow - eg from manual used 8 so I will too 
    stream = al_create_audio_stream(8, SNDBUFLEN, FREQ, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1);
    if (!stream)
    {
        rpclog("Could not create stream.\n");
        exit(1); // HACK
    }
    
    if (!al_attach_audio_stream_to_mixer(stream, al_get_default_mixer()))
    {
        rpclog("Could not attach stream to mixer.\n");
        exit(1); // HACK
    }
}
    
void giveSoundBuffer(int16_t *soundBuf)
{
    void* alBuf = al_get_audio_stream_fragment(stream);

    if (alBuf)
    {
        memcpy(alBuf, soundBuf, SNDBUFLEN*2);
        
        if (!al_set_audio_stream_fragment(stream, alBuf))
            rpclog("Error passing sound buffer to Allegro sound routines.\n");
    }
}

void givealbufferdd(int16_t *buf)
{
//	int processed;
//	int state;
//	int c;
//
////        return;
////        rpclog("DDnoise1\n");
//
//	if (!sndddnoise)
//		return;
////        rpclog("DDnoise2\n");
//
////return;
//	alGetSourcei(source[1], AL_SOURCE_STATE, &state);
//
//	if (state == 0x1014)
//	{
//		alSourcePlay(source[1]);
////                printf("Resetting sounddd\n");
//	}
//	alGetSourcei(source[1], AL_BUFFERS_PROCESSED, &processed);
////rpclog("Get source\n");
//	check();
////rpclog("Got source\n");
//	if (processed >= 1)
//	{
//		ALuint buffer;
//		//ALint temp;
//
////rpclog("Unqueue\n");
//		alSourceUnqueueBuffers(source[1], 1, &buffer);
//		check();
//
//		for (c = 0; c < (4410 * 2); c++)
//			zbuf[c] = buf[c >> 1];             //^0x8000;
//
////rpclog("BufferData\n");
//		alBufferData(buffer, AL_FORMAT_STEREO16, zbuf, 4410 * 4, 44100);
//		check();
//
////rpclog("Queue\n");
//		alSourceQueueBuffers(source[1], 1, &buffer);
//		check();
//	}
//
////        rpclog("DDnoise3\n");
}

void mixtapenoise(int16_t *buf)
{
}

