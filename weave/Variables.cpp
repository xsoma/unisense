#include "Variables.h"
#include "Hooks.h"
#include "aes256/aes256.hpp"
#include <sstream>


unsigned char enc_key[32] = { 0x1f, 0x01, 0x02, 0x03, 0x04, 0x13, 0x05, 0x06, 0x07,0x17, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f,
									  0x10, 0x11, 0x12, 0x14, 0x15, 0x16, 0x18, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x19, 0x00 };


ByteArray encryption_key;

void CConfig::ReadConfigs(LPCTSTR lpszFileName)
{
	ConfigList.push_back(lpszFileName);
}
void CConfig::ResetToDefault()
{
	vars.ragebot.enable = false;
	vars.ragebot.autoscope = false;
	vars.ragebot.autoshoot = false;
	vars.ragebot.hitchance_consider_hitbox = false;
	vars.ragebot.posadj = false;
	vars.ragebot.backshoot_bt = false;
	vars.ragebot.resolver = false;
	vars.ragebot.onshot_invert = false;
	vars.ragebot.onshot_resolver = false;
	vars.ragebot.base_angle = false;
	vars.ragebot.shotrecord = false;
	vars.ragebot.shot_clr = color_t(255, 255, 255, 255);

	vars.ragebot.disable_dt_delay = false;
	vars.ragebot.dt_speed_boost = 5;

	vars.ragebot.override_dmg = new c_bind();
	vars.ragebot.override_dmg->key = 0;
	vars.ragebot.override_dmg->active = false;
	vars.ragebot.override_dmg->type = 1;

	vars.ragebot.force_body = new c_bind();
	vars.ragebot.force_body->key = 0;
	vars.ragebot.force_body->active = false;
	vars.ragebot.force_body->type = 1;

	vars.ragebot.double_tap = new c_bind();
	vars.ragebot.double_tap->active = false;
	vars.ragebot.double_tap->key = 0;
	vars.ragebot.double_tap->type = 1;
	vars.ragebot.hide_shots = new c_bind();
	vars.ragebot.hide_shots->active = false;
	vars.ragebot.hide_shots->type = 1;
	vars.ragebot.dt_instant = false;

	vars.misc.airstuckkey = new c_bind();
	vars.misc.airstuckkey->key = 0;
	vars.misc.airstuckkey->active = false;
	vars.misc.airstuckkey->type = 1;

	for (auto a : vars.ragebot.weapon) {
		a.enable = false;
		a.mindamage = 0;
		a.mindamage_override = 0;
		a.hitchance = 0;
		a.hitchancetype = 0;
		a.multipoint = false;
		a.quickstop = false;
		a.pointscale_head = 0.f;
		a.pointscale_body = 0.f;
		a.pointscale_limbs = 0.f;
		a.hitscan = 0;
		a.hitscan_baim = 0;
		a.baim = 0;
		a.baim_under_hp = 0;
		a.max_misses = 0;
		a.adaptive_baim = false;
	}
	
	vars.ragebot.dt_charge_delay = 0;
	vars.ragebot.extend_backtrack = false;
	vars.ragebot.active_index = 0;

	vars.antiaim.enable = false;
	
	vars.antiaim.aa_override.enable = false;

	vars.antiaim.aa_override.left = new c_bind();
	vars.antiaim.aa_override.left->active = false;
	vars.antiaim.aa_override.left->key = 0;
	vars.antiaim.aa_override.left->type = 1;

	vars.antiaim.aa_override.right = new c_bind();
	vars.antiaim.aa_override.right->active = false;
	vars.antiaim.aa_override.right->key = 0;
	vars.antiaim.aa_override.right->type = 1;

	vars.antiaim.aa_override.back = new c_bind();
	vars.antiaim.aa_override.back->active = false;
	vars.antiaim.aa_override.back->key = 0;
	vars.antiaim.aa_override.back->type = 1;

	vars.antiaim.aa_override.inverter = new c_bind();
	vars.antiaim.aa_override.inverter->active = false;
	vars.antiaim.aa_override.inverter->key = 0;
	vars.antiaim.aa_override.inverter->type = 1;

	vars.antiaim.fakelag = 0;
	vars.antiaim.fakelagfactor = 1;
	vars.antiaim.fakelagvariance = 25;
	vars.antiaim.fakelag_onshot = false;

	vars.antiaim.fakeduck = new c_bind();
	vars.antiaim.fakeduck->active = false;
	vars.antiaim.fakeduck->key = 0;
	vars.antiaim.fakeduck->type = 1;

	vars.antiaim.slowwalk = new c_bind();
	vars.antiaim.slowwalk->active = false;
	vars.antiaim.slowwalk->key = 0;
	vars.antiaim.slowwalk->type = 1;

	vars.visuals.enable = false;
	vars.visuals.dormant = false;
	vars.visuals.skeleton = false;
	vars.visuals.skeleton_color = color_t(255, 255, 255, 255);
	vars.visuals.box = false;
	vars.visuals.box_color = color_t(255, 255, 255, 255);
	vars.visuals.healthbar = false;
	vars.visuals.override_hp = false;
	vars.visuals.hp_color = color_t(0, 255, 0, 255);
	vars.visuals.weapon = false;
	vars.visuals.weapon_color = color_t(255, 255, 255, 255);
	vars.visuals.ammo = false;
	vars.visuals.ammo_color = color_t(255, 255, 255, 255);
	vars.visuals.flags = 0;
	vars.visuals.flags_color = color_t(255, 255, 255, 255);
	vars.visuals.out_of_fov = false;
	vars.visuals.out_of_fov_distance = 30;
	vars.visuals.out_of_fov_size = 30;
	vars.visuals.out_of_fov_color = color_t(255, 255, 255, 255);
	vars.visuals.hitmarker = false;
	vars.visuals.hitmarker_sound = false;
	vars.visuals.hitmarker_color = color_t(255, 255, 255, 255);
	vars.visuals.speclist = false;
	vars.visuals.nadepred = false;
	vars.visuals.nadepred_color = color_t(255, 255, 255, 255);

	vars.visuals.watermark = true;
	vars.misc.thirdperson_bind = false;

	vars.misc.thirdperson_bind = new c_bind();
	vars.misc.thirdperson_bind->active = false;
	vars.misc.thirdperson_bind->key = 0;
	vars.misc.thirdperson_bind->type = 2;

	vars.visuals.thirdperson_dist = 150;
	vars.visuals.aspect_ratio = 0;
	vars.visuals.indicators = 0;
	vars.visuals.eventlog = false;
	vars.visuals.eventlog_color = color_t(255, 255, 255, 255);
	vars.visuals.bullet_tracer = false;
	vars.visuals.bullet_tracer_type = 0;
	vars.visuals.bullet_tracer_color = color_t(255, 0, 0, 255);
	vars.visuals.bullet_impact = false;
	vars.visuals.bullet_impact_color = color_t(0, 0, 255, 255);
	vars.visuals.client_impact_color = color_t(255, 0, 0, 255);
	vars.visuals.remove = 0;
	vars.visuals.nightmode = false;
	vars.visuals.nightmode_color = color_t(43, 41, 46, 255);
	vars.visuals.nightmode_prop_color = color_t(255, 255, 255, 255);
	vars.visuals.nightmode_skybox_color = color_t(255, 255, 255, 255);
	vars.visuals.kill_effect = false;
	vars.misc.worldfov = 90;
	vars.misc.viewmodelfov = 68;

	vars.visuals.chams = false;
	vars.visuals.chamsxqz = false;
	vars.visuals.glow = false;
	vars.visuals.glowtype = 0;
	vars.visuals.local_glow = false;
	vars.visuals.glow_color = color_t(214, 192, 47, 255);
	vars.visuals.local_glow_clr = color_t(214, 192, 47, 255);
	vars.visuals.chamscolor = color_t(214, 192, 47, 255);
	vars.visuals.chamscolor_xqz = color_t(214, 92, 47, 255);
	vars.visuals.glow_col = color_t(255, 255, 255, 0);
	vars.visuals.glow_col_xqz = color_t(255, 255, 255, 0);
	vars.visuals.chamstype = 0;
	vars.visuals.metallic_clr = color_t(255, 229, 219, 150);
	vars.visuals.metallic_clr2 = color_t(255, 229, 219, 255);
	vars.visuals.phong_exponent = 0;
	vars.visuals.phong_boost = 0;
	vars.visuals.rim = 0;
	vars.visuals.overlay = 0;
	vars.visuals.overlay_xqz = 0;
	vars.visuals.chams_brightness = 100.f;

	vars.visuals.localchams = false;
	vars.visuals.localchams_color = color_t(200, 255, 82, 255);
	vars.visuals.localchams_color_glow = color_t(255, 255, 255, 0);
	vars.visuals.localchamstype = 0;
	vars.visuals.local_chams.metallic_clr = color_t(255, 229, 219, 150);
	vars.visuals.local_chams.metallic_clr2 = color_t(255, 229, 219, 255);
	vars.visuals.local_chams.phong_exponent = 0;
	vars.visuals.local_chams.phong_boost = 0;
	vars.visuals.local_chams.rim = 0;
	vars.visuals.local_chams_brightness = 100.f;
	vars.visuals.local_chams.overlay = 0;

	vars.visuals.interpolated_bt = false;
	vars.visuals.interpolated_model = false;
	vars.visuals.interpolated_dsy = false;

	for (int i = 0; i < 4; ++i) {
		vars.visuals.misc_chams[i].enable = false;
		vars.visuals.misc_chams[i].clr = color_t(255, 255, 255, 255);
		vars.visuals.misc_chams[i].glow_clr = color_t(255, 255, 255, 0);
		vars.visuals.misc_chams[i].material = 0;
		vars.visuals.misc_chams[i].metallic_clr = color_t(255, 255, 255, 150);
		vars.visuals.misc_chams[i].metallic_clr2 = color_t(255, 255, 255, 255);
		vars.visuals.misc_chams[i].phong_exponent = 0;
		vars.visuals.misc_chams[i].phong_boost = 0;
		vars.visuals.misc_chams[i].rim = 0;
		vars.visuals.misc_chams[i].chams_brightness = 100.f;
		vars.visuals.misc_chams[i].overlay = 0;
	}
	vars.misc.antiuntrusted = true;
	vars.misc.bunnyhop = false;
	vars.misc.knifebot = false;
	vars.visuals.clantagspammer = false;
	vars.misc.hold_firinganims = false;
	vars.misc.viewmodel_x = 0;
	vars.misc.viewmodel_y = 0;
	vars.misc.viewmodel_z = 0;

	vars.misc.autobuy.enable = false;
	vars.misc.autobuy.main = 0;
	vars.misc.autobuy.misc = 0;
	vars.misc.autobuy.pistol = 0;
}

