#ifdef _MSC_VER
#pragma comment(lib, "user32")
#endif

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#define JrPlugin_THISISDLL
#include "JrPluginFuncs.h"
#include "JrPluginFuncs_FARR.h"
#include "everything_ipc.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#pragma pack(push, 1)

#define COPYDATA_IPCTEST_QUERYCOMPLETEW	0
#define COPYDATA_IPCTEST_QUERYCOMPLETEA	1

/* Customize: */
#ifdef DEBUG
#define ThisPlugin_DisplayName				"TinyEverything :DEBUG:"
#else
#define ThisPlugin_DisplayName				"TinyEverything"
#endif
#define ThisPlugin_VersionString			"0.9.5"
#define ThisPlugin_ReleaseDateString		"12/6/2009"
#define ThisPlugin_Author					"AKX"
#define ThisPlugin_HomepageURL				"-"
#define ThisPlugin_UpdateURL				""
#define ThisPlugin_ShortDescription			"Search Everything."
#define ThisPlugin_LongDescription			"Searches content in the Everything content indexer."
#define ThisPlugin_AdvConfigString			""
#define ThisPlugin_ReadMeString				""
#define ThisPlugin_ReadMeFile				""
#define ThisPlugin_IconFilename				"tinyev.ico"

#define ThisPlugin_AliasStr					"ev"
#define ThisPlugin_RegexStr					""
#define ThisPlugin_RegexFilterStr			""
#define ThisPlugin_KeywordStr				""
#define ThisPlugin_ScoreStr					"300"

Fp_GlobalPluginCallback_GetStrVal callbackfp_get_strval=NULL;
Fp_GlobalPluginCallback_SetStrVal callbackfp_set_strval=NULL;
void* hostptr=NULL;
char iconPath[MAX_PATH];
char iniPath[MAX_PATH];

void ExecuteCallback_SearchStateChanged();
Fp_GlobalPluginCallback_NotifySearchStateChanged callbackfp_notifysearchstatechanged=NULL;

int TinyEV_Init();
void TinyEV_Exit();


#ifdef DEBUG
	#include "debugmagic.c"
#else
#define debugmsg farrStatus
#endif

/*
#define strcmp scomp
#define strcpy scpy
#define strncpy sncpy
//#define strlen slen
//#define strrchr srchr

int scomp (const char * s1, const char * s2)
{
    for(; *s1 == *s2; ++s1, ++s2) if(*s1 == 0) return 0;
	return 1;
    //return *(unsigned char *)s1 < *(unsigned char *)s2 ? -1 : 1;
}


void scpy(char *s1, const char s2)
{
	while(*s2) (*s1++)=(*s2++);
	*s1=0;
}

void sncpy(char *s1, const char *s2, int n)
{
	while(*s2&&n--) (*s1++)=(*s2++);
	*s1=0;
}


int slen(const char *s)
{
	int l=0;
	while(*s++) l++;
	return l;
}

char *srchr(const char *s,char l)
{
	s+=slen(s)-1;
	while(*s--!=l);
	return s;
}*/


/**
 * Called by host when plugins are discovered.
 * Most of the time all you have to do is record data you want to remember for later.
 * 
 * @param fullfilename The dll's own filename
 * @param thisptr A pointer it will use when performing callbacks
 * @param hostname The name of the host program
 * @param hostversionstring The char* string describing the host program version number
 * @param incallbackfp pointer to global generic callback function of host
 * @param incallbackfp_get_strval pointer to simple get_strval callback function of host
 * @return TRUE/FALSE - FALSE if the plugin wants to refuse to be loaded (useful if we check hostname/version and know we don't support it)
 */
