#include "materde.h"
#include "charba.h"
#include "message.h"

void schoolfood::EatEffect(character* Eater, float Amount, float NPModifier)
{
	Eater->ReceiveSchoolFoodEffect(Volume * Amount / 100);
	NormalFoodEffect(Eater, Amount, NPModifier);
	MinusAmount(Amount);
}

void schoolfood::HitEffect(character* Enemy)
{
	Enemy->ReceiveSchoolFoodEffect(Volume);
}

void darkfrogflesh::EatEffect(character* Eater, float Amount, float NPModifier)
{
	Eater->Darkness(Volume * Amount / 100);
	NormalFoodEffect(Eater, Amount, NPModifier);
	MinusAmount(Amount);
}

void darkfrogflesh::HitEffect(character* Enemy)
{
	Enemy->Darkness(Volume);
}

void omleurine::EatEffect(character* Eater, float Amount, float NPModifier)
{
	Eater->ReceiveOmleUrineEffect(Volume * Amount / 100);
	NormalFoodEffect(Eater, Amount, NPModifier);
	MinusAmount(Amount);
}

void omleurine::HitEffect(character* Enemy)
{
	Enemy->ReceiveOmleUrineEffect(Volume);
}

void pepsi::EatEffect(character* Eater, float Amount, float NPModifier)
{
	Eater->ReceivePepsiEffect(Volume * Amount / 100);
	NormalFoodEffect(Eater, Amount, NPModifier);
	MinusAmount(Amount);
}

void pepsi::HitEffect(character* Enemy)
{
	Enemy->ReceivePepsiEffect(Volume);
}

void bone::EatEffect(character* Eater, float Amount, float NPModifier)
{
	Eater->SetRelations(NEUTRAL);
	if(Eater == game::GetPlayer())
		ADD_MESSAGE("You feel like a hippie.");
	else
		ADD_MESSAGE("%s doesn't seem to care about you anymore.", Eater->CNAME(DEFINITE));
	NormalFoodEffect(Eater, Amount, NPModifier);
	MinusAmount(Amount);
}
