#ifndef __MATERDE_H__
#define __MATERDE_H__

#include "materba.h"
#include "graphics.h"

class MATERIAL
(
	iron,
	material,
public:
	virtual ushort GetHitValue() const				{ return 100; }
	virtual uchar GetConsumeType() const				{ return HARD; }
	virtual ushort GetDensity() const				{ return 8000; }
	virtual ushort OfferValue() const				{ return 10; }
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(64, 64, 64); }
	virtual ulong RawPrice() const { return GetVolume() >> 8; }
protected:
	virtual std::string NameStem() const	{ return "iron"; }
	virtual std::string Article() const { return "an"; }
);

class MATERIAL
(
	valpurium,
	material,
public:
	virtual ushort GetHitValue() const				{ return 400; }
	virtual uchar GetConsumeType() const				{ return HARD; }
	virtual ushort GetDensity() const				{ return 3000; }
	virtual ushort OfferValue() const				{ return 100; }
	virtual uchar Alignment() const				{ return GOOD; }
	virtual ushort GetEmitation() const { return 320; }
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(192, 192, 192); }
	virtual bool CanBeWished() const { return false; }
	virtual ulong RawPrice() const { return GetVolume() << 2; }
protected:
	virtual std::string NameStem() const	{ return "valpurium"; }
);

class MATERIAL
(
	stone,
	material,
public:
	virtual ushort GetHitValue() const				{ return 60; }
	virtual uchar GetConsumeType() const				{ return HARD; }
	virtual ushort GetDensity() const				{ return 3000; }
	virtual ushort OfferValue() const				{ return 5; }
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(64, 64, 64); }
protected:
	virtual std::string NameStem() const	{ return "stone"; }
);

class MATERIAL
(
	bananaflesh,
	material,
public:
	virtual ushort GetHitValue() const				{ return 5; }
	virtual uchar GetConsumeType() const				{ return FRUIT; }
	virtual ushort GetDensity() const				{ return 1200; }
	virtual ushort OfferValue() const				{ return 10; }
	virtual void EatEffect(character* Eater, float Amount, float NPModifier)	{ NormalFoodEffect(Eater, Amount, NPModifier); MinusAmount(Amount); }
	virtual short NutritionValue() const			{ return 200; }
	virtual ushort GetColor() const { return MAKE_RGB(225, 225, 0); }
	virtual ulong RawPrice() const { return GetVolume() >> 4; }
protected:
	virtual std::string NameStem() const	{ return "banana flesh"; }
);

class MATERIAL
(
	gravel,
	material,
public:
	virtual ushort GetHitValue() const				{ return 40; }
	virtual uchar GetConsumeType() const				{ return ODD; }
	virtual ushort GetDensity() const				{ return 2500; }
	virtual ushort OfferValue() const				{ return 1; }
	virtual ushort GetColor() const { return MAKE_RGB(64, 64, 64); }
protected:
	virtual std::string NameStem() const	{ return "gravel"; }
);

class MATERIAL
(
	moraine,
	material,
public:
	virtual ushort GetHitValue() const				{ return 20; }
	virtual uchar GetConsumeType() const				{ return ODD; }
	virtual ushort GetDensity() const				{ return 2500; }
	virtual ushort OfferValue() const				{ return 2; }
	virtual ushort GetColor() const { return MAKE_RGB(111, 74, 37); }
protected:
	virtual std::string NameStem() const	{ return "moraine"; }
);

class MATERIAL
(
	schoolfood,
	material,
public:
	virtual ushort GetHitValue() const				{ return 5; }
	virtual uchar GetConsumeType() const				{ return SCHOOLFOOD; }
	virtual ushort GetDensity() const				{ return 1500; }
	virtual ushort OfferValue() const				{ return 20; }
	virtual uchar Alignment() const				{ return EVIL; }
	virtual short NutritionValue() const			{ return 100; }
	virtual void EatEffect(character*, float, float);
	virtual void HitEffect(character* Enemy);
	virtual ushort GetColor() const { return MAKE_RGB(111, 74, 37); }
protected:
	virtual std::string NameStem() const	{ return "school food"; }
);