PREFUNCDEF BOOL PluginFunc_DoInit(
	const char* fullfilename,
	void* thisptr,
	const char *hostname,
	const char *hostversionstring,
	Fp_GlobalPluginCallback_GetStrVal incallbackfp_get_strval,
	Fp_GlobalPluginCallback_SetStrVal incallbackfp_set_strval
)
{
	char dllPath[MAX_PATH];
	hostptr = thisptr;
	callbackfp_get_strval=incallbackfp_get_strval;
	callbackfp_set_strval=incallbackfp_set_strval;
	strncpy(dllPath, fullfilename, MAX_PATH);
	char* rpathchar=strrchr(dllPath,'\\');
	if (rpathchar!=NULL) *(rpathchar+1)='\0';
	//sprintf(iconPath, "%s%s", dllPath, "tinyev.ico");
	sprintf(iconPath, "%s%s", dllPath, "tinyev.ico");
	sprintf(iniPath, "%s%s", dllPath, "farr_tinyev.ini");
	if(TinyEV_Init())
	{
		TinyEV_Exit();
		return FALSE;
	}
	return TRUE;
}


/**
 * Cleanup.
 */
PREFUNCDEF BOOL PluginFunc_DoShutDown()
{
	TinyEV_Exit();
	return TRUE;
}


/**
 * String data request interface.
 * @param varname Variable to get.
 * @param destbuf Target buffer.
 * @param maxlen Allocated size of target buffer.
 * @return true/false TRUE if found. FALSE if not.
 */


PREFUNCDEF BOOL PluginFunc_GetStrVal(const char* varname, char *destbuf, int maxlen)
{
	#define CHECKRET(x) if ( strcmp( varname, DEF_FieldName_##x )==0) { strncpy(destbuf, ThisPlugin_##x, maxlen); return TRUE; }
	CHECKRET(DisplayName);
	CHECKRET(VersionString);
	CHECKRET(ReleaseDateString);
	CHECKRET(Author);
	CHECKRET(UpdateURL);
	CHECKRET(HomepageURL);
	CHECKRET(ShortDescription);
	CHECKRET(LongDescription);
	CHECKRET(AdvConfigString);
	CHECKRET(ReadMeString);
	CHECKRET(IconFilename);
	CHECKRET(RegexStr);
	CHECKRET(RegexFilterStr);
	CHECKRET(KeywordStr);
	CHECKRET(ScoreStr);
	CHECKRET(AliasStr);
	#undef CHECKRET
	
	return FALSE;
}

/**
 * Host-specific option/value setting interface.
 * @param varname Variable to set.
 * @param val Value (depends on variable).
 * @return true/false TRUE if set. FALSE if can't be set (not know, etc)
 */
PREFUNCDEF BOOL PluginFunc_SetStrVal(const char* varname, void *val)
{
	if (strcmp(varname, DEF_FieldName_NotifySearchCallbackFp)==0)
	{
		callbackfp_notifysearchstatechanged = (Fp_GlobalPluginCallback_NotifySearchStateChanged)val;
		return TRUE;
	}

	return FALSE;
}

/**
 * Protocol/feature/host check.
 * @param testname Feature to check.
 * @param version Feature version to check.
 * @return true/false True if supported.
 */
PREFUNCDEF BOOL PluginFunc_SupportCheck(const char* testname, int version)
{
	if (strcmp(testname,DEF_FARRAPI_IDENTIFIER)==0) return TRUE;
	return FALSE;
}

/**
 * Advanced config interface.
 */
/*
PREFUNCDEF BOOL PluginFunc_DoAdvConfig()
{
	return FALSE;
}
*/

/**
 * Readme interface.
 */
/*
PREFUNCDEF BOOL PluginFunc_DoShowReadMe()
{
	return FALSE;
}
*/

/**
 * State change.
 * @return true/false False on error.
 */
/*
PREFUNCDEF BOOL PluginFunc_SetState(int newstateval)
{
	return TRUE;
}
*/

/**
 * Report error.
 */
/*
void ReportErrorToHost(char *estring)
{
	if (callbackfp_set_strval != NULL && hostptr!=NULL) callbackfp_set_strval(hostptr, "reporterror", estring);
}
 */

