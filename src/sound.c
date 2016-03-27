/*Atomulator v1.0 by Tom Walker
   port to Allegro 5 sound by Malcolm Harrow */

#include "atom.h"
#include <allegro5/allegro_audio.h>

#define FREQ 31200

extern ALLEGRO_AUDIO_STREAM *stream;
extern ALLEGRO_AUDIO_STREAM *ddstream;

bool initSound()
{
	if (!al_reserve_samples(16)) // 11 used for disk drive samples plus a few spare!
	{
		rpclog("ERROR: Could not set up Allegro voice and mixer\n");
		return(false);
	}
    
    // How many buffer fragments to allow - eg from manual used 8, this seems to work ok when
    // collecting 312*2 samples (every 1/25 second or two PAL frames
    stream = al_create_audio_stream(8, SNDBUFLEN, FREQ, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1);
    if (!stream)
    {
        rpclog("ERROR: Could not create audio stream.\n");
        return(false);
    }

	// Change the default volume - I find it too quiet
	al_set_audio_stream_gain(stream, 2.0);

    // Create an audio stream for the disk drive noises
    // Samples given with code are 44.1k any single channel
    ddstream = al_create_audio_stream(8, DDSNDBUFLEN, 44100, ALLEGRO_AUDIO_DEPTH_INT16, ALLEGRO_CHANNEL_CONF_1);
    if (!ddstream)
    {
        rpclog("ERROR: Could not create disk drive audio stream.\n");
        return(false);
    }
    
    if (!al_attach_audio_stream_to_mixer(stream, al_get_default_mixer()))
    {
        rpclog("ERROR: Could not attach stream to mixer.\n");
        return(false);
    }

	if (!al_attach_audio_stream_to_mixer(ddstream, al_get_default_mixer()))
    {
        rpclog("ERROR: Could not attach disk drive audio stream to mixer.\n");
        return(false);
    }

	// Change the default volume - I find it too quiet
	al_set_audio_stream_gain(ddstream, 2.0);
    
	return(true);
}

// Pass sound data to Allegro sound routines
void giveSoundBuffer(ALLEGRO_AUDIO_STREAM *stream, int16_t *soundBuf, int16_t lenBufInBytes)
{
    void* alBuf = al_get_audio_stream_fragment(stream);

    if (alBuf)
    {
        memcpy(alBuf, soundBuf, lenBufInBytes);
        
        if (!al_set_audio_stream_fragment(stream, alBuf))
            rpclog("Error passing sound buffer to Allegro sound routines.\n");
    }
}