class MATERIAL
(
	air,
	material,
public:
	virtual ushort GetHitValue() const				{ return 0; }
	virtual ushort GetDensity() const				{ return 1; }
	virtual ushort OfferValue() const				{ return 0; }
	virtual ushort GetColor() const { return MAKE_RGB(128, 128, 128); }
protected:
	virtual std::string NameStem() const	{ return "air"; }
	virtual std::string Article() const { return "an"; }
);

class MATERIAL
(
	wood,
	material,
public:
	virtual ushort GetHitValue() const				{ return 30; }
	virtual uchar GetConsumeType() const				{ return HARD; }
	virtual ushort GetDensity() const				{ return 500; }
	virtual ushort OfferValue() const				{ return 5; }
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(140, 100, 60); }
protected:
	virtual std::string NameStem() const	{ return "wood"; }
);

class MATERIAL
(
	flesh,
	material,
public:
	virtual ushort GetHitValue() const 				{ return 10; }
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort GetDensity() const				{ return 1200; }
	virtual ushort OfferValue() const				{ return 15; }
	virtual void EatEffect(character* Eater, float Amount, float NPModifier)	{ NormalFoodEffect(Eater, Amount, NPModifier); MinusAmount(Amount); }
	virtual short NutritionValue() const			{ return 200; }
	virtual ushort GetColor() const { return MAKE_RGB(64, 64, 64); }
	virtual bool IsFlesh() { return true; }
protected:
	virtual std::string NameStem() const	{ return "flesh"; }
);

class MATERIAL
(
	goblinoidflesh,
	flesh,
public:
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort OfferValue() const				{ return 10; }
	virtual ushort GetColor() const { return MAKE_RGB(0, 128, 0); }
protected:
	virtual std::string NameStem() const	{ return "green flesh"; }
);

class MATERIAL
(
	pork,
	flesh,
public:
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort OfferValue() const				{ return 20; }
	virtual short NutritionValue() const			{ return 400; }
	virtual ushort GetColor() const { return MAKE_RGB(128, 128, 128); }
	virtual ulong RawPrice() const { return GetVolume() >> 3; }
protected:
	virtual std::string NameStem() const	{ return "pork"; }
);

class MATERIAL
(
	beef,
	flesh,
public:
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort OfferValue() const				{ return 20; }
	virtual short NutritionValue() const			{ return 400; }
	virtual ushort GetColor() const { return MAKE_RGB(128, 128, 128); }
	virtual ulong RawPrice() const { return GetVolume() >> 3; }
protected:
	virtual std::string NameStem() const	{ return "beef"; }
);

class MATERIAL
(
	bone,
	material,
public:
	virtual ushort GetHitValue() const 				{ return 30; }
	virtual ushort GetDensity() const				{ return 2000; }
	virtual ushort OfferValue() const				{ return 5; }
	virtual uchar GetConsumeType() const				{ return BONE; }
	virtual void EatEffect(character*, float, float);
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(96, 96, 96); }
	virtual bool IsFlesh() { return true; }
	virtual short NutritionValue() const			{ return 20; }
protected:
	virtual std::string NameStem() const	{ return "bone"; }
);

class MATERIAL
(
	darkfrogflesh,
	flesh,
public:
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort OfferValue() const				{ return 50; }
	virtual uchar Alignment() const				{ return EVIL; }
	virtual void EatEffect(character*, float, float);
	virtual void HitEffect(character* Enemy);
	virtual ushort GetColor() const { return MAKE_RGB(32, 32, 32); }
	virtual short NutritionValue() const			{ return 1000; }
protected:
	virtual std::string NameStem() const	{ return "dark frog flesh"; }
);

class MATERIAL
(
	elpuriflesh,
	darkfrogflesh,
public:
	virtual ushort GetHitValue() const 				{ return 30; }
	virtual ushort GetDensity() const				{ return 2400; }
	virtual ushort OfferValue() const				{ return 1; }
	virtual ushort GetColor() const { return MAKE_RGB(32, 32, 32); }
protected:
	virtual std::string NameStem() const	{ return "Elpuri's flesh"; }
	virtual std::string Article() const { return "an"; }
);

class MATERIAL
(
	glass,
	material,
public:
	virtual ushort GetHitValue() const 				{ return 30; }
	virtual uchar GetConsumeType() const				{ return HARD; }
	virtual ushort GetDensity() const				{ return 2500; }
	virtual ushort OfferValue() const				{ return 5; }
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(128, 128, 255); }
protected:
	virtual std::string NameStem() const	{ return "glass"; }
);

