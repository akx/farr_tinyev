#ifndef PluginFuncs_FARRH
#define PluginFuncs_FARRH


#define DEF_FARRAPI_IDENTIFIER	"farrapi"

#define DEF_FieldName_AliasStr		"aliasstr"
#define DEF_FieldName_RegexStr		"regexstr"
#define DEF_FieldName_RegexFilterStr	"regexfilterstr"
#define DEF_FieldName_KeywordStr	"keywordstr"
#define DEF_FieldName_ScoreStr		"scorestr"
#define DEF_FieldName_DefaultSearchText	"defaultsearchtext"

#define DEF_FieldName_NotifySearchCallbackFp	"notifysearchcallback"
#define DEF_FieldName_EnterInRichEdit "enterinrichedit"

#define DEF_GRABRESULTS_MAXLEN	512

typedef enum {
	E_SearchState_Stopped=0,
	E_SearchState_Searching=1,
	E_SearchState_dummybig=79000
} E_SearchStateT ;

typedef enum {
	E_ResultAvailableState_None=0,
	E_ResultAvailableState_ItemResults=1,
	E_ResultAvailableState_DisplayText=2,
	E_ResultAvailableState_DisplayHtml=3,
	E_ResultAvailableState_Grid=4,
	E_ResultAvailableState_WinTakeover=5,
	E_ResultAvailableState_dummybig=79000
} E_ResultAvailableStateT ;

typedef enum {
	E_WantFeatures_searchinput_regex=0,
	E_SupportFeatures_searchinput_explicit=1,
	E_SupportFeatures_searchinput_all=2,
	E_SupportFeatures_tryhandle_trigger=3,
	E_SupportFeatures_addinfo_files=4,
	E_SupportFeatures_scoreall_files=5,
	E_SupportFeatures_scoregood_files=6,
	E_SupportFeatures_dominate_results=7,
	E_SupportFeatures_dummybig=79000
} E_WantFeaturesT ;

typedef enum {
	E_ResultPostProcessing_ImmediateDisplay = 0,
	E_ResultPostProcessing_AddScore = 1,
	E_ResultPostProcessing_MatchAgainstSearch = 2 ,
	E_ResultPostProcessing_AddScore_wPats =3 ,
	E_ResultPostProcessing_MatchAgainstSearch_wPats = 4,
	E_ResultPostProcessing_dummybig=79000
} E_ResultPostProcessingT ;

typedef enum {
	E_EntryType_UNKNOWN=0,
	E_EntryType_FILE=1,
	E_EntryType_FOLDER=2,
	E_EntryType_ALIAS=3,
	E_EntryType_URL=4,
	E_EntryType_PLUGIN=5,
	E_EntryType_CLIP=6,
	E_EntryType_ALIASFILE=7,
	E_EntryType_dummybig=79000
} E_EntryTypeT ;

typedef enum {
	E_TriggerMethodType_Normal=0,
	E_TriggerMethodType_PressEnter=1,
	E_TriggerMethodType_dummybig=79000
} E_TriggerMethodTypeT ;

typedef enum {
	E_AllowProcessTriggerMode_NonExplicit=0,
	E_AllowProcessTriggerMode_Explicit=1,
	E_AllowProcessTriggerMode_PrepareContextMenu=2,
	E_AllowProcessTriggerMode_ResultSelected=3,
	E_AllowProcessTriggerMode_Tabbed=4,
	E_AllowProcessTriggerMode_dummybig=79000
} E_AllowProcessTriggerModeT ;

#define EFuncName_Inform_SearchBegins PluginFunc_Inform_SearchBegins
typedef BOOL (*FpFunc_Inform_SearchBegins)(const char* searchstring_raw, const char *searchstring_lc_nokeywords, BOOL explicitinvocation);

#define EFuncName_Inform_SearchBeginsV2 PluginFunc_Inform_SearchBeginsV2
typedef BOOL (*FpFunc_Inform_SearchBeginsV2)(const char* searchstring_raw, const char *searchstring_lc_nokeywords, BOOL explicitinvocation, const char *modifierstring, int triggermethod);

#define EFuncName_Inform_RegexSearchMatch PluginFunc_Inform_RegexSearchMatch
typedef BOOL (*FpFunc_Inform_RegexSearchMatch)(const char* searchstring_raw, const char *searchstring_lc_nokeywords, int regexgroups, char** regexcharps);

