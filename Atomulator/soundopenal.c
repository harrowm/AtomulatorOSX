/*Atomulator v1.0 by Tom Walker
   port to Allegro 5 sound by Malcolm Harrow */

#include "atom.h"

#include "Allegro5/allegro_audio.h"

#define FREQ 31200
#define SNDBUFLEN (312*2*2)

extern ALLEGRO_AUDIO_STREAM *stream;
extern ALLEGRO_AUDIO_STREAM *ddstream;

void inital()
{
    if (!al_reserve_samples(16)) // 11 used for disk drive smaples plus a few spare!
        rpclog("Could not set up Allegro voice and mixer\n");
    
    // How many buffer fragments to allow - eg from manual used 8, 2 appears to be too few from testing .. 4 seems to work ok and minimizes any delay
    stream = al_create_audio_stream(4, SNDBUFLEN, FREQ, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1);
    if (!stream)
    {
        rpclog("Could not create audio stream.\n");
        exit(1); // HACK
    }

    // Create an audio stream for the disk drive noises
    // Samples given with code are 44.1k any single channel
    ddstream = al_create_audio_stream(4, SNDBUFLEN, 44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1);
    if (!ddstream)
    {
        rpclog("Could not create disk drive audio stream.\n");
        exit(1); // HACK
    }
    
    if (!al_attach_audio_stream_to_mixer(stream, al_get_default_mixer()))
    {
        rpclog("Could not attach stream to mixer.\n");
        exit(1); // HACK
    }
    
    // attach disk drive stream - but ensure not playing first
    al_set_audio_stream_playing(ddstream, false);
    if (!al_attach_audio_stream_to_mixer(ddstream, al_get_default_mixer()))
    {
        rpclog("Could not attach disk drive audio stream to mixer.\n");
        exit(1); // HACK
    }

}

// replaces old givealsoundbuffer() routine
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

