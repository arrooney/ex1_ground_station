/* @(#) pfcustom.c 98/01/26 1.3 */

#ifndef PF_USER_CUSTOM

/***************************************************************
** Call Custom Functions for pForth
**
** Create a file similar to this and compile it into pForth
** by setting -DPF_USER_CUSTOM="mycustom.c"
**
** Using this, you could, for example, call X11 from Forth.
** See "pf_cglue.c" for more information.
**
** Author: Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** The pForth software code is dedicated to the public domain,
** and any third party may reproduce, distribute and modify
** the pForth software code or any derivative works thereof
** without any compensation or license.  The pForth software
** code is provided on an "as is" basis without any warranty
** of any kind, including, without limitation, the implied
** warranties of merchantability and fitness for a particular
** purpose and their equivalents under the laws of any jurisdiction.
**
***************************************************************/
#include "pf_all.h"
#include <stdlib.h>
#include "pfunix.h"
#include "pfdfgm.h"
#include "pfrings.h"
#include "pfgomshell.h"
#include "pfeps.h"

/****************************************************************
** Step 1: Put your own special glue routines here
**     or link them in from another file or library.
****************************************************************/

/****************************************************************
** Step 2: Create CustomFunctionTable.
**     Do not change the name of CustomFunctionTable!
**     It is used by the pForth kernel.
****************************************************************/
#ifdef PF_NO_GLOBAL_INIT
/******************
** If your loader does not support global initialization, then you
** must define PF_NO_GLOBAL_INIT and provide a function to fill
** the table. Some embedded system loaders require this!
** Do not change the name of LoadCustomFunctionTable()!
** It is called by the pForth kernel.
*/
#define NUM_CUSTOM_FUNCTIONS  (2)
CFunc0 CustomFunctionTable[NUM_CUSTOM_FUNCTIONS];

Err LoadCustomFunctionTable( void )
{
    CustomFunctionTable[0] = CTest0;
    CustomFunctionTable[1] = CTest1;
    return 0;
}

#else
/******************
** If your loader supports global initialization (most do.) then just
** create the table like this.
*/
CFunc0 CustomFunctionTable[] =
{
	(CFunc0) gomshellCommand,		/* GOM */
	(CFunc0) programExit,			/* FIN */
	(CFunc0) gomshellPrintErrorCodes,	/* GOM.HELP */
	(CFunc0) gomshellFtpDownload,		/* GOM.FTP.DOWNLOAD */
	(CFunc0) gomshellErrorOK,		/* GOM.ERR.OK */
	(CFunc0) gomshellErrorMem,		/* GOM.ERR.MEM */
	(CFunc0) gomshellErrorFTP,		/* GOM.ERR.FTP */
	(CFunc0) gomshellOCPCommand,		/* GOM.COMMAND */
	(CFunc0) gomshellErrorSyntax,		/* GOM.ERR.USER */
	(CFunc0) gomshellRingDownload,		/* GOM.RING.DOWNLOAD */
	(CFunc0) gomshellRingWod,		/* GOM.RING.WOD */
	(CFunc0) gomshellRingDFGMRaw,		/* GOM.RING.DFGM-RAW */
	(CFunc0) gomshellRingDFGMS0,		/* GOM.RING.DFGM-S0 */
	(CFunc0) gomshellRingDFGMS1,		/* GOM.RING.DFGM-S1 */
	(CFunc0) gomshellRingDFGMHK,		/* GOM.RING.DFGM-HK */
	(CFunc0) gomshellRingAthena,		/* GOM.RING.ATHENA */
	(CFunc0) gomshellFtpUpload,		/* GOM.FTP.UPLOAD */
	(CFunc0) gomshellErrorCom,		/* GOM.ERR.COM */
	(CFunc0) gomshellRingFetch,		/* GOM.RING.FETCH-ALL-TAILS */
	(CFunc0) gomshellFtpRemove,		/* GOM.FTP.REMOVE */
	(CFunc0) gomshellFetchRingByID,		/* GOM.RING.FETCH */
	(CFunc0) gomshellRingMove,		/* GOM.RING.MOVE */
	(CFunc0) processBlock,			/* BLOCK */
	(CFunc0) unixShellCommand,		/* SYSTEM */
	(CFunc0) processSleep,			/* SLEEP */
	(CFunc0) dfgm_mock_init,		/* DFGM-MOCK.INIT */
	(CFunc0) dfgm_mock_packet_value,	/* DFGM-MOCK.VAL */
	(CFunc0) incrementRingName,		/* NEXT-RING */
	(CFunc0) dfgm_mock_use_ring,		/* DFGM-MOCK.READ-RING */
	(CFunc0) dfgm_mock_min_max_toggle,	/* DFGM-MOCK.MIN-MAX */
	(CFunc0) gomshellPing,			/* PING */
	(CFunc0) colorType,			/* TYPE.COLOR */
	(CFunc0) timeStamp,			/* TYPE.TIME */
	(CFunc0) epsUpdateHK,			/* GOM.EPS.GET-HK */
	(CFunc0) epsIndexHK,			/* GOM.EPS.INDEX-HK */
	(CFunc0) obcBootState,			/* GOM.OBC.BOOT-STATE */
	(CFunc0) pushUnixTime,			/* TIME */
	(CFunc0) epsPrint			/* GOM.EPS.PRINT */
};
#endif

