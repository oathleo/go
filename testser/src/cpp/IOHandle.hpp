#ifndef	__IOHandle_HPP
#define	__IOHandle_HPP

#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif
#include <errno.h>

// Machine Byte Order
#undef _M_BIG_ENDIAN
#undef _M_LITTLE_ENDIAN

#if defined(__sparc)
	#define _M_BIG_ENDIAN
#elif defined(_ARCH_PPC)
	#define _M_BIG_ENDIAN
#elif defined(__ia64) && defined(__hpux)
	#define _M_BIG_ENDIAN
#elif defined(_M_IX86) || defined(_M_X64)
	#define _M_LITTLE_ENDIAN
#elif defined(__i386) || defined(__x86_64)
	#define _M_LITTLE_ENDIAN
#elif defined(__mips) || defined(__arm__)
	#define _M_LITTLE_ENDIAN
#else
#  error "Unknown platform - please report to magustek dev team"
#endif

// little endian
inline int get_short_le(const char* buf)
{
	return (buf[0]&0xff) + (buf[1]<<8);
}

inline int get_ushort_le(const char* buf)
{
	return (buf[0]&0xff) + ((buf[1]&0xff)<<8);
}

inline int get_int_le(const char* buf)
{
#ifdef _M_BIG_ENDIAN
    return (buf[0]&0xff) + ((buf[1]&0xff)<<8) + ((buf[2]&0xff)<<16) + ((buf[3]&0xff)<<24);
#else
    int i;
    memcpy(&i, buf, sizeof(int));
    return i;
#endif
}

inline float get_float_le(const char* buf) 
{
    float f;
#ifdef _M_BIG_ENDIAN
	char *p = (char *)&f;
	p[0] = buf[3];
	p[1] = buf[2];
	p[2] = buf[1];
	p[3] = buf[0];
#else
	memcpy(&f, buf, sizeof(float));
#endif
	return f;
}

// big endian
inline int get_short(const char* buf)
{
	return (buf[0]<<8) + (buf[1]&0xff);
}

inline int get_ushort(const char* buf)
{
	return ((buf[0]&0xff)<<8) + (buf[1]&0xff);
}

inline int get_int3(const char* buf)
{
	return ((buf[0]&0xff)<<16) + ((buf[1]&0xff)<<8) + (buf[2]&0xff);
}

inline int get_int(const char* buf)
{
#ifdef _M_BIG_ENDIAN
    int i;
    memcpy(&i, buf, sizeof(int));
    return i;
#else
    return ((buf[0]&0xff)<<24) + ((buf[1]&0xff)<<16) + ((buf[2]&0xff)<<8) + (buf[3]&0xff);
#endif
}
inline unsigned int get_uint(const char* buf)
{
#ifdef _M_BIG_ENDIAN
    unsigned int i;
    memcpy(&i, buf, sizeof(unsigned int));
    return i;
#else
    return (unsigned int)((buf[0]&0xff)<<24) + ((buf[1]&0xff)<<16) + ((buf[2]&0xff)<<8) + (buf[3]&0xff);
#endif
}

inline long long get_long(const char* A)
{
#ifdef _M_BIG_ENDIAN
    long long i;
    memcpy(&i, A, sizeof(long long));
    return i;
#else
    return ((unsigned long long)(((unsigned int) ((unsigned char)(A)[7])) +\
		(((unsigned int) ((unsigned char) (A)[6])) << 8) +\
		(((unsigned int) ((unsigned char) (A)[5])) << 16) +\
		(((unsigned int) ((unsigned char) (A)[4])) << 24)) +\
		(((unsigned long long)(((unsigned int)((unsigned char)(A)[3])) +\
		(((unsigned int) ((unsigned char) (A)[2])) << 8) +\
		(((unsigned int) ((unsigned char) (A)[1])) << 16) +\
		(((unsigned int) ((unsigned char) (A)[0])) << 24))) <<\
		32));
#endif
}

inline float get_float(const char* buf) 
{
    float f;
#ifdef _M_BIG_ENDIAN
    memcpy(&f, buf, sizeof(float));
#else
	char *p = (char *)&f;
	p[0] = buf[3];
	p[1] = buf[2];
	p[2] = buf[1];
	p[3] = buf[0];
#endif
	return f;
}

inline double get_double(const char *buf)
{
	double f;
#ifdef _M_BIG_ENDIAN
    memcpy(&f, buf, sizeof(double));
#else
	char *p = (char *)&f;
	p[0] = buf[7];
	p[1] = buf[6];
	p[2] = buf[5];
	p[3] = buf[4];
	p[4] = buf[3];
	p[5] = buf[2];
	p[6] = buf[1];
	p[7] = buf[0];
#endif
	return f;
}

inline void put_short(char *buf, int i)
{
	buf[0] = (char)(i >> 8);
	buf[1] = (char)i;
}

inline void put_int3(char *buf, int i)
{
	buf[0] = (char)(i >> 16);
	buf[1] = (char)(i >> 8);
	buf[2] = (char)i;
}

inline void put_int(char *buf, int i)
{
#ifdef _M_BIG_ENDIAN
    memcpy(buf, &i, sizeof(int));
#else
	buf[0] = (char)(i >> 24);
	buf[1] = (char)(i >> 16);
	buf[2] = (char)(i >> 8);
	buf[3] = (char)i;
#endif
}

inline void put_long(char *buf, long long i)
{
#ifdef _M_BIG_ENDIAN
    memcpy(buf, &i, sizeof(long long));
#else
	buf[0] = (char)(i >> 56);
	buf[1] = (char)(i >> 48);
	buf[2] = (char)(i >> 40);
	buf[3] = (char)(i >> 32);
	buf[4] = (char)(i >> 24);
	buf[5] = (char)(i >> 16);
	buf[6] = (char)(i >> 8);
	buf[7] = (char)i;
#endif
}

inline void put_float(char *buf, float f)
{
#ifdef _M_BIG_ENDIAN
	memcpy(buf, (char *)&f, sizeof(float));
#else
	char *p = (char *)&f;
	buf[0] = p[3];
	buf[1] = p[2];
	buf[2] = p[1];
	buf[3] = p[0];
#endif
}

inline void put_double(char *buf, double f)
{
#ifdef _M_BIG_ENDIAN
	memcpy(buf, (char *)&f, sizeof(double));
#else
	char *p = (char *)&f;
	buf[0] = p[7];
	buf[1] = p[6];
	buf[2] = p[5];
	buf[3] = p[4];
	buf[4] = p[3];
	buf[5] = p[2];
	buf[6] = p[1];
	buf[7] = p[0];
#endif
}

inline int read_n(int fd, char *buf, int len)
{
	int rv, off=0;
	while (len > 0) {
		rv = read(fd, buf+off, len);
		if (rv <= 0) {
			if (errno == EINTR)
				continue;
			return errno;
		}
		off += rv;
		len -= rv;
	}
	return 0;
}

inline int write_n(int fd, char *buf, int len)
{
	int rv, off=0;
	while (len > 0) {
		rv = write(fd, buf+off, len);
		if (rv <= 0) {
			if (errno == EINTR)
				continue;
			return errno;
		}
		off += rv;
		len -= rv;
	}
	return 0;
}


#endif // #ifndef __IOHandle_HPP

