

#pragma once

bool P_GiveHealth(const int num, const int max, const bool stat);
bool P_GiveMegaHealth(const bool stat);
bool P_GiveArmor(const armortype_t armortype, const bool stat);
bool P_GiveAllCards(void);
bool P_GiveAllKeyCards(void);
bool P_GiveAllSkullKeys(void);
bool P_GiveAllCardsInMap(void);
bool P_GivePower(const int power, const bool stat);
bool P_GiveAllWeapons(void);
bool P_GiveBackpack(const bool giveammo, const bool stat);
bool P_GiveFullAmmo(void);
bool P_TakeBackpack(void);
void P_AddBonus(void);
void P_UpdateAmmoStat(const ammotype_t ammotype, const int num);
void P_UpdateArmorStat(const int num);
void P_UpdateHealthStat(const int num);
void P_KillMobj(mobj_t *target, mobj_t *inflicter, mobj_t *source, const bool telefragged);

extern int  cardsfound;
