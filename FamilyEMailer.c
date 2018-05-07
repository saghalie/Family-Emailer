/*
**   Filename: FamilyEMailer.c
**
**   Author: WKS Software
**
**   DESC: C Module for the FamilyEMailer
**
**   Copyright © 2005, All Rights Reserved.
**
*/

#include "FamilyEMailer.h"

#define VERSION "0"
#define REVISION "01a"
#define DATBUF_SIZE 1024

struct Library * MUIMasterBase = NULL;

struct Prefs *GetMyPrefs (void);
void GetPrefsStrings (void);
struct Prefs *BuildDefPrefs (void);
BOOL SavePrefs (struct Prefs *prefs, BOOL save);
static void init( void );

STRPTR *To;

/* from smtpfuncs.c - Copyright (c) 2003, Mayukh Bose - see file for further info */
/* these are routines I changed to allow them run on the Amiga. */
extern BOOL __regargs RecvDat(STRPTR DatBuf, int *DatLen);
extern void __regargs SendCmd(STRPTR CmdText, STRPTR ParmText);
extern BOOL __regargs SendDat(STRPTR SendData);
extern void __regargs SocketConnect(void);
extern void __regargs SocketDisconnect(void);

struct Prefs
{
   UBYTE UserName [256];
   UBYTE SmtpName [256];
   UBYTE Password [256];
   UBYTE AddrFile [256];
   UBYTE MesgFile [256];
   UBYTE EditFile [256];
   UBYTE Subject  [256];
};

struct Prefs *Prefs = NULL;

extern STRPTR SMTPHost;
extern STRPTR SMTPUser;
extern STRPTR SMTPPswd;
extern STRPTR SMTPSubj;

struct NewMenu	fe_Menu [] =
{
	NM_TITLE,	"Project",             0,  0, 0, 0,
	NM_ITEM,	"Mail...",            "M", 0, 0, (APTR) ID_Mail,
	NM_ITEM,	 NM_BARLABEL,	       0,  0, 0, 0,
	NM_ITEM,	"Preferences...",     "P", 0, 0, (APTR) ID_Prefs,
	NM_ITEM,	 NM_BARLABEL,	       0,  0, 0, 0,
	NM_ITEM,	"About...",           "A", 0, 0, (APTR) ID_About,
	NM_ITEM,	 NM_BARLABEL,	       0,  0, 0, 0,
	NM_ITEM,	"Quit",               "Q", 0, 0, (APTR) ID_Quit,
	NM_TITLE,	"MUI Stuff",           0,  0, 0, 0,
	NM_ITEM,	"MUI Prefs...",       "U", 0, 0, (APTR) ID_PrefsM,
	NM_ITEM,	"MUI About...",       "?", 0, 0, (APTR) ID_AboutM,

	NM_END, 	 NULL,		       0,  0, 0, 0
};

/* since sc doesn't know about the MUI library, we have to open/close it manually */
static void init( void )
{
   if (!(MUIMasterBase = OpenLibrary(MUIMASTER_NAME,MUIMASTER_VMIN)))
   {
      printf("Can't Open MUIMaster Library");
      exit(20);
   }
}

/* this is where we create our GUI (in MUI) */
struct ObjApp * CreateApp(void)
{
	struct ObjApp * Object;

	APTR	GROUP_Root, GR_Main, GR_Main_Buttons, GROUP_Prefs, GR_Files, Space_01;
	APTR	GR_Files_Addr, LA_Address, Space_02, GR_Files_Mail, LA_Mail, Space_03;
	APTR	GR_Files_Editor, LA_Editor, Space_04, REC_label_2, Space_05, GR_Files_Buttons;
	APTR	GR_EMail, Space_06, Space_07, Space_08, Space_09, REC_label_3, Space_10;
	APTR	GR_Mail_Buttons, GROUP_Splash, GR_Splash, IM_Splash;

