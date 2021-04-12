#include "Hooks.h"
#include "AntiAims.h"
#include "Ragebot.h"

bool need_to_flip = false;

bool CanDT() {
	int idx = csgo->weapon->GetItemDefinitionIndex();
	return csgo->local->isAlive() && csgo->weapon->DTable()
		&& csgo->client_state->iChokedCommands <= 1 
		&& idx != WEAPON_REVOLVER
		&& idx != WEAPON_ZEUSX27
		&& vars.ragebot.double_tap->active && !csgo->fake_duck;
}

bool CanHS() {
	int idx = csgo->weapon->GetItemDefinitionIndex();
	return csgo->local->isAlive() && !csgo->weapon->IsMiscWeapon() && csgo->client_state->iChokedCommands
		&& vars.ragebot.hide_shots->active && !csgo->fake_duck;
}

void CMAntiAim::Fakelag(bool& send_packet)
{
	if (!vars.antiaim.enable)
		return;

	bool dt = CanDT();
	bool hs = CanHS();

	bool exp = dt || hs;

	if (csgo->fake_duck && csgo->local->GetFlags() & FL_ONGROUND && !(csgo->cmd->buttons & IN_JUMP))
	{
		if (csgo->local->GetFlags() & FL_ONGROUND)
			return;
	}

	if (CanHS()
		|| interfaces.engine->IsVoiceRecording()) {
		csgo->max_fakelag_choke = 1;
		return;
	}

	if (dt && did_shot)
		return;

	if ((csgo->cmd->buttons & IN_ATTACK) && !vars.antiaim.fakelag_onshot) {
		send_packet = true;
		csgo->max_fakelag_choke = exp ? 1 : 2;
		return;
	}

	if (vars.antiaim.disable_fakelag_when_stand)
	{
		if (exp || csgo->local->GetVelocity().Length2D() < 10.f)
		{
			send_packet = exp ? csgo->client_state->iChokedCommands >= 1 : csgo->client_state->iChokedCommands >= 1;

			csgo->max_fakelag_choke = 1/*exp ? 1 : vars.antiaim.break_lby ? 2 : 1*/;
			return;
		}
	}

	else
	{
		if (exp)
		{
			send_packet = exp ? csgo->client_state->iChokedCommands >= 1 : csgo->client_state->iChokedCommands >= 1;

			csgo->max_fakelag_choke = 1/*exp ? 1 : vars.antiaim.break_lby ? 2 : 1*/;
			return;
		}
	}

	auto animstate = csgo->local->GetPlayerAnimState();
	if (!animstate)
		return;

	int tick_to_choke = 1;
	csgo->max_fakelag_choke = /*csgo->game_rules->IsValveDS() ? 6 :*/ 14;

	static Vector oldOrigin;

	if (!(csgo->local->GetFlags() & FL_ONGROUND))
	{
		csgo->canDrawLC = true;
	}
	else {
		csgo->canDrawLC = false;
		csgo->canBreakLC = false;
	}

	if (vars.antiaim.fakelag < 0)
		tick_to_choke = 2;
	else
	{
		switch (vars.antiaim.fakelag)
		{
		case 0:
			tick_to_choke = 1;
			break;
		case 1:
			tick_to_choke = vars.antiaim.fakelagfactor;
			break;
		case 2:
		{
			int factor = vars.antiaim.fakelagvariance;
			if (factor == 0)
				factor = 15;
			else if (factor > 100)
				factor = 100;

			if (csgo->cmd->command_number % factor < vars.antiaim.fakelagfactor)
				tick_to_choke = min(vars.antiaim.fakelagfactor, csgo->max_fakelag_choke);
			else
				tick_to_choke = 1;
		}
		break;
		}
	}

	if (tick_to_choke < 1)
		tick_to_choke = 1;

	if (tick_to_choke > csgo->max_fakelag_choke)
		tick_to_choke = csgo->max_fakelag_choke;

	send_packet = csgo->client_state->iChokedCommands >= tick_to_choke;

	static Vector sent_origin = Vector();

	if (csgo->canDrawLC) {
		if (send_packet)
			sent_origin = csgo->local->GetAbsOrigin();

		if ((sent_origin - oldOrigin).LengthSqr() > 4096.f) {
			csgo->canBreakLC = true;
		}
		else
			csgo->canBreakLC = false;

		if (send_packet)
			oldOrigin = csgo->local->GetAbsOrigin();
	}
}

