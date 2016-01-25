/* vim:set ft=cpp ts=4 sts=4 sw=4 sta noet ai si cin bs=2: */
/**
 * \file
 * \author TaeDong Kim <fly1004@gmail.com>
 * \date 20160125_125058~
 * \copyright Copyright (C) 2016- KTD, Inc. All rights reserved.
 **/
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>


static void InitSysTime(void)
{
	struct timeval tv;
	memset(&tv, 0, sizeof(tv));
	settimeofday(&tv, NULL);
}

static void RTCWrite(time_t t)
{
	struct timeval tv;
	memset(&tv, 0, sizeof(tv));
	tv.tv_sec = t;
	settimeofday(&tv, NULL);
	system("hwclock -w");
}

static time_t RTCRead(void)
{
	struct timeval tv;
	memset(&tv, 0, sizeof(tv));
	system("hwclock -s");
	gettimeofday(&tv, NULL);
	return tv.tv_sec;
}


const char* Time2Str(time_t t, char* buf, int len)
{
	struct tm tm;
	gmtime_r(&t, &tm); 

	snprintf(buf, len, "%04d-%02d-%02d %02d:%02d:%02d",
			tm.tm_year + 1900,
			tm.tm_mon + 1,
			tm.tm_mday,
			tm.tm_hour,
			tm.tm_min,
			tm.tm_sec);
	return buf;
}


static void WriteReadTester(void)
{
	enum {BUF_LEN = 1024};
	char buf[BUF_LEN];
	char buf1[BUF_LEN];
	for (unsigned int i=0; i<INT_MAX;) {
		RTCWrite(i);
		InitSysTime();
		time_t nt = RTCRead();

		if ((nt - i) < 2) {
			printf("%s - %s[%2d] : OK\n", Time2Str(i, buf, BUF_LEN), Time2Str(nt, buf1, BUF_LEN), (int)(nt - i));
		} else {
			printf("%s - %s[%2d] : FAIL\n", Time2Str(i, buf, BUF_LEN), Time2Str(nt, buf1, BUF_LEN), (int)(nt - i));
		}

		i += rand() % (60*60*24*30);
	}
}


static void WriteSleepReadTester(void)
{
	enum {BUF_LEN = 1024};
	char buf[BUF_LEN];
	char buf1[BUF_LEN];
	for (unsigned int i=0; i<INT_MAX;) {
		RTCWrite(i);

		const int delay = rand() % 60;
		sleep(delay);

		InitSysTime();
		time_t nt = RTCRead();

		if ((nt - i) < (delay + 3)) {
			printf("%s - %s[%2d,%2d] : OK\n", Time2Str(i, buf, BUF_LEN), Time2Str(nt, buf1, BUF_LEN), delay, (int)(nt - i));
		} else {
			printf("%s - %s[%2d,%2d] : FAIL\n", Time2Str(i, buf, BUF_LEN), Time2Str(nt, buf1, BUF_LEN), delay, (int)(nt - i));
		}

		i += rand() % (60*60*24*30*12);
	}
}


static void WriteSleepReadTester1(void)
{
	time_t START = 31536000 - 30;
	time_t INTERVAL = 31536000;

	enum {BUF_LEN = 1024};
	char buf[BUF_LEN];
	char buf1[BUF_LEN];
	for (unsigned int i=START; i<INT_MAX;) {
		RTCWrite(i);

		const int delay = 60;
		sleep(delay);

		InitSysTime();
		time_t nt = RTCRead();

		if ((nt - i) < (delay + 3)) {
			printf("%s - %s[%2d,%2d] : OK\n", Time2Str(i, buf, BUF_LEN), Time2Str(nt, buf1, BUF_LEN), delay, (int)(nt - i));
		} else {
			printf("%s - %s[%2d,%2d] : FAIL\n", Time2Str(i, buf, BUF_LEN), Time2Str(nt, buf1, BUF_LEN), delay, (int)(nt - i));
		}

		i += INTERVAL;
	}
}

int main(int /* argc */, char** /* argv */)
{
	srand(time(NULL));

	WriteReadTester();
	WriteSleepReadTester();
	WriteSleepReadTester1();
	return 0;
}