	if (!(Object = AllocVec(sizeof(struct ObjApp), MEMF_PUBLIC|MEMF_CLEAR)))
		return(NULL);

	Object->STR_TX_Main_01	 = "\033c\nWelcome to the Family EMailer!";
	Object->STR_TX_Main_02	 = "\033cWritten by Wawa Kopa Saghalie\n";
	Object->STR_TX_Prefs_01  = "\033c\nFamily Emailer Preferences\n";
	Object->STR_TX_Splash_01 = "\033cWelcome to the Family Emailer!";
	Object->STR_TX_Splash_02 = "\033chttp://www.kcmhosting.com";

	Object->STR_GR_Prefs[0] = "File Prefs";
	Object->STR_GR_Prefs[1] = "Email Prefs";
	Object->STR_GR_Prefs[2] = NULL;

	Object->TX_Main_01 = TextObject,
		MUIA_Background, MUII_WindowBack,
		MUIA_Text_Contents, Object->STR_TX_Main_01,
		MUIA_Text_SetMin, TRUE,
	End;

	Object->TX_Main_02 = TextObject,
		MUIA_Background, MUII_WindowBack,
		MUIA_Text_Contents, Object->STR_TX_Main_02,
		MUIA_Text_SetMin, TRUE,
	End;

	GR_Main = GroupObject,
		MUIA_HelpNode, "GR_Main",
		Child, Object->TX_Main_01,
		Child, Object->TX_Main_02,
	End;

	Object->BT_Pref  = SimpleButton("_Preferences");
	Object->BT_About = SimpleButton("_About");
	Object->BT_Edit  = SimpleButton("_Edit Message");
	Object->BT_Addr  = SimpleButton("E_dit Addresses");
	Object->BT_Send  = SimpleButton("_Send");
	Object->BT_Quit  = SimpleButton("_Quit");

	GR_Main_Buttons = GroupObject,
		MUIA_HelpNode, "GR_Main_Buttons",
		MUIA_Group_Columns, 2,
		Child, Object->BT_Pref,
		Child, Object->BT_About,
		Child, Object->BT_Edit,
		Child, Object->BT_Addr,
		Child, Object->BT_Send,
		Child, Object->BT_Quit,
	End;

	GROUP_Root = GroupObject,
		Child, GR_Main,
		Child, GR_Main_Buttons,
	End;

	Object->MainWindow = WindowObject,
		MUIA_Window_Title, "Family EMailer",
		MUIA_Window_ID, MAKE_ID('0', 'W', 'I', 'N'),
		WindowContents, GROUP_Root,
	End;

	Object->TX_Prefs_01 = TextObject,
		MUIA_Background, MUII_WindowBack,
		MUIA_Text_Contents, Object->STR_TX_Prefs_01,
		MUIA_Text_SetMin, TRUE,
	End;

	Space_01 = VSpace(20);

	LA_Address	       = Label("Address File:");
	Object->STR_PA_Address = String(Prefs->AddrFile, 80);
	Object->PA_Address     = PopButton(MUII_PopFile);

	Object->PA_Address = PopaslObject,
		MUIA_HelpNode, "PA_Address",
		MUIA_Popasl_Type, 0,
		MUIA_Popstring_String, Object->STR_PA_Address,
		MUIA_Popstring_Button, Object->PA_Address,
	End;

	GR_Files_Addr = GroupObject,
		MUIA_HelpNode, "GR_Files_Addr",
		MUIA_Group_Horiz, TRUE,
		Child, LA_Address,
		Child, Object->PA_Address,
	End;

	Space_02 = VSpace(10);
	LA_Mail = Label("EMail File:");
	Object->STR_PA_Mail = String(Prefs->MesgFile, 80);

	Object->PA_Mail = PopButton(MUII_PopFile);