void CMAntiAim::Pitch()
{
	csgo->cmd->viewangles.x = 89;
}
void CMAntiAim::Sidemove() {
	if (!csgo->should_sidemove)
		return;

	float sideAmount = 2 * ((csgo->cmd->buttons & IN_DUCK || csgo->cmd->buttons & IN_WALK) ? 3.f : 0.505f);
	if (csgo->local->GetVelocity().Length2D() <= 0.f || std::fabs(csgo->local->GetVelocity().z) <= 100.f)
		csgo->cmd->sidemove += csgo->cmd->command_number % 2 ? sideAmount : -sideAmount;
}

float GetCurtime() {
	if (!csgo->local)
		return 0;
	int g_tick = 0;
	CUserCmd* g_pLastCmd = nullptr;
	if (!g_pLastCmd || g_pLastCmd->hasbeenpredicted) {
		g_tick = csgo->local->GetTickBase();
	}
	else {
		++g_tick;
	}
	g_pLastCmd = csgo->cmd;
	float curtime = g_tick * interfaces.global_vars->interval_per_tick;
	return curtime;
}

bool UpdateLBY(const float yaw_to_break, CUserCmd* cmd)
{
	static float next_lby_update_time = 0;
	const float current_time = interfaces.global_vars->interval_per_tick * csgo->local->GetTickBase()/*GetCurtime()*/;

	if (csgo->should_sidemove || csgo->local->GetVelocity().Length2D() > 0.1)
	{
		next_lby_update_time = current_time + TICKS_TO_TIME(1);
		return false;
	}
	else {
		if (next_lby_update_time < current_time)
		{
			next_lby_update_time = current_time + 1.1f;
			return true;
		}
	}
	return false;
}

