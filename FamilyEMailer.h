/*
**  Filename: FamilyEmailer.h
**
**  H Module for the Family EMailer
**
**  Copyright © 2005, Troy E Bouchard, All Rights Reserved.
*/

/* MUI */
#include <libraries/mui.h>	  // MUI Library.
#include <proto/muimaster.h>	  // Protos for MUIMasterBase.

/* System */
#include <clib/exec_protos.h>	  // Protos for OpenLibrary() etc...
#include <exec/memory.h>	  // For MEMF_PUBLIC etc...
#include <clib/alib_protos.h>	  // Protos for DoMethod().

#include <dos/dos.h>		  // For SYStem output.
#include <graphics/gfxmacros.h>   // For Graphics.
#include <workbench/workbench.h>  // For Workbench related.
#include <libraries/gadtools.h>   // For struct NewMenu and sundry.

#include <proto/dos.h>
#include <proto/exec.h>
#include <proto/intuition.h>

/* Prototypes */
#include <clib/dos_protos.h>
#include <clib/icon_protos.h>
#include <clib/graphics_protos.h>
#include <clib/intuition_protos.h>
#include <clib/gadtools_protos.h>
#include <clib/utility_protos.h>
#include <clib/asl_protos.h>

#ifndef __GNUC__
#include <clib/muimaster_protos.h>
#else
#include <inline/muimaster.h>
#endif

/* ANSI C */
#include <stdlib.h>
#include <strings.h>
#include <stdio.h>
#include <time.h>
#include <sys/dir.h>
#include <dos/stdio.h>
#include <dos/dosextens.h>

/* MUIBuilder defines */
#ifndef MAKE_ID
#define MAKE_ID(a,b,c,d) ((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))
#endif

struct ObjApp
{
	APTR	App;
	APTR	AboutMUIWin;
	APTR	MainWindow;
	APTR	TX_Main_01;
	APTR	TX_Main_02;
	APTR	BT_Pref;
	APTR	BT_About;
	APTR	BT_Edit;
	APTR	BT_Addr;
	APTR	BT_Send;
	APTR	BT_Quit;
	APTR	PrefsWindow;
	APTR	TX_Prefs_01;
	APTR	GR_Prefs;
	APTR	PA_Address;
	APTR	STR_PA_Address;
	APTR	PA_Mail;
	APTR	STR_PA_Mail;
	APTR	PA_Editor;
	APTR	STR_PA_Editor;
	APTR	BT_Save;
	APTR	BT_Close;
	APTR	STR_Server;
	APTR	STR_Username;
	APTR	STR_Password;
	APTR	BT_Save_1;
	APTR	BT_Close_1;
	APTR	SplashWindow;
	APTR	TX_Splash_01;
	APTR	TX_Splash_02;
	char *	STR_TX_Main_01;
	char *	STR_TX_Main_02;
	char *	STR_TX_Prefs_01;
	char *	STR_TX_Splash_01;
	char *	STR_TX_Splash_02;
	char *	STR_GR_Prefs[3];
};

enum {
   ID_Prefs=1, // Bring up the Preferences Window
   ID_Mail,    // Menu Mail
   ID_About,   // All About this silly program
   ID_Send,    // Send button
   ID_Close,	// Preferences Cancel Button
   ID_Save,    // Preferences Save Button
   ID_Save_1,
   ID_Close_1,
   ID_Quit,
   ID_Edit,
   ID_Addr,
   ID_PrefsM,
   ID_AboutM,
};

extern struct ObjApp * CreateApp(void);
extern void DisposeApp(struct ObjApp *);
extern int main ( int argc, char *argv[] );
extern int send_mail(const char *smtpserver, const char *from, const char *to,
				const char *subject, const char *replyto, const char *msg);
extern int connect_to_server(const char *server);
extern int send_command(int n_sock, const char *prefix, const char *cmd,
					const char *suffix, int ret_code);
extern int send_mail_message(int n_sock, const char *from, const char *to,
						const char *subject, const char *replyto, const char *msg);


extern char * strip_newline( char *str, int size );