void CConfig::Setup()
{
	for (unsigned char i = 0; i < 32; i++)
		encryption_key.push_back(enc_key[i]);
	ResetToDefault();
};

string CConfig::GetModuleFilePath(HMODULE hModule)
{
	string ModuleName = "";
	char szFileName[MAX_PATH] = { 0 };

	if (GetModuleFileNameA(hModule, szFileName, MAX_PATH))
		ModuleName = szFileName;

	return ModuleName;
}

string CConfig::GetModuleBaseDir(HMODULE hModule)
{
	string ModulePath = GetModuleFilePath(hModule);
	return ModulePath.substr(0, ModulePath.find_last_of("\\/"));
}

void CConfig::Save(string cfg_name)
{
	json_t configuration;

	auto& json = configuration["config"];
	json["name"] = cfg_name;

	auto& ragebot = json["ragebot"]; {
		ragebot["enable"] = vars.ragebot.enable;
		ragebot["auto_shoot"] = vars.ragebot.autoshoot;
		ragebot["auto_scope"] = vars.ragebot.autoscope;
		ragebot["delayshot"] = vars.ragebot.delayshot;
		ragebot["hitchance_consider_hitbox"] = vars.ragebot.hitchance_consider_hitbox;
		ragebot["posadj"] = vars.ragebot.posadj;
		ragebot["backshoot_bt"] = vars.ragebot.backshoot_bt;
		ragebot["resolver"] = vars.ragebot.resolver;
		ragebot["shot_record"] = vars.ragebot.shotrecord;
		ragebot["dt_instant"] = vars.ragebot.dt_instant;
		
		SaveBind(vars.ragebot.override_dmg, "override_dmg", &ragebot);
		SaveBind(vars.ragebot.double_tap, "double_tap", &ragebot);
		SaveBind(vars.ragebot.hide_shots, "hide_shots", &ragebot);
		SaveBind(vars.ragebot.force_body, "force_body", &ragebot);

		SaveColor(vars.ragebot.shot_clr, "shot_clr", &ragebot);

		auto& weapon_cfg = ragebot["weapon_cfg"];
		for (int i = 0; i < 7; i++) {
			string category = "weapon_" + std::to_string(i);
			weapon_cfg[category]["enable"] = vars.ragebot.weapon[i].enable;
			weapon_cfg[category]["mindamage"] = vars.ragebot.weapon[i].mindamage;
			weapon_cfg[category]["mindamage_override"] = vars.ragebot.weapon[i].mindamage_override;
			weapon_cfg[category]["hitchance"] = vars.ragebot.weapon[i].hitchance;
			weapon_cfg[category]["hitchance_type"] = vars.ragebot.weapon[i].hitchancetype;
			weapon_cfg[category]["multipoint"] = vars.ragebot.weapon[i].multipoint;
			weapon_cfg[category]["quickstop"] = vars.ragebot.weapon[i].quickstop;
			weapon_cfg[category]["quickstop_options"] = vars.ragebot.weapon[i].quickstop_options;
			weapon_cfg[category]["pointscale_head"] = vars.ragebot.weapon[i].pointscale_head;
			weapon_cfg[category]["pointscale_body"] = vars.ragebot.weapon[i].pointscale_body;
			weapon_cfg[category]["pointscale_limbs"] = vars.ragebot.weapon[i].pointscale_limbs;
			weapon_cfg[category]["hitscan"] = vars.ragebot.weapon[i].hitscan;
			weapon_cfg[category]["hitscan_baim"] = vars.ragebot.weapon[i].hitscan_baim;
			weapon_cfg[category]["baim"] = vars.ragebot.weapon[i].baim;
			weapon_cfg[category]["baim_under_hp"] = vars.ragebot.weapon[i].baim_under_hp;
			weapon_cfg[category]["adaptive_baim"] = vars.ragebot.weapon[i].adaptive_baim;
			weapon_cfg[category]["max_misses"] = vars.ragebot.weapon[i].max_misses;
		}
	}

	auto& antiaim = json["antiaim"]; {
		antiaim["enable"] = vars.antiaim.enable;
		antiaim["desync_type"] = vars.antiaim.desync;

		SaveBind(vars.antiaim.fakeduck, "fakeduck", &antiaim);
		SaveBind(vars.antiaim.slowwalk, "slowwalk", &antiaim);
		SaveBind(vars.antiaim.inverter, "inverter", &antiaim);

		auto& antiaim_override = antiaim["antiaim_override"]; {
			antiaim_override["enable"] = vars.antiaim.aa_override.enable;
			SaveBind(vars.antiaim.aa_override.left, "left", &antiaim_override);
			SaveBind(vars.antiaim.aa_override.right, "right", &antiaim_override);
			SaveBind(vars.antiaim.aa_override.back, "back", &antiaim_override);
			SaveBind(vars.antiaim.aa_override.inverter, "inventer", &antiaim_override);
		}
		auto& fakelag = antiaim["fakelag"]; {
			fakelag["type"] = vars.antiaim.fakelag;
			fakelag["factor"] = vars.antiaim.fakelagfactor;
			fakelag["switch_ticks"] = vars.antiaim.fakelagvariance;
			fakelag["fakelag_onshot"] = vars.antiaim.fakelag_onshot;
		}
	}

	auto& players = json["players"]; {
		auto& esp = players["esp"]; {
			esp["enable"] = vars.visuals.enable;
			esp["dormant"] = vars.visuals.dormant;

			esp["box"] = vars.visuals.box; {
				esp["box_color"]["red"] = vars.visuals.box_color.get_red();
				esp["box_color"]["green"] = vars.visuals.box_color.get_green();
				esp["box_color"]["blue"] = vars.visuals.box_color.get_blue();
				esp["box_color"]["alpha"] = vars.visuals.box_color.get_alpha();
			}

			esp["skeleton"] = vars.visuals.skeleton; {
				esp["skeleton_color"]["red"] = vars.visuals.skeleton_color.get_red();
				esp["skeleton_color"]["green"] = vars.visuals.skeleton_color.get_green();
				esp["skeleton_color"]["blue"] = vars.visuals.skeleton_color.get_blue();
				esp["skeleton_color"]["alpha"] = vars.visuals.skeleton_color.get_alpha();
			}

			esp["healthbar"]["enable"] = vars.visuals.healthbar;
			esp["healthbar"]["override_hp"] = vars.visuals.override_hp; {
				esp["healthbar"]["hp_color"]["red"] = vars.visuals.hp_color.get_red();
				esp["healthbar"]["hp_color"]["green"] = vars.visuals.hp_color.get_green();
				esp["healthbar"]["hp_color"]["blue"] = vars.visuals.hp_color.get_blue();
				esp["healthbar"]["hp_color"]["alpha"] = vars.visuals.hp_color.get_alpha();
			}

			esp["name"] = vars.visuals.name; {
				esp["name_color"]["red"] = vars.visuals.name_color.get_red();
				esp["name_color"]["green"] = vars.visuals.name_color.get_green();
				esp["name_color"]["blue"] = vars.visuals.name_color.get_blue();
				esp["name_color"]["alpha"] = vars.visuals.name_color.get_alpha();
			}

			esp["weapon"] = vars.visuals.weapon; {
				esp["weapon_color"]["red"] = vars.visuals.weapon_color.get_red();
				esp["weapon_color"]["green"] = vars.visuals.weapon_color.get_green();
				esp["weapon_color"]["blue"] = vars.visuals.weapon_color.get_blue();
				esp["weapon_color"]["alpha"] = vars.visuals.weapon_color.get_alpha();
			}

			esp["ammo"] = vars.visuals.ammo; {
				esp["ammo_color"]["red"] = vars.visuals.ammo_color.get_red();
				esp["ammo_color"]["green"] = vars.visuals.ammo_color.get_green();
				esp["ammo_color"]["blue"] = vars.visuals.ammo_color.get_blue();
				esp["ammo_color"]["alpha"] = vars.visuals.ammo_color.get_alpha();
			}

			esp["flags"] = vars.visuals.flags; {
				esp["flags_color"]["red"] = vars.visuals.flags_color.get_red();
				esp["flags_color"]["green"] = vars.visuals.flags_color.get_green();
				esp["flags_color"]["blue"] = vars.visuals.flags_color.get_blue();
				esp["flags_color"]["alpha"] = vars.visuals.flags_color.get_alpha();
			}
			esp["show_multipoint"] = vars.visuals.shot_multipoint;
			esp["out_of_fov"]["enable"] = vars.visuals.out_of_fov; {
				esp["out_of_fov"]["size"] = vars.visuals.out_of_fov_size;
				esp["out_of_fov"]["distance"] = vars.visuals.out_of_fov_distance;

				esp["out_of_fov"]["color"]["red"] = vars.visuals.out_of_fov_color.get_red();
				esp["out_of_fov"]["color"]["green"] = vars.visuals.out_of_fov_color.get_green();
				esp["out_of_fov"]["color"]["blue"] = vars.visuals.out_of_fov_color.get_blue();
				esp["out_of_fov"]["color"]["alpha"] = vars.visuals.out_of_fov_color.get_alpha();
			}
		}
		auto& models = players["models"]; {  // ебал € в рот это все переносить пизда (@opai), пиздец еще и лоад делать ƒј ЅЋя“№
			auto& chams_cfg = models["chams"]; {
			
				chams_cfg["enemy"]["enable"] = vars.visuals.chams; {
					chams_cfg["enemy"]["visible_color"]["red"] = vars.visuals.chamscolor.get_red();
					chams_cfg["enemy"]["visible_color"]["green"] = vars.visuals.chamscolor.get_green();
					chams_cfg["enemy"]["visible_color"]["blue"] = vars.visuals.chamscolor.get_blue();
					chams_cfg["enemy"]["visible_color"]["alpha"] = vars.visuals.chamscolor.get_alpha();

					chams_cfg["enemy"]["through_walls"] = vars.visuals.chamsxqz;
					chams_cfg["enemy"]["through_walls_color"]["red"] = vars.visuals.chamscolor_xqz.get_red();
					chams_cfg["enemy"]["through_walls_color"]["green"] = vars.visuals.chamscolor_xqz.get_green();
					chams_cfg["enemy"]["through_walls_color"]["blue"] = vars.visuals.chamscolor_xqz.get_blue();
					chams_cfg["enemy"]["through_walls_color"]["alpha"] = vars.visuals.chamscolor_xqz.get_alpha();

					chams_cfg["enemy"]["overlay_color"]["red"] = vars.visuals.glow_col.get_red();
					chams_cfg["enemy"]["overlay_color"]["green"] = vars.visuals.glow_col.get_green();
					chams_cfg["enemy"]["overlay_color"]["blue"] = vars.visuals.glow_col.get_blue();
					chams_cfg["enemy"]["overlay_color"]["alpha"] = vars.visuals.glow_col.get_alpha();

					chams_cfg["enemy"]["xqz_overlay_color"]["red"] = vars.visuals.glow_col_xqz.get_red();
					chams_cfg["enemy"]["xqz_overlay_color"]["green"] = vars.visuals.glow_col_xqz.get_green();
					chams_cfg["enemy"]["xqz_overlay_color"]["blue"] = vars.visuals.glow_col_xqz.get_blue();
					chams_cfg["enemy"]["xqz_overlay_color"]["alpha"] = vars.visuals.glow_col_xqz.get_alpha();

					chams_cfg["enemy"]["material"] = vars.visuals.chamstype;
					chams_cfg["enemy"]["phong_exponent"] = vars.visuals.phong_exponent;
					chams_cfg["enemy"]["phong_boost"] = vars.visuals.phong_boost;
					chams_cfg["enemy"]["rim"] = vars.visuals.rim;
					chams_cfg["enemy"]["brightness"] = vars.visuals.chams_brightness;
					chams_cfg["enemy"]["pearlescent"] = vars.visuals.pearlescent;
					chams_cfg["enemy"]["overlay"] = vars.visuals.overlay;
					chams_cfg["enemy"]["overlay_xqz"] = vars.visuals.overlay_xqz;

					chams_cfg["enemy"]["metallic_color"]["red"] = vars.visuals.metallic_clr.get_red();
					chams_cfg["enemy"]["metallic_color"]["green"] = vars.visuals.metallic_clr.get_green();
					chams_cfg["enemy"]["metallic_color"]["blue"] = vars.visuals.metallic_clr.get_blue();
					chams_cfg["enemy"]["metallic_color"]["alpha"] = vars.visuals.metallic_clr.get_alpha();

					chams_cfg["enemy"]["phong_color"]["red"] = vars.visuals.metallic_clr2.get_red();
					chams_cfg["enemy"]["phong_color"]["green"] = vars.visuals.metallic_clr2.get_green();
					chams_cfg["enemy"]["phong_color"]["blue"] = vars.visuals.metallic_clr2.get_blue();
					chams_cfg["enemy"]["phong_color"]["alpha"] = vars.visuals.metallic_clr2.get_alpha();
				}

				chams_cfg["local"]["enable"] = vars.visuals.localchams; {
					chams_cfg["local"]["interpolated"] = vars.visuals.interpolated_model;
					chams_cfg["local"]["material"] = vars.visuals.localchamstype;
					chams_cfg["local"]["blend_on_scope"] = vars.visuals.blend_on_scope;
					chams_cfg["local"]["blend_value"] = vars.visuals.blend_value;

					chams_cfg["local"]["overlay_color"]["red"] = vars.visuals.local_glow_color.get_red();
					chams_cfg["local"]["overlay_color"]["green"] = vars.visuals.local_glow_color.get_green();
					chams_cfg["local"]["overlay_color"]["blue"] = vars.visuals.local_glow_color.get_blue();
					chams_cfg["local"]["overlay_color"]["alpha"] = vars.visuals.local_glow_color.get_alpha();

					chams_cfg["local"]["visible_color"]["red"] = vars.visuals.localchams_color.get_red();
					chams_cfg["local"]["visible_color"]["green"] = vars.visuals.localchams_color.get_green();
					chams_cfg["local"]["visible_color"]["blue"] = vars.visuals.localchams_color.get_blue();
					chams_cfg["local"]["visible_color"]["alpha"] = vars.visuals.localchams_color.get_alpha();

					chams_cfg["local"]["phong_exponent"] = vars.visuals.local_chams.phong_exponent;
					chams_cfg["local"]["phong_boost"] = vars.visuals.local_chams.phong_boost;
					chams_cfg["local"]["rim"] = vars.visuals.local_chams.rim;
					chams_cfg["local"]["brightness"] = vars.visuals.local_chams_brightness;
					chams_cfg["local"]["pearlescent"] = vars.visuals.local_chams.pearlescent;
					chams_cfg["local"]["overlay"] = vars.visuals.local_chams.overlay;

					chams_cfg["local"]["metallic_color"]["red"] = vars.visuals.local_chams.metallic_clr.get_red();
					chams_cfg["local"]["metallic_color"]["green"] = vars.visuals.local_chams.metallic_clr.get_green();
					chams_cfg["local"]["metallic_color"]["blue"] = vars.visuals.local_chams.metallic_clr.get_blue();
					chams_cfg["local"]["metallic_color"]["alpha"] = vars.visuals.local_chams.metallic_clr.get_alpha();

					chams_cfg["local"]["phong_color"]["red"] = vars.visuals.local_chams.metallic_clr2.get_red();
					chams_cfg["local"]["phong_color"]["green"] = vars.visuals.local_chams.metallic_clr2.get_green();
					chams_cfg["local"]["phong_color"]["blue"] = vars.visuals.local_chams.metallic_clr2.get_blue();
					chams_cfg["local"]["phong_color"]["alpha"] = vars.visuals.local_chams.metallic_clr2.get_alpha();
				}

				chams_cfg["misc_chams_2"]["interpolated"] = vars.visuals.interpolated_bt;
				chams_cfg["misc_chams_3"]["interpolated"] = vars.visuals.interpolated_dsy;
				for (int i = 0; i < 4; i++) {
					string category = "misc_chams_" + std::to_string(i);
					chams_cfg[category]["enable"] = vars.visuals.misc_chams[i].enable;
					chams_cfg[category]["material"] = vars.visuals.misc_chams[i].material;
					chams_cfg[category]["brightness"] = vars.visuals.misc_chams[i].chams_brightness;
					chams_cfg[category]["pearlescent"] = vars.visuals.misc_chams[i].pearlescent;
					chams_cfg[category]["phong_exponent"] = vars.visuals.misc_chams[i].phong_exponent;
					chams_cfg[category]["phong_boost"] = vars.visuals.misc_chams[i].phong_boost;
					chams_cfg[category]["rim"] = vars.visuals.misc_chams[i].rim;
					chams_cfg[category]["overlay"] = vars.visuals.misc_chams[i].overlay;

					chams_cfg[category]["model_color"]["red"] = vars.visuals.misc_chams[i].clr.get_red();
					chams_cfg[category]["model_color"]["green"] = vars.visuals.misc_chams[i].clr.get_green();
					chams_cfg[category]["model_color"]["blue"] = vars.visuals.misc_chams[i].clr.get_blue();
					chams_cfg[category]["model_color"]["alpha"] = vars.visuals.misc_chams[i].clr.get_alpha();

					chams_cfg[category]["overlay_color"]["red"] = vars.visuals.misc_chams[i].glow_clr.get_red();
					chams_cfg[category]["overlay_color"]["green"] = vars.visuals.misc_chams[i].glow_clr.get_green();
					chams_cfg[category]["overlay_color"]["blue"] = vars.visuals.misc_chams[i].glow_clr.get_blue();
					chams_cfg[category]["overlay_color"]["alpha"] = vars.visuals.misc_chams[i].glow_clr.get_alpha();

					chams_cfg[category]["metallic_color"]["red"] = vars.visuals.misc_chams[i].metallic_clr.get_red();
					chams_cfg[category]["metallic_color"]["green"] = vars.visuals.misc_chams[i].metallic_clr.get_green();
					chams_cfg[category]["metallic_color"]["blue"] = vars.visuals.misc_chams[i].metallic_clr.get_blue();
					chams_cfg[category]["metallic_color"]["alpha"] = vars.visuals.misc_chams[i].metallic_clr.get_alpha();

					chams_cfg[category]["phong_color"]["red"] = vars.visuals.misc_chams[i].metallic_clr2.get_red();
					chams_cfg[category]["phong_color"]["green"] = vars.visuals.misc_chams[i].metallic_clr2.get_green();
					chams_cfg[category]["phong_color"]["blue"] = vars.visuals.misc_chams[i].metallic_clr2.get_blue();
					chams_cfg[category]["phong_color"]["alpha"] = vars.visuals.misc_chams[i].metallic_clr2.get_alpha();
				}
			}
			auto& glow = models["glow"]; {
				glow["style"] = vars.visuals.glowtype;

				glow["enemy"]["enable"] = vars.visuals.glow;
				glow["enemy"]["color"]["red"] = vars.visuals.glow_color.get_red();
				glow["enemy"]["color"]["green"] = vars.visuals.glow_color.get_green();
				glow["enemy"]["color"]["blue"] = vars.visuals.glow_color.get_blue();
				glow["enemy"]["color"]["alpha"] = vars.visuals.glow_color.get_alpha();

				glow["local"]["enable"] = vars.visuals.local_glow;
				glow["local"]["color"]["red"] = vars.visuals.local_glow_clr.get_red();
				glow["local"]["color"]["green"] = vars.visuals.local_glow_clr.get_green();
				glow["local"]["color"]["blue"] = vars.visuals.local_glow_clr.get_blue();
				glow["local"]["color"]["alpha"] = vars.visuals.local_glow_clr.get_alpha();
			}
		}
		auto& misc = players["misc"]; {
			//misc["spec_list"] = vars.visuals.speclist;
			//misc["monitor"] = vars.visuals.monitor;
			misc["nadepred"]["enable"] = vars.visuals.nadepred;

			SaveColor(vars.visuals.nadepred_color, "color", &misc["nadepred"]);

			misc["indicators"] = vars.visuals.indicators;
			misc["aspect_ratio"] = vars.visuals.aspect_ratio;
			misc["thirdperson"]["enable"] = vars.misc.thirdperson_bind;
			misc["thirdperson"]["distance"] = vars.visuals.thirdperson_dist;
			SaveBind(vars.misc.thirdperson_bind, "bind", &misc["thirdperson"]);

			SaveBind(vars.misc.airstuckkey, "airstuckkey", &misc);

			misc["eventlog"]["enable"] = vars.visuals.eventlog;
			misc["watermark"] = vars.visuals.watermark;

			misc["eventlog"]["color"]["red"] = vars.visuals.eventlog_color.get_red();
			misc["eventlog"]["color"]["green"] = vars.visuals.eventlog_color.get_green();
			misc["eventlog"]["color"]["blue"] = vars.visuals.eventlog_color.get_blue();
			misc["eventlog"]["color"]["alpha"] = vars.visuals.eventlog_color.get_alpha();

			misc["hitmarker"]["enable"] = vars.visuals.hitmarker;
			misc["hitmarker"]["enable_sound"] = vars.visuals.hitmarker_sound;

			misc["hitmarker"]["color"]["red"] = vars.visuals.hitmarker_color.get_red();
			misc["hitmarker"]["color"]["green"] = vars.visuals.hitmarker_color.get_green();
			misc["hitmarker"]["color"]["blue"] = vars.visuals.hitmarker_color.get_blue();
			misc["hitmarker"]["color"]["alpha"] = vars.visuals.hitmarker_color.get_alpha();
		}
	}

	auto& world = json["world"]; {
		auto& tracers = world["tracers"]; {
			tracers["bullet_tracer"]["size"] = vars.visuals.impacts_size;
			tracers["bullet_tracer"]["enable"] = vars.visuals.bullet_tracer; {
				tracers["bullet_tracer"]["sprite"] = vars.visuals.bullet_tracer_type;
				tracers["bullet_tracer"]["color"]["red"] = vars.visuals.bullet_tracer_color.get_red();
				tracers["bullet_tracer"]["color"]["green"] = vars.visuals.bullet_tracer_color.get_green();
				tracers["bullet_tracer"]["color"]["blue"] = vars.visuals.bullet_tracer_color.get_blue();
				tracers["bullet_tracer"]["color"]["alpha"] = vars.visuals.bullet_tracer_color.get_alpha();

				tracers["bullet_tracer"]["local"] = vars.visuals.bullet_tracer_local;
				tracers["bullet_tracer"]["local_color"]["red"] = vars.visuals.bullet_tracer_local_color.get_red();
				tracers["bullet_tracer"]["local_color"]["green"] = vars.visuals.bullet_tracer_local_color.get_green();
				tracers["bullet_tracer"]["local_color"]["blue"] = vars.visuals.bullet_tracer_local_color.get_blue();
				tracers["bullet_tracer"]["local_color"]["alpha"] = vars.visuals.bullet_tracer_local_color.get_alpha();
			}
			
			tracers["bullet_impact"]["enable"] = vars.visuals.bullet_impact; {
				tracers["bullet_impact"]["color"]["red"] = vars.visuals.bullet_impact_color.get_red();
				tracers["bullet_impact"]["color"]["green"] = vars.visuals.bullet_impact_color.get_green();
				tracers["bullet_impact"]["color"]["blue"] = vars.visuals.bullet_impact_color.get_blue();
				tracers["bullet_impact"]["color"]["alpha"] = vars.visuals.bullet_impact_color.get_alpha();

				tracers["bullet_impact_client"]["color"]["red"] = vars.visuals.client_impact_color.get_red();
				tracers["bullet_impact_client"]["color"]["green"] = vars.visuals.client_impact_color.get_green();
				tracers["bullet_impact_client"]["color"]["blue"] = vars.visuals.client_impact_color.get_blue();
				tracers["bullet_impact_client"]["color"]["alpha"] = vars.visuals.client_impact_color.get_alpha();
			}
		}
		auto& effects = world["effects"]; {
			effects["removals"] = vars.visuals.remove;
			effects["force_crosshair"] = vars.visuals.force_crosshair;
			effects["kill_effect"] = vars.visuals.kill_effect;
			effects["world_fov"] = vars.misc.worldfov;
			effects["viewmodel_fov"] = vars.misc.viewmodelfov;

			effects["nightmode"]["enable"] = vars.visuals.nightmode; {
				effects["nightmode"]["color"]["world"]["red"] = vars.visuals.nightmode_color.get_red();
				effects["nightmode"]["color"]["world"]["green"] = vars.visuals.nightmode_color.get_green();
				effects["nightmode"]["color"]["world"]["blue"] = vars.visuals.nightmode_color.get_blue();
				effects["nightmode"]["color"]["world"]["alpha"] = vars.visuals.nightmode_color.get_alpha();

				effects["nightmode"]["color"]["prop"]["red"] = vars.visuals.nightmode_prop_color.get_red();
				effects["nightmode"]["color"]["prop"]["green"] = vars.visuals.nightmode_prop_color.get_green();
				effects["nightmode"]["color"]["prop"]["blue"] = vars.visuals.nightmode_prop_color.get_blue();
				effects["nightmode"]["color"]["prop"]["alpha"] = vars.visuals.nightmode_prop_color.get_alpha();

				effects["nightmode"]["color"]["skybox"]["red"] = vars.visuals.nightmode_skybox_color.get_red();
				effects["nightmode"]["color"]["skybox"]["green"] = vars.visuals.nightmode_skybox_color.get_green();
				effects["nightmode"]["color"]["skybox"]["blue"] = vars.visuals.nightmode_skybox_color.get_blue();
				effects["nightmode"]["color"]["skybox"]["alpha"] = vars.visuals.nightmode_skybox_color.get_alpha();
			}
		}
	}

	auto& misc = json["misc"]; {
		misc["anti_untrusted"] = vars.misc.antiuntrusted;
		misc["bunnyhop"] = vars.misc.bunnyhop;
		misc["knifebot"] = vars.misc.knifebot;
		misc["clantag"] = vars.visuals.clantagspammer;

		misc["watermark"] = vars.visuals.watermark;
		misc["hold_firinganims"] = vars.misc.hold_firinganims;

		misc["autobuy"]["enable"] = vars.misc.autobuy.enable;
		misc["autobuy"]["main"] = vars.misc.autobuy.main;
		misc["autobuy"]["pistol"] = vars.misc.autobuy.pistol;
		misc["autobuy"]["misc"] = vars.misc.autobuy.misc;

		misc["viewmodel"]["x"] = vars.misc.viewmodel_x;
		misc["viewmodel"]["y"] = vars.misc.viewmodel_y;
		misc["viewmodel"]["z"] = vars.misc.viewmodel_z;
	}

	static TCHAR path[256];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + ("\\uni.beta\\");
		file = std::string(path) + ("\\uni.beta\\" + cfg_name);
	}

	CreateDirectory(folder.c_str(), NULL);
	std::ofstream file_out(file);
