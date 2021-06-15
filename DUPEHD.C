/*
 *
 * dupehd.c
 * Thursday, 10/6/1994.
 * Craig
 */

#define INCL_DOSDEVICES
#include <os2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define BUFFSIZE  32768U

char szBuff [BUFFSIZE];



int main (int argc, char *argv[])
   {
   char   szSourceDir[256], szDestDir[256];
   ULONG  ulTotalRead, ulTotalWrite, ulZero;
   USHORT uReadSize, uWriteSize, Zero;
   USHORT uAction, uRet, i;
   HFILE  hfSource, hfDest;

   if (argc != 3)
      return printf ("USAGE: DUPEHD sourcedir destdir");

   strcpy (szSourceDir, argv[1]);
   strcpy (szDestDir, argv[2]);

   /*--- disk opens ---*/
   uRet = DosOpen (szSourceDir, 
                   &hfSource, 
                   &uAction, 
                   0L, 
                   0, 
                   FILE_OPEN,
                   OPEN_ACCESS_READONLY | OPEN_FLAGS_DASD | OPEN_SHARE_DENYWRITE,
                   0);
   if (uRet)
      return printf ("Open Source Error %d.", uRet);

   uRet = DosOpen (szDestDir, 
                   &hfDest, 
                   &uAction, 
                   0L, 
                   0, 
                   FILE_OPEN,
                   OPEN_SHARE_DENYREADWRITE |
                   OPEN_ACCESS_READWRITE    |
                   OPEN_FLAGS_DASD,           
                   0);
   if (uRet)
      return printf ("Open Dest Error %d.", uRet);

   /*----- disk seeks ---*/
   ulZero = 0L;
   if (uRet = DosChgFilePtr (hfSource, 0, FILE_BEGIN, &ulZero))
      return printf ("source seek error %d.", uRet);

   if (uRet = DosChgFilePtr (hfDest,   0, FILE_BEGIN, &ulZero))
      return printf ("Dest seek error %d.", uRet);

   uReadSize = uWriteSize = 1;
   ulTotalRead = ulTotalWrite = 0L;       

   /*--- Lock the drive ---*/
   DosDevIOCtl (0L, &Zero, 0, 8, hfDest);

   /*--- do copy ---*/
   for (i=0; uReadSize && uWriteSize; i++)
      {
      if (uRet = DosRead  (hfSource, szBuff, BUFFSIZE, &uReadSize))
         printf ("Read Error %d.\n", uRet);
      ulTotalRead += uReadSize;

      if (uRet = DosWrite (hfDest, szBuff, uReadSize, &uWriteSize))
         printf ("Write Error %d.\n", uRet);
      ulTotalWrite += uWriteSize;
      if (!(i % 32))
         printf (".");
      }
   printf ("\n");

   /*--- UnLock the drive ---*/
   DosDevIOCtl (0L, &Zero, 1, 8, hfDest);

   DosClose (hfSource);
   DosClose (hfDest);

   printf ("Total Bytes Read   : %lu\n", ulTotalRead);
   printf ("Total Bytes Written: %lu\n", ulTotalWrite);
  
   return 0;
   }

