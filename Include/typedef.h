#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#define RET(Val) { return Val; }
#define RETV(XVal,YVal) { return vector2d(XVal, YVal); }

typedef		signed		char		schar;
typedef		unsigned	char		uchar;
typedef		unsigned	short		ushort;
typedef		unsigned	long		ulong;

#endif



