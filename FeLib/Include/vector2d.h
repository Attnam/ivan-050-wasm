#ifndef __VECTOR2D_H__
#define __VECTOR2D_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include "felibdef.h"

/* Standard structure for representing positions */

struct vector2d
{
  typedef uchar vbyte;
  vector2d()				      { }
  vector2d(short X, short Y) : X(X), Y(Y)     { }
  vector2d  operator +   (vector2d H) const   { return vector2d(X + H.X, Y + H.Y); }
  vector2d& operator +=  (vector2d H)	      { X += H.X; Y += H.Y; return *this; }
  vector2d  operator -   (vector2d H) const   { return vector2d(X - H.X, Y - H.Y); }
  vector2d& operator -=  (vector2d H)	      { X -= H.X; Y -= H.Y; return *this; }
  vector2d  operator -   () const	      { return vector2d(-X, -Y); }
  vector2d  operator *   (short H) const      { return vector2d(X * H, Y * H); }
  vector2d& operator *=  (short H)	      { X *= H; Y *= H; return *this; }
  vector2d  operator /   (short H) const      { return vector2d(X / H, Y / H); }
  vector2d& operator /=  (short H)	      { X /= H; Y /= H; return *this; }
  vector2d  operator *   (float H) const      { return vector2d(short(X * H), short(Y * H)); }
  vector2d& operator *=  (float H)	      { X = short(X * H); Y = short(Y * H); return *this; }
  vector2d  operator /   (float H) const      { return vector2d(short(X / H), short(Y / H)); }
  vector2d& operator /=  (float H)	      { X = short(X / H); Y = short(Y / H); return *this; }
  bool	    operator ==  (vector2d H) const   { return X == H.X && Y == H.Y; }
  bool	    operator !=  (vector2d H) const   { return X != H.X || Y != H.Y; }
  vector2d  operator <<  (uchar S) const      { return vector2d(X << S, Y << S); }
  vector2d& operator <<= (uchar S)	      { X <<= S; Y <<= S; return *this; }
  vector2d  operator >>  (uchar S) const      { return vector2d(X >> S, Y >> S); }
  vector2d& operator >>= (uchar S)	      { X >>= S; Y >>= S; return *this; }
  bool	    operator <	 (vector2d H) const   { return X < H.X || (X == H.X && Y < H.Y); }
  ulong GetLengthSquare() const		      { return long(X) * long(X) + long(Y) * long(Y); }
  /* Also returns true if V == *this */
  bool IsAdjacent(vector2d V) const	      { return V.X >= X - 1 && V.X <= X + 1 && V.Y <= Y + 1 && V.Y >= Y - 1; }
  /* Pack a position of a 16x16 map to a byte and vice versa */
  static vbyte PackToByte(short X, short Y)   { return (Y << 4) | (X & 0xF); }
  static vbyte PackToByte(vector2d V)	      { return (V.Y << 4) | (V.X & 0xF); }
  static vector2d UnpackByte(vbyte B)	      { return vector2d(B & 0xF, B >> 4); }
  short X, Y;
};

/* Rotates a position Vect of a square map of size Size x Size according to
   Flags (see felibdef.h) */

inline void Rotate(vector2d& Vect, ushort Size, uchar Flags)
{
  if(Flags & ROTATE)
    {
      const short T = Vect.X;
      Vect.X = Size - Vect.Y - 1;
      Vect.Y = T;
    }

  if(Flags & MIRROR)
    Vect.X = Size - Vect.X - 1;

  if(Flags & FLIP)
    Vect.Y = Size - Vect.Y - 1;
}

#endif