#define EFuncName_Inform_RegexSearchMatchV2 PluginFunc_Inform_RegexSearchMatchV2
typedef BOOL (*FpFunc_Inform_RegexSearchMatchV2)(const char* searchstring_raw, const char *searchstring_lc_nokeywords, int regexgroups, char** regexcharps, const char *modifierstring, int triggermethod);

#define EFuncName_Inform_SearchEnds PluginFunc_Inform_SearchEnds
typedef void (*FpFunc_Inform_SearchEnds)();

#define EFuncName_Inform_WindowIsHiding PluginFunc_Inform_WindowIsHiding
typedef void (*FpFunc_Inform_WindowIsHiding)(HWND hwndp);

#define EFuncName_Inform_WindowIsUnHiding PluginFunc_Inform_WindowIsUnHiding
typedef void (*FpFunc_Inform_WindowIsUnHiding)(HWND hwndp);

#define EFuncName_Ask_WantFeature PluginFunc_Ask_WantFeature
typedef BOOL (*FpFunc_Ask_WantFeature)(E_WantFeaturesT featureid);

#define EFuncName_Ask_SearchState PluginFunc_Ask_SearchState
typedef E_SearchStateT (*FpFunc_Ask_SearchState)();

#define EFuncName_Ask_IsAnyResultAvailable PluginFunc_Ask_IsAnyResultAvailable
typedef E_ResultAvailableStateT (*FpFunc_Ask_IsAnyResultAvailable)();

#define EFuncName_Ask_HowManyItemResultsAreReady PluginFunc_Ask_HowManyItemResultsAreReady
typedef int (*FpFunc_Ask_HowManyItemResultsAreReady)();

#define EFuncName_Request_LockResults PluginFunc_Request_LockResults
typedef BOOL (*FpFunc_Request_LockResults)(BOOL dolock);

#define EFuncName_Request_ItemResultByIndex PluginFunc_Request_ItemResultByIndex
typedef BOOL (*FpFunc_Request_ItemResultByIndex)(int resultindex, char *destbuf_path, char *destbuf_caption, char *destbuf_groupname, char *destbuf_iconfilename, void** tagvoidpp, int maxlen, E_ResultPostProcessingT *resultpostprocmodep, int *scorep, E_EntryTypeT *entrytypep);

#define EFuncName_Request_TextResultCharp PluginFunc_Request_TextResultCharp
typedef BOOL (*FpFunc_Request_TextResultCharp)(char **charp);

#define EFuncName_Allow_ProcessTrigger PluginFunc_Allow_ProcessTrigger
typedef BOOL (*FpFunc_Allow_ProcessTrigger)(const char* destbuf_path, const char* destbuf_caption, const char* destbuf_groupname, int pluginid,int thispluginid, int score, E_EntryTypeT entrytype, void* tagvoidp, BOOL *closeafterp);

#define EFuncName_Allow_ProcessTriggerV2 PluginFunc_Allow_ProcessTriggerV2
typedef BOOL (*FpFunc_Allow_ProcessTriggerV2)(const char* destbuf_path, const char* destbuf_caption, const char* destbuf_groupname, int pluginid,int thispluginid, int score, E_EntryTypeT entrytype, void* tagvoidp, BOOL *closeafterp, int triggermode);

#define EFuncName_Do_AdjustResultScore PluginFunc_Do_AdjustResultScore
typedef BOOL (*FpFunc_Do_AdjustResultScore)(const char* itempath, int *scorep);

#define EFuncName_Do_AddInfo PluginFunc_Do_AddInfo
typedef BOOL (*FpFunc_Do_AddInfo)(const char* itempath);

// Callbacks

#define EFuncName_GlobalPluginCallback_NotifySearchStateChanged	PluginHostFunc_GlobalPluginCallback_NotifySearchStateChanged
typedef void (*Fp_GlobalPluginCallback_NotifySearchStateChanged)(void *JrPluginPointerp, int newresultcount,E_SearchStateT newstate);
PREFUNCDEFCB void EFuncName_GlobalPluginCallback_NotifySearchStateChanged(void *JrPluginPointerp, int newresultcount,E_SearchStateT newstate);

#endif
