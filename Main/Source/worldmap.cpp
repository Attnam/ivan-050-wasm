#include <cmath>

#include "worldmap.h"
#include "wsquare.h"
#include "wterrade.h"
#include "charba.h"
#include "allocate.h"
#include "level.h"
#include "materba.h"
#include "proto.h"
#include "error.h"
#include "save.h"

worldmap::worldmap(ushort XSize, ushort YSize) : area(XSize, YSize), Continent(1, 0)
{
	Map = (worldmapsquare***)area::Map;

	for(ushort x = 0; x < XSize; x++)
		for(ulong y = 0; y < YSize; y++)
		{
			Map[x][y] = new worldmapsquare(this, vector2d(x, y));
			Map[x][y]->ChangeWorldMapTerrain(new ocean, new atmosphere);
		}

	TypeBuffer = Alloc2D<ushort>(XSize, YSize);
	AltitudeBuffer = Alloc2D<short>(XSize, YSize);
	ContinentBuffer = Alloc2D<uchar>(XSize, YSize, 0);

	continent::TypeBuffer = TypeBuffer;
	continent::AltitudeBuffer = AltitudeBuffer;
	continent::ContinentBuffer = ContinentBuffer;
}

worldmap::~worldmap()
{
	delete [] TypeBuffer;
	delete [] AltitudeBuffer;
	delete [] ContinentBuffer;
}

void worldmap::Save(outputfile& SaveFile) const
{
	area::Save(SaveFile);

	for(ulong c = 0; c < XSizeTimesYSize; ++c)
		Map[0][c]->Save(SaveFile);

	SaveFile.GetBuffer().write((char*)TypeBuffer[0], sizeof(ushort) * XSizeTimesYSize);
	SaveFile.GetBuffer().write((char*)AltitudeBuffer[0], sizeof(short) * XSizeTimesYSize);
	SaveFile.GetBuffer().write((char*)ContinentBuffer[0], sizeof(uchar) * XSizeTimesYSize);

	SaveFile << Continent;	 
}

void worldmap::Load(inputfile& SaveFile)
{
	area::Load(SaveFile);

	Map = (worldmapsquare***)area::Map;

	for(ushort x = 0; x < XSize; x++)
		for(ulong y = 0; y < YSize; y++)
		{
			Map[x][y] = new worldmapsquare(this, vector2d(x, y));
			Map[x][y]->Load(SaveFile);
		}

	TypeBuffer = Alloc2D<ushort>(XSize, YSize);
	AltitudeBuffer = Alloc2D<short>(XSize, YSize);
	ContinentBuffer = Alloc2D<uchar>(XSize, YSize);

	SaveFile.GetBuffer().read((char*)TypeBuffer[0], sizeof(ushort) * XSizeTimesYSize);
	SaveFile.GetBuffer().read((char*)AltitudeBuffer[0], sizeof(short) * XSizeTimesYSize);
	SaveFile.GetBuffer().read((char*)ContinentBuffer[0], sizeof(uchar) * XSizeTimesYSize);

	continent::TypeBuffer = TypeBuffer;
	continent::AltitudeBuffer = AltitudeBuffer;
	continent::ContinentBuffer = ContinentBuffer;

	SaveFile >> Continent;
}

void worldmap::Draw() const
{
	ushort XMax = GetXSize() < game::GetCamera().X + 50 ? GetXSize() : game::GetCamera().X + 50;
	ushort YMax = GetYSize() < game::GetCamera().Y + 30 ? GetYSize() : game::GetCamera().Y + 30;

	if(game::GetSeeWholeMapCheat())
		for(ushort x = game::GetCamera().X; x < XMax; x++)
			for(ushort y = game::GetCamera().Y; y < YMax; y++)
			{
				long xDist = long(x) - game::GetPlayer()->GetPos().X, yDist = long(y) - game::GetPlayer()->GetPos().Y;

				if(Map[x][y]->RetrieveFlag() && (xDist * xDist + yDist * yDist) <= game::GetPlayer()->LOSRangeLevelSquare())
					Map[x][y]->UpdateMemorizedAndDraw();
				else
					Map[x][y]->DrawCheat();
			}
	else
		for(ushort x = game::GetCamera().X; x < XMax; x++)
			for(ushort y = game::GetCamera().Y; y < YMax; y++)
			{
				long xDist = (long(x) - game::GetPlayer()->GetPos().X), yDist = (long(y) - game::GetPlayer()->GetPos().Y);

				if(Map[x][y]->RetrieveFlag() && (xDist * xDist + yDist * yDist) <= game::GetPlayer()->LOSRangeLevelSquare())
					Map[x][y]->UpdateMemorizedAndDraw();
				else
					Map[x][y]->DrawMemorized();
			}
}

