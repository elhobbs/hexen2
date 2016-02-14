// sys_ctr.c -- ctr system interface code
#ifdef _3DS

#include <3ds.h>
#include "quakedef.h"
#include "errno.h"
#include "keyboard.h"
#include <unistd.h>
#include <sys/stat.h>

#define CRC_A 59461 // "Who's Ridin' With Chaos?"
#define CRC_B 54866 // "Santa needs a new sled!"

#ifdef GLQUAKE
#define MINIMUM_WIN_MEMORY		0x1000000
#define MAXIMUM_WIN_MEMORY		0x1800000
#else
#define MINIMUM_WIN_MEMORY		0x0C00000
#define MAXIMUM_WIN_MEMORY		0x1600000
#endif

#define CONSOLE_ERROR_TIMEOUT	60.0	// # of seconds to wait on Sys_Error running
//  dedicated before exiting
#define PAUSE_SLEEP		50				// sleep time on pause or minimization
#define NOT_FOCUS_SLEEP	20				// sleep time when not focus

int			starttime;
qboolean	ActiveApp, Minimized;
qboolean	Win32AtLeastV4, WinNT;
qboolean	LegitCopy = true;

static double		pfreq;
static double		curtime = 0.0;
static double		lastcurtime = 0.0;
static int			lowshift;
qboolean			isDedicated;
static qboolean		sc_return_on_enter = false;
//HANDLE				hinput, houtput;

static char			*tracking_tag = "Sticky Buns";

//static HANDLE	tevent;
//static HANDLE	hFile;
//static HANDLE	heventParent;
//static HANDLE	heventChild;

void MaskExceptions(void);
void Sys_InitFloatTime(void);
void Sys_PushFPCW_SetHigh(void);
void Sys_PopFPCW(void);

cvar_t		sys_delay = { "sys_delay","0", true };

volatile int					sys_checksum;

int GetTempPath(int cb, char *buf) {
	return 0;
}

/*
================
Sys_PageIn
================
*/
void Sys_PageIn(void *ptr, int size)
{
}


/*
===============================================================================

FILE IO

===============================================================================
*/

#define	MAX_HANDLES		10
FILE	*sys_handles[MAX_HANDLES];

int		findhandle(void)
{
	int		i;

	for (i = 1; i<MAX_HANDLES; i++)
		if (!sys_handles[i])
			return i;
	Sys_Error("out of handles");
	return -1;
}

/*
================
filelength
================
*/
int filelength(FILE *f)
{
	int		pos;
	int		end;
	int		t;

	pos = ftell(f);
	fseek(f, 0, SEEK_END);
	end = ftell(f);
	fseek(f, pos, SEEK_SET);

	return end;
}

int Sys_FileOpenRead(char *path, int *hndl)
{
	FILE	*f;
	int		i, retval;

	i = findhandle();

	f = fopen(path, "rb");

	if (!f)
	{
		*hndl = -1;
		retval = -1;
	}
	else
	{
		sys_handles[i] = f;
		*hndl = i;
		retval = filelength(f);
	}

	return retval;
}

int Sys_FileOpenWrite(char *path)
{
	FILE	*f;
	int		i;

	i = findhandle();

	f = fopen(path, "wb");
	if (!f)
		Sys_Error("Error opening %s: %s", path, strerror(errno));
	sys_handles[i] = f;

	return i;
}

void Sys_FileClose(int handle)
{
	fclose(sys_handles[handle]);
	sys_handles[handle] = NULL;
}

void Sys_FileSeek(int handle, int position)
{
	fseek(sys_handles[handle], position, SEEK_SET);
}

int Sys_FileRead(int handle, void *dest, int count)
{
	int x = fread(dest, 1, count, sys_handles[handle]);
	return x;
}

int Sys_FileWrite(int handle, void *data, int count)
{
	int x = fwrite(data, 1, count, sys_handles[handle]);
	return x;
}

int	Sys_FileTime(char *path)
{
	FILE	*f;
	int		retval;

	f = fopen(path, "rb");

	if (f)
	{
		fclose(f);
		retval = 1;
	}
	else
	{
		retval = -1;
	}

	return retval;
}

void Sys_mkdir(char *path)
{
	mkdir(path, S_IRWXU);
}


/*
===============================================================================

SYSTEM IO

===============================================================================
*/

/*
================
Sys_MakeCodeWriteable
================
*/
void Sys_MakeCodeWriteable(unsigned long startaddr, unsigned long length)
{
}


void Sys_SetFPCW(void)
{
}

void Sys_PushFPCW_SetHigh(void)
{
}

void Sys_PopFPCW(void)
{
}

void MaskExceptions(void)
{
}

void _3ds_shutdown() {
	gpuExit();
	gfxExit();
}

