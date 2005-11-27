/*
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h> /* gettimeofday */
#include <dirent.h>
#include <unistd.h>
#include <time.h>

#include <windows.h>

// #define TRACEN(u) u;
#define TRACEN(u)  /* */

typedef LONG NTSTATUS;
#define STATUS_SUCCESS                   0x00000000

#define TICKSPERSEC        10000000
#define TICKSPERMSEC       10000
#define SECSPERDAY         86400
#define SECSPERHOUR        3600
#define SECSPERMIN         60
#define MINSPERHOUR        60
#define HOURSPERDAY        24
#define EPOCHWEEKDAY       1  /* Jan 1, 1601 was Monday */
#define DAYSPERWEEK        7
#define EPOCHYEAR          1601
#define DAYSPERNORMALYEAR  365
#define DAYSPERLEAPYEAR    366
#define MONSPERYEAR        12
#define DAYSPERQUADRICENTENNIUM (365 * 400 + 97)
#define DAYSPERNORMALCENTURY (365 * 100 + 24)
#define DAYSPERNORMALQUADRENNIUM (365 * 4 + 1)

/* 1601 to 1970 is 369 years plus 89 leap days */
#define SECS_1601_TO_1970  ((369 * 365 + 89) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1970 (SECS_1601_TO_1970 * TICKSPERSEC)
/* 1601 to 1980 is 379 years plus 91 leap days */
#define SECS_1601_TO_1980  ((379 * 365 + 91) * (ULONGLONG)SECSPERDAY)
#define TICKS_1601_TO_1980 (SECS_1601_TO_1980 * TICKSPERSEC)
typedef short CSHORT;
static inline void NormalizeTimeFields(CSHORT *FieldToNormalize, CSHORT *CarryField,int Modulus) {
  *FieldToNormalize = (CSHORT) (*FieldToNormalize - Modulus);
  *CarryField = (CSHORT) (*CarryField + 1);
}

static int TIME_GetBias(time_t utc, int *pdaylight) {
  struct tm *ptm;
  static time_t last_utc=0; // FIXED
  static int last_bias;
  static int last_daylight;
  int ret;

  if(utc == last_utc) {
    *pdaylight = last_daylight;
    ret = last_bias;
  } else {
    ptm = localtime(&utc);
    *pdaylight = last_daylight = ptm->tm_isdst; /* daylight for local timezone */
    ptm = gmtime(&utc);
    ptm->tm_isdst = *pdaylight; /* use local daylight, not that of Greenwich */
    last_utc = utc;
    ret = last_bias = (int)(utc-mktime(ptm));
  }
  return ret;
  *pdaylight = 0;
  return 0;
}


static void RtlSystemTimeToLocalTime( const LARGE_INTEGER *SystemTime,
                                      LARGE_INTEGER *LocalTime ) {
  int bias, daylight;
  time_t gmt = time(NULL);
  bias = TIME_GetBias(gmt, &daylight);
  LocalTime->QuadPart = SystemTime->QuadPart + bias * (LONGLONG)10000000;
}

void WINAPI RtlSecondsSince1970ToFileTime( DWORD Seconds, LPFILETIME ft ) {
  ULONGLONG secs = Seconds * (ULONGLONG)TICKSPERSEC + TICKS_1601_TO_1970;
  ft->dwLowDateTime  = (DWORD)secs;
  ft->dwHighDateTime = (DWORD)(secs >> 32);
  TRACEN((printf("RtlSecondsSince1970ToFileTime %lx => %lx %lx\n",(long)Seconds,(long)ft->dwHighDateTime,(long)ft->dwLowDateTime)))
}


BOOL WINAPI DosDateTimeToFileTime( WORD fatdate, WORD fattime, LPFILETIME ft) {
  struct tm newtm;
#ifndef HAVE_TIMEGM

  struct tm *gtm;
  time_t time1, time2;
#endif

  TRACEN((printf("DosDateTimeToFileTime\n")))
  newtm.tm_sec  = (fattime & 0x1f) * 2;
  newtm.tm_min  = (fattime >> 5) & 0x3f;
  newtm.tm_hour = (fattime >> 11);
  newtm.tm_mday = (fatdate & 0x1f);
  newtm.tm_mon  = ((fatdate >> 5) & 0x0f) - 1;
  newtm.tm_year = (fatdate >> 9) + 80;
#ifdef HAVE_TIMEGM

  TRACEN((printf("DosDateTimeToFileTime-1\n")))
  RtlSecondsSince1970ToFileTime( timegm(&newtm), ft );
#else

  TRACEN((printf("DosDateTimeToFileTime-2\n")))
  time1 = mktime(&newtm);
  gtm = gmtime(&time1);
  time2 = mktime(gtm);
  RtlSecondsSince1970ToFileTime( 2*time1-time2, ft );
#endif

  return TRUE;
}