void CMAntiAim::Yaw(bool& send_packet)
{
	csgo->should_sidemove = true;

	static bool swap = false;

	if (vars.antiaim.aa_override.inverter->active)
	{
		if (!swap)
		{
			swap = true;
		}
	}

	else
	{
		if (swap)
		{
			swap = false;
		}
	}

	if (!vars.antiaim.jitter && !vars.antiaim.lagsync)
	{
		if (vars.antiaim.desync == 1)
		{
			if (!swap)
			{
				if (!csgo->send_packet)
				{
					csgo->cmd->viewangles.y += 116.f;
				}
			}

			else
			{
				if (!csgo->send_packet)
				{
					csgo->cmd->viewangles.y -= 116.f;
				}
			}
		}

		if (vars.antiaim.desync == 2)
		{
			static auto alternate = false;

			if (!swap)
			{
				if (!csgo->send_packet)
					csgo->cmd->viewangles.y = std::remainderf(csgo->cmd->viewangles.y + 120.f * 1, 360.f);
			}

			else
			{
				if (!csgo->send_packet)
					csgo->cmd->viewangles.y = std::remainderf(csgo->cmd->viewangles.y + 120.f * -1, 360.f);
			}

			float sideAmount = 2.f * ((csgo->cmd->buttons & IN_DUCK || csgo->cmd->buttons & IN_WALK) ? 3.f : 1.f);

			if (csgo->local->GetVelocity().Length2D() <= 0.f || std::fabs(csgo->local->GetVelocity().z <= 100.f))
				csgo->cmd->sidemove += alternate ? sideAmount : -sideAmount;

			alternate = !alternate;
		}

		if (vars.antiaim.desync == 3)
		{

			if (!swap)
			{
				if (!csgo->send_packet)
				{
					csgo->cmd->viewangles.y -= 120;
				}

				static auto jitterrun = 0;
				jitterrun++;
				if (jitterrun < 3)
				{
					if (jitterrun == 0)
					{
						csgo->cmd->viewangles.y += 180.f;
					}

					if (jitterrun == 1)
					{
						csgo->cmd->viewangles.y -= 90.f;
					}

					if (jitterrun == 2)
					{
						csgo->cmd->viewangles.y -= 180.f;
					}
				}
				else
					jitterrun = 0;
			}

			else
			{
				if (!csgo->send_packet)
				{
					csgo->cmd->viewangles.y += 120;
				}

				static auto jitterrun = 0;
				jitterrun++;
				if (jitterrun < 3)
				{
					if (jitterrun == 0)
					{
						csgo->cmd->viewangles.y += 180.f;
					}

					if (jitterrun == 1)
					{
						csgo->cmd->viewangles.y += 90.f;
					}

					if (jitterrun == 2)
					{
						csgo->cmd->viewangles.y -= 180.f;
					}
				}
				else
					jitterrun = 0;
			}
		}

		if (vars.antiaim.desync == 4)
		{
			if (UpdateLBY)
			{
				send_packet = false; // choke

				if (!swap)
				{
					csgo->cmd->viewangles.y += (csgo->local->GetDSYDelta() * 2);
				}

				else
				{
					csgo->cmd->viewangles.y += (csgo->local->GetDSYDelta() * 2) * -1;
				}

				csgo->cmd->viewangles.y = Math::NormalizeYaw(csgo->cmd->viewangles.y);

				return;
			}

			else
			{
				if (!swap)
				{
					if (!csgo->send_packet)
					{
						csgo->cmd->viewangles.y += 58.f;
					}
				}

				else
				{
					if (!csgo->send_packet)
					{
						csgo->cmd->viewangles.y -= 58.f;
					}
				}
			}
		}

		if (vars.antiaim.desync == 5)
		{
			csgo->should_sidemove = true;
			if (!swap)
			{
				if (!send_packet)
					csgo->cmd->viewangles.y += 25.f;
			}

			else
			{
				if (!send_packet)
					csgo->cmd->viewangles.y -= 25.f;
			}
		}
	}

	if (csgo->send_packet)
		need_to_flip = !need_to_flip;

	if (vars.antiaim.lagsync)
	{
		if (!swap)
		{
			if (!csgo->send_packet)
			{
				static auto lagsync = 0;
				lagsync++;
				if (lagsync < 5)
				{
					if (lagsync == 0)
					{
						csgo->cmd->viewangles.y += 38.f;
					}

					if (lagsync == 1)
					{
						csgo->cmd->viewangles.y += 49.f;
					}

					if (lagsync == 2)
					{
						csgo->cmd->viewangles.y += 58.f;
					}

					if (lagsync == 3)
					{
						csgo->cmd->viewangles.y += 120.f;
					}

					if (lagsync == 4)
					{
						csgo->cmd->viewangles.y += 67.f;
					}
				}
				else
					lagsync = 0;
			}
		}

		else
		{
			if (!csgo->send_packet)
			{
				static auto lagsync = 0;
				lagsync++;
				if (lagsync < 5)
				{
					if (lagsync == 0)
					{
						csgo->cmd->viewangles.y -= 38.f;
					}

					if (lagsync == 1)
					{
						csgo->cmd->viewangles.y -= 49.f;
					}

					if (lagsync == 2)
					{
						csgo->cmd->viewangles.y -= 58.f;
					}

					if (lagsync == 3)
					{
						csgo->cmd->viewangles.y -= 120.f;
					}

					if (lagsync == 4)
					{
						csgo->cmd->viewangles.y -= 67.f;
					}
				}
				else
					lagsync = 0;
			}
		}
	}

	if (vars.antiaim.jitter)
	{
		csgo->cmd->viewangles.y += 180.f;
		need_to_flip ? csgo->cmd->viewangles.y = std::remainderf(csgo->cmd->viewangles.y + 160.f * 1, 360.f) : csgo->cmd->viewangles.y = std::remainderf(csgo->cmd->viewangles.y + 145.f * -1, 360.f);
	}

	if (vars.antiaim.aa_override.enable)
	{
		static bool left, right, back;
		if (vars.antiaim.aa_override.left->active)
		{
			left = true;
			right = false;
			back = false;
		}
		else if (vars.antiaim.aa_override.right->active)
		{
			left = false;
			right = true;
			back = false;
		}
		else if (vars.antiaim.aa_override.back->active)
		{
			left = false;
			right = false;
			back = true;
		}

		if (left)
			csgo->cmd->viewangles.y -= 90;
		if (right)
			csgo->cmd->viewangles.y += 90;
	}
	csgo->cmd->viewangles.y += 180;
}