void worldmap::Generate()
{
	RandomizeAltitude();
	SmoothAltitude();
	GenerateClimate();
	SmoothClimate();
	CalculateContinents();

	std::vector<continent*> PerfectForAttnam;

	for(uchar c = 1; c < Continent.size(); ++c)
		if(Continent[c]->GetGroundTerrainAmount(evergreenforest::StaticType()) > 1)
			PerfectForAttnam.push_back(Continent[c]);

	continent* PerttuLikes = PerfectForAttnam[rand() % PerfectForAttnam.size()];

	vector2d AttnamPos = PerttuLikes->GetRandomMember(evergreenforest::StaticType());

	for(vector2d ElpuriCavePos = PerttuLikes->GetRandomMember(evergreenforest::StaticType()); ElpuriCavePos == AttnamPos; ElpuriCavePos = PerttuLikes->GetRandomMember(evergreenforest::StaticType()));

	GetWorldMapSquare(AttnamPos)->ChangeOverWorldMapTerrain(new attnam);
	GetWorldMapSquare(ElpuriCavePos)->ChangeOverWorldMapTerrain(new elpuricave);

	GetWorldMapSquare(AttnamPos)->AddCharacter(game::GetPlayer());
}

void worldmap::RandomizeAltitude()
{
	for(ushort x = 0; x < XSize; x++)
		for(ushort y = 0; y < YSize; y++)
			AltitudeBuffer[x][y] = rand() % 3001 - rand() % 3000;
}

void worldmap::SmoothAltitude()
{
	short** OldAltitudeBuffer = Alloc2D<short>(XSize, YSize);

	for(uchar c = 0; c < 6; ++c)
	{
		if(c < 4)
		{
			for(uchar c1 = 0; c1 < rand() % 20; c1++)
			{
				ushort PlaceX = 5 + rand() % (XSize-10), PlaceY = 5 + rand() % (YSize-10);
				short Change = rand() % 10000 - rand() % 10000;

				for(int c2 = 0; c2 < rand() % 50; c2++)
					AltitudeBuffer[(PlaceX + rand() % 5 - rand() % 5)][(PlaceY + rand() % 5 - rand() % 5)] += Change;
			}
		}

		for(ushort y = 0; y < YSize; y++)
			for(ushort x = 0; x < XSize; x++)
			{
				long HeightNear = 0;
				uchar SquaresNear = 0;

				OldAltitudeBuffer[x][y] = AltitudeBuffer[x][y];

				DO_FOR_SQUARES_AROUND_IN_TWO_PARTS(x, y, XSize, YSize, HeightNear += OldAltitudeBuffer[DoX][DoY], HeightNear += AltitudeBuffer[DoX][DoY], SquaresNear++)

				AltitudeBuffer[x][y] = HeightNear / SquaresNear;
			}
	}

	delete [] OldAltitudeBuffer;
}

#define MAX_TEMPERATURE			27		//increase for more tropical world
#define LATITUDE_EFFECT			40		//increase for more effect
#define ALTITUDE_EFFECT			0.02

#define COLD				10
#define NORMAL				12
#define WARM				17
#define HOT				19

void worldmap::GenerateClimate()
{
	for(ushort y = 0; y < YSize; y++)
	{
		float DistanceFromEquator = fabs(float(y) / YSize - 0.5f);

		bool LatitudeRainy = DistanceFromEquator <= 0.05 || (DistanceFromEquator > 0.25 && DistanceFromEquator <= 0.45) ? true : false;

		for(ushort x = 0; x < XSize; x++)
		{
			if(AltitudeBuffer[x][y] <= 0)
			{
				TypeBuffer[x][y] = ocean::StaticType();
				continue;
			}

			bool Rainy = LatitudeRainy;

			if(!Rainy)
				DO_FOR_SQUARES_AROUND(x, y, XSize, YSize, if(AltitudeBuffer[DoX][DoY] <= 0) { Rainy = true; break; })

			char Temperature = char(MAX_TEMPERATURE - DistanceFromEquator * LATITUDE_EFFECT - AltitudeBuffer[x][y] * ALTITUDE_EFFECT);

			if(Temperature <= COLD)
				if(Rainy)
					TypeBuffer[x][y] = snow::StaticType();
				else
					TypeBuffer[x][y] = glacier::StaticType();

			if(Temperature > COLD && Temperature <= NORMAL)
				if(Rainy)
					TypeBuffer[x][y] = evergreenforest::StaticType();
				else
					TypeBuffer[x][y] = snow::StaticType();

			if(Temperature > NORMAL && Temperature <= WARM)
				if(Rainy)
					TypeBuffer[x][y] = leafyforest::StaticType();
				else
					TypeBuffer[x][y] = steppe::StaticType();

			if(Temperature > WARM && Temperature <= HOT)
				if(Rainy)
					TypeBuffer[x][y] = leafyforest::StaticType();
				else
					TypeBuffer[x][y] = desert::StaticType();

			if(Temperature > HOT)
				if(Rainy)
					TypeBuffer[x][y] = jungle::StaticType();
				else
					TypeBuffer[x][y] = desert::StaticType();
		}
	}
}