class MATERIAL
(
	omleurine,
	material,
public:
	virtual ushort GetHitValue() const 				{ return 1; }
	virtual uchar GetConsumeType() const				{ return LIQUID; }
	virtual ushort GetDensity() const				{ return 1000; }
	virtual ushort OfferValue() const				{ return 100; }
	virtual void EatEffect(character*, float, float);
	virtual void HitEffect(character* Enemy);
	virtual short NutritionValue() const			{ return 30; }
	virtual ushort GetColor() const { return MAKE_RGB(0, 128, 0); }
	virtual bool CanBeWished() const { return false; }
	virtual ulong RawPrice() const { return GetVolume() >> 2; }
protected:
	virtual std::string NameStem() const	{ return "Omle urine"; }
	virtual std::string Article() const { return "an"; }
);

class MATERIAL
(
	bananapeal,
	material,
public:
	virtual ushort GetHitValue() const 				{ return 10; }
	virtual uchar GetConsumeType() const				{ return ODD; }
	virtual ushort GetDensity() const				{ return 500; }
	virtual ushort OfferValue() const				{ return 1; }
	virtual ushort GetColor() const { return MAKE_RGB(225, 225, 0); }
protected:
	virtual std::string NameStem() const	{ return "banana peal"; }
);

class MATERIAL
(
	parchment,
	material,
public:
	virtual ushort GetHitValue() const 				{ return 15; }
	virtual uchar GetConsumeType() const				{ return ODD; }
	virtual ushort GetDensity() const				{ return 600; }
	virtual ushort OfferValue() const				{ return 5; }
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(192, 192, 192); }
protected:
	virtual std::string NameStem() const	{ return "parchment"; }
);

class MATERIAL
(
	cloth,
	material,
public:
	virtual ushort GetHitValue() const 				{ return 5; }
	virtual uchar GetConsumeType() const				{ return ODD; }
	virtual ushort GetDensity() const				{ return 100; }
	virtual ushort OfferValue() const				{ return 5; }
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(64, 64, 128); }
protected:
	virtual std::string NameStem() const	{ return "cloth"; }
);

class MATERIAL
(
	humanflesh,
	flesh,
public:
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort OfferValue() const				{ return 30; }
	virtual uchar Alignment() const				{ return EVIL; }
	virtual ushort GetColor() const { return MAKE_RGB(90, 64, 64); }
protected:
	virtual std::string NameStem() const	{ return "human flesh"; }
);

class MATERIAL
(
	slime,
	material,
public:
	virtual ushort GetHitValue() const 				{ return 2; }
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort GetDensity() const 				{ return 400; }
	virtual ushort OfferValue() const				{ return 1; }
	virtual short NutritionValue() const			{ return 100; }
	virtual void EatEffect(character* Eater, float Amount, float NPModifier)	{ NormalFoodEffect(Eater, Amount, NPModifier); MinusAmount(Amount); }
	virtual ushort GetColor() const { return MAKE_RGB(0, 128, 0); }
	virtual bool IsFlesh() { return true; }
protected:
	virtual std::string NameStem() const	{ return "slime"; }
);

class MATERIAL
(
	brownslime,
	slime,
public:
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort OfferValue() const				{ return 1; }
	virtual ushort GetColor() const { return MAKE_RGB(111, 74, 37); }
protected:
	virtual std::string NameStem() const	{ return "brown slime"; }
);

class MATERIAL
(
	wolfflesh,
	flesh,
public:
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort OfferValue() const				{ return 15; }
	virtual ushort GetColor() const { return MAKE_RGB(32, 32, 32); }
	virtual short NutritionValue() const			{ return 400; }
protected:
	virtual std::string NameStem() const	{ return "wolf flesh"; }
);

class MATERIAL
(
	dogflesh,
	wolfflesh,
public:
	virtual ushort OfferValue() const				{ return 15; }
	virtual uchar Alignment() const				{ return EVIL; }
	virtual ushort GetColor() const { return MAKE_RGB(111, 74, 37); }
protected:
	virtual std::string NameStem() const	{ return "dog flesh"; }
);