/****************************************************************
** Step 3: Add custom functions to the dictionary.
**     Do not change the name of CompileCustomFunctions!
**     It is called by the pForth kernel.
****************************************************************/

#if (!defined(PF_NO_INIT)) && (!defined(PF_NO_SHELL))
Err CompileCustomFunctions( void )
{
    Err err;
    int i = 0;
    
/* Compile Forth words that call your custom functions.
   Make sure order of functions matches that in LoadCustomFunctionTable().
   Parameters are: Name in UPPER CASE, Function, Index, Mode, NumParams
*/
    err = CreateGlueToC( "GOM", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "FIN", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.HELP", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.FTP.DOWNLOAD", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.ERR.OK", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.ERR.MEM", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.ERR.FTP", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.COMMAND", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.ERR.USER", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.DOWNLOAD", i++, C_RETURNS_VOID, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.WOD", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.DFGM-RAW", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.DFGM-S0", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.DFGM-S1", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.DFGM-HK", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.ATHENA", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.FTP.UPLOAD", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.ERR.COM", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.FETCH-ALL-TAILS", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.FTP.REMOVE", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.FETCH", i++, C_RETURNS_VOID, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.RING.MOVE", i++, C_RETURNS_VOID, 3 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "BLOCK", i++, C_RETURNS_VOID, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "SYSTEM", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "SLEEP", i++, C_RETURNS_VOID, 1 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "DFGM-MOCK.INIT", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "DFGM-MOCK.VAL", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "NEXT-RING", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "DFGM-MOCK.READ-RING", i++, C_RETURNS_VOID, 3 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "DFGM-MOCK.MIN-MAX", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.PING", i++, C_RETURNS_VOID, 3 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "TYPE.COLOR", i++, C_RETURNS_VOID, 3 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "TYPE.TIME", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.EPS.GET-HK", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.EPS.INDEX-HK", i++, C_RETURNS_VOID, 2 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.OBC.BOOT-STATE", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "TIME", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    err = CreateGlueToC( "GOM.EPS.PRINT", i++, C_RETURNS_VOID, 0 );
    if( err < 0 ) return err;
    return 0;
}
#else
Err CompileCustomFunctions( void ) { return 0; }
#endif

/****************************************************************
** Step 4: Recompile using compiler option PF_USER_CUSTOM
**         and link with your code.
**         Then rebuild the Forth using "pforth -i system.fth"
**         Test:   10 Ctest0 ( should print message then '11' )
****************************************************************/

#endif  /* PF_USER_CUSTOM */