	Object->PA_Mail = PopaslObject,
		MUIA_HelpNode, "PA_Mail",
		MUIA_Popasl_Type, 0,
		MUIA_Popstring_String, Object->STR_PA_Mail,
		MUIA_Popstring_Button, Object->PA_Mail,
	End;

	GR_Files_Mail = GroupObject,
		MUIA_HelpNode, "GR_Files_Mail",
		MUIA_Group_Horiz, TRUE,
		Child, LA_Mail,
		Child, Object->PA_Mail,
	End;

	Space_03 = VSpace(10);

	LA_Editor = Label("Editor File:");
	Object->STR_PA_Editor = String(Prefs->EditFile, 80);
	Object->PA_Editor = PopButton(MUII_PopFile);

	Object->PA_Editor = PopaslObject,
		MUIA_HelpNode, "PA_Editor",
		MUIA_Popasl_Type, 0,
		MUIA_Popstring_String, Object->STR_PA_Editor,
		MUIA_Popstring_Button, Object->PA_Editor,
	End;

	GR_Files_Editor = GroupObject,
		MUIA_HelpNode, "GR_Files_Editor",
		MUIA_Group_Horiz, TRUE,
		Child, LA_Editor,
		Child, Object->PA_Editor,
	End;

	Space_04 = VSpace(10);

	REC_label_2 = RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

	Space_05 = VSpace(10);

	Object->BT_Save  = SimpleButton("_Save");
	Object->BT_Close = SimpleButton("_Close");

	GR_Files_Buttons = GroupObject,
		MUIA_HelpNode, "GR_Files_Buttons",
		MUIA_Group_Columns, 2,
		Child, Object->BT_Save,
		Child, Object->BT_Close,
	End;

	GR_Files = GroupObject,
		MUIA_HelpNode, "GR_Files",
		MUIA_Background, MUII_ButtonBack,
		MUIA_Frame, MUIV_Frame_Button,
		Child, Space_01,
		Child, GR_Files_Addr,
		Child, Space_02,
		Child, GR_Files_Mail,
		Child, Space_03,
		Child, GR_Files_Editor,
		Child, Space_04,
		Child, REC_label_2,
		Child, Space_05,
		Child, GR_Files_Buttons,
	End;

	Space_06 = VSpace(20);

	Object->STR_Server = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "SMTP Server",
		MUIA_HelpNode, "STR_Server",
		MUIA_String_Contents, Prefs->SmtpName,
	End;

	Space_07 = VSpace(10);

	Object->STR_Username = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "Username",
		MUIA_HelpNode, "STR_Username",
		MUIA_String_Contents, Prefs->UserName,
	End;

	Space_08 = VSpace(10);

	Object->STR_Password = StringObject,
		MUIA_Frame, MUIV_Frame_String,
		MUIA_FrameTitle, "Password",
		MUIA_HelpNode, "STR_Password",
		MUIA_String_Contents, Prefs->Password,
	End;

	Space_09 = VSpace(10);

	REC_label_3 = RectangleObject,
		MUIA_Rectangle_HBar, TRUE,
		MUIA_FixHeight, 8,
	End;

	Space_10 = VSpace(10);

	Object->BT_Save_1  = SimpleButton("_Save");
	Object->BT_Close_1 = SimpleButton("_Close");

	GR_Mail_Buttons = GroupObject,
		MUIA_HelpNode, "GR_Mail_Buttons",
		MUIA_Group_Columns, 2,
		Child, Object->BT_Save_1,
		Child, Object->BT_Close_1,
	End;

	GR_EMail = GroupObject,
		MUIA_HelpNode, "GR_EMail",
		MUIA_Background, MUII_ButtonBack,
		MUIA_Frame, MUIV_Frame_Button,
		Child, Space_06,
		Child, Object->STR_Server,
		Child, Space_07,
		Child, Object->STR_Username,
		Child, Space_08,
		Child, Object->STR_Password,
		Child, Space_09,
		Child, REC_label_3,
		Child, Space_10,
		Child, GR_Mail_Buttons,
	End;

