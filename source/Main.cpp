#include "plugin.h"
#include "game_vc\CClock.h"
#include "game_vc\CPickups.h"
#include "game_vc\ePickupType.h"
#include "game_vc\CWorld.h"
#include "game_vc\CHud.h"
#include "game_vc\CGame.h"
#include "game_vc\CTheScripts.h"

using namespace plugin;

class AssetAutocollector {
public:
    AssetAutocollector() {
        static CdeclEvent<AddressList<0x4A44F7, H_CALL>, PRIORITY_AFTER, ArgPickNone, void()> myOnClockUpdate;
        static unsigned int lastStoredHour = 0;
        static unsigned int lastStoredMinute = 0;
        static unsigned int numAssets = 0;
        static unsigned int totalMoney = 0;
        static bool playerCantReceive = true;

        myOnClockUpdate.after += [] {
            if (CGame::currArea != 0 || CTheScripts::IsPlayerOnAMission() || CWorld::Players[0].m_nPlayerState != PLAYERSTATE_PLAYING) {
                playerCantReceive = true;
            }
            else {
                playerCantReceive = false;
                totalMoney = 0;
                numAssets = 0;
            }
            if (!playerCantReceive) {
                for (unsigned int i = 0; i < 336; ++i) {
                    if (CPickups::aPickUps[i].bPickupType == PICKUP_ASSET_REVENUE) {
                        static unsigned int revenue = CPickups::aPickUps[i].dwPickupQuantity;
                        if (revenue >= 1500 && revenue <= 10000) {
                            totalMoney += revenue;
                            CPickups::aPickUps[i].fStandProximity = 0.0f;
                            ++numAssets;
                        }
                    }
                }
            }

            if (CClock::ms_nGameClockHours < lastStoredHour && lastStoredHour >= 12 && (lastStoredMinute == CClock::ms_nGameClockMinutes || CClock::ms_nGameClockMinutes - lastStoredMinute == 1 || (lastStoredMinute == 59 && CClock::ms_nGameClockMinutes == 0 && (CClock::ms_nGameClockHours - lastStoredHour == 1 || lastStoredHour == 23 && CClock::ms_nGameClockHours == 0)))) {
                if (totalMoney > 0) {
                    CWorld::Players[0].m_nMoney += totalMoney;
                    static char text[256];
                    if (numAssets < 10) {
                        sprintf(text, "You have earned $%d from %d/10 assets.", totalMoney, numAssets);
                    }
                    else {
                        sprintf(text, "You have earned $%d from 10 assets.", totalMoney);
                    }
                    CHud::SetHelpMessage(text, true, false);
                }
            }
            lastStoredHour = CClock::ms_nGameClockHours;
            lastStoredMinute = CClock::ms_nGameClockMinutes;
            };
    }
} AssetAutocollectorPlugin;