void worldmap::SmoothClimate()
{
	OldTypeBuffer = Alloc2D<ushort>(XSize, YSize);

	for(ushort c = 0; c < 3; ++c)
		for(ushort y = 0; y < YSize; y++)
			for(ushort x = 0, NewType; x < XSize; x++)
				if((OldTypeBuffer[x][y] = TypeBuffer[x][y]) != ocean::StaticType() && (NewType = WhatTerrainIsMostCommonAroundCurrentTerritorySquareIncludingTheSquareItself(x, y)))
					TypeBuffer[x][y] = NewType;

	for(ushort x = 0; x < XSize; x++)
		for(ushort y = 0; y < YSize; y++)
			Map[x][y]->ChangeWorldMapTerrain(protocontainer<groundworldmapterrain>::GetProto(TypeBuffer[x][y])->Clone(), new atmosphere);

	delete [] OldTypeBuffer;
}

ushort worldmap::WhatTerrainIsMostCommonAroundCurrentTerritorySquareIncludingTheSquareItself(ushort x, ushort y)
{
	static ushort Types = protocontainer<groundworldmapterrain>::GetProtoAmount() + 1;
	static uchar* Type = new uchar[Types];

	for(ushort n = 0; n < Types; n++)
		Type[n] = 0;

	DO_FOR_SQUARES_AROUND_IN_TWO_PARTS(x, y, XSize, YSize, Type[OldTypeBuffer[DoX][DoY]]++, Type[TypeBuffer[DoX][DoY]]++, ;)

	Type[TypeBuffer[x][y]]++;

	uchar MostCommon = 0;

	for(ushort c = 1; c < Types; ++c)
		if(Type[c] > Type[MostCommon] && c != ocean::StaticType())
			MostCommon = c;

	return MostCommon;
}

void worldmap::CalculateContinents()
{
	for(ushort x = 0; x < XSize; x++)
		for(ushort y = 0; y < YSize; y++)
			if(AltitudeBuffer[x][y] > 0)
			{
				bool Attached = false;

				DO_FOR_SQUARES_AROUND(x, y, XSize, YSize,
				{
					if(ContinentBuffer[DoX][DoY])
					{
						if(ContinentBuffer[x][y])
						{
							if(ContinentBuffer[x][y] != ContinentBuffer[DoX][DoY])
								if(Continent[ContinentBuffer[x][y]]->Size() < Continent[ContinentBuffer[DoX][DoY]]->Size())
									Continent[ContinentBuffer[x][y]]->AttachTo(Continent[ContinentBuffer[DoX][DoY]]);
								else
									Continent[ContinentBuffer[DoX][DoY]]->AttachTo(Continent[ContinentBuffer[x][y]]);
						}
						else
							Continent[ContinentBuffer[DoX][DoY]]->Add(vector2d(x, y));

						Attached = true;
					}
				})

				if(!Attached)
				{
					if(Continent.size() == 255)
					{
						RemoveEmptyContinents();

						if(Continent.size() == 255)
							ABORT("Valpuri shall not carry more continents!");
					}

					continent* NewContinent = new continent(Continent.size());
					NewContinent->Add(vector2d(x, y));
					Continent.push_back(NewContinent);
				}
			}

	RemoveEmptyContinents();

	for(uchar c = 1; c < Continent.size(); ++c)
		Continent[c]->GenerateInfo();
}

void worldmap::RemoveEmptyContinents()
{
	for(uchar c = 1; c < Continent.size(); ++c)
		if(!Continent[c]->Size())
			for(uchar i = Continent.size() - 1; i >= c; i--)
				if(Continent[i]->Size())
				{
					Continent[i]->AttachTo(Continent[c]);
					delete Continent[i];
					Continent.pop_back();
					break;
				}
				else
				{
					delete Continent[i];
					Continent.pop_back();
				}
}