	Object->GR_Prefs = RegisterObject,
		MUIA_Register_Titles, Object->STR_GR_Prefs,
		MUIA_HelpNode, "GR_Prefs",
		Child, GR_Files,
		Child, GR_EMail,
	End;

	GROUP_Prefs = GroupObject,
		Child, Object->TX_Prefs_01,
		Child, Object->GR_Prefs,
	End;

	Object->PrefsWindow = WindowObject,
		MUIA_Window_Title, "FE Prefs",
		MUIA_Window_ID, MAKE_ID('1', 'W', 'I', 'N'),
		WindowContents, GROUP_Prefs,
	End;

	Object->TX_Splash_01 = TextObject,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Frame, MUIV_Frame_Button,
		MUIA_Text_Contents, Object->STR_TX_Splash_01,
		MUIA_Text_SetMin, TRUE,
	End;

	IM_Splash = ImageObject,
		MUIA_Image_Spec, "5:data/image.jpg",
		MUIA_Frame, MUIV_Frame_ImageButton,
		MUIA_Image_FreeVert, TRUE,
		MUIA_Image_FreeHoriz, TRUE,
		MUIA_FixHeight, 46,
		MUIA_FixWidth, 222,
	End;

	Object->TX_Splash_02 = TextObject,
		MUIA_Background, MUII_ButtonBack,
		MUIA_Frame, MUIV_Frame_Button,
		MUIA_Text_Contents, Object->STR_TX_Splash_02,
		MUIA_Text_SetMin, TRUE,
	End;

	GR_Splash = GroupObject,
		MUIA_HelpNode, "GR_Splash",
		Child, Object->TX_Splash_01,
		Child, IM_Splash,
		Child, Object->TX_Splash_02,
	End;

	GROUP_Splash = GroupObject,
		Child, GR_Splash,
	End;

	Object->SplashWindow = WindowObject,
		MUIA_Window_ID, MAKE_ID('2', 'W', 'I', 'N'),
		MUIA_Window_AppWindow  , TRUE,
		MUIA_Window_Borderless , TRUE,
		MUIA_Window_CloseGadget, FALSE,
		MUIA_Window_DepthGadget, FALSE,
		MUIA_Window_DragBar    , FALSE,
		MUIA_Window_SizeGadget , FALSE,
		WindowContents, GROUP_Splash,
	End;

	Object->App = ApplicationObject,
		MUIA_Application_Author      , "Wawa Kopa Saghalie",
		MUIA_Application_Base	     , "FamilyEmailer",
		MUIA_Application_Title	     , "Family EMailer",
		MUIA_Application_Version     , "v0.01a",
		MUIA_Application_Copyright   , "Copyright © 2006\nAll Rights Reserved.",
		MUIA_Application_Description , "Family Emailer",
		MUIA_Application_Menustrip   , MUI_MakeObject(MUIO_MenustripNM, fe_Menu,0),
		SubWindow, Object->MainWindow,
		SubWindow, Object->PrefsWindow,
		SubWindow, Object->SplashWindow,
	End;


	if (!Object->App)
	{
		FreeVec(Object);
		return(NULL);
	}

	/* Get the buttons ready for the main window */
	DoMethod(Object->MainWindow,
		MUIM_Window_SetCycleChain, Object->BT_Pref,
		Object->BT_About,
		Object->BT_Edit,
		Object->BT_Addr,
		Object->BT_Send,
		Object->BT_Quit,
		0
		);

	/* define the preferences window */
	DoMethod(Object->PrefsWindow,
		MUIM_Window_SetCycleChain, Object->GR_Prefs,
		Object->PA_Address,
		Object->PA_Mail,
		Object->PA_Editor,
		Object->BT_Save,
		Object->BT_Close,
		Object->STR_Server,
		Object->STR_Username,
		Object->STR_Password,
		Object->BT_Save_1,
		Object->BT_Close_1,
		0
		);

