#ifndef __ID_H__
#define __ID_H__

#ifdef VC
#pragma warning(disable : 4786)
#endif

#include "typedef.h"

class festring;

#define CHAR_NAME(Case) GetName(Case).CStr()
#define CHAR_DESCRIPTION(Case) GetDescription(Case).CStr()

class id
{
 public:
  virtual void AddName(festring&, uchar, ushort) const;
  virtual festring GetName(uchar, ushort) const;
  virtual void AddName(festring&, uchar) const;
  virtual festring GetName(uchar) const;
 protected:
  virtual const festring& GetNameSingular() const = 0;
  virtual void AddNameSingular(festring&, bool) const;
  virtual const festring& GetNamePlural() const = 0;
  virtual const festring& GetArticle() const = 0;
  virtual bool AddAdjective(festring&, bool) const;
  virtual const festring& GetAdjective() const = 0;
  virtual const festring& GetAdjectiveArticle() const = 0;
  virtual bool AddMaterialDescription(festring&, bool) const { return false; }
  virtual const festring& GetPostFix() const = 0;
  virtual void AddPostFix(festring&) const;
  virtual uchar GetArticleMode() const;
  virtual bool ShowMaterial() const { return false; }
  virtual void AddLockPostFix(festring&, uchar) const;
  virtual bool AddActiveAdjective(festring&, bool) const;
};

#endif
