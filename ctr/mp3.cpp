#include "mp3.h"
#include <stdio.h>
#include <string.h>

#include <3ds/linear.h>
#include <3ds/ndsp/ndsp.h>

extern "C" int COM_FOpenFile2(char *filename, FILE **file);

// 64K is > 1 second at 16-bit, 22050 Hz
#define	WAV_BUFFERS				128
#define	WAV_MASK				0x7F
#define	WAV_BUFFER_SIZE			0x0400

//static int	sample16;
static int	snd_sent, snd_completed;

static int	gSndBufSize = 0;

static ndspWaveBuf gWavebuf[WAV_BUFFERS];
static float gMix[12];

void mp3::init() {
	int i;

	snd_sent = 0;
	snd_completed = 0;
	m_mixing = false;

	/*
	* Allocate and lock memory for the waveform data. The memory
	* for waveform data must be globally allocated with
	* GMEM_MOVEABLE and GMEM_SHARE flags.

	*/
	gSndBufSize = WAV_BUFFERS*WAV_BUFFER_SIZE;
	m_playbackBuffer = (u8 *)linearAlloc(gSndBufSize);
	if (!m_playbackBuffer)
	{
		printf("Sound: Out of memory.\n");
		return;
	}
	memset(m_playbackBuffer, 0, gSndBufSize);
}

/*
==============
SNDDMA_GetDMAPos

return the current sample position (in mono samples read)
inside the recirculating dma buffer, so the mixing code will know
how many sample are required to fill it up.
===============
*/
int mp3_GetDMAPos( )
{
	int		s;

	s = (snd_sent * WAV_BUFFER_SIZE) >> 1;

	s &= gSndBufSize/2 - 1;

	return s;
}

static	int		mp3_buffers;
static	u64		mp3_oldsamplepos;

void mp3_reset_timer() {
	mp3_buffers = 0;
	mp3_oldsamplepos = 0;
	snd_sent = 0;
	snd_completed = 0;
}

u64 mp3_GetSoundtime(void)
{
	u64		samplepos;
	u64		fullsamples;

	fullsamples = gSndBufSize / 2;

	// it is possible to miscount buffers if it has wrapped twice between
	// calls to S_Update.  Oh well.
	samplepos = mp3_GetDMAPos();

	if (samplepos < mp3_oldsamplepos)
	{
		mp3_buffers++;					// buffer wrapped
	}
	mp3_oldsamplepos = samplepos;

	return mp3_buffers*fullsamples + samplepos;
}


inline u64 mp3::sample_pos() {
	return mp3_GetSoundtime()/m_channels;
}


