#ifndef __ITEM_H__
#define __ITEM_H__

#include <stdlib.h>

#include <cmath>

#include "typedef.h"
#include "vector.h"

#include "game.h"
#include "object.h"

class bitmap;
class character;
class material;
class object;
class vector;
class stack;

/* Presentation of the item class */

class item : public object
{
public:
	item(bool = true, bool = true);
	virtual float GetWeaponStrength(void) const;
	virtual void DrawToTileBuffer(void) const;
	virtual void PositionedDrawToTileBuffer(uchar) const;
	virtual std::string Name(uchar Case) const {return NameWithMaterial(Case);}
	virtual ushort CEmitation(void) const;
	virtual vector GetInHandsPic(void) const {return vector(0,0);}
	virtual uchar GetConsumeType(void) const {return ODD;}
	virtual ushort TryToOpen(stack*) {return 0xFFFF;}
	virtual ulong CWeight(void) const;
	virtual bool Consume(character*, float = 100) { return false; };
	virtual ushort GetArmorValue(void) const {return 100;}
	virtual bool IsHeadOfElpuri(void) const {return false;}
	virtual bool IsPerttusNut(void) const {return false;}
	virtual bool IsMaakotkaShirt(void) const {return false;}
	virtual bool CanBeRead(character*) const { return false; };
	virtual bool Read(character*) { return false; };
	virtual void ReceiveHitEffect(character*, character*) {}
	virtual bool CanBeDippedInto(item*) const {return false;}
	virtual void DipInto(item*) {}
	virtual material* BeDippedInto(void) {return 0;}
	virtual bool CanBeDipped(void) const {return false;}
	virtual bool CanBeWorn(void) const {return false;}
	virtual void SMaterial(ushort Where, material* What) { Material[Where] = What; }
	virtual bool Consumable(character*) const;
	virtual item* BetterVersion(void) const { return 0; }
	virtual void ImpactDamage(ushort, bool, stack*) {};
	virtual short CalculateOfferValue(char) const;
	virtual float OfferModifier(void) const { return 0; }
	virtual long Score(void) const { return 0; }
	virtual bool Destroyable(void) const { return true; }
	virtual bool Fly(uchar, ushort, stack*, bool = true);
	virtual bool HitCharacter(character*, float, bool);
	virtual bool DogWillCatchAndConsume(void) const { return false; }
	virtual uchar GetDipMaterialNumber(void) const { return CMaterials() - 1; }
	virtual ushort PrepareForConsuming(character*, stack*); // Stack where the item IS
	virtual item* Clone(bool = true, bool = true) const = 0;
	virtual ushort Possibility(void) const = 0;
	virtual bool CanBeWished(void) const { return true; }
	virtual item* CreateWishedItem(void) const { return game::CItemPrototype(Type())->Clone(); }
	virtual bool Apply(character*) { ADD_MESSAGE("You can't apply this!"); return false; }
protected:
	virtual void SetDefaultStats(void) = 0;
	virtual ushort CFormModifier(void) const {return 0;}
};

