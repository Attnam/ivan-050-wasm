#define __FILE_OF_STATIC_CHARACTER_PROTOTYPE_DEFINITIONS__

#include "proto.h"
#include "char.h"

std::vector<character::prototype*> protocontainer<character>::ProtoData;
valuemap protocontainer<character>::CodeNameMap;
characterprototype character_ProtoType(0, 0, "character");
const characterprototype* character::GetProtoType() const { return &character_ProtoType; }

#include "human.h"
#include "nonhuman.h"

#undef __FILE_OF_STATIC_CHARACTER_PROTOTYPE_DEFINITIONS__

#include "stack.h"
#include "message.h"
#include "actions.h"
#include "wterras.h"
#include "colorbit.h"
#include "team.h"
#include "config.h"
#include "god.h"
#include "festring.h"
#include "felist.h"
#include "miscitem.h"
#include "gear.h"
#include "confdef.h"
#include "room.h"
#include "game.h"
#include "graphics.h"
#include "wskill.h"
#include "materia.h"
#include "bitmap.h"
#include "lterras.h"

#include "human.cpp"
#include "nonhuman.cpp"