void CMAntiAim::Run(bool& send_packet)
{
	if (vars.antiaim.slowwalk->active || csgo->should_stop_slide)
	{
		const auto weapon = csgo->weapon;

		if (weapon) {

			const auto info = csgo->weapon->GetCSWpnData();

			float speed = 0.1f;
			if (info) {
				float max_speed = weapon->GetZoomLevel() == 0 ? info->m_flMaxSpeed : info->m_flMaxSpeedAlt;
				float ratio = max_speed / 250.0f;
				speed *= ratio;
			}


			csgo->cmd->forwardmove *= speed;
			csgo->cmd->sidemove *= speed;
		}

		//csgo->should_stop_fast = false;
	}

	shouldAA = true;
	if (!vars.antiaim.enable) {
		shouldAA = false;
		return;
	}
	if (csgo->cmd->buttons & IN_USE)
	{
		shouldAA = false;
		return;
	}
	if (csgo->game_rules->IsFreezeTime()
		|| csgo->local->GetMoveType() == MOVETYPE_NOCLIP
		|| csgo->local->GetMoveType() == MOVETYPE_LADDER)
	{
		shouldAA = false;
		return;
	}
	bool shit = false;
	for (int i = 1; i < 65; i++)
	{
		auto ent = interfaces.ent_list->GetClientEntity(i);
		if (!ent)
			continue;
		if (
			!ent->isAlive()
			|| ent == csgo->local
			|| ent->GetTeam() == csgo->local->GetTeam()
			)
			continue;
		shit = true;
		break;

	}
	if (!shit)
	{
		if (csgo->ForceOffAA)
		{
			shouldAA = false;
			return;
		}
	}
	if (csgo->weapon->GetItemDefinitionIndex() == WEAPON_REVOLVER)
	{
		if (Ragebot::Get().shot /*&& Ragebot::Get().IsAbleToShoot()*/)
		{
			shouldAA = false;
			return;
		}
	}
	else
	{
		if (F::Shooting() || csgo->TickShifted || (CanDT() && csgo->cmd->buttons & IN_ATTACK /*&& Ragebot::Get().IsAbleToShoot()*/))
		{
			shouldAA = false;
			return;
		}
		if (csgo->weapon->IsKnife()) {
			if ((csgo->cmd->buttons & IN_ATTACK || csgo->cmd->buttons & IN_ATTACK2) && Ragebot::Get().IsAbleToShoot())
			{
				shouldAA = false;
				return;
			}
		}
	}
	if (vars.antiaim.attarget)
	{
		auto best_fov = FLT_MAX;
		IBasePlayer* best_player = nullptr;

		Vector viewangles;
		interfaces.engine->GetViewAngles(viewangles);

		for (int i = 1; i < 65; i++)
		{
			auto ent = interfaces.ent_list->GetClientEntity(i);
			if (!ent)
				continue;
			if (
				!ent->isAlive()
				|| ent == csgo->local
				|| ent->GetTeam() == csgo->local->GetTeam()
				)
				continue;
			float dist = ent->GetOrigin().DistTo(csgo->local->GetOrigin());

			if (dist < best_fov)
			{
				best_player = ent;
				best_fov = dist;
			}
		}
	}

	if (shouldAA)
	{
		Pitch();
		Yaw(send_packet);
	}
}