	/* splash window for startup. */
	DoMethod(Object->SplashWindow, MUIM_Window_SetCycleChain, 0);

	/* Getting it all set up so that the window will properly show with all children */
	DoMethod(Object->MainWindow,
	    MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	    Object->MainWindow,3,
	    MUIM_Set, MUIA_Window_Open, FALSE
	   );

	DoMethod( Object->MainWindow,
	     MUIM_Notify, MUIA_Window_CloseRequest, TRUE,
	     Object->MainWindow,2,
	     MUIM_Application_ReturnID, MUIV_Application_ReturnID_Quit
	   );

	// Main window buttons
	DoMethod(Object->BT_Pref ,MUIM_Notify,MUIA_Pressed,FALSE,Object->App,2,MUIM_Application_ReturnID,ID_Prefs);
	DoMethod(Object->BT_About,MUIM_Notify,MUIA_Pressed,FALSE,Object->App,2,MUIM_Application_ReturnID,ID_About);
	DoMethod(Object->BT_Edit ,MUIM_Notify,MUIA_Pressed,FALSE,Object->App,2,MUIM_Application_ReturnID,ID_Edit);
	DoMethod(Object->BT_Addr ,MUIM_Notify,MUIA_Pressed,FALSE,Object->App,2,MUIM_Application_ReturnID,ID_Addr);
	DoMethod(Object->BT_Send ,MUIM_Notify,MUIA_Pressed,FALSE,Object->App,2,MUIM_Application_ReturnID,ID_Send);
	DoMethod(Object->BT_Quit ,MUIM_Notify,MUIA_Pressed,FALSE,Object->App,2,MUIM_Application_ReturnID,ID_Quit);

	// Preferences window buttons
	DoMethod(Object->BT_Save   ,MUIM_Notify,MUIA_Pressed,FALSE,Object->App,2,MUIM_Application_ReturnID,ID_Save);
	DoMethod(Object->BT_Close  ,MUIM_Notify,MUIA_Pressed,FALSE,Object->App,2,MUIM_Application_ReturnID,ID_Close);
	DoMethod(Object->BT_Save_1 ,MUIM_Notify,MUIA_Pressed,FALSE,Object->App,2,MUIM_Application_ReturnID,ID_Save_1);
	DoMethod(Object->BT_Close_1,MUIM_Notify,MUIA_Pressed,FALSE,Object->App,2,MUIM_Application_ReturnID,ID_Close_1);

	set(Object->SplashWindow, MUIA_Window_Open, TRUE);
	Delay(100);
	set(Object->SplashWindow, MUIA_Window_Open, FALSE);

	/* Now the window is ready for display */
	set(Object->MainWindow, MUIA_Window_Open, TRUE);

	return(Object);
}

/* close up the library and dispose of the object when ready */
void DisposeApp(struct ObjApp * Object)
{
   MUI_DisposeObject(Object->App);
   FreeVec(Object);

   if ( MUIMasterBase )
   {
      CloseLibrary(MUIMasterBase);
      MUIMasterBase = NULL;
   }
}

