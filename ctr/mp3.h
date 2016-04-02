#pragma once
#include <3ds.h>
#include <stdio.h>
#ifdef WIN32
#include "Mixer_win32.h"
#endif
#include "mp3dec.h"

#define MP3_OUTBUF_SIZE	(MAX_NCHAN * MAX_NGRAN * MAX_NSAMP)
#define MP3_FILE_BUFFER_SIZE (2 * 1024)

#define MP3_AUDIO_BUFFER_SAMPS (1 * 1024)
#define MP3_AUDIO_BUFFER_SIZE (MP3_AUDIO_BUFFER_SAMPS*2)

typedef enum {
	MP3_IDLE = 0,
	MP3_STARTING = 1,
	MP3_PLAYING = 2,
	MP3_PAUSING = 3,
	MP3_RESUMING = 4,
	MP3_PAUSED = 5,
	MP3_STOPPING = 6,
	MP3_ERROR = 0xffffffff
} mp3_state;

class mp3 {
public:
	mp3();

	void update();

	void play(int track, bool loop);
	void play(char *name, bool loop);
	void stop();
	void pause();
	void resume();

	void init();

private:

	mp3_state m_state;

	void fill_buffer();

	u64 sample_pos();
	void frame();
	void frames(u64 endtime);

	void starting();
	void playing();
	void pausing();
	void resuming();
	void stopping();


	u64			m_sound_time;
	u64			m_paint_time;
	int			m_samples;
	int			m_speed;
	int			m_channels;

	bool		m_loop;
	int			m_length;
	int			m_bytesLeft;
	u8			*m_readPtr;
	u8			m_file_buffer[MP3_FILE_BUFFER_SIZE * 2];

	short		m_outbuf[MP3_OUTBUF_SIZE/2];

	MP3FrameInfo	m_mp3FrameInfo;
	HMP3Decoder		m_hMP3Decoder;
	FILE			*m_file;
	int				m_file_start_ofs;
	u8				*m_playbackBuffer;
	bool			m_mixing;
	//MixerHardware	*m_hw;
	//sysFile			*m_file;
};

inline mp3::mp3() {
	m_sound_time = 0;
	m_paint_time = 0;
	m_samples = 0;
	m_hMP3Decoder = 0;
	m_state = MP3_IDLE;
	m_bytesLeft = 0;
	m_readPtr = 0;
	//m_hw = 0;
}

