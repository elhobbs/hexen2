#include "quakedef.h"
#include <3ds.h>
#include <3ds/allocator/linear.h>
#include <3ds/ndsp/ndsp.h>

// 64K is > 1 second at 16-bit, 22050 Hz
#define	WAV_BUFFERS				128
#define	WAV_MASK				0x7F
#define	WAV_BUFFER_SIZE			0x0400
#define SECONDARY_BUFFER_SIZE	0x10000

typedef enum { SIS_SUCCESS, SIS_FAILURE, SIS_NOTAVAIL,SIS_MAX= 0xFFFFFFFF} sndinitstat;

static qboolean	wav_init;
static qboolean	snd_firsttime = true, snd_iswave;
static qboolean	primary_format_set;

// starts at 0 for disabled
static int	snd_buffer_count = 0;
static int	sample16;
static int	snd_sent, snd_completed;

static int	gSndBufSize = 0;

static ndspWaveBuf gWavebuf[WAV_BUFFERS];
static float gMix[12];


qboolean SNDDMA_InitDirect(void);
qboolean SNDDMA_InitWav(void);


/*
==================
S_BlockSound
==================
*/
void S_BlockSound(void)
{

	// DirectSound takes care of blocking itself
	if (snd_iswave)
	{
		snd_blocked++;

		//if (snd_blocked == 1)
		//	waveOutReset(hWaveOut);
	}
}


/*
==================
S_UnblockSound
==================
*/
void S_UnblockSound(void)
{

	// DirectSound takes care of blocking itself
	if (snd_iswave)
	{
		snd_blocked--;
	}
}


/*
==================
FreeSound
==================
*/
void FreeSound(void)
{
	if (wav_init == false) {
		return;
	}
	ndspChnWaveBufClear(0);
	svcSleepThread(20000);
	ndspExit();

	wav_init = false;
}


/*
==================
Snd_ReleaseBuffer
==================
*/
void Snd_ReleaseBuffer(void)
{
}


/*
==================
Snd_AcquireBuffer
==================
*/
void Snd_AcquireBuffer(void)
{
}

/*
==================
SNDDM_InitWav

Crappy windows multimedia base
==================
*/
qboolean SNDDMA_InitWav(void)
{

	int i;

	shm = &sn;

	shm->channels = 2;
	shm->samplebits = 16;
	shm->speed = 11025;

	snd_sent = 0;
	snd_completed = 0;

	/*
	* Allocate and lock memory for the waveform data. The memory
	* for waveform data must be globally allocated with
	* GMEM_MOVEABLE and GMEM_SHARE flags.

	*/
	gSndBufSize = WAV_BUFFERS*WAV_BUFFER_SIZE;
	void *lpData = linearAlloc(gSndBufSize);
	if (!lpData)
	{
		Con_SafePrintf("Sound: Out of memory.\n");
		FreeSound();
		return false;
	}
	memset(lpData, 0, gSndBufSize);

	ndspInit();
	ndspChnSetInterp(0, NDSP_INTERP_NONE);
	ndspChnSetRate(0, 11025.0f);
	ndspChnSetFormat(0, NDSP_FORMAT_STEREO_PCM16);
	memset(gWavebuf, 0, sizeof(ndspWaveBuf)*WAV_BUFFERS);
	memset(gMix, 0, sizeof(gMix));

	DSP_FlushDataCache(lpData, gSndBufSize);

	/* After allocation, set up and prepare headers. */
	for (i = 0; i<WAV_BUFFERS; i++)
	{
		gWavebuf[i].nsamples = WAV_BUFFER_SIZE / (shm->channels * shm->samplebits / 8);
		gWavebuf[i].looping = false;
		gWavebuf[i].status = NDSP_WBUF_FREE;
		gWavebuf[i].data_vaddr = lpData + i*WAV_BUFFER_SIZE;
		//ch->mix[0] = master_volume * (float)ch->left / 128.0f;
		//ch->mix[1] = master_volume * (float)ch->right / 128.0f;
		//ndspChnSetMix(cnum, ch->mix);
		//ndspChnWaveBufAdd(cnum, &ch->wavebuf);
	}

	shm->soundalive = true;
	shm->splitbuffer = false;
	shm->samples = gSndBufSize / (shm->samplebits / 8);
	shm->samplepos = 0;
	shm->submission_chunk = 1;
	shm->buffer = (unsigned char *)lpData;
	sample16 = (shm->samplebits / 8) - 1;


	wav_init = true;

	return true;
}

/*
==================
SNDDMA_Init

Try to find a sound device to mix for.
Returns false if nothing is found.
==================
*/

qboolean SNDDMA_Init(void)
{
	sndinitstat	stat;

	wav_init = 0;

	stat = SIS_FAILURE;	// assume DirectSound won't initialize


	// if DirectSound didn't succeed in initializing, try to initialize
	// waveOut sound, unless DirectSound failed because the hardware is
	// already allocated (in which case the user has already chosen not
	// to have sound)
	if (snd_firsttime || snd_iswave)
	{

		snd_iswave = SNDDMA_InitWav();

		if (snd_iswave)
		{
			if (snd_firsttime)
				Con_SafePrintf("Wave sound initialized\n");
		}
		else
		{
			Con_SafePrintf("Wave sound failed to init\n");
		}
	}

	snd_firsttime = false;

	snd_buffer_count = 1;

	if (!wav_init)
	{
		if (snd_firsttime)
			Con_SafePrintf("No sound device initialized\n");

		return false;
	}

	return true;
}

/*
==============
SNDDMA_GetDMAPos

return the current sample position (in mono samples read)
inside the recirculating dma buffer, so the mixing code will know
how many sample are required to fill it up.
===============
*/
int SNDDMA_GetDMAPos(void)
{
	int		s;

	if (wav_init)
	{
		s = snd_sent * WAV_BUFFER_SIZE;
	}

	s >>= sample16;

	s &= (shm->samples - 1);

	return s;
}

/*
==============
SNDDMA_Submit

Send sound to device if buffer isn't really the dma buffer
===============
*/
void SNDDMA_Submit(void)
{
	if (!wav_init)
		return;

	//
	// find which sound blocks have completed
	//
	while (1)
	{
		if (snd_completed == snd_sent)
		{
			Con_DPrintf("Sound overrun\n");
			break;
		}

		//if (!(lpWaveHdr[snd_completed & WAV_MASK].dwFlags & WHDR_DONE))
		//{
			//break;
		//}

		if (gWavebuf[snd_completed & WAV_MASK].status != NDSP_WBUF_DONE) {
			break;
		}

		snd_completed++;	// this buffer has been played
	}

	//
	// submit two new sound blocks
	//
	while (((snd_sent - snd_completed) >> sample16) < 4)
	{
		//h = lpWaveHdr + (snd_sent&WAV_MASK);
		ndspChnWaveBufAdd(0, gWavebuf + (snd_sent&WAV_MASK));

		snd_sent++;
		/*
		* Now the data block can be sent to the output device. The
		* waveOutWrite function returns immediately and waveform
		* data is sent to the output device in the background.
		*/

	}
}

/*
==============
SNDDMA_Shutdown

Reset the sound device for exiting
===============
*/
void SNDDMA_Shutdown(void)
{
	FreeSound();
}