int main ( int argc, char *argv[] )
{
   struct ObjApp * App = NULL;

   BOOL running = TRUE;
   ULONG signal = 0;
   BPTR fhIn = NULL, fhMn = NULL;
   ULONG fhInBuff = 512;
   ULONG fhMnBuff = 512;

   char *username, *password, *mailsrvr;
   char *address, *editor, *mail, *title, *version, *author, *copyright;

   init(); // Open MUIMaster Library

   if(!(Prefs = GetMyPrefs()))
   {
      Prefs = BuildDefPrefs();
   }

   if(!(App = CreateApp()))
   {
      printf("Unable to create Application!\n");
      return 20;
   }

   while ( running )
   {
      switch ( DoMethod ( App->App, MUIM_Application_Input, &signal ) )
      {
	 case MUIV_Application_ReturnID_Quit:
	    running = FALSE;
	    break;

	 case ID_About:
	    get( App->App, MUIA_Application_Title,     &title	  );
	    get( App->App, MUIA_Application_Version,   &version   );
	    get( App->App, MUIA_Application_Author,    &author	  );
	    get( App->App, MUIA_Application_Copyright, &copyright );

	    MUI_Request( App->App, NULL, 0, "::About::", "_Continue", "\n\033c %s\n%s \n\nWritten by\n%s\n\n%s\n\nEmail: cowlitz@comcast.net\n",title,version,author,copyright,TAG_END );
	    break;

	 case ID_PrefsM:
	    DoMethod(App->App, MUIM_Application_OpenConfigWindow, 0);
	    break;

	 case ID_AboutM:
	    App->AboutMUIWin = AboutmuiObject, MUIA_Aboutmui_Application, App->App, End;
	    set(App->AboutMUIWin, MUIA_Window_Open, TRUE);
	    break;

	 case ID_Send:
	 {
	    char * AddrBuff;
	    char * MesgBuff;
	    char AddrBuffer[512];

	    int len;
	    int len1;

	    /* read in addresses */
	    AddrBuff = ( char * )AllocMem( fhInBuff, MEMF_ANY|MEMF_CLEAR );

	    if(!( fhIn = Open( Prefs->AddrFile, MODE_OLDFILE )))
	    {
	       MUI_Request(App->App,NULL,0,NULL,"_OK","Unable to open address file\n");
	       break;
	    }

	    len = Read( fhIn, AddrBuff, fhInBuff);
	    if(!len)
		printf("Unable to read file\n");

	    strncpy( AddrBuffer, AddrBuff, sizeof(AddrBuffer));
	    strip_newline(AddrBuffer, sizeof(AddrBuffer));

	    /* read in message */
	    MesgBuff = ( char * )AllocMem( fhMnBuff, MEMF_ANY|MEMF_CLEAR );

	    if(!( fhMn = Open( Prefs->MesgFile, MODE_OLDFILE )))
	    {
	       MUI_Request(App->App,NULL,0,NULL,"_OK","Unable to open message file\n");
	       break;
	    }

	    len1 = Read( fhMn, MesgBuff, fhMnBuff);
	    if(!len)
		printf("Unable to read file\n");

	    printf("Server: %s\nFrom: %s\nTo: %s\nSubject: %s\nReplyTo: %s\nMessage: %s\n",Prefs->SmtpName,Prefs->UserName,AddrBuffer,Prefs->Subject,Prefs->UserName,MesgBuff);
/*
	    if ( send_mail ( Prefs->SmtpName,  // SMTP Server Name
			     Prefs->UserName,  // From user
			     AddrBuffer,       // To users
			     Prefs->Subject,   // Subject
			     Prefs->UserName,  // ReplyTo
			     MesgBuff ) != 0 ) // Message File
	       MUI_Request(App->App, NULL, 0, "::Failure::","_Continue","\033c Send Message Failed!\n", TAG_END);
	    else
	       MUI_Request(App->App, NULL, 0, "::Success::","_Continue","\033c Message Sent!\n", TAG_END);
*/
	    Close ( fhIn );
	    Close ( fhMn );

	    fhIn = NULL;
	    fhMn = NULL;

	    FreeMem( MesgBuff, fhMnBuff );
	    FreeMem( AddrBuff, fhInBuff );

	    break;
	 }

	 case ID_Prefs:
	    set(App->MainWindow , MUIA_Window_Sleep, TRUE);
	    set(App->PrefsWindow, MUIA_Window_Open,  TRUE);
	    break;

	 case ID_Save:
	 case ID_Save_1:
	    get (App->STR_Username, MUIA_String_Contents, &username);
	    strcpy (Prefs->UserName, username);

	    get (App->STR_Password, MUIA_String_Contents, &password);
	    strcpy (Prefs->Password, password);

	    get (App->STR_Server, MUIA_String_Contents, &mailsrvr);
	    strcpy (Prefs->SmtpName, mailsrvr);

	    get (App->STR_PA_Address, MUIA_String_Contents, &address);
	    strcpy (Prefs->AddrFile, address);

	    get (App->STR_PA_Mail, MUIA_String_Contents, &mail);
	    strcpy (Prefs->MesgFile, mail);

	    get (App->STR_PA_Editor, MUIA_String_Contents, &editor);
	    strcpy (Prefs->EditFile, editor);

	    // get (App->STR_SUBJ, MUIA_String_Contents, &subject);
	    // strcpy (Prefs->Subject, subject);

	    SavePrefs(Prefs, TRUE);

	    set(App->PrefsWindow, MUIA_Window_Open,  FALSE);
	    set(App->MainWindow , MUIA_Window_Sleep, FALSE);

	    break;

	 case ID_Close:
	 case ID_Close_1:
	    set(App->PrefsWindow, MUIA_Window_Open,  FALSE);
	    set(App->MainWindow , MUIA_Window_Sleep, FALSE);
	    break;

	 case ID_Edit:
	 {
	    UBYTE cmdline [256];

	    set(App->App, MUIA_Window_Sleep, TRUE);
	    sprintf(cmdline, "%s %s", Prefs->EditFile, Prefs->MesgFile);
	    if(!(SystemTagList(cmdline, TAG_END)))
	       set(App->App, MUIA_Window_Sleep, FALSE);
	    else
	       MUI_Request(App->App, NULL, 0, "::Failure::","_Continue","\n\033c Unable to Open Editor!\n", TAG_END);
	    break;
	 }

	 case ID_Addr:
	 {
	    UBYTE cmdline [256];

	    set(App->App, MUIA_Window_Sleep, TRUE);
	    sprintf(cmdline, "%s %s", Prefs->EditFile, Prefs->AddrFile);
	    if(!(SystemTagList(cmdline, TAG_END)))
	       set(App->App, MUIA_Window_Sleep, FALSE);
	    else
	       MUI_Request(App->App, NULL, 0, "::Failure::","_Continue","\n\033c Unable to Open Editor!\n", TAG_END);
	    break;
	 }

	 case ID_Quit:
	    running = FALSE;
	    break;
      }

      if ( running && signal )
	 Wait ( signal );
   }

   DisposeApp(App);

   return 0;
}