/* FIXME : Should it be signed division instead? */
static ULONGLONG WINAPI RtlLargeIntegerDivide( ULONGLONG a, ULONGLONG b, ULONGLONG *rem ) {
  ULONGLONG ret = a / b;
  if (rem)
    *rem = a - ret * b;
  return ret;
}


BOOLEAN WINAPI RtlTimeToSecondsSince1970( const LARGE_INTEGER *Time, DWORD *Seconds ) {
  ULONGLONG tmp = Time->QuadPart;
  TRACEN((printf("RtlTimeToSecondsSince1970-1 %llx\n",tmp)))
  tmp = RtlLargeIntegerDivide( tmp, 10000000, NULL );
  tmp -= SECS_1601_TO_1970;
  TRACEN((printf("RtlTimeToSecondsSince1970-2 %llx\n",tmp)))
  if (tmp > 0xffffffff)
    return FALSE;
  *Seconds = (DWORD)tmp;
  return TRUE;
}

BOOL WINAPI FileTimeToDosDateTime( const FILETIME *ft, WORD *fatdate, WORD *fattime ) {
  LARGE_INTEGER       li;
  ULONG               t;
  time_t              unixtime;
  struct tm*          tm;

  TRACEN((printf("FileTimeToDosDateTime\n")))
  li.QuadPart = ft->dwHighDateTime;
  li.QuadPart = (li.QuadPart << 32) | ft->dwLowDateTime;
  RtlTimeToSecondsSince1970( &li, &t );
  unixtime = t;
  tm = gmtime( &unixtime );
  if (fattime)
    *fattime = (tm->tm_hour << 11) + (tm->tm_min << 5) + (tm->tm_sec / 2);
  if (fatdate)
    *fatdate = ((tm->tm_year - 80) << 9) + ((tm->tm_mon + 1) << 5) + tm->tm_mday;
  return TRUE;
}

BOOL WINAPI FileTimeToLocalFileTime( const FILETIME *utcft, LPFILETIME localft ) {
  LARGE_INTEGER local, utc;

  TRACEN((printf("FileTimeToLocalFileTime\n")))
  utc.QuadPart = utcft->dwHighDateTime;
  utc.QuadPart = (utc.QuadPart << 32) | utcft->dwLowDateTime;
  RtlSystemTimeToLocalTime( &utc, &local );
  localft->dwLowDateTime = (DWORD)local.QuadPart;
  localft->dwHighDateTime = (DWORD)(local.QuadPart >> 32);

  return TRUE;
}


typedef short CSHORT;
typedef struct _TIME_FIELDS {
  CSHORT Year;
  CSHORT Month;
  CSHORT Day;
  CSHORT Hour;
  CSHORT Minute;
  CSHORT Second;
  CSHORT Milliseconds;
  CSHORT Weekday;
}
TIME_FIELDS, *PTIME_FIELDS;