/*
==============
SNDDMA_Submit

Send sound to device if buffer isn't really the dma buffer
===============
*/
void mp3_submit(void)
{
	//
	// find which sound blocks have completed
	//
	while (1)
	{
		//printf("snd_completed: %d snd_sent: %d\n", snd_completed, snd_sent);
		if (snd_completed == snd_sent)
		{
			//printf("Sound overrun\n");
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
	while (((snd_sent - snd_completed) >> 1) < 4)
	{
		//h = lpWaveHdr + (snd_sent&WAV_MASK);
		//printf("submit: %d\n", snd_sent&WAV_MASK);
		ndspChnWaveBufAdd(1, gWavebuf + (snd_sent&WAV_MASK));

		snd_sent++;
		/*
		* Now the data block can be sent to the output device. The
		* waveOutWrite function returns immediately and waveform
		* data is sent to the output device in the background.
		*/

	}
}

static void paint(short *pAudioData, int count, short *outp, int start, int mask, int num_channels)
{
	int volume = 1;
	int size = count * 2;

	if (count <= 0) {
		return;
	}

	int pos = (start*num_channels) & mask;
	short *p = pAudioData;
	int val;
	if (num_channels > 1) {
		while (count--) {
			val = (*p * volume) >> 0;
			p += 1;//step;
			if (val > 0x7fff)
				val = 0x7fff;
			else if (val < (short)0x8000)
				val = (short)0x8000;
			outp[pos++] = val;// (val >> 8);

			val = (*p * volume) >> 0;
			p += 1;//step;
			if (val > 0x7fff)
				val = 0x7fff;
			else if (val < (short)0x8000)
				val = (short)0x8000;
			outp[pos++] = val;// (val >> 8);

			pos &= mask;
		}
	}
	else {
		while (count--) {
			val = (*p * volume) >> 0;
			p += 1;//step;
			if (val > 0x7fff)
				val = 0x7fff;
			else if (val < (short)0x8000)
				val = (short)0x8000;
			outp[pos] = val;// (val >> 8) + 128;
			pos = (pos + 1) & mask;
		}
	}
	//this is wrong
	DSP_FlushDataCache(outp + start, size);
	//printf("upd short: %08x %6d %6d\n", pAudioData, count, m_bufferPos);
}


void mp3::frame() {
	int offset, err;

	// if mp3 is set to loop indefinitely, don't bother with how many data is left
	if (m_loop && m_bytesLeft < 2 * MAINBUF_SIZE)
		m_bytesLeft += MP3_FILE_BUFFER_SIZE;


	/* find start of next MP3 frame - assume EOF if no sync found */
	offset = MP3FindSyncWord((u8 *)m_readPtr, m_bytesLeft);
	if (offset < 0) {
		//printf("mp3: finished\n");
		m_state = MP3_IDLE;
		return;
	}
	m_readPtr += offset;
	m_bytesLeft -= offset;


	err = MP3Decode(m_hMP3Decoder, &m_readPtr, &m_bytesLeft, m_outbuf, 0);
	//printf("read: %08x %d %d %d\n", m_readPtr, m_bytesLeft, offset, err);
	if (err) {
		/* error occurred */
		switch (err) {
		case ERR_MP3_INDATA_UNDERFLOW:
			//outOfData = 1;
			break;
		case ERR_MP3_MAINDATA_UNDERFLOW:
			/* do nothing - next call to decode will provide more mainData */
			return;
		case ERR_MP3_FREE_BITRATE_SYNC:
		default:
			printf("mp3: error %d stopping\n", err);
			m_state = MP3_ERROR;
			break;
		}
		return;
	}
	/* no error */
	MP3GetLastFrameInfo(m_hMP3Decoder, &m_mp3FrameInfo);

	//start the mixer if needed
	if (m_mixing == false) {
		m_mixing = true;
		//printf("no m_hw\n");
		m_speed = m_mp3FrameInfo.samprate;
		m_channels = m_mp3FrameInfo.nChans;

		ndspChnSetInterp(1, NDSP_INTERP_NONE);
		ndspChnSetRate(1, m_speed);
		ndspChnSetFormat(1, m_channels == 2 ? NDSP_FORMAT_STEREO_PCM16 : NDSP_FORMAT_MONO_PCM16);
		memset(gWavebuf, 0, sizeof(ndspWaveBuf)*WAV_BUFFERS);
		memset(gMix, 0, sizeof(gMix));

		memset(m_playbackBuffer, 0, gSndBufSize);
		DSP_FlushDataCache(m_playbackBuffer, gSndBufSize);

		/* After allocation, set up and prepare headers. */
		for (int i = 0; i<WAV_BUFFERS; i++)
		{
			gWavebuf[i].nsamples = WAV_BUFFER_SIZE / (m_channels * 16 / 8);
			gWavebuf[i].looping = false;
			gWavebuf[i].status = NDSP_WBUF_FREE;
			gWavebuf[i].data_vaddr = m_playbackBuffer + i*WAV_BUFFER_SIZE;
		}
	}

	paint(m_outbuf, m_mp3FrameInfo.outputSamps >> 1, (short *)m_playbackBuffer, m_paint_time, (WAV_BUFFERS*WAV_BUFFER_SIZE / 2)-1, m_channels);
	m_paint_time += (m_mp3FrameInfo.outputSamps>>(m_channels-1));
	mp3_submit();
}

void mp3::fill_buffer() {
	int n = fread(m_file_buffer + MP3_FILE_BUFFER_SIZE, 1, MP3_FILE_BUFFER_SIZE, m_file);
	//printf("fill: %d %d\n", n, snd_sent);
	if (m_loop && n < MP3_FILE_BUFFER_SIZE) {
		fseek(m_file, m_file_start_ofs, SEEK_SET);
		n = fread(m_file_buffer + MP3_FILE_BUFFER_SIZE+n, 1, MP3_FILE_BUFFER_SIZE-n, m_file);
		//printf("fill2: %d %d\n", n, m_file_start_ofs);
	}
}

void mp3::frames(u64 endtime)
{

	while (m_paint_time < endtime && m_state != MP3_ERROR)
	{
		//printf("%lld %lld\n", m_paint_time, endtime);

		frame();

		// check if we moved onto the 2nd file data buffer, if so move it to the 1st one and request a refill
		if (m_readPtr > (m_file_buffer + MP3_FILE_BUFFER_SIZE)) {
			m_readPtr = m_readPtr - MP3_FILE_BUFFER_SIZE;
			memcpy((void *)m_readPtr, (void *)(m_readPtr + MP3_FILE_BUFFER_SIZE), MP3_FILE_BUFFER_SIZE - (m_readPtr - m_file_buffer));
			fill_buffer();
		}
	}
	mp3_submit();
}

void mp3::playing() {

	m_sound_time = sample_pos();

	// check to make sure that we haven't overshot
	if (m_paint_time < m_sound_time)
	{
		m_paint_time = m_sound_time;
	}

	// mix ahead of current position
	u64 endtime = m_sound_time + (m_mp3FrameInfo.samprate / 10);

	frames(endtime);
}

void mp3::resume() {

	//if (m_state == MP3_PAUSED) {
		m_sound_time = m_paint_time = sample_pos();
		//if (m_hw) {
		//	m_hw->flush();
		//}

		frames(MP3_AUDIO_BUFFER_SAMPS / 2);

		m_state = MP3_PLAYING;
	//}
}

void mp3::starting() {
	if (m_hMP3Decoder == 0) {
		printf("mp3 starting\n");
		if ((m_hMP3Decoder = MP3InitDecoder()) == 0) {
			m_state = MP3_IDLE;
			return;
		}
	}

	resume();
}

void mp3::resuming() {
		resume();
}

void mp3::pause() {
	if (m_state == MP3_PLAYING) {
		pause();
	}
}

void mp3::pausing() {
	pause();
}

void mp3::stopping() {
	printf("mp3 stopping\n");
	//m_hw->flush();
	MP3FreeDecoder(m_hMP3Decoder);
	m_hMP3Decoder = 0;
	m_state = MP3_IDLE;
}

void mp3::update() {
	switch (m_state) {
	case MP3_STARTING:
		starting();
		break;
	case MP3_PLAYING:
		playing();
		break;
	case MP3_PAUSING:
		pausing();
		break;
	case MP3_RESUMING:
		resuming();
		break;
	case MP3_STOPPING:
		stopping();
		break;
	case MP3_IDLE:
	case MP3_PAUSED:
	case MP3_ERROR:
		break;
	}
}

void mp3::play(char *name, bool loop) {
	stop();
	m_file = 0;
	int len = COM_FOpenFile2(name, &m_file);
	if (len == -1) {
		printf("unable to open: %s\n", name);
		m_state = MP3_IDLE;
		return;
	}

	m_length = m_bytesLeft = len;
	m_loop = loop;
	m_file_start_ofs = ftell(m_file);

	fread(m_file_buffer, MP3_FILE_BUFFER_SIZE * 2, 1, m_file);
	m_readPtr = m_file_buffer;
	m_state = MP3_STARTING;
	printf("playing %s\n", name);
}

void mp3::play(int track, bool loop) {
	char name[128];

	sprintf(name, "music/%02d.mp3", track);
	play(name, loop);
}

void mp3::stop() {
	if (m_file) {
		fclose(m_file);
		m_file = 0;
	}
	memset(m_playbackBuffer, 0, gSndBufSize);

	m_state = MP3_IDLE;

	ndspChnWaveBufClear(1);
	svcSleepThread(20000);
	mp3_reset_timer();
	m_mixing = false;

}


mp3 g_mp3;

extern "C" {
	void mp3_init() {
		g_mp3.init();
	}
	void mp3_play(int track) {
		g_mp3.play("02.mp3", true);
	}
	void mp3_stop() {
		g_mp3.stop();
	}
	void mp3_pause() {
		//g_mp3.pause();
	}
	void mp3_resume() {
		//g_mp3.resume();
	}
	void mp3_update() {
		g_mp3.update();
	}
	void mp3_shutdown() {
		g_mp3.stop();
	}
};