char * strip_newline( char *str, int size )
{
    int i;


    /* remove the null terminator */
    for (  i = 0; i < size; ++i )
    {
	if ( str[i] == '\n' )
	{
	    str[i] = '\0';

	    /* we're done, so just exit the function by returning */
	    return( str );
	}
    }
    /* if we get all the way to here, there must not have been a newline! */
}

struct Prefs *GetMyPrefs (void)
{
	/*-------------*/
	/* L O C A L S */
	/*-------------*/
	BPTR	       prefFile = NULL;  // Prefs file handle.
	struct Prefs  *prefs;		 // Prefs to get and return.
	struct ObjApp *App = NULL;	 // Pointer to the application

	/*---------*/
	/* C O D E */
	/*---------*/
	/*---------------------------------------*/
	/* Try to open ENV:MUIEmail.prefs first. */
	/*---------------------------------------*/
	if (!(prefFile = Open ("ENV:Email.prefs", MODE_OLDFILE)))
	{
		/*----------------------*/
		/* Try ENVARC: instead. */
		/*----------------------*/
		if (!(prefFile = Open ("ENVARC:Email.prefs", MODE_OLDFILE)))
		{
			/*-------------*/
			/* We give up. */
			/*-------------*/
			return (NULL);
		}
	}

	/*----------------------------------------------------------------------------*/
	/* We have opened the prefs file successfully. Now we allocate mem for prefs. */
	/*----------------------------------------------------------------------------*/
	if (!(prefs = AllocVec (sizeof (struct Prefs), MEMF_ANY | MEMF_CLEAR)))
	{
		Close (prefFile);
		return (NULL);
	}

