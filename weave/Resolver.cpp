#include "Hooks.h"
#include "Resolver.h"
#include "RageBacktracking.h"
#include "Ragebot.h"
#include "AnimationFix.h"


std::string ResolverMode[65];
int last_ticks[65];
int IBasePlayer::GetChokedPackets() {
    auto ticks = TIME_TO_TICKS(GetSimulationTime() - GetOldSimulationTime());
    if (ticks == 0 && last_ticks[GetIndex()] > 0) {
        return last_ticks[GetIndex()] - 1;
    }
    else {
        last_ticks[GetIndex()] = ticks;
        return ticks;
    }
}

float CResolver::GetAngle(IBasePlayer* player) {
    return Math::NormalizeYaw(player->GetEyeAngles().y);
}

float CResolver::GetForwardYaw(IBasePlayer* player) {
    return Math::NormalizeYaw(GetBackwardYaw(player) - 180.f);
}

float CResolver::GetBackwardYaw(IBasePlayer* player) {
    return Math::CalculateAngle(csgo->local->GetOrigin(), player->GetOrigin()).y;
}

float CResolver::GetLeftYaw(IBasePlayer* player) {
    return Math::NormalizeYaw(Math::CalculateAngle(csgo->local->GetOrigin(), player->GetOrigin()).y - 90.f);
}

float CResolver::GetRightYaw(IBasePlayer* player) {
    return Math::NormalizeYaw(Math::CalculateAngle(csgo->local->GetOrigin(), player->GetOrigin()).y + 90.f);
}
bool CResolver::TargetJitter(IBasePlayer* player, bool v2) {
    float yaw = v2 ? GetRightYaw(player) : GetLeftYaw(player);
    return fabsf(GetAngle(player) - Math::NormalizeYaw(yaw + 90.f))
        >= fabsf(GetAngle(player) - Math::NormalizeYaw(yaw - 90.f));
}
bool CResolver::TargetSide(IBasePlayer* player) {
    float yaw = Math::NormalizeYaw(GetBackwardYaw(player));
    float angle = GetAngle(player);
    return fabsf(angle - Math::NormalizeYaw(yaw + 90.f))
        >= fabsf(angle - Math::NormalizeYaw(yaw - 90.f));
}

bool CResolver::DoesHaveJitter(IBasePlayer* player, int* new_side) {
    static float LastAngle[64];
    static int LastBrute[64];
    static bool Switch[64];
    static float LastUpdateTime[64];

    int i = player->GetIndex();

    float CurrentAngle = player->GetEyeAngles().y;
    if (!Math::IsNearEqual(CurrentAngle, LastAngle[i], 50.f)) {
        Switch[i] = !Switch[i];
        LastAngle[i] = CurrentAngle;
        *new_side = Switch[i] ? 1 : -1;
        LastBrute[i] = *new_side;
        LastUpdateTime[i] = interfaces.global_vars->curtime;
        return true;
    }
    else {
        if (fabsf(LastUpdateTime[i] - interfaces.global_vars->curtime >= TICKS_TO_TIME(17))
            || player->GetSimulationTime() != player->GetOldSimulationTime()) {
            LastAngle[i] = CurrentAngle;
        }
        *new_side = LastBrute[i];
    }
    return false;
}


void CResolver::StoreAntifreestand()
{
    if (!csgo->local->isAlive())
        return;

    if (!csgo->weapon->IsGun())
        return;


    for (int i = 1; i < interfaces.engine->GetMaxClients(); ++i)
    {
        auto player = interfaces.ent_list->GetClientEntity(i);

        if (!player || !player->isAlive() || player->IsDormant() || player->GetTeam() == csgo->local->GetTeam())
            continue;

        bool Autowalled = false, HitSide1 = false, HitSide2 = false;
        auto idx = player->GetIndex();
        float angToLocal = Math::CalculateAngle(csgo->local->GetOrigin(), player->GetOrigin()).y;
        Vector ViewPoint = csgo->local->GetOrigin() + Vector(0, 0, 90);
        Vector2D Side1 = { (45 * sin(DEG2RAD(angToLocal))),(45 * cos(DEG2RAD(angToLocal))) };
        Vector2D Side2 = { (45 * sin(DEG2RAD(angToLocal + 180))) ,(45 * cos(DEG2RAD(angToLocal + 180))) };

        Vector2D Side3 = { (50 * sin(DEG2RAD(angToLocal))),(50 * cos(DEG2RAD(angToLocal))) };
        Vector2D Side4 = { (50 * sin(DEG2RAD(angToLocal + 180))) ,(50 * cos(DEG2RAD(angToLocal + 180))) };

        Vector Origin = player->GetOrigin();

        Vector2D OriginLeftRight[] = { Vector2D(Side1.x, Side1.y), Vector2D(Side2.x, Side2.y) };

        Vector2D OriginLeftRightLocal[] = { Vector2D(Side3.x, Side3.y), Vector2D(Side4.x, Side4.y) };

        for (int side = 0; side < 2; side++)
        {
            Vector OriginAutowall = { Origin.x + OriginLeftRight[side].x,  Origin.y - OriginLeftRight[side].y , Origin.z + 90 };
            Vector ViewPointAutowall = { ViewPoint.x + OriginLeftRightLocal[side].x,  ViewPoint.y - OriginLeftRightLocal[side].y , ViewPoint.z };

            if (g_AutoWall.CanHitFloatingPoint(OriginAutowall, ViewPoint))
            {
                if (side == 0)
                {
                    HitSide1 = true;
                    FreestandSide[idx] = -1;
                }
                else if (side == 1)
                {
                    HitSide2 = true;
                    FreestandSide[idx] = 1;
                }

                Autowalled = true;
            }
            else
            {
                for (int sidealternative = 0; sidealternative < 2; sidealternative++)
                {
                    Vector ViewPointAutowallalternative = { Origin.x + OriginLeftRight[sidealternative].x,  Origin.y - OriginLeftRight[sidealternative].y , Origin.z + 90 };

                    if (g_AutoWall.CanHitFloatingPoint(ViewPointAutowallalternative, ViewPointAutowall))
                    {
                        if (sidealternative == 0)
                        {
                            HitSide1 = true;
                            FreestandSide[idx] = -1;

                        }
                        else if (sidealternative == 1)
                        {
                            HitSide2 = true;
                            FreestandSide[idx] = 1;

                        }

                        Autowalled = true;
                    }
                }
            }
        }
    }
}


