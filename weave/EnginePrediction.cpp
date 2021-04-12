#include "EnginePrediction.h"
#include "checksum_md5.h"
#include "AntiAims.h"

void CEnginePrediction::Start(CUserCmd* cmd, IBasePlayer* local) {
	old_vars.curtime = interfaces.global_vars->curtime;
	old_vars.frametime = interfaces.global_vars->frametime;
	old_vars.tickcount = interfaces.global_vars->tickcount;

	interfaces.global_vars->curtime = TICKS_TO_TIME(local->GetTickBase());
	interfaces.global_vars->frametime = interfaces.global_vars->interval_per_tick;
	interfaces.global_vars->tickcount = TIME_TO_TICKS(interfaces.global_vars->curtime);

	interfaces.game_movement->StartTrackPredictionErrors(local);
	interfaces.move_helper->SetHost(local);

	memset(&data, 0, sizeof(data));

	interfaces.prediction->SetupMove(local, cmd, interfaces.move_helper, &data);
	interfaces.game_movement->ProcessMovement(local, &data);
	interfaces.prediction->FinishMove(local, cmd, &data);
}

void CEnginePrediction::Finish(IBasePlayer* local) {
	interfaces.game_movement->FinishTrackPredictionErrors(local);
	interfaces.move_helper->SetHost(nullptr);

	interfaces.global_vars->curtime = old_vars.curtime;
	interfaces.global_vars->frametime = old_vars.frametime;
}