	/*--------------------*/
	/* Read in the prefs. */
	/*--------------------*/
	if (Read (prefFile, prefs, sizeof (struct Prefs)) != sizeof (struct Prefs))
	{
		MUI_Request (App->App, NULL, 0, NULL, "Okay",
			     "Family EMailer V%s.%s\n\nThe preference file has a wrong length,\n"
			     "probably because it's an old version. I'm\n"
			     "going to use internal defaults.",
			      VERSION, REVISION, TAG_END);

		Close (prefFile);
		FreeVec (prefs);
		prefs = NULL;
		prefs = BuildDefPrefs ();
		return (prefs);
	}

	/*---------------------------------------------------------------*/
	/* Everything is A-Okay, we close the file and return the prefs. */
	/*---------------------------------------------------------------*/
	Close (prefFile);
	return (prefs);
}

struct Prefs *BuildDefPrefs (void)
{
	/*-------------*/
	/* L O C A L S */
	/*-------------*/
	struct Prefs	*prefs;

	/*---------*/
	/* C O D E */
	/*---------*/
	/*----------------------------*/
	/* We allocate mem for prefs. */
	/*----------------------------*/
	if (!(prefs = AllocVec (sizeof (struct Prefs), MEMF_ANY | MEMF_CLEAR)))
	{
		return (NULL);
	}

	/*-------------------------*/
	/* Fill in default values. */
	/*-------------------------*/
	strcpy (prefs->UserName, "user@somewhere.net");
	strcpy (prefs->Password, "yourpassword");
	strcpy (prefs->SmtpName, "mail.somewhere.net");
	strcpy (prefs->Subject,  "Family EMailer v0.01a");
	strcpy (prefs->EditFile, "ed");
	strcpy (prefs->AddrFile, "address.txt");
	strcpy (prefs->MesgFile, "message.txt");


	/*-------------------*/
	/* Return the prefs. */
	/*-------------------*/
	return (prefs);
}

BOOL SavePrefs (struct Prefs *prefs, BOOL save)
{
	/*-------------*/
	/* L O C A L S */
	/*-------------*/
	BPTR	prefFile;    // Prefs file handle.

	/*---------*/
	/* C O D E */
	/*---------*/
	/*-----------------------------------------*/
	/* We always want to at least save to ENV: */
	/*-----------------------------------------*/
	if (!(prefFile = Open ("ENV:Email.prefs", MODE_NEWFILE)))
	{
		return (FALSE);
	}

	/*---------------*/
	/* Write to ENV: */
	/*---------------*/
	if (Write (prefFile, prefs, sizeof (struct Prefs)) != sizeof (struct Prefs))
	{
		Close (prefFile);
		return (FALSE);
	}

	/*-----------------*/
	/* Close the file. */
	/*-----------------*/
	Close (prefFile);

	/*---------------------------------------------*/
	/* Check if we need to save it to ENVARC: too. */
	/*---------------------------------------------*/
	if (save)
	{
		/*--------------------*/
		/* We save to ENVARC: */
		/*--------------------*/
		if (!(prefFile = Open ("ENVARC:Email.prefs", MODE_NEWFILE)))
		{
			return (FALSE);
		}

		/*------------------*/
		/* Write to ENVARC: */
		/*------------------*/
		if (Write (prefFile, prefs, sizeof (struct Prefs)) != sizeof (struct Prefs))
		{
			Close (prefFile);
			return (FALSE);
		}

		/*-----------------*/
		/* Close the file. */
		/*-----------------*/
		Close (prefFile);
	}
	return (TRUE);
}
