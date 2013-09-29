// TestBuffer.cpp : 定义控制台应用程序的入口点。
//
#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "OPStream.hpp"


int op_gettimeofday(struct timeval *tp)
{
	SYSTEMTIME tm;
	int year, month;
    time_t days;
    static const int dayoffset[12] =
	{306, 337, 0, 31, 61, 92, 122, 153, 184, 214, 245, 275};

	GetSystemTime(&tm);

    year = tm.wYear - 1900;
	month = tm.wMonth - 1;
    if (year < 70 || ((sizeof(time_t) <= 4) && (year >= 138))) 
        return -1;

    /* shift new year to 1st March in order to make leap year calc easy */
    if (month < 2)
        year--;

    /* Find number of days since 1st March 1900 (in the Gregorian calendar). */
    days = year * 365 + year / 4 - year / 100 + (year / 100 + 3) / 4;
    days += dayoffset[month] + tm.wDay - 1;
    days -= 25508;              /* 1 jan 1970 is 25508 days since 1 mar 1900 */
    if (days < 0) 
        return -1;

    tp->tv_sec = ((days * 24 + tm.wHour) * 60 + tm.wMinute) * 60 + tm.wSecond;
	tp->tv_usec = tm.wMilliseconds * 1000;
	return 0;
};


struct OPValue {
    int time;
    short status;
    float value;
};


int main(int argc, char* argv[])
{
    Buffer b;
    OPStream<Buffer> io(b);
    struct timeval beg, mid, end;

    srand(time(0));
    int rv, i, num = 1000000;
    OPValue *v = new OPValue[num];
    for (i=0; i<num; i++) {
        v[i].time = i;
        v[i].status = 100;
        v[i].value = 100.0 * rand() / RAND_MAX;
    }
    printf("before value %d, %g\n", v[i-1].time, v[i-1].value);

    // pack
    op_gettimeofday(&beg);
    rv = 0;
    for (i=0; i<num && rv==0; i++) {
        rv = io.writeInt(v[i].time);
        rv+= io.writeShort(v[i].status);
        rv+= io.writeFloat(v[i].value);
    }
    rv+= io.flush(1);
    if (rv!=0) {
        printf("write error\n");
        return 0;
    }
    op_gettimeofday(&mid);

    // unpack
    for (i=0; i<num && rv==0; i++) {
        rv = io.readInt(&v[i].time);
        rv+= io.readShort(&v[i].status);
        rv+= io.readFloat(&v[i].value);
    }
    if (rv!=0) printf("read error\n");
    op_gettimeofday(&end);

    double m1 = 1000.0 * (mid.tv_sec - beg.tv_sec) + (mid.tv_usec - beg.tv_usec)/1000.0;
    double ms = 1000.0 * (end.tv_sec - beg.tv_sec) + (end.tv_usec - beg.tv_usec)/1000.0;
    printf("after value %d, %g\n", v[i-1].time, v[i-1].value);
    printf("total record %d\n", num);
    printf("time pack %g ms\n", m1);
    printf("time unpack %g ms\n", ms-m1);
    printf("time total %g ms\n", ms);

	return 0;
}