static const int MonthLengths[2][MONSPERYEAR] = {
      {
        31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
      },
      { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
    };

static inline int IsLeapYear(int Year) {
  return Year % 4 == 0 && (Year % 100 != 0 || Year % 400 == 0) ? 1 : 0;
}


static VOID WINAPI RtlTimeToTimeFields(
  const LARGE_INTEGER *liTime,
  PTIME_FIELDS TimeFields) {
  const int *Months;
  int SecondsInDay, DeltaYear;
  int LeapYear, CurMonth;
  long int Days;
  LONGLONG Time = liTime->QuadPart;

  /* Extract millisecond from time and convert time into seconds */
  TimeFields->Milliseconds = (CSHORT) ((Time % TICKSPERSEC) / TICKSPERMSEC);
  Time = Time / TICKSPERSEC;

  /* The native version of RtlTimeToTimeFields does not take leap seconds
   * into account */

  /* Split the time into days and seconds within the day */
  Days = Time / SECSPERDAY;
  SecondsInDay = Time % SECSPERDAY;

  /* compute time of day */
  TimeFields->Hour = (CSHORT) (SecondsInDay / SECSPERHOUR);
  SecondsInDay = SecondsInDay % SECSPERHOUR;
  TimeFields->Minute = (CSHORT) (SecondsInDay / SECSPERMIN);
  TimeFields->Second = (CSHORT) (SecondsInDay % SECSPERMIN);

  /* compute day of week */
  TimeFields->Weekday = (CSHORT) ((EPOCHWEEKDAY + Days) % DAYSPERWEEK);

  /* compute year */
  /* FIXME: handle calendar modifications */
  TimeFields->Year = EPOCHYEAR;
  DeltaYear = Days / DAYSPERQUADRICENTENNIUM;
  TimeFields->Year += DeltaYear * 400;
  Days -= DeltaYear * DAYSPERQUADRICENTENNIUM;
  DeltaYear = Days / DAYSPERNORMALCENTURY;
  TimeFields->Year += DeltaYear * 100;
  Days -= DeltaYear * DAYSPERNORMALCENTURY;
  DeltaYear = Days / DAYSPERNORMALQUADRENNIUM;
  TimeFields->Year += DeltaYear * 4;
  Days -= DeltaYear * DAYSPERNORMALQUADRENNIUM;
  DeltaYear = Days / DAYSPERNORMALYEAR;
  TimeFields->Year += DeltaYear;
  Days -= DeltaYear * DAYSPERNORMALYEAR;

  LeapYear = IsLeapYear(TimeFields->Year);

  /* Compute month of year */
  Months = MonthLengths[LeapYear];
  for (CurMonth = 0; Days >= (long) Months[CurMonth]; CurMonth++)
    Days = Days - (long) Months[CurMonth];
  TimeFields->Month = (CSHORT) (CurMonth + 1);
  TimeFields->Day = (CSHORT) (Days + 1);
}


BOOL WINAPI FileTimeToSystemTime( const FILETIME *ft, LPSYSTEMTIME syst ) {
  TIME_FIELDS tf;
  LARGE_INTEGER t;

  TRACEN((printf("FileTimeToSystemTime\n")))
  t.QuadPart = ft->dwHighDateTime;
  t.QuadPart = (t.QuadPart << 32) | ft->dwLowDateTime;
  RtlTimeToTimeFields(&t, &tf);

  syst->wYear = tf.Year;
  syst->wMonth = tf.Month;
  syst->wDay = tf.Day;
  syst->wHour = tf.Hour;
  syst->wMinute = tf.Minute;
  syst->wSecond = tf.Second;
  syst->wMilliseconds = tf.Milliseconds;
  syst->wDayOfWeek = tf.Weekday;
  return TRUE;
}


static void WINAPI RtlLocalTimeToSystemTime( const LARGE_INTEGER *LocalTime,
    LARGE_INTEGER *SystemTime) {
  time_t gmt;
  int bias, daylight;

  TRACEN((printf("RtlLocalTimeToSystemTime\n")))
  gmt = time(NULL);
  bias = TIME_GetBias(gmt, &daylight);

  SystemTime->QuadPart = LocalTime->QuadPart - bias * (LONGLONG)10000000;
}

BOOL WINAPI LocalFileTimeToFileTime( const FILETIME *localft, LPFILETIME utcft ) {
  LARGE_INTEGER local, utc;

  TRACEN((printf("LocalFileTimeToFileTime\n")))
  local.QuadPart = localft->dwHighDateTime;
  local.QuadPart = (local.QuadPart << 32) | localft->dwLowDateTime;
  RtlLocalTimeToSystemTime( &local, &utc );
  utcft->dwLowDateTime = (DWORD)utc.QuadPart;
  utcft->dwHighDateTime = (DWORD)(utc.QuadPart >> 32);

  return TRUE;
}

/***********************************************************************
 *       NtQuerySystemTime [NTDLL.@]
 *       ZwQuerySystemTime [NTDLL.@]
 *
 * Get the current system time.
 *
 * PARAMS
 *   Time [O] Destination for the current system time.
 *
 * RETURNS
 *   Success: STATUS_SUCCESS.
 *   Failure: An NTSTATUS error code indicating the problem.
 */
NTSTATUS WINAPI NtQuerySystemTime( LARGE_INTEGER *Time ) {
  struct timeval now;

  TRACEN((printf("NtQuerySystemTime\n")))
  gettimeofday( &now, 0 );
  Time->QuadPart = now.tv_sec * (ULONGLONG)TICKSPERSEC + TICKS_1601_TO_1970;
  Time->QuadPart += now.tv_usec * 10;
  return STATUS_SUCCESS;
}

/*********************************************************************
 *      GetSystemTime                                   (KERNEL32.@)
 *
 * Get the current system time.
 *
 * RETURNS
 *  Nothing.
 */
VOID WINAPI GetSystemTime(LPSYSTEMTIME systime) /* [O] Destination for current time */
{
  FILETIME ft;
  LARGE_INTEGER t;

  TRACEN((printf("GetSystemTime\n")))
  NtQuerySystemTime(&t);
  ft.dwLowDateTime = (DWORD)(t.QuadPart);
  ft.dwHighDateTime = (DWORD)(t.QuadPart >> 32);
  FileTimeToSystemTime(&ft, systime);
}

/******************************************************************************
 *       RtlTimeFieldsToTime [NTDLL.@]
 *
 * Convert a TIME_FIELDS structure into a time.
 *
 * PARAMS
 *   ftTimeFields [I] TIME_FIELDS structure to convert.
 *   Time         [O] Destination for the converted time.
 *
 * RETURNS
 *   Success: TRUE.
 *   Failure: FALSE.
 */
BOOLEAN WINAPI RtlTimeFieldsToTime(
  PTIME_FIELDS tfTimeFields,
  LARGE_INTEGER *Time) {
  int CurYear, CurMonth, DeltaYear;
  LONGLONG rcTime;
  TIME_FIELDS TimeFields = *tfTimeFields;

  TRACEN((printf("RtlTimeFieldsToTime\n")))

  rcTime = 0;

  /* FIXME: normalize the TIME_FIELDS structure here */
  while (TimeFields.Second >= SECSPERMIN) {
    NormalizeTimeFields(&TimeFields.Second, &TimeFields.Minute, SECSPERMIN);
  }
  while (TimeFields.Minute >= MINSPERHOUR) {
    NormalizeTimeFields(&TimeFields.Minute, &TimeFields.Hour, MINSPERHOUR);
  }
  while (TimeFields.Hour >= HOURSPERDAY) {
    NormalizeTimeFields(&TimeFields.Hour, &TimeFields.Day, HOURSPERDAY);
  }
  while (TimeFields.Day > MonthLengths[IsLeapYear(TimeFields.Year)][TimeFields.Month - 1]) {
    NormalizeTimeFields(&TimeFields.Day, &TimeFields.Month, SECSPERMIN);
  }
  while (TimeFields.Month > MONSPERYEAR) {
    NormalizeTimeFields(&TimeFields.Month, &TimeFields.Year, MONSPERYEAR);
  }

  /* FIXME: handle calendar corrections here */
  CurYear = TimeFields.Year - EPOCHYEAR;
  DeltaYear = CurYear / 400;
  CurYear -= DeltaYear * 400;
  rcTime += DeltaYear * DAYSPERQUADRICENTENNIUM;
  DeltaYear = CurYear / 100;
  CurYear -= DeltaYear * 100;
  rcTime += DeltaYear * DAYSPERNORMALCENTURY;
  DeltaYear = CurYear / 4;
  CurYear -= DeltaYear * 4;
  rcTime += DeltaYear * DAYSPERNORMALQUADRENNIUM;
  rcTime += CurYear * DAYSPERNORMALYEAR;

  for (CurMonth = 1; CurMonth < TimeFields.Month; CurMonth++) {
    rcTime += MonthLengths[IsLeapYear(CurYear)][CurMonth - 1];
  }
  rcTime += TimeFields.Day - 1;
  rcTime *= SECSPERDAY;
  rcTime += TimeFields.Hour * SECSPERHOUR + TimeFields.Minute * SECSPERMIN + TimeFields.Second;
  rcTime *= TICKSPERSEC;
  rcTime += TimeFields.Milliseconds * TICKSPERMSEC;
  Time->QuadPart = rcTime;

  return TRUE;
}

/*********************************************************************
 *      SystemTimeToFileTime                            (KERNEL32.@)
 */
BOOL WINAPI SystemTimeToFileTime( const SYSTEMTIME *syst, LPFILETIME ft ) {
  TIME_FIELDS tf;
  LARGE_INTEGER t;

  TRACEN((printf("SystemTimeToFileTime\n")))

  tf.Year = syst->wYear;
  tf.Month = syst->wMonth;
  tf.Day = syst->wDay;
  tf.Hour = syst->wHour;
  tf.Minute = syst->wMinute;
  tf.Second = syst->wSecond;
  tf.Milliseconds = syst->wMilliseconds;

  RtlTimeFieldsToTime(&tf, &t);
  ft->dwLowDateTime = (DWORD)t.QuadPart;
  ft->dwHighDateTime = (DWORD)(t.QuadPart>>32);
  return TRUE;
}