BOOL APIENTRY DllMain (HINSTANCE hInst, DWORD reason, LPVOID reserved)
{
    return TRUE;
}
//-----------------------------------------------------------------------

void farrStatus(char *msg, ...)
{
	va_list args;
	char tmp[600];
	if(callbackfp_set_strval && hostptr)
	{
		va_start (args, msg);
		vsnprintf (tmp, 598, msg, args);
		va_end (args);
		callbackfp_set_strval(hostptr, "statusbar", tmp);
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

int nResults=0;
E_SearchStateT currentState = E_SearchState_Stopped;
E_ResultAvailableStateT resultState = E_ResultAvailableState_None;
int lockState=0;

HWND windowHandle=0;
int gotData=0;
int timeout=0;
int isTutorial=0;
int scoringType=0;

typedef struct {
	char path[MAX_PATH];
	char name[MAX_PATH];
	unsigned char isFolder;
} EResult;

EResult* eResults;
int eResultsN=0;

void clearResults()
{
	if(eResults)
	{
		// free(eResults);
		HeapFree(GetProcessHeap(), 0, eResults);
		eResults=NULL;
	}
	eResultsN=0;
}

inline int processResults(EVERYTHING_IPC_LIST *list)
{
	DWORD i;
	
	clearResults();
	eResultsN=list->numitems;
	//eResults=malloc(sizeof(EResult)*eResultsN);
	eResults=HeapAlloc(GetProcessHeap(), 0, sizeof(EResult)*eResultsN);
	//debugmsg("Allocating %d bytes of memory for eResults (one eResult = %d bytes).", sizeof(EResult)*eResultsN, sizeof(EResult));
	if(!eResults)
	{
		farrStatus("Couldn't allocate memory for Everything! (.. haha)");
		clearResults();
		return 0;
	}
	
	for(i=0;i<eResultsN;i++)
	{
		if (!(list->items[i].flags & EVERYTHING_IPC_DRIVE))
		{
			if(list->items[i].flags & EVERYTHING_IPC_FOLDER)
			{
				snprintf(eResults[i].path, MAX_PATH, "%s\\%s\\",
					EVERYTHING_IPC_ITEMPATH(list,&list->items[i]),
					EVERYTHING_IPC_ITEMFILENAME(list,&list->items[i])
				);
				eResults[i].isFolder=1;
			}
			else
			{
				snprintf(eResults[i].path, MAX_PATH, "%s\\%s",
					EVERYTHING_IPC_ITEMPATH(list,&list->items[i]),
					EVERYTHING_IPC_ITEMFILENAME(list,&list->items[i])
				);
				eResults[i].isFolder=0;
			}
			
			snprintf(eResults[i].name, MAX_PATH, "%s",
				EVERYTHING_IPC_ITEMFILENAME(list,&list->items[i])
			);
			//debugmsg("P = [%s]", eResults[i].path);
		}
	}
	return 1;
	//debugmsg("processed %d results", eResultsN);
}

LRESULT __stdcall TinyEV_WinProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch(msg)
	{
		case WM_COPYDATA:
		{
			COPYDATASTRUCT *cds = (COPYDATASTRUCT *)lParam;
			
			switch(cds->dwData)
			{
				case COPYDATA_IPCTEST_QUERYCOMPLETEA:
					gotData=processResults((EVERYTHING_IPC_LIST *)cds->lpData);
					return TRUE;
			}
			
			break;
		}
	}
	return DefWindowProc(hwnd,msg,wParam,lParam);
}

BOOL DoFarrSearchBegin(const char *search);

int TinyEV_Init()
{
	WNDCLASSEX windowClass;
	ZeroMemory(&windowClass,sizeof(windowClass));
	windowClass.cbSize = sizeof(windowClass);
	
	if (!GetClassInfoEx(GetModuleHandle(0), "TinyEV_WndClass", &windowClass))
	{
		ZeroMemory(&windowClass,sizeof(windowClass));
		windowClass.cbSize = sizeof(windowClass);
		windowClass.hInstance = GetModuleHandle(0);
		windowClass.lpfnWndProc = TinyEV_WinProc;
		windowClass.lpszClassName = "TinyEV_WndClass";
		if (!RegisterClassEx(&windowClass))
		{
			debugmsg("Failed to register window class");
			return 1;
		}
	}
	
	if (!(windowHandle = CreateWindow("TinyEV_WndClass", "",0,0,0,0,0,0,0,GetModuleHandle(0),0)))
	{
		debugmsg("Failed to create window");
		return 1;
	}

	E_ResultPostProcessingT tmpScoring;
	tmpScoring = (E_ResultPostProcessingT) GetPrivateProfileInt("TinyEV", "ScoringType", (DWORD)E_ResultPostProcessing_AddScore_wPats, iniPath);
	if(tmpScoring<0||tmpScoring>4) tmpScoring=E_ResultPostProcessing_AddScore_wPats;
	scoringType=tmpScoring;
	
	return 0;
}

void TinyEV_Exit()
{
	if(windowHandle)
	{
		DestroyWindow(windowHandle);
		windowHandle=NULL;
	}
	UnregisterClass("TinyEV_WndClass", GetModuleHandle(0));
}

PREFUNCDEF BOOL EFuncName_Inform_SearchBeginsV2(
	const char* searchstring_raw,
	const char *searchstring_lc_nokeywords,
	BOOL explicitinvocation,
	const char *modifierstring,
	int triggermethod
){
	if (!explicitinvocation) return FALSE;
	return DoFarrSearchBegin(searchstring_lc_nokeywords);
}

PREFUNCDEF BOOL EFuncName_Inform_RegexSearchMatchV2(
	const char* searchstring_raw,
	const char *searchstring_lc_nokeywords,
	int regexgroups,
	char** regexcharps,
	const char *modifierstring,
	int triggermethod
){
	if(regexgroups>=1)
	{
		return DoFarrSearchBegin(regexcharps[1]);
	}
	return FALSE;
}

PREFUNCDEF void EFuncName_Inform_SearchEnds()
{
	currentState=E_SearchState_Stopped;
	nResults=0;
	resultState = E_ResultAvailableState_None;
	clearResults();
	ExecuteCallback_SearchStateChanged();
	return;
}
/*
PREFUNCDEF void EFuncName_Inform_WindowIsHiding(HWND hwndp)
{
	return;
}

PREFUNCDEF void EFuncName_Inform_WindowIsUnHiding(HWND hwndp)
{
	return;
}
*/

//-----------------------------------------------------------------------
// Host is asking us if we want a certain feature or call
// We should use a switch function here to tell what is being asked
//
// Returns TRUE or FALSE depending on whether we want the feature
//
PREFUNCDEF BOOL EFuncName_Ask_WantFeature(E_WantFeaturesT featureid)
{
	switch (featureid)
	{
		case E_WantFeatures_searchinput_regex:	return TRUE; break;
		case E_SupportFeatures_searchinput_explicit: return TRUE; break;
		case E_SupportFeatures_dominate_results: return TRUE; break;
		case E_SupportFeatures_scoreall_files: return TRUE; break;
		default: return FALSE; break;
	}
	return FALSE;
}
//-----------------------------------------------------------------------

PREFUNCDEF E_SearchStateT EFuncName_Ask_SearchState()
{
	return currentState;
}

PREFUNCDEF E_ResultAvailableStateT EFuncName_Ask_IsAnyResultAvailable()
{
	return resultState;
}

PREFUNCDEF int EFuncName_Ask_HowManyItemResultsAreReady()
{
	return nResults;
}
//-----------------------------------------------------------------------



//-----------------------------------------------------------------------
// Host calls this before iterating through the result requests
// It will be TRUE on lock before retrieving
//  and then FALSE when all done retrieving
// In this way the dll should refrain from altering the results until retrieval is done
// One way to do this for asynchronous searching is to MOVE results to a return store
//  on TRUE lock, and then free on FALSE.
//
// Returns TRUE on success
//
PREFUNCDEF BOOL EFuncName_Request_LockResults(BOOL dolock)
{
	// set lock
	lockState=(dolock?1:0);

	if (!lockState)
	{
		//debugmsg("lock freed, emptying results");
		nResults=0;
		resultState = E_ResultAvailableState_None;
		clearResults();
		ExecuteCallback_SearchStateChanged();
	}
	else
	{
		//debugmsg("lock acquired...");
	}
	return TRUE;
}


void sendTutorial()
{
	isTutorial=1;
	nResults=3;
	resultState=E_ResultAvailableState_ItemResults;
	currentState=E_SearchState_Stopped;
	ExecuteCallback_SearchStateChanged();
}


PREFUNCDEF BOOL EFuncName_Request_ItemResultByIndex(
	int resultindex,
	char *destbuf_path,
	char *destbuf_caption,
	char *destbuf_groupname,
	char *destbuf_iconfilename,
	void** tagvoidpp,
	int maxlen,
	E_ResultPostProcessingT *resultpostprocmodep,
	int *scorep,
	E_EntryTypeT *entrytypep
){
	if(isTutorial)
	{
		if(resultindex>=3) return FALSE;
		strcpy(destbuf_groupname,"");
		*tagvoidpp=NULL;
		*resultpostprocmodep=E_ResultPostProcessing_ImmediateDisplay;
		*entrytypep=E_EntryType_FILE;
		*scorep=1337;
		strcpy(destbuf_iconfilename, iconPath);
		#define RES(type, path, caption) *entrytypep=type; strncpy(destbuf_path, path, maxlen); strncpy(destbuf_caption, caption, maxlen); return TRUE;
		switch(resultindex)
		{
			case 0:	RES(E_EntryType_ALIAS,	"dosearch everything",					"Search for Everything to launch it"); break;
			case 1:	RES(E_EntryType_URL,	"http://voidtools.com", 				"Visit voidtools.com to download and install the Everything tool"); break;
		}
		#undef RES
		return FALSE;
	}
	else
	{
		if(resultindex>=eResultsN) return FALSE;
		
		strcpy(destbuf_groupname,"");
		*tagvoidpp=NULL;
		*resultpostprocmodep=scoringType;
		if(eResults[resultindex].isFolder)	*entrytypep=E_EntryType_FOLDER;
		else 								*entrytypep=E_EntryType_FILE;
		
		*scorep=1000;
	
		strncpy(destbuf_path,			eResults[resultindex].path,		maxlen);
		strncpy(destbuf_caption,		eResults[resultindex].name,		maxlen);
		strncpy(destbuf_iconfilename,	"",								maxlen);
		return TRUE;
	}
}

/*
PREFUNCDEF BOOL EFuncName_Request_TextResultCharp(char **charp)
{
	*charp=NULL;
	return FALSE;
}

PREFUNCDEF BOOL EFuncName_Allow_ProcessTrigger(const char* destbuf_path, const char* destbuf_caption, const char* destbuf_groupname, int pluginid,int thispluginid, int score, E_EntryTypeT entrytype, void* tagvoidp, BOOL *closeafterp)
{
	return FALSE;
}

PREFUNCDEF BOOL EFuncName_Do_AdjustResultScore(const char* itempath, int *scorep)
{
	return FALSE;
}
 */

int _EverythingSendQueryA(HWND hwnd, char *search_string, int flags)
{
	EVERYTHING_IPC_QUERY *query;
	int len, size;
	HWND everything_hwnd;
	COPYDATASTRUCT cds;
	
	everything_hwnd = FindWindow(EVERYTHING_IPC_WNDCLASS, 0);
	if (everything_hwnd)
	{
		len = (int)strlen(search_string);
		
		size = sizeof(EVERYTHING_IPC_QUERY) - sizeof(CHAR) + len*sizeof(CHAR) + sizeof(CHAR);
		
		query = (EVERYTHING_IPC_QUERY *)HeapAlloc(GetProcessHeap(),0,size);
		if (query)
		{
			query->max_results = 50;
			query->offset = 0;
			query->reply_copydata_message = COPYDATA_IPCTEST_QUERYCOMPLETEA;
			query->search_flags = flags;
			query->reply_hwnd = hwnd;
			CopyMemory(query->search_string,search_string,(len+1)*sizeof(CHAR));
		
			cds.cbData = size;
			cds.dwData = EVERYTHING_IPC_COPYDATAQUERY;
			cds.lpData = query;

			if (SendMessage(everything_hwnd,WM_COPYDATA,(WPARAM)hwnd,(LPARAM)&cds) == TRUE)
			{
				//debugmsg("Query sent.\n");
				HeapFree(GetProcessHeap(),0,query);
				return 1;
			}
			else
			{
				farrStatus("Everything IPC service not running!");
				isTutorial=1;
			}

			HeapFree(GetProcessHeap(),0,query);
		}
		else
		{
			farrStatus("Failed to allocate %d bytes for Everything IPC query.",size);
			return 0;
		}
	}
	else
	{
		farrStatus("Everything IPC service not found. Everything must be running.");
		isTutorial=1;
	}

	return 0;
}

int EverythingSendQuery(char *searchString)
{
	int ret, flags=0;
	MSG msg;
	
	timeout=30;
	gotData=0;
	if(searchString[0]=='$'&&strlen(searchString)>1)
	{
		flags|=EVERYTHING_IPC_REGEX;
		searchString++;
	}
	if(_EverythingSendQueryA(windowHandle, searchString, flags)==1)
	{
		//WaitMessage();
		while(1)
		{
			timeout--;
			if(timeout<=0)
			{
				farrStatus("Timed out while waiting for blob from Everything!");
				break;
			}
			
			WaitMessage();
			while(PeekMessage(&msg, NULL, 0, 0, 0)) 
			{
				ret = (int)GetMessage(&msg, 0, 0, 0);
				if (ret == -1 || !ret) break;
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			if(gotData) break;
		}
		return 1;
	}
	
	return 0;
}

void ExecuteCallback_SearchStateChanged()
{
	if (callbackfp_notifysearchstatechanged) callbackfp_notifysearchstatechanged(hostptr, nResults , currentState);
}

BOOL DoFarrSearchBegin(const char *search)
{
	if(strlen(search)>0)
	{
		farrStatus("Searching Everything for '%s'...", search);
		currentState=E_SearchState_Searching;
		isTutorial=0;
		if(EverythingSendQuery((char*)search))
		{
			if(eResultsN)
			{
				nResults=eResultsN;
				resultState=E_ResultAvailableState_ItemResults;
				currentState=E_SearchState_Stopped;
				farrStatus("Everything: %d results for '%s'.", nResults, search);
				ExecuteCallback_SearchStateChanged();
			}
			else
			{
				farrStatus("Everything: no results.");
			}
		}
		else if(isTutorial) sendTutorial();
	}
	/*
	else
	{
		callbackfp_set_strval(hostptr,"window.richeditmode",
			"Hello! This is Farr_TinyEV (" ThisPlugin_VersionString ") speaking.\r\n"
			"\r\n"
			"You can now type ev <something> to search for things in your Everything index.\r\n"
			"To search using regular expressions, try prefixing your query with a $: ev $some[twgy]ing.\r\n"
			"\r\n"
			"For some more customization, see the Readme file in the plugin's directory."
		);
		nResults=0;
	}*/
	return FALSE;
}