/*	string decrypted = configuration.toStyledString();
	ByteArray txt(decrypted.begin(), decrypted.end()), enc;
	Aes256::encrypt(encryption_key, txt, enc)*/;
	if (file_out.good())
		file_out << configuration/*string(enc.begin(), enc.end())*/;
	file_out.close();
}
ByteArray ReadAllBytes(char const* filename)
{
	ifstream ifs(filename, ios::binary | ios::ate);
	ifstream::pos_type pos = ifs.tellg();

	ByteArray result(pos);

	ifs.seekg(0, ios::beg);
	ifs.read((char*)&result[0], pos);

	return result;
}
void CConfig::Load(string cfg_name)
{
	json_t configuration;
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		folder = std::string(path) + ("\\uni.beta\\");
		file = std::string(path) + ("\\uni.beta\\" + cfg_name);

	}
	CreateDirectory(folder.c_str(), NULL);
	std::ifstream file_out(file);
	//string in;
	if (file_out.good())
		file_out >> configuration/*in*/;
	//file_out.close();
	//if (in.size() == 0)
	//	return;

	//ByteArray txt = ReadAllBytes(file.c_str()), enc;
	//Aes256::decrypt(encryption_key, txt, enc);
	//if (enc.size() == 0)
	//	return;
	/*configuration = json_t(string(enc.begin(), enc.end()))*/;
	if (!configuration.isMember("config"))
		return;

	auto& json = configuration["config"];
	json["name"] = cfg_name;

	auto& ragebot = json["ragebot"]; {
		LoadBool(&vars.ragebot.enable, "enable", ragebot);
		LoadBool(&vars.ragebot.autoshoot, "auto_shoot", ragebot);
		LoadBool(&vars.ragebot.autoscope, "auto_scope", ragebot);
		LoadBool(&vars.ragebot.delayshot, "delayshot", ragebot);
		LoadBool(&vars.ragebot.hitchance_consider_hitbox, "hitchance_consider_hitbox", ragebot);
		LoadBool(&vars.ragebot.posadj, "posadj", ragebot);
		LoadBool(&vars.ragebot.backshoot_bt, "backshoot_bt", ragebot);
		LoadBool(&vars.ragebot.resolver, "resolver", ragebot);
		//LoadBool(&vars.ragebot.onshot_invert, "onshot_invert", ragebot);
		//LoadBool(&vars.ragebot.onshot_resolver, "onshot_resolver", ragebot);
		LoadBool(&vars.ragebot.shotrecord, "shot_record", ragebot);
		LoadBind(vars.ragebot.override_dmg, "override_dmg", ragebot);
		LoadBind(vars.ragebot.force_body, "force_body", ragebot);
		LoadBind(vars.ragebot.double_tap, "double_tap", ragebot);
		LoadBind(vars.ragebot.hide_shots, "hide_shots", ragebot);
		LoadBool(&vars.ragebot.dt_instant, "dt_instant", ragebot);

		LoadColor(&vars.ragebot.shot_clr, "shot_clr", ragebot);

		auto& weapon_cfg = ragebot["weapon_cfg"];
		for (int i = 0; i < 7; i++) {
			string category = "weapon_" + std::to_string(i);
			LoadBool(&vars.ragebot.weapon[i].enable, "enable", weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].mindamage, "mindamage", weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].mindamage_override, "mindamage_override", weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].hitchance, "hitchance", weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].hitchancetype, "hitchance_type", weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].multipoint, "multipoint", weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].quickstop, "quickstop", weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].quickstop_options, "quickstop_options", weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].pointscale_head, "pointscale_head", weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].pointscale_body, "pointscale_body", weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].pointscale_limbs, "pointscale_limbs", weapon_cfg[category]);
			LoadUInt(&vars.ragebot.weapon[i].hitscan, "hitscan", weapon_cfg[category]);
			LoadUInt(&vars.ragebot.weapon[i].hitscan_baim, "hitscan_baim", weapon_cfg[category]);
			LoadUInt(&vars.ragebot.weapon[i].baim, "baim", weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].baim_under_hp, "baim_under_hp", weapon_cfg[category]);
			LoadBool(&vars.ragebot.weapon[i].adaptive_baim, "adaptive_baim", weapon_cfg[category]);
			LoadInt(&vars.ragebot.weapon[i].max_misses, "max_misses", weapon_cfg[category]);
		}
	}

	auto& antiaim = json["antiaim"]; {
		LoadBool(&vars.antiaim.enable, "enable", antiaim);
		LoadBind(vars.antiaim.fakeduck, "fakeduck", antiaim);
		LoadBind(vars.antiaim.slowwalk, "slowwalk", antiaim);
		LoadInt(&vars.antiaim.desync, "desync_type", antiaim);
		LoadInt(&vars.antiaim.slowwalk_speed, "slowwalk_speed", antiaim);
		
		LoadBind(vars.antiaim.inverter, "inverter", antiaim);

		auto& antiaim_override = antiaim["antiaim_override"]; {
			LoadBool(&vars.antiaim.aa_override.enable, "enable", antiaim_override);
			LoadBind(vars.antiaim.aa_override.left, "left", antiaim_override);
			LoadBind(vars.antiaim.aa_override.right, "right", antiaim_override);
			LoadBind(vars.antiaim.aa_override.back, "back", antiaim_override);
			LoadBind(vars.antiaim.aa_override.inverter, "inventer", antiaim_override);
		}
		auto& fakelag = antiaim["fakelag"]; {
			LoadInt(&vars.antiaim.fakelag, "type", fakelag);
			LoadInt(&vars.antiaim.fakelagfactor, "factor", fakelag);
			LoadInt(&vars.antiaim.fakelagvariance, "switch_ticks", fakelag);
			LoadBool(&vars.antiaim.fakelag_onshot, "fakelag_onshot", fakelag);
		}
	}

	auto& players = json["players"]; {
		auto& esp = players["esp"];

		LoadBool(&vars.visuals.enable, "enable", esp);
		LoadBool(&vars.visuals.dormant, "dormant", esp);
		LoadBool(&vars.visuals.skeleton, "box", esp);
		LoadColor(&vars.visuals.skeleton_color, "box_color", esp);
		LoadBool(&vars.visuals.box, "box", esp);
		LoadColor(&vars.visuals.box_color, "box_color", esp);
		LoadBool(&vars.visuals.healthbar, "enable", esp["healthbar"]);
		LoadBool(&vars.visuals.override_hp, "override_hp", esp["healthbar"]);
		LoadColor(&vars.visuals.hp_color, "hp_color", esp["healthbar"]);

		LoadBool(&vars.visuals.name, "name", esp);
		LoadColor(&vars.visuals.name_color, "name_color", esp);

		LoadBool(&vars.visuals.weapon, "weapon", esp);
		LoadColor(&vars.visuals.weapon_color, "weapon_color", esp);

		LoadBool(&vars.visuals.ammo, "ammo", esp);
		LoadColor(&vars.visuals.ammo_color, "ammo_color", esp);

		LoadUInt(&vars.visuals.flags, "flags", esp);
		LoadColor(&vars.visuals.flags_color, "flags_color", esp);
		LoadBool(&vars.visuals.shot_multipoint, "show_multipoint", esp);
		LoadBool(&vars.visuals.out_of_fov, "enable", esp["out_of_fov"]);
		LoadInt(&vars.visuals.out_of_fov_size, "size", esp["out_of_fov"]);
		LoadInt(&vars.visuals.out_of_fov_distance, "distance", esp["out_of_fov"]);
		LoadColor(&vars.visuals.out_of_fov_color, "color", esp["out_of_fov"]);

		auto& models = players["models"]; {  
			auto& chams_cfg = models["chams"]; {
				LoadBool(&vars.visuals.chams, "enable", chams_cfg["enemy"]);
				LoadBool(&vars.visuals.chamsxqz, "through_walls", chams_cfg["enemy"]);
				LoadColor(&vars.visuals.chamscolor, "visible_color", chams_cfg["enemy"]);
				LoadColor(&vars.visuals.chamscolor_xqz, "through_walls_color", chams_cfg["enemy"]);
				LoadColor(&vars.visuals.glow_col, "overlay_color", chams_cfg["enemy"]);
				LoadColor(&vars.visuals.glow_col_xqz, "xqz_overlay_color", chams_cfg["enemy"]);
				LoadInt(&vars.visuals.overlay, "overlay", chams_cfg["enemy"]);
				LoadInt(&vars.visuals.overlay_xqz, "overlay_xqz", chams_cfg["enemy"]);
				LoadInt(&vars.visuals.chamstype, "material", chams_cfg["enemy"]);
				LoadInt(&vars.visuals.phong_exponent, "phong_exponent", chams_cfg["enemy"]);
				LoadInt(&vars.visuals.phong_boost, "phong_boost", chams_cfg["enemy"]);
				LoadInt(&vars.visuals.rim, "rim", chams_cfg["enemy"]);
				LoadInt(&vars.visuals.chams_brightness, "brightness", chams_cfg["enemy"]);
				LoadInt(&vars.visuals.pearlescent, "pearlescent", chams_cfg["enemy"]);

				LoadColor(&vars.visuals.metallic_clr, "metallic_color", chams_cfg["enemy"]);
				LoadColor(&vars.visuals.metallic_clr2, "phong_color", chams_cfg["enemy"]);

				LoadBool(&vars.visuals.localchams, "enable", chams_cfg["local"]); {
					LoadBool(&vars.visuals.interpolated_model, "interpolated", chams_cfg["local"]);
					LoadBool(&vars.visuals.blend_on_scope, "blend_on_scope", chams_cfg["local"]);
					LoadInt(&vars.visuals.blend_value, "blend_value", chams_cfg["local"]);
					LoadBool(&vars.visuals.localchams, "enable", chams_cfg["local"]);
					LoadColor(&vars.visuals.localchams_color, "visible_color", chams_cfg["local"]);
					LoadColor(&vars.visuals.local_glow_color, "overlay_color", chams_cfg["local"]);
					LoadInt(&vars.visuals.localchamstype, "material", chams_cfg["local"]);
					LoadInt(&vars.visuals.local_chams.phong_exponent, "phong_exponent", chams_cfg["local"]);
					LoadInt(&vars.visuals.local_chams.phong_boost, "phong_boost", chams_cfg["local"]);
					LoadInt(&vars.visuals.local_chams.rim, "rim", chams_cfg["local"]);
					LoadInt(&vars.visuals.local_chams_brightness, "brightness", chams_cfg["local"]);
					LoadInt(&vars.visuals.local_chams.pearlescent, "pearlescent", chams_cfg["local"]);
					LoadInt(&vars.visuals.local_chams.overlay, "overlay", chams_cfg["local"]);

					LoadColor(&vars.visuals.local_chams.metallic_clr, "metallic_color", chams_cfg["local"]);
					LoadColor(&vars.visuals.local_chams.metallic_clr2, "phong_color", chams_cfg["local"]);
				}

				LoadBool(&vars.visuals.interpolated_bt, "interpolated", chams_cfg["misc_chams_2"]);
				LoadBool(&vars.visuals.interpolated_dsy, "interpolated", chams_cfg["misc_chams_3"]);

				for (int i = 0; i < 4; i++) {
					string category = "misc_chams_" + std::to_string(i);
					LoadBool(&vars.visuals.misc_chams[i].enable, "enable", chams_cfg[category]);
					LoadColor(&vars.visuals.misc_chams[i].clr, "model_color", chams_cfg[category]);
					LoadColor(&vars.visuals.misc_chams[i].glow_clr, "overlay_color", chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].material, "material", chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].phong_exponent, "phong_exponent", chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].phong_boost, "phong_boost", chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].rim, "rim", chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].chams_brightness, "brightness", chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].pearlescent, "pearlescent", chams_cfg[category]);
					LoadInt(&vars.visuals.misc_chams[i].overlay, "overlay", chams_cfg[category]);

					LoadColor(&vars.visuals.misc_chams[i].metallic_clr, "metallic_color", chams_cfg[category]);
					LoadColor(&vars.visuals.misc_chams[i].metallic_clr2, "phong_color", chams_cfg[category]);
				}
			}
			auto& glow = models["glow"]; {
				LoadInt(&vars.visuals.glowtype, "style", glow);
				LoadBool(&vars.visuals.glow, "enable", glow["enemy"]);
				LoadColor(&vars.visuals.glow_color, "color", glow["enemy"]);

				LoadBool(&vars.visuals.local_glow, "enable", glow["local"]);
				LoadColor(&vars.visuals.local_glow_clr, "color", glow["local"]);
			}
		}

		auto& misc = players["misc"]; {

			//LoadBool(&vars.visuals.speclist, "spec_list", misc);
			//LoadBool(&vars.visuals.monitor, "monitor", misc);
			LoadInt(&vars.visuals.thirdperson_dist, "distance", misc["thirdperson"]);
			LoadBind(vars.misc.thirdperson_bind, "bind", misc["thirdperson"]);
			LoadUInt(&vars.visuals.indicators, "indicators", misc);
			LoadInt(&vars.visuals.aspect_ratio, "aspect_ratio", misc);
			LoadBool(&vars.visuals.eventlog, "enable", misc["eventlog"]);
			LoadColor(&vars.visuals.eventlog_color, "color", misc["eventlog"]);

			LoadInt(&vars.visuals.hitmarker, "enable", misc["hitmarker"]);
			LoadBool(&vars.visuals.hitmarker_sound, "enable_sound", misc["hitmarker"]);
			LoadColor(&vars.visuals.hitmarker_color, "color", misc["hitmarker"]);

			LoadBool(&vars.visuals.nadepred, "enable", misc["nadepred"]);
			LoadColor(&vars.visuals.nadepred_color, "color", misc["nadepred"]);
		}
	}

	auto& world = json["world"]; {
		auto& tracers = world["tracers"]; {
			LoadBool(&vars.visuals.bullet_tracer, "enable", tracers["bullet_tracer"]);
			LoadFloat(&vars.visuals.impacts_size, "size", tracers["bullet_tracer"]);
			LoadInt(&vars.visuals.bullet_tracer_type, "sprite", tracers["bullet_tracer"]);
			LoadColor(&vars.visuals.bullet_tracer_color, "color", tracers["bullet_tracer"]);
			LoadBool(&vars.visuals.bullet_tracer_local, "local", tracers["bullet_tracer"]);
			LoadColor(&vars.visuals.bullet_tracer_local_color, "local_color", tracers["bullet_tracer"]);

			LoadBool(&vars.visuals.bullet_impact, "enable", tracers["bullet_impact"]);
			LoadColor(&vars.visuals.bullet_impact_color, "color", tracers["bullet_impact"]);
			LoadColor(&vars.visuals.client_impact_color, "color", tracers["bullet_impact_client"]);
;
		}
		auto& effects = world["effects"]; {
			LoadUInt(&vars.visuals.remove, "removals", effects);
			LoadBool(&vars.visuals.force_crosshair, "force_crosshair", effects);
			LoadBool(&vars.visuals.kill_effect, "kill_effect", effects);
			LoadInt(&vars.misc.worldfov, "world_fov", effects);
			LoadInt(&vars.misc.viewmodelfov, "viewmodel_fov", effects);

			LoadBool(&vars.visuals.nightmode, "enable", effects["nightmode"]);
			LoadColor(&vars.visuals.nightmode_color, "world", effects["nightmode"]["color"]);
			LoadColor(&vars.visuals.nightmode_prop_color, "prop", effects["nightmode"]["color"]);
			LoadColor(&vars.visuals.nightmode_skybox_color, "skybox", effects["nightmode"]["color"]);
		}
	}

	auto& misc = json["misc"]; {
		LoadBool(&vars.misc.antiuntrusted, "anti_untrusted", misc);
		LoadBool(&vars.misc.bunnyhop, "bunnyhop", misc);
		LoadBool(&vars.misc.knifebot, "knifebot", misc);
		LoadBool(&vars.visuals.clantagspammer, "clantag", misc);
		LoadBool(&vars.visuals.watermark, "watermark", misc);
		LoadBool(&vars.misc.hold_firinganims, "hold_firinganims", misc);
		LoadBool(&vars.misc.autobuy.enable, "enable", misc["autobuy"]);
		LoadInt(&vars.misc.autobuy.pistol, "pistol", misc["autobuy"]);
		LoadInt(&vars.misc.autobuy.main, "main", misc["autobuy"]);
		LoadUInt(&vars.misc.autobuy.misc, "misc", misc["autobuy"]);

		LoadBind(vars.misc.airstuckkey, "airstuckkey", misc);

		LoadInt(&vars.misc.viewmodel_x, "x", misc["viewmodel"]);
		LoadInt(&vars.misc.viewmodel_y, "y", misc["viewmodel"]);
		LoadInt(&vars.misc.viewmodel_z, "z", misc["viewmodel"]);
	}

}

CConfig Config;
CGlobalVariables vars;