class MATERIAL
(
	ennerbeastflesh,
	humanflesh, //???
public:
	virtual ushort GetHitValue() const 				{ return 30; }
	virtual ushort GetDensity() const				{ return 1800; }
	virtual ushort OfferValue() const				{ return 5; }
	virtual ushort GetColor() const { return MAKE_RGB(90, 64, 64); }
protected:
	virtual std::string NameStem() const	{ return "Enner Beast flesh"; }
	virtual std::string Article() const { return "an"; }
);

class MATERIAL
(
	pepsi,
	material,
public:
	virtual ushort GetHitValue() const 				{ return 200; }
	virtual uchar GetConsumeType() const				{ return LIQUID; }
	virtual ushort GetDensity() const 				{ return 1500; }
	virtual ushort OfferValue() const				{ return 50; }
	virtual uchar Alignment() const				{ return EVIL; }
	virtual void EatEffect(character*, float, float);
	virtual void HitEffect(character* Enemy);
	virtual short NutritionValue() const			{ return 400; }
	virtual ushort GetColor() const { return MAKE_RGB(32, 32, 32); }
	virtual bool IsFlesh() { return true; }
protected:
	virtual std::string NameStem() const	{ return "pepsi"; }
);

class MATERIAL
(
	mithril,
	material,
public:
	virtual ushort GetHitValue() const 				{ return 200; }
	virtual uchar GetConsumeType() const				{ return HARD; }
	virtual ushort GetDensity() const 				{ return 5000; }
	virtual ushort OfferValue() const				{ return 25; }
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(128, 128, 128); }
	virtual ulong RawPrice() const { return GetVolume() >> 1; }
protected:
	virtual std::string NameStem() const	{ return "mithril"; }
);

class MATERIAL
(
	spiderflesh,
	flesh,
public:
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort OfferValue() const				{ return 4; }
	virtual ushort GetColor() const { return MAKE_RGB(32, 32, 32); }
protected:
	virtual std::string NameStem() const	{ return "spider flesh"; }
);

class MATERIAL
(
	jackalflesh,
	flesh,
public:
	virtual uchar GetConsumeType() const				{ return MEAT; }
	virtual ushort OfferValue() const				{ return 6; }
	virtual short NutritionValue() const			{ return 400; }
	virtual ushort GetColor() const { return MAKE_RGB(200, 200, 200); }
protected:
	virtual std::string NameStem() const	{ return "jackal flesh"; }
);

class MATERIAL
(
	donkeyflesh,
	flesh,
public:
	virtual uchar GetConsumeType() const		{ return MEAT; }
	virtual ushort OfferValue() const			{ return 6; }
	virtual ushort GetColor() const { return MAKE_RGB(64, 64, 64); }
	virtual short NutritionValue() const			{ return 400; }
protected:
	virtual std::string NameStem() const	{ return "donkey flesh"; }
);

class MATERIAL
(
	marble,
	material,
public:
	virtual ushort GetHitValue() const				{ return 50; }
	virtual uchar GetConsumeType() const			{ return HARD; }
	virtual ushort GetDensity() const				{ return 3000; }
	virtual ushort OfferValue() const				{ return 7; }
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(128, 128, 128); }
protected:
	virtual std::string NameStem() const	{ return "marble"; }
);

class MATERIAL
(
	gold,
	material,
public:
	virtual ushort GetHitValue() const				{ return 30; }
	virtual uchar GetConsumeType() const				{ return HARD; }
	virtual ushort GetDensity() const				{ return 20000; }
	virtual ushort OfferValue() const				{ return 50; }
	virtual uchar Alignment() const					{ return NEUTRAL; }
	virtual bool IsSolid() const { return true; }
	virtual ushort GetColor() const { return MAKE_RGB(192, 192, 0); }
	virtual bool CanBeWished() const { return false; }
	virtual ulong RawPrice() const { return GetVolume() << 1; }
protected:
	virtual std::string NameStem() const	{ return "gold"; }
);

class MATERIAL
(
	grass,
	material,
public:
	virtual ushort GetHitValue() const				{ return 2; }
	virtual uchar GetConsumeType() const			{ return ODD; }
	virtual ushort GetDensity() const				{ return 100; }
	virtual ushort OfferValue() const				{ return 2; }
	virtual ushort GetColor() const { return MAKE_RGB(0, 128, 0); }
protected:
	virtual std::string NameStem() const	{ return "grass"; }
);

#endif