void CResolver::Do(IBasePlayer* player) {
    auto animstate = player->GetPlayerAnimState();
    if (!animstate)
        return;
    animstate->m_flGoalFeetYaw = GetAngle(player);
    if (!vars.ragebot.resolver)
        return;
    if (!csgo->local->isAlive())
        return;
    if (player->GetChokedPackets() <= 0)
        return;

    int idx = player->GetIndex();

    if (player->GetPlayerInfo().fakeplayer)
        return;

    if (!(player->GetFlags() & FL_ONGROUND))
        return;

    float angle = GetAngle(player);
    int new_side = 0;
    if (DoesHaveJitter(player, &new_side) && player->GetEyeAngles().x < 45) {
        switch (csgo->missedshots[idx] % 2) {
        case 0:
            ResolverMode[idx] = "Detect Jitter";
            animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle + 180.f * new_side);
            break;
        case 1:
            ResolverMode[idx] = "Detect Jitter";
            animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle - 180.f * new_side);
            break;
        }
    }
    else if (DoesHaveJitter(player, &new_side) && player->GetEyeAngles().x > 15) 
    {
        switch (csgo->missedshots[idx] % 2) {
        case 0:
            ResolverMode[idx] = "Detect Dodgebrute";
            animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle - 17);
            break;
        }
    }
    else
    {
        bool forward = fabsf(Math::NormalizeYaw(GetAngle(player) - GetForwardYaw(player))) < 90.f;
        if (csgo->missedshots[idx] > 4) {
            ResolverMode[idx] = "Detect sides";

            if (forward) {
                FreestandSide[idx] *= -1;
                ResolverMode[idx] += ": 2 mode";
            }
            else
                ResolverMode[idx] += ": 1 mode";
            animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle - 90.f * FreestandSide[idx]);
        }
        else {
            if (forward) {
                switch (csgo->missedshots[idx] % 3) {
                case 1:
                    ResolverMode[idx] = "Bruteforce";
                    if (FreestandSide[idx] == 1)
                        animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle - 45.f);
                    else
                        animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle + 45.f);
                    break;
                case 2:
                    ResolverMode[idx] = "Bruteforce";
                    if (FreestandSide[idx] == 1)
                        animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle + 45.f);
                    else
                        animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle - 45.f);
                    break;
                case 3:
                    ResolverMode[idx] = "Dodge bruteforce";
                    animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle - 17.f);
                    break;

                }
            }
            else {
                switch (csgo->missedshots[idx] % 3) {
                case 1:
                    ResolverMode[idx] = "Bruteforce";
                    if (FreestandSide[idx] == 1)
                        animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle - 58.f);
                    else
                        animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle + 58.f);
                    break;
                case 2:
                    ResolverMode[idx] = "Bruteforce";
                    if (FreestandSide[idx] == 1)
                        animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle + 58.f);
                    else
                        animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle - 58.f);
                    break;
                case 3:
                    ResolverMode[idx] = "Dodge Bruteforce";
                    animstate->m_flGoalFeetYaw = Math::NormalizeYaw(angle - 17.f);
                    break;
                }
            }
        }
    }

    animstate->m_flGoalFeetYaw = Math::NormalizeYaw(animstate->m_flGoalFeetYaw);
}