/*
 *
 *  Iter Vehemens ad Necem 
 *  Copyright (C) Timo Kiviluoto
 *  See LICENSING which should included with this file
 *
 */

#define __FILE_OF_STATIC_MATERIAL_PROTOTYPE_DEFINITIONS__

#include "proto.h"
#include "materia.h"

materialprototype** protocontainer<material>::ProtoData;
valuemap protocontainer<material>::CodeNameMap;
int protocontainer<material>::Size;
materialprototype material_ProtoType(0, 0, "material");
const materialprototype* material::GetProtoType() const { return &material_ProtoType; }

#include "materias.h"

#undef __FILE_OF_STATIC_MATERIAL_PROTOTYPE_DEFINITIONS__

#include "char.h"
#include "database.h"
#include "confdef.h"
#include "message.h"
#include "save.h"
#include "fluid.h"
#include "smoke.h"
#include "bitmap.h"
#include "game.h"
#include "rawbit.h"
#include "whandler.h"
#include "rain.h"

#include "materia.cpp"
#include "materias.cpp"
#include "fluid.cpp"
#include "smoke.cpp"
#include "rain.cpp"