#ifdef __FILE_OF_STATIC_PROTOTYPE_DECLARATIONS__

	#define ITEM(name, base, initmaterials, setstats, data)\
	\
	class name : public base\
	{\
	public:\
		name(bool CreateMaterials = true, bool SetStats = true) : base(false, false) { if(CreateMaterials) initmaterials ; if(SetStats) SetDefaultStats(); }\
		name(material* Material, bool SetStats = true) : base(false, false) { InitMaterials(Material); if(SetStats) SetDefaultStats(); }\
		virtual item* Clone(bool CreateMaterials = true, bool SetStats = true) const { return new name(CreateMaterials, SetStats); }\
	protected:\
		virtual void SetDefaultStats(void) { setstats }\
		virtual ushort Type(void) const;\
		data\
	};\
	\
	class proto_##name\
	{\
	public:\
		proto_##name(void) : Index(game::AddProtoType(new name(false, false))) {}\
		ushort GetIndex(void) const { return Index; }\
	private:\
		ushort Index;\
	} static Proto_##name;\
	\
	ushort name::Type(void) const { return Proto_##name.GetIndex(); }

#else

	#define ITEM(name, base, initmaterials, setstats, data)\
	\
	class name : public base\
	{\
	public:\
		name(bool CreateMaterials = true, bool SetStats = true) : base(false, false) { if(CreateMaterials) initmaterials ; if(SetStats) SetDefaultStats(); }\
		name(material* Material, bool SetStats = true) : base(false, false) { InitMaterials(Material); if(SetStats) SetDefaultStats(); }\
		virtual item* Clone(bool CreateMaterials = true, bool SetStats = true) const { return new name(CreateMaterials, SetStats); }\
	protected:\
		virtual void SetDefaultStats(void) { setstats }\
		virtual ushort Type(void) const;\
		data\
	};

#endif

#define ABSTRACT_ITEM(name, base, data)\
\
class name : public base\
{\
public:\
	name(bool CreateMaterials, bool SetStats) : base(CreateMaterials, SetStats) {}\
	data\
};

ABSTRACT_ITEM(
	meleeweapon,
	item,
public:
	virtual void ReceiveHitEffect(character*, character*);
	virtual void DipInto(item*);
	virtual bool CanBeDippedInto(item*) const RET(Material[2] ? false : true)
	virtual bool CanBeDipped(void) const RET(true)
);

ITEM(
	banana,
	item,
	InitMaterials(2, new bananapeal(30), new bananaflesh(150)),
	{
		SSize(20);
	},
public:
	virtual ushort Possibility(void) const RET(50)
	virtual std::string Name(uchar Case) const RET(NameSized(Case,"a", 15, 40))
	virtual uchar GetConsumeType(void) const RET(Material[1]->CConsumeType())
	virtual vector GetInHandsPic(void) const RET(vector(160, 112))
	virtual bool Consume(character*, float = 100);
	virtual std::string NameSingular(void) const RET("banana")
	virtual std::string NamePlural(void) const RET("bananas")
	virtual float OfferModifier(void) const RET(1)
	virtual vector CBitmapPos(void) const RETV(0,112)
protected:
	virtual ushort CFormModifier(void) const RET(25)
);

ITEM(
	holybananaofliukasvipro,
	banana,
	InitMaterials(2, new bananapeal(40), new bananaflesh(300)),
	{
		SSize(30);
	},
public:
	virtual ushort Possibility(void) const RET(1)
	virtual std::string Name(uchar Case) const RET(NameArtifact(Case, bananapeal::StaticType()))
	virtual std::string NameSingular(void) const RET("holy banana of Liukas Vipro")
	virtual std::string NamePlural(void) const RET("holy bananas of Liukas Vipro")
	virtual float OfferModifier(void) const RET(40)
	virtual long Score(void) const RET(250)
	virtual vector CBitmapPos(void) const RETV(0,112)
protected:
	virtual ushort CFormModifier(void) const RET(35)
);

ITEM(
	lamp,
	item,
	InitMaterials(new glass(850)),
	{
		SSize(30);
	},
public:
	virtual ushort Possibility(void) const RET(10)
	virtual void PositionedDrawToTileBuffer(uchar) const;
	virtual ushort CEmitation(void) const RET(300)
	virtual std::string NameSingular(void) const RET("lamp")
	virtual std::string NamePlural(void) const RET("lamps")
	virtual vector GetInHandsPic(void) const RET(vector(160, 128))
	virtual float OfferModifier(void) const RET(1)
	virtual vector CBitmapPos(void) const RETV(0,192)
protected:
	virtual ushort CFormModifier(void) const RET(30)
);

ITEM(
	can,
	item,
	InitMaterials(2, new iron(50), rand() % 2 ? (material*)new bananaflesh(600) : (material*)new schoolfood(600)),
	{
		SSize(10);
	},
public:
	virtual ushort Possibility(void) const RET(100)
	virtual void PositionedDrawToTileBuffer(uchar) const;
	virtual std::string Name(uchar Case) const RET(NameContainer(Case))
	virtual ushort TryToOpen(stack*);
	virtual uchar GetConsumeType(void) const RET(Material[1] ? Material[1]->CConsumeType() : ODD)
	virtual std::string NameSingular(void) const RET("can")
	virtual std::string NamePlural(void) const RET("cans")
	virtual vector GetInHandsPic(void) const RET(vector(160, 144))
	virtual float OfferModifier(void) const RET(0.5)
	virtual ushort PrepareForConsuming(character*, stack*);
	virtual vector CBitmapPos(void) const RETV(144,288)
protected:
	virtual ushort CFormModifier(void) const RET(20)
);

ITEM(
	lump,
	item,
	InitMaterials(rand() % 2 ? (material*)new bananaflesh(600) : (material*)new schoolfood(600)),
	{
		SSize(10);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual std::string Name(uchar Case) const RET(NameThingsThatAreLikeLumps(Case, "a")) 
	virtual uchar GetConsumeType(void) const RET(Material[0]->CConsumeType())
	virtual bool Consume(character*, float = 100);
	virtual void ReceiveHitEffect(character*, character*);
	virtual bool CanBeDippedInto(item*) const RET(true)
	virtual material* BeDippedInto(void);
	virtual std::string NameSingular(void) const RET("lump")
	virtual std::string NamePlural(void) const RET("lumps")
	virtual vector GetInHandsPic(void) const RET(vector(160, 112))
	virtual float OfferModifier(void) const RET(0.5)
	virtual vector CBitmapPos(void) const RETV(144,48)
	virtual bool CanBeWished(void) const RET(false)
protected:
	virtual ushort CFormModifier(void) const RET(10)
);

ITEM(
	sword,
	meleeweapon,
	InitMaterials(3, new iron(2500), new iron(100), 0),
	{
		SSize(150);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual vector GetInHandsPic(void) const RET(vector(160,32))
	virtual std::string NameSingular(void) const RET("sword")
	virtual std::string NamePlural(void) const RET("swords")
	virtual float OfferModifier(void) const RET(0.5)
	virtual vector CBitmapPos(void) const RETV(0,0)
protected:
	virtual ushort CFormModifier(void) const RET(100)
);

ITEM(
	twohandedsword,
	sword,
	InitMaterials(3, new iron(5500), new iron(250), 0),
	{
		SSize(175);
	},
public:
	virtual ushort Possibility(void) const RET(3)
	virtual std::string NameSingular(void) const RET("two-handed sword")
	virtual std::string NamePlural(void) const RET("two-handed swords")
	virtual vector CBitmapPos(void) const RETV(0,0)
	virtual float OfferModifier(void) const RET(0.25)
protected:
	virtual ushort CFormModifier(void) const RET(125)
);

ITEM(
	curvedtwohandedsword,
	twohandedsword,
	InitMaterials(3, new iron(5500), new iron(250), 0),
	{
		SSize(175);
	},
public:
	virtual ushort Possibility(void) const RET(1)
	virtual std::string NameSingular(void) const RET("curved two-handed sword")
	virtual std::string NamePlural(void) const RET("curved two-handed swords")
	virtual float OfferModifier(void) const RET(0.25)
	virtual vector CBitmapPos(void) const RETV(0,16)
protected:
	virtual ushort CFormModifier(void) const RET(150)
);

ITEM(
	valpurijustifier,
	sword,
	InitMaterials(3, new valpurium(6500), new valpurium(300), 0),
	{
		SSize(200);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual std::string Name(uchar Case) const RET(NameArtifact(Case, valpurium::StaticType()))
	virtual std::string NameSingular(void) const RET("holy broadsword named Valpuri's Justifier")
	virtual std::string NamePlural(void) const RET("holy broadswords named Valpuri's Justifier")
	virtual float OfferModifier(void) const RET(0.5)
	virtual long Score(void) const RET(1000)
	virtual vector CBitmapPos(void) const RETV(0,64)
	virtual bool CanBeWished(void) const RET(false)
protected:
	virtual ushort CFormModifier(void) const RET(400)
);

ITEM(
	axe,
	meleeweapon,
	InitMaterials(3, new iron(450), new iron(900), 0),
	{
		SSize(125);
	},
public:
	virtual ushort Possibility(void) const RET(25)
	virtual vector GetInHandsPic(void) const RET(vector(160,16))
	virtual std::string NameSingular(void) const RET("axe")
	virtual std::string NamePlural(void) const RET("axes")
	virtual float OfferModifier(void) const RET(0.25)
	virtual vector CBitmapPos(void) const RETV(144,256)
protected:
	virtual ushort CFormModifier(void) const RET(150)
);

ITEM(
	pickaxe,
	axe,
	InitMaterials(3, new iron(1000), new wood(1050), 0),
	{
		SSize(150);
	},
public:
	virtual ushort Possibility(void) const RET(10)
	virtual std::string NameSingular(void) const RET("pick-axe")
	virtual std::string NamePlural(void) const RET("pick-axes")
	virtual vector GetInHandsPic(void) const RET(vector(160, 64))
	virtual float OfferModifier(void) const RET(0.25)
	virtual vector CBitmapPos(void) const RETV(0,96)
	virtual bool Apply(character*);
protected:
	virtual ushort CFormModifier(void) const RET(150)
);

ITEM(
	spear,
	meleeweapon,
	InitMaterials(3, new iron(150), new wood(1500), 0),
	{
		SSize(200);
	},
public:
	virtual ushort Possibility(void) const RET(25)
	virtual vector GetInHandsPic(void) const RET(vector(160,96))
	virtual bool CanBeDippedInto(item*) const RET(Material[2] ? false : true)
	virtual std::string NameSingular(void) const RET("spear")
	virtual std::string NamePlural(void) const RET("spears")
	virtual float OfferModifier(void) const RET(1)
	virtual vector CBitmapPos(void) const RETV(144,144)
protected:
	virtual ushort CFormModifier(void) const RET(200)
);

ABSTRACT_ITEM(
	torsoarmor,
	item,
public:
	virtual bool CanBeWorn(void) const RET(true)
protected:
	virtual ushort CFormModifier(void) const RET(15)
);

ITEM(
	platemail,
	torsoarmor,
	InitMaterials(new iron(4000)),
	{
		SSize(75);
	},
public:
	virtual ushort Possibility(void) const RET(3)
	virtual ushort GetArmorValue(void) const { float Base = 80 - sqrt(Material[0]->GetHitValue()) * 3; if(Base < 0) Base = 0; if(Base > 100) Base = 100; return ushort(Base); }
	virtual std::string NameSingular(void) const RET("plate mail")
	virtual std::string NamePlural(void) const RET("plate mails")
	virtual float OfferModifier(void) const RET(0.5)
	virtual vector CBitmapPos(void) const RETV(144,128)
);

ITEM(
	chainmail,
	torsoarmor,
	InitMaterials(new iron(2000)),
	{
		SSize(75);
	},
public:
	virtual ushort Possibility(void) const RET(10)
	virtual ushort GetArmorValue(void) const { float Base = 90 - sqrt(Material[0]->GetHitValue()) * 2; if(Base < 0) Base = 0; if(Base > 100) Base = 100; return ushort(Base); }
	virtual std::string NameSingular(void) const RET("chain mail")
	virtual std::string NamePlural(void) const RET("chain mails")
	virtual float OfferModifier(void) const RET(0.5)
	virtual vector CBitmapPos(void) const RETV(144,96)
);

ABSTRACT_ITEM(
	shirt,
	torsoarmor,
private:
	void Temporary(void) {} //...
);

ITEM(
	maakotkashirt,
	shirt,
	InitMaterials(new cloth(1000)),
	{
		SSize(60);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual ushort GetArmorValue(void) const RET(10)
	virtual std::string NameSingular(void) const RET("Maakotka shirt")
	virtual std::string NamePlural(void) const RET("Maakotka shirts")
	virtual short CalculateOfferValue(char) const RET(750)
	virtual long Score(void) const RET(1000)
	virtual bool IsMaakotkaShirt(void) const RET(true);
	virtual vector CBitmapPos(void) const RETV(144,112)
	virtual bool CanBeWished(void) const RET(false)
	virtual bool Destroyable(void) const { return false; }
);

ITEM(
	corpse,
	item,
	InitMaterials(new humanflesh(60000)),
	{
		SSize(0);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual uchar GetConsumeType(void) const RET(Material[0]->CConsumeType())
	virtual bool Consume(character*, float = 100);
	virtual std::string NameSingular(void) const RET("corpse")
	virtual std::string NamePlural(void) const RET("corpses")
	virtual float OfferModifier(void) const RET(0.01f)
	virtual vector CBitmapPos(void) const RETV(144,192)
	virtual bool CanBeWished(void) const RET(false)
protected:
	virtual ushort CFormModifier(void) const RET(20)
);

ITEM(
	potion,
	item,
	InitMaterials(2, new glass(50), rand() % 3 ? 0 : new omleurine(1500)),
	{
		SSize(30);
	},
public:
	virtual ushort Possibility(void) const RET(25)
	virtual uchar GetConsumeType(void) const RET(Material[1] ? Material[1]->CConsumeType() : ODD)
	virtual bool Consume(character*, float = 100);
	virtual std::string Name(uchar Case) const RET(NameContainer(Case))
	virtual std::string NameSingular(void) const RET("bottle")
	virtual std::string NamePlural(void) const RET("bottles")
	virtual void ImpactDamage(ushort, bool, stack*);
	virtual void PositionedDrawToTileBuffer(uchar) const;
	virtual float OfferModifier(void) const RET(0.1f)
	virtual vector CBitmapPos(void) const RETV(0,144)
protected:
	virtual ushort CFormModifier(void) const RET(40)
);

ITEM(
	bananapeals,
	item,
	InitMaterials(new bananapeal(15)),
	{
		SSize(20);
	},
public:
	virtual ushort Possibility(void) const RET(25)
	virtual std::string Name(uchar Case) const RET(NameHandleDefaultMaterial(Case, "a", bananapeal::StaticType()))
	virtual std::string NameSingular(void) const RET("banana peal")
	virtual std::string NamePlural(void) const RET("banana peals")
	virtual item* BetterVersion(void) const { return new banana; }
	virtual float OfferModifier(void) const RET(0)
	virtual long Score(void) const RET(-1)
	virtual vector CBitmapPos(void) const RETV(0,128)
protected:
	virtual ushort CFormModifier(void) const RET(20)
);

ITEM(
	brokenbottle,
	item,
	InitMaterials(new glass(50)),
	{
		SSize(10);
	},
public:
	virtual ushort Possibility(void) const RET(25)
	virtual std::string NameSingular(void) const RET("broken bottle")
	virtual std::string NamePlural(void) const RET("broken bottles")
	virtual item* BetterVersion(void) const { item* P = new potion(false); P->InitMaterials(2, new glass(50), new omleurine(1500)); return P; }
	virtual float OfferModifier(void) const RET(0)
	virtual vector CBitmapPos(void) const RETV(144,160)
protected:
	virtual ushort CFormModifier(void) const RET(60)
);

ABSTRACT_ITEM(
	scroll,
	item,
public:
	virtual bool CanBeRead(character*) const;
protected:
	virtual ushort CFormModifier(void) const RET(30)
);

ITEM(
	scrollofcreatemonster,
	scroll,
	InitMaterials(new parchment(200)),
	{
		SSize(30);
	},
public:
	virtual ushort Possibility(void) const RET(25)
	virtual std::string NameSingular(void) const RET("scroll of create monster")
	virtual std::string NamePlural(void) const RET("scrolls of create monster")
	virtual float OfferModifier(void) const RET(5)
	virtual vector CBitmapPos(void) const RETV(144,176)
	virtual bool Read(character*);
);

ITEM(
	scrollofteleport,
	scroll,
	InitMaterials(new parchment(200)),
	{
		SSize(30);
	},
public:
	virtual ushort Possibility(void) const RET(25)
	virtual std::string NameSingular(void) const RET("scroll of teleportation")
	virtual std::string NamePlural(void) const RET("scrolls of teleportation")
	virtual float OfferModifier(void) const RET(5)
	virtual vector CBitmapPos(void) const RETV(144,176)
	virtual bool Read(character*);
);

ITEM(
	head,
	item,
	InitMaterials(new humanflesh(2000)),
	{
		SSize(0);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual std::string NameSingular(void) const RET("head")
	virtual std::string NamePlural(void) const RET("heads")
	virtual float OfferModifier(void) const RET(0.1f)
	virtual vector CBitmapPos(void) const RETV(0,0)
	virtual bool CanBeWished(void) const RET(false)
protected:
	virtual ushort CFormModifier(void) const RET(10)
);

ITEM(
	headofelpuri,
	head,
	InitMaterials(new elpuriflesh(25000)),
	{
		SSize(60);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual std::string Name(uchar Case) const RET(NameArtifact(Case, elpuriflesh::StaticType()))
	virtual bool IsHeadOfElpuri(void) const RET(true)
	virtual std::string NameSingular(void) const RET("head of Elpuri")
	virtual std::string NamePlural(void) const RET("heads of Elpuri")
	virtual long Score(void) const RET(500);
	virtual vector CBitmapPos(void) const RETV(144,0)
	virtual bool CanBeWished(void) const RET(false)
	virtual bool Destroyable(void) const { return false; }
);

ITEM(
	nut,
	item,
	InitMaterials(new humanflesh(15)),
	{
		SSize(0);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual std::string NameSingular(void) const RET("nut")
	virtual std::string NamePlural(void) const RET("nuts")
	virtual float OfferModifier(void) const RET(10)
	virtual vector CBitmapPos(void) const RETV(0,0)
protected:
	virtual ushort CFormModifier(void) const RET(10)
);

ITEM(
	leftnutofperttu,
	nut,
	InitMaterials(new humanflesh(150)),
	{
		SSize(10);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual bool IsPerttusNut(void) const RET(true)
	virtual std::string Name(uchar Case) const RET(NameArtifact(Case, humanflesh::StaticType()))
	virtual std::string NameSingular(void) const RET("left nut of Perttu")
	virtual std::string NamePlural(void) const RET("left nuts of Perttu")		//???
	virtual long Score(void) const RET(2500)
	virtual vector CBitmapPos(void) const RETV(144,208)
	virtual item* CreateWishedItem(void) const;
	virtual bool Destroyable(void) const { return false; }
);

ITEM(
	abone,
	item,
	InitMaterials(new bone(2000)),
	{
		SSize(50);
	},
public:
	virtual ushort Possibility(void) const RET(50)
	virtual std::string NameSingular(void) const RET("bone")
	virtual std::string NamePlural(void) const RET("bones")
	virtual std::string Name(uchar Case) const RET(NameSized(Case,"a", 15, 40))
	virtual float OfferModifier(void) const RET(0.1f)
	virtual bool Consume(character*, float = 100);
	virtual bool DogWillCatchAndConsume(void) const RET(true);
	virtual uchar GetConsumeType(void) const RET(Material[0]->CConsumeType());
	virtual vector CBitmapPos(void) const RETV(144,240)
protected:
	virtual ushort CFormModifier(void) const RET(50)
);

ITEM(
	poleaxe,
	axe,
	InitMaterials(3, new iron(1500), new wood(3000), 0),
	{
		SSize(225);
	},
public:
	virtual ushort Possibility(void) const RET(15)
	virtual std::string NameSingular(void) const RET("poleaxe")
	virtual std::string NamePlural(void) const RET("poleaxes")
	virtual float OfferModifier(void) const RET(0.25f)
	virtual vector CBitmapPos(void) const RETV(0,80)
protected:
	virtual ushort CFormModifier(void) const RET(100)
);

ITEM(
	spikedmace,
	meleeweapon,
	InitMaterials(3, new iron(5000), new wood(3000), 0),
	{
		SSize(150);
	},
public:
	virtual ushort Possibility(void) const RET(5)
	virtual std::string NameSingular(void) const RET("spiked mace")
	virtual std::string NamePlural(void) const RET("spiked maces")
	virtual vector GetInHandsPic(void) const RET(vector(160, 0))
	virtual float OfferModifier(void) const RET(0.125)
	virtual vector CBitmapPos(void) const RETV(0,32)
protected:
	virtual ushort CFormModifier(void) const RET(75)
);

ITEM(
	htaedfoneercseulb,
	spikedmace,
	InitMaterials(3, new mithril(15000), new iron(8000), new darkfrogflesh(1000)),
	{
		SSize(200);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual std::string Name(uchar Case) const RET(NameArtifact(Case, mithril::StaticType()))
	virtual std::string NameSingular(void) const RET("ancient mace named H'taed Foneer Cse-ulb")
	virtual std::string NamePlural(void) const RET("ancient maces named H'taed Foneer Cse-ulb")
	virtual vector GetInHandsPic(void) const RET(vector(160, 0))
	virtual float OfferModifier(void) const RET(0.25)
	virtual long Score(void) const RET(1000)
	virtual vector CBitmapPos(void) const RETV(0,32)
	virtual bool CanBeWished(void) const RET(false)
protected:
	virtual ushort CFormModifier(void) const RET(100)
);

ITEM(
	loaf,
	item,
	InitMaterials(rand() % 2 ? (material*)new pork(2000) : (material*)new beef(2000)),
	{
		SSize(40);
	},
public:
	virtual ushort Possibility(void) const RET(200)
	virtual std::string Name(uchar Case) const RET(NameThingsThatAreLikeLumps(Case, "a")) 
	virtual std::string NameSingular(void) const RET("loaf")
	virtual std::string NamePlural(void) const RET("loaves")
	virtual uchar GetConsumeType(void) const RET(Material[0]->CConsumeType())
	virtual bool Consume(character*, float = 100);
	virtual float OfferModifier(void) const RET(0.125)
	virtual vector CBitmapPos(void) const RETV(0,272)
protected:
	virtual ushort CFormModifier(void) const RET(15)
);

ITEM(
	scrollofwishing,
	scroll,
	InitMaterials(new parchment(200)),
	{
		SSize(30);
	},
public:
	virtual ushort Possibility(void) const RET(1)
	virtual std::string NameSingular(void) const RET("scroll of wishing")
	virtual std::string NamePlural(void) const RET("scrolls of wishing")
	virtual float OfferModifier(void) const RET(50)
	virtual vector CBitmapPos(void) const RETV(144,176)
	virtual bool CanBeWished(void) const RET(false)
	virtual bool Read(character*);
);

ITEM(
	cheapcopyofleftnutofperttu,
	nut,
	InitMaterials(new glass(150)),
	{
		SSize(10);
	},
public:
	virtual ushort Possibility(void) const RET(0)
	virtual std::string NameSingular(void) const RET("cheap copy of left nut of Perttu")
	virtual std::string NamePlural(void) const RET("cheap copies of left nut of Perttu")		//???
	virtual long Score(void) const RET(1)
	virtual vector CBitmapPos(void) const RETV(144,208)
);

#endif