/*
================
Sys_Init
================
*/
void Sys_Init(void)
{
	gfxInitDefault();
	consoleInit(GFX_BOTTOM, 0);
	gpuInit();
	keyboard_init();

	atexit(_3ds_shutdown);

	LegitCopy = true;
}


void Sys_Error(char *error, ...)
{
	va_list		argptr;
	char		text[1024], text2[1024];
	char		*text3 = "Press Enter to exit\n";
	char		*text4 = "***********************************\n";
	char		*text5 = "\n";

	va_start(argptr, error);
	vsprintf(text, error, argptr);
	va_end(argptr);

	printf(text);

	//let it burn!!!
	while (1);

	Host_Shutdown();

	exit(1);
}

void Sys_Printf(char *fmt, ...)
{
	//maybe this should do something...
}

void Sys_Quit(void)
{

	Host_Shutdown();

	exit(0);
}


/*
================
Sys_FloatTime
================
*/

#define TICKS_PER_SEC 268123480.0

double Sys_FloatTime(void)
{
	u64 delta = svcGetSystemTick();
	double temp = (u32)(delta >> 32);
	temp *= 0x100000000ULL;
	temp += (u32)delta;

	return temp / TICKS_PER_SEC;
}


/*
================
Sys_InitFloatTime
================
*/
void Sys_InitFloatTime(void)
{
}


char *Sys_ConsoleInput(void)
{
	return NULL;
}

void Sys_Sleep(void)
{
	svcSleepThread(1);
}


void Sys_SendKeyEvents(void)
{
}

int GetCurrentDirectory(int cb, char *buf) {
	return getcwd(buf, cb);
}

int DeleteFile(char *f) {
	return remove(f);
}

void waitforit(char *text) {
#if 0
	printf(text);
	printf("\npress A...");
	do {
		scanKeys();
		gspWaitForEvent(GSPGPU_EVENT_VBlank0, false);
	} while ((keysDown() & KEY_A) == 0);
	do {
		scanKeys();
		gspWaitForEvent(GSPGPU_EVENT_VBlank0, false);
	} while ((keysDown() & KEY_A) == KEY_A);
	printf("done");
	printf("\n");
#endif
}


int main(int argc, char **argv)
{
	quakeparms_t	parms;
	double			time, oldtime, newtime;
	static	char	hcwd[1024];
	int				t;

	Sys_Init();

	waitforit("CL_RemoveGIPFiles");
	CL_RemoveGIPFiles(NULL);


	waitforit("GetCurrentDirectory");
	if (!GetCurrentDirectory(sizeof(hcwd), hcwd)) {
		strcpy(hcwd, ".");
	}
	strcpy(hcwd, "/hexen2");

	if (hcwd[strlen(hcwd) - 1] == '/')
		hcwd[strlen(hcwd) - 1] = 0;

	printf("cwd: %s %08x\n", hcwd, hcwd);
	waitforit("hcwd");
	parms.basedir = hcwd;
	parms.cachedir = NULL;

	parms.argc = argc;
	parms.argv = argv;

	waitforit("COM_InitArgv");
	COM_InitArgv(parms.argc, parms.argv);

	parms.argc = com_argc;
	parms.argv = com_argv;

	isDedicated = (COM_CheckParm("-dedicated") != 0);


	// take the greater of all the available memory or half the total memory,
	// but at least 8 Mb and no more than 16 Mb, unless they explicitly
	// request otherwise
	parms.memsize = MAXIMUM_WIN_MEMORY;

	if (COM_CheckParm("-heapsize"))
	{
		t = COM_CheckParm("-heapsize") + 1;

		if (t < com_argc)
			parms.memsize = atoi(com_argv[t]) * 1024;
	}

	parms.membase = malloc(parms.memsize);

	if (!parms.membase)
		Sys_Error("Not enough memory free\n");

	waitforit("S_BlockSound");
	// because sound is off until we become active
	S_BlockSound();

	waitforit("Host_Init");
	Sys_Printf("Host_Init\n");
	Host_Init(&parms);

	waitforit("Sys_FloatTime");
	oldtime = Sys_FloatTime();

	waitforit("Cvar_RegisterVariable");
	Cvar_RegisterVariable(&sys_delay);

	/* main window message loop */
	while (1)
	{
		waitforit("loop");
		// yield the CPU for a little while when paused, minimized, or not the focus
		if (cl.paused)
		{
			//SleepUntilInput(PAUSE_SLEEP);
			scr_skipupdate = 1;		// no point in bothering to draw
		}

		waitforit("Sys_FloatTime");
		newtime = Sys_FloatTime();
		time = newtime - oldtime;

#if WIN32
			if (sys_delay.value)
				Sleep(sys_delay.value);
#endif

		waitforit("Host_Frame");
		Host_Frame(time);
		oldtime = newtime;

	}

	/* return success of application */
	return 0;
}

#endif
