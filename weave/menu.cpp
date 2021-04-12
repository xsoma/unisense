#include "Menu.h"
#include "GUI/gui.h"

#include <unordered_map>
#include <set>
#include <algorithm>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#define FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or 
#define FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed 

#include "DLL_MAIN.h"

vector<string> ConfigList;
typedef void(*LPSEARCHFUNC)(LPCTSTR lpszFileName);

void add_log(std::string date, std::vector<string> log, c_child* child, bool last = false)
{
	child->add_element(new c_text(date.c_str(), []() { return true; }, last ? color_t(105, 97, 255, 255) : color_t(235, 235, 235, 255)));

	for (int i = 0; i < log.size(); i++)
		child->add_element(new c_text(log.at(i), []() { return true; }, color_t(235, 235, 235, 255)));

	child->add_element(new c_text("", []() { return true; })); // skip next line
}

BOOL SearchFiles(LPCTSTR lpszFileName, LPSEARCHFUNC lpSearchFunc, BOOL bInnerFolders = FALSE)
{
	LPTSTR part;
	char tmp[MAX_PATH];
	char name[MAX_PATH];

	HANDLE hSearch = NULL;
	WIN32_FIND_DATA wfd;
	memset(&wfd, 0, sizeof(WIN32_FIND_DATA));

	if (bInnerFolders)
	{
		if (GetFullPathName(lpszFileName, MAX_PATH, tmp, &part) == 0) return FALSE;
		strcpy(name, part);
		strcpy(part, "*.*");
		wfd.dwFileAttributes = FILE_ATTRIBUTE_DIRECTORY;
		if (!((hSearch = FindFirstFile(tmp, &wfd)) == INVALID_HANDLE_VALUE))
			do
			{
				if (!strncmp(wfd.cFileName, ".", 1) || !strncmp(wfd.cFileName, "..", 2))
					continue;

				if (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					char next[MAX_PATH];
					if (GetFullPathName(lpszFileName, MAX_PATH, next, &part) == 0) return FALSE;
					strcpy(part, wfd.cFileName);
					strcat(next, "\\");
					strcat(next, name);

					SearchFiles(next, lpSearchFunc, TRUE);
				}
			} while (FindNextFile(hSearch, &wfd));
			FindClose(hSearch);
	}

	if ((hSearch = FindFirstFile(lpszFileName, &wfd)) == INVALID_HANDLE_VALUE)
		return TRUE;
	do
		if (!(wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		{
			char file[MAX_PATH];
			if (GetFullPathName(lpszFileName, MAX_PATH, file, &part) == 0) return FALSE;
			strcpy(part, wfd.cFileName);

			lpSearchFunc(wfd.cFileName);
		}
	while (FindNextFile(hSearch, &wfd));
	FindClose(hSearch);
	return TRUE;
}
void ReadConfigs(LPCTSTR lpszFileName)
{
	ConfigList.push_back(lpszFileName);
}

void RefreshConfigs()
{
	static TCHAR path[MAX_PATH];
	std::string folder, file;

	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_APPDATA, NULL, 0, path)))
	{
		ConfigList.clear();
		string ConfigDir = std::string(path) + "\\uni.beta\\*.json";
		SearchFiles(ConfigDir.c_str(), ReadConfigs, FALSE);
	}

}
void EnableHiddenCVars()
{
	auto p = **reinterpret_cast<ConCommandBase***>(interfaces.cvars + 0x34);
	for (auto c = p->m_pNext; c != nullptr; c = c->m_pNext)
	{
		ConCommandBase* cmd = c;
		cmd->m_nFlags &= ~FCVAR_DEVELOPMENTONLY;
		cmd->m_nFlags &= ~FCVAR_HIDDEN;
	}
}

Vector2D g_mouse;
color_t main_color = color_t(139, 0, 255);

bool enable_legit() { return vars.legitbot.legitenable; };
bool enable_rage() { return vars.ragebot.enable; };
bool enable_antiaim() { return vars.antiaim.enable; };
bool enable_esp() { return vars.visuals.enable; };

const std::string currentDateTime() {
	time_t     now = time(0);
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%X", &tstruct);

	return buf;
}


void c_menu::draw_indicators()
{
	if (!vars.visuals.watermark)
		return;

	char path[MAX_PATH], * name = NULL;
	GetEnvironmentVariable((LPCTSTR)"USERPROFILE", (LPSTR)path, (DWORD)MAX_PATH);
	name = &path[std::strlen(path) - 1];
	for (; *name != '\\'; --name);
	++name;

	std::stringstream ss;

	auto net_channel = interfaces.engine->GetNetChannelInfo();

	auto local_player = reinterpret_cast<IBasePlayer*>(interfaces.ent_list->GetClientEntity(interfaces.engine->GetLocalPlayer()));
	std::string outgoing = local_player ? std::to_string((int)(net_channel->GetLatency(FLOW_OUTGOING) * 1000)) : "0";

	ss << "unisense.pw | Alpha | " << name << " | " << outgoing.c_str() << "ms" << " | build v2";

	int x, y, w, h;
	int textsize = Drawing::GetStringWidth(fonts::esp_name, ss.str().c_str());

	int screen_x, screen_y;
	interfaces.engine->GetScreenSize(screen_x, screen_y);

	g_Render->FilledRect(screen_x - (textsize + 20), 5, textsize + 10, 18, color_t(33, 33, 33, 150));
    // g_Render->FilledRect(screen_x - (textsize + 20), 5, textsize + 8, 2, color_t(139, 0, 255));
	g_Render->DrawString(screen_x - (textsize + 15), 7, color_t(200, 200, 200), false, fonts::menu_main, ss.str().c_str());
}
bool need_warning() {
	return (csgo->expire_date - time(0)) / 3600 < 48;
};
void c_menu::render() {

	if (window) {
		window->update_keystates();
		window->update_animation();
		update_binds();
	}

	static bool once = false;
	if (!once) {
		Config.ResetToDefault();
		vars.menu.open = true;
		once = true;
	}

	if (initialized) {
		if (window->get_transparency() < 100.f && vars.menu.open)
			window->increase_transparency(animation_speed * 80.f);
	}
	else {
		window = new c_window();
		window->set_size(Vector2D(538, 600));
		window->set_position(Vector2D(10, 10));

		window->add_tab(new c_tab("legitbot", tab_t::legitbot, window)); {
			auto main_child = new c_child("main", tab_t::legitbot, window);
			main_child->set_size(Vector2D(250, 255));
			main_child->set_position(Vector2D(0, 0)); {

				main_child->add_element(new c_checkbox("enable", &vars.legitbot.legitenable));
				main_child->add_element(new c_combo("fov type", &vars.legitbot.fov_type, { "static", "dynamic" }));
				main_child->add_element(new c_combo("smooth type", &vars.legitbot.smooth_type, { "slow near target", "linear" }));
				main_child->add_element(new c_checkbox("friendly fire", &vars.legitbot.deathmatch));
				main_child->add_element(new c_checkbox("smoke check", &vars.legitbot.check_smoke));
				main_child->add_element(new c_checkbox("flash check", &vars.legitbot.check_flash));
				main_child->add_element(new c_checkbox("jump check", &vars.legitbot.check_jump));
				main_child->add_element(new c_keybind("legit", vars.legitbot.legitkey, []() {
					return enable_legit(); }));

				main_child->add_element(new c_combo("aim type", &vars.legitbot.aim_type, { "hitbox","nearest" }));

				if (vars.legitbot.aim_type == 0) {
					main_child->add_element(new c_combo("hitbox", &vars.legitbot.hitbox, { "head","neck","pelvis","stomach","lower chest","chest","upper chest","left thigh", "right thigh" }));
				}

				main_child->add_element(new c_combo("aim priority", &vars.legitbot.priority, { "fov","health","damage","distance" }));


				main_child->add_element(new c_checkbox("curve", &vars.legitbot.humanize));
				if (vars.legitbot.humanize)
					main_child->add_element(new c_slider("curviness", &vars.legitbot.curviness, -10.f, 10.f, "%1.f"));

				main_child->add_element(new c_slider("fov", &vars.legitbot.fov, 0.f, 20.f, "%1.f"));
				if (vars.legitbot.silent) {
				}
				main_child->add_element(new c_slider("smooth", &vars.legitbot.smooth, 0.f, 15.f, "%1.f"));
				if (!vars.legitbot.silent) {
					main_child->add_element(new c_slider("shot delay", &vars.legitbot.shot_delay, 0.f, 100.f, "%1.f"));
				}
				main_child->add_element(new c_slider("kill delay", &vars.legitbot.kill_delay, 0.f, 1000.f, "%1.f"));

				main_child->add_element(new c_checkbox("rcs enable", &vars.legitbot.rcs));
				main_child->add_element(new c_combo("rcs type", &vars.legitbot.rcs_type, { "standalone", "rcs" }));
				main_child->add_element(new c_checkbox("rcs custom fov", &vars.legitbot.rcs_fov_enabled));


				if (vars.legitbot.rcs_fov_enabled) {
					main_child->add_element(new c_slider("rcs fov", &vars.legitbot.rcs_fov, 0.f, 20.f, "%1.f"));
				}
				main_child->add_element(new c_checkbox("rcs custom smooth", &vars.legitbot.rcs_smooth_enabled));
				if (vars.legitbot.rcs_smooth_enabled) {
					main_child->add_element(new c_slider("rcs smooth", &vars.legitbot.rcs_smooth, 1.f, 15.f, "%1.f"));
				}
				main_child->add_element(new c_slider("rcs on x", &vars.legitbot.rcs_x, 0.f, 100.f, "%1.f"));
				main_child->add_element(new c_slider("rcs on y", &vars.legitbot.rcs_y, 0.f, 100.f, "%1.f"));
				main_child->add_element(new c_slider("rcs start", &vars.legitbot.rcs_start, 1.f, 20.f, "%1.f"));

				main_child->initialize_elements();
			}
			window->add_element(main_child);

			auto antiaim_leg = new c_child("legit anti-aim", tab_t::legitbot, window);
			antiaim_leg->set_size(Vector2D(250, 255));
			antiaim_leg->set_position(Vector2D(0, 265)); {

				antiaim_leg->add_element(new c_checkbox("enable",
					&vars.legitaa.enableaa));

				antiaim_leg->add_element(new c_combo("desync", &vars.legitaa.deslegit,
					{
						"off",
						"static",
						"low delta legit",
						"lagsync"
					},
					enable_antiaim));

				antiaim_leg->initialize_elements();
			}
			window->add_element(antiaim_leg);
		}

		window->add_tab(new c_tab("ragebot", tab_t::rage, window)); {
			auto main_child = new c_child("main", tab_t::rage, window);
			main_child->set_size(Vector2D(250, 520));
			main_child->set_position(Vector2D(0, 0)); {

				main_child->add_element(new c_checkbox("enable",
					&vars.ragebot.enable));

				main_child->add_element(new c_checkbox("auto fire",
					&vars.ragebot.autoshoot, enable_rage));

				main_child->add_element(new c_checkbox("auto scope",
					&vars.ragebot.autoscope, enable_rage));

				main_child->add_element(new c_checkbox("backtrack",
					&vars.ragebot.posadj, enable_rage));

				main_child->add_element(new c_checkbox("on shot priority", &vars.ragebot.backshoot_bt,
					[]() { return vars.ragebot.enable && vars.ragebot.posadj; }));

				main_child->add_element(new c_checkbox("delay shot", &vars.ragebot.delayshot,
					enable_rage));

				main_child->add_element(new c_checkbox("resolver", &vars.ragebot.resolver,
					enable_rage));

				main_child->add_element(new c_checkbox("hitchance consider hitbox",
					&vars.ragebot.hitchance_consider_hitbox, enable_rage));

				main_child->add_element(new c_keybind("force baim", vars.ragebot.force_body, []() {
					return vars.ragebot.enable;
					}));

				main_child->add_element(new c_colorpicker(&vars.ragebot.shot_clr,
					color_t(255, 255, 255, 255), [] { return vars.ragebot.enable && vars.ragebot.shotrecord; }));

				main_child->add_element(new c_checkbox("shot record",
					&vars.ragebot.shotrecord, enable_rage));

				main_child->add_element(new c_keybind("override damage",
					vars.ragebot.override_dmg, enable_rage));

				main_child->add_element(new c_keybind("double tap", vars.ragebot.double_tap, []() {
					return vars.ragebot.enable;
					}));

				main_child->add_element(new c_keybind("hide shots", vars.ragebot.hide_shots, []() {
					return vars.ragebot.enable;
					}));

				main_child->add_element(new c_keybind("Airstuck", vars.misc.airstuckkey, []() {
					return vars.ragebot.enable;
					}));

				main_child->add_element(new c_checkbox("instant",
					&vars.ragebot.dt_instant, []() { return vars.ragebot.double_tap->key > 0; }));

				main_child->add_element(new c_checkbox("disable dt delay",
					&vars.ragebot.disable_dt_delay, []() { return vars.ragebot.double_tap->key > 0 && enable_rage || vars.ragebot.double_tap->type == 4 && enable_rage; }));

				main_child->add_element(new c_slider("doubletap speed boost (beta)", &vars.ragebot.dt_speed_boost, 0.f, 100.f, "%.0f", //premium meme
					[]() { return vars.ragebot.double_tap->key > 0 && enable_rage || vars.ragebot.double_tap->type == 4 && enable_rage; }));

				main_child->initialize_elements();
			}
			window->add_element(main_child);

			reinit_weapon_cfg();
		}

		window->add_tab(new c_tab("antiaim", tab_t::antiaim, window)); {
			auto antiaim_main = new c_child("main", tab_t::antiaim, window);
			antiaim_main->set_size(Vector2D(250, 520));
			antiaim_main->set_position(Vector2D(0, 0)); {

				auto antiaim_main = new c_child("anti-aim", tab_t::antiaim, window);
				antiaim_main->set_size(Vector2D(250, 520));
				antiaim_main->set_position(Vector2D(0, 0)); {

					antiaim_main->add_element(new c_checkbox("enable", &vars.antiaim.enable));

					antiaim_main->add_element(new c_combo("desync", &vars.antiaim.desync,
						{
							"off",
							"static",
							"dynamic",
							"jitter",
							"hybrid",
							"low delta"
						},
						enable_antiaim));

					antiaim_main->add_element(new c_keybind("desync inverter", vars.antiaim.aa_override.inverter, []() { return enable_antiaim(); }));

					antiaim_main->add_element(new c_checkbox("at targets",
						&vars.antiaim.attarget, []() { return enable_antiaim(); }));

					antiaim_main->add_element(new c_checkbox("jitter side",
						&vars.antiaim.jitter, []() { return enable_antiaim(); }));

					antiaim_main->add_element(new c_checkbox("lagsync",
						&vars.antiaim.lagsync, []() { return enable_antiaim(); }));

					antiaim_main->add_element(new c_checkbox("manunal override",
						&vars.antiaim.aa_override.enable, []() { return enable_antiaim(); }));

					antiaim_main->add_element(new c_keybind("left", vars.antiaim.aa_override.left,
						[]() { return enable_antiaim() && vars.antiaim.aa_override.enable; }));
					antiaim_main->add_element(new c_keybind("right", vars.antiaim.aa_override.right,
						[]() { return enable_antiaim() && vars.antiaim.aa_override.enable; }));
					antiaim_main->add_element(new c_keybind("back", vars.antiaim.aa_override.back,
						[]() { return enable_antiaim() && vars.antiaim.aa_override.enable; }));

					antiaim_main->add_element(new c_combo("fake lag", &vars.antiaim.fakelag,
						{
							"off",
							"factor",
							"switch",
						},
						enable_antiaim));

					antiaim_main->add_element(new c_slider("", &vars.antiaim.fakelagfactor, 1, 14, "%.0f packets",
						[]() { return enable_antiaim() && vars.antiaim.fakelag > 0; }));

					antiaim_main->add_element(new c_slider("", &vars.antiaim.fakelagvariance, 0.f, 100.f, "%.0f ticks to switch",
						[]() { return enable_antiaim() && vars.antiaim.fakelag == 2; }));

					antiaim_main->add_element(new c_checkbox("on shot fake lag",
						&vars.antiaim.fakelag_onshot, []() { return enable_antiaim() && vars.antiaim.fakelag; }));

					// antiaim_main->add_element(new c_checkbox("disable fake lag when stand",
						// &vars.antiaim.disable_fakelag_when_stand, []() { return enable_antiaim() && vars.antiaim.fakelag; }));

					antiaim_main->add_element(new c_keybind("fake duck", vars.antiaim.fakeduck,
						[]() { return enable_antiaim(); }));

					antiaim_main->add_element(new c_keybind("slow walk", vars.antiaim.slowwalk,
						[]() { return enable_antiaim(); }));

					antiaim_main->initialize_elements();
				}
				window->add_element(antiaim_main);
			}

		}

		window->set_transparency(100.f);
		window->add_tab(new c_tab("players", tab_t::esp, window)); {
			reinit_chams();

			auto esp_main = new c_child("esp", tab_t::esp, window);
			esp_main->set_size(Vector2D(250, 255));
			esp_main->set_position(Vector2D(258, 0)); {

				esp_main->add_element(new c_checkbox("enable",
					&vars.visuals.enable));

				esp_main->add_element(new c_checkbox("on dormant",
					&vars.visuals.dormant, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.box_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.box; }));

				esp_main->add_element(new c_checkbox("box",
					&vars.visuals.box, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.skeleton_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.skeleton; }));

				esp_main->add_element(new c_checkbox("skeleton",
					&vars.visuals.skeleton, enable_esp));

				esp_main->add_element(new c_checkbox("health",
					&vars.visuals.healthbar, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.hp_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.override_hp; }));

				esp_main->add_element(new c_checkbox("override health",
					&vars.visuals.override_hp, [] { return enable_esp() && vars.visuals.healthbar; }));

				esp_main->add_element(new c_colorpicker(&vars.visuals.name_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.name; }));

				esp_main->add_element(new c_checkbox("name",
					&vars.visuals.name, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.weapon_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.weapon; }));

				esp_main->add_element(new c_checkbox("weapon",
					&vars.visuals.weapon, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.ammo_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.ammo; }));

				esp_main->add_element(new c_checkbox("ammo",
					&vars.visuals.ammo, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.flags_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.flags > 0; }));

				esp_main->add_element(new c_multicombo("flags",
					&vars.visuals.flags, {
						"armor",
						"scoped",
						"flashed",
						"can hit",
						"resolver mode",
						"choke count"
					}, enable_esp));
				esp_main->add_element(new c_checkbox("show multipoint", &vars.visuals.shot_multipoint, enable_esp));

				esp_main->add_element(new c_colorpicker(&vars.visuals.out_of_fov_color,
					color_t(255, 255, 255, 255), []() { return enable_esp() && vars.visuals.out_of_fov; }));

				esp_main->add_element(new c_checkbox("out of fov arrow",
					&vars.visuals.out_of_fov, enable_esp));

				esp_main->add_element(new c_slider("size", &vars.visuals.out_of_fov_size, 10, 30, "%.0f px",
					[]() { return enable_esp() && vars.visuals.out_of_fov; }));

				esp_main->add_element(new c_slider("distance", &vars.visuals.out_of_fov_distance, 5, 30, "%.0f",
					[]() { return enable_esp() && vars.visuals.out_of_fov; }));

				esp_main->initialize_elements();
			}
			window->add_element(esp_main);

			auto misc_esp_main = new c_child("misc", tab_t::esp, window);
			misc_esp_main->set_size(Vector2D(250, 255));
			misc_esp_main->set_position(Vector2D(258, 265)); {

				misc_esp_main->add_element(new c_colorpicker(&vars.visuals.hitmarker_color,
					color_t(255, 255, 255, 255), []() { return vars.visuals.hitmarker > 0; }));

				misc_esp_main->add_element(new c_combo("hitmarker",
					&vars.visuals.hitmarker, { "off", "circle", "cross" }));

				misc_esp_main->add_element(new c_checkbox("hitmarker sound",
					&vars.visuals.hitmarker_sound, []() { return vars.visuals.hitmarker > 0; }));

				misc_esp_main->add_element(new c_colorpicker(&vars.visuals.eventlog_color,
					color_t(255, 255, 255, 255), []() { return vars.visuals.eventlog; }));

				misc_esp_main->add_element(new c_checkbox("event log",
					&vars.visuals.eventlog));

				misc_esp_main->add_element(new c_multicombo("indicator",
					&vars.visuals.indicators, {
						"fake",
						"lag comp",
						"fake duck",
						"override dmg",
						"force baim",
						"double tap",

					}));

				misc_esp_main->add_element(new c_keybind("thirdperson key", vars.misc.thirdperson_bind));

				misc_esp_main->add_element(new c_slider("", &vars.visuals.thirdperson_dist, 0, 300, "%.0f units"));

				misc_esp_main->add_element(new c_checkbox("disable thirdperson with nade", &vars.visuals.disable_thirdperson_with_granade));

				misc_esp_main->initialize_elements();
			}
			window->add_element(misc_esp_main);
		}

		window->add_tab(new c_tab("world", tab_t::world, window)); {

			auto trace_child = new c_child("tracers", tab_t::world, window);
			trace_child->set_size(Vector2D(250, 520));
			trace_child->set_position(Vector2D(0, 0)); {
				trace_child->add_element(new c_colorpicker(&vars.visuals.bullet_tracer_color,
					color_t(255, 255, 255, 255), []() { return vars.visuals.bullet_tracer; }));

				trace_child->add_element(new c_checkbox("bullet tracer",
					&vars.visuals.bullet_tracer));

				trace_child->add_element(new c_colorpicker(&vars.visuals.bullet_tracer_local_color,
					color_t(255, 255, 255, 255), []() { return vars.visuals.bullet_tracer_local; }));

				trace_child->add_element(new c_checkbox("local bullet tracer",
					&vars.visuals.bullet_tracer_local));

				trace_child->add_element(new c_combo("sprite", &vars.visuals.bullet_tracer_type,
					{
						"default",
						"phys beam",
						"bubble",
						"glow"
					},
					[]() { return vars.visuals.bullet_tracer; }));

				trace_child->add_element(new c_checkbox("show impacts",
					&vars.visuals.bullet_impact));

				trace_child->add_element(new c_colorpicker(&vars.visuals.bullet_impact_color,
					color_t(255, 0, 0, 255), []() { return vars.visuals.bullet_impact; }));

				trace_child->add_element(new c_text("server impact", []() { return vars.visuals.bullet_impact; }));

				trace_child->add_element(new c_colorpicker(&vars.visuals.client_impact_color,
					color_t(255, 0, 0, 255), []() { return vars.visuals.bullet_impact; }));

				trace_child->add_element(new c_text("client impact", []() { return vars.visuals.bullet_impact; }));

				trace_child->add_element(new c_slider("impacts size", &vars.visuals.impacts_size, 0.f, 5.f, "%.2f%%"));

				trace_child->add_element(new c_colorpicker(&vars.visuals.nadepred_color,
					color_t(255, 255, 255, 255), []() { return vars.visuals.nadepred; }));

				trace_child->add_element(new c_checkbox("grenade tracer",
					&vars.visuals.nadepred));

				trace_child->initialize_elements();
			}
			window->add_element(trace_child);

			auto effects_child = new c_child("effects", tab_t::world, window);
			effects_child->set_size(Vector2D(250, 520));
			effects_child->set_position(Vector2D(258, 0)); {

				effects_child->add_element(new c_multicombo("removals",
					&vars.visuals.remove, {
						"visual recoil",
						"smoke",
						"flash",
						"scope",
						"zoom",
						"post processing",
						"fog",
						"shadow"
					}));

				effects_child->add_element(new c_checkbox("world modulation", &vars.visuals.nightmode));
				{
					effects_child->add_element(new c_checkbox("fullbright", &vars.visuals.fullbright, []() { return vars.visuals.nightmode; }));

					effects_child->add_element(new c_checkbox("fog", &vars.visuals.fog, []() { return vars.visuals.nightmode; }));

					effects_child->add_element(new c_colorpicker(&vars.visuals.fog_color,
						color_t(255, 255, 255, 255), []() { return vars.visuals.nightmode; }));

					effects_child->add_element(new c_text("fog color", []() { return vars.visuals.nightmode; }));

					effects_child->add_element(new c_colorpicker(&vars.visuals.nightmode_color,
						color_t(101, 97, 107, 255), []() { return vars.visuals.nightmode; }));

					effects_child->add_element(new c_text("world", []() { return vars.visuals.nightmode; }));


					effects_child->add_element(new c_colorpicker(&vars.visuals.nightmode_prop_color,
						color_t(255, 255, 255, 255), []() { return vars.visuals.nightmode; }));

					effects_child->add_element(new c_text("props", []() { return vars.visuals.nightmode; }));


					effects_child->add_element(new c_colorpicker(&vars.visuals.nightmode_skybox_color,
						color_t(194, 101, 35, 255), []() { return vars.visuals.nightmode; }));

					effects_child->add_element(new c_text("skybox", []() { return vars.visuals.nightmode; }));
				}

				effects_child->add_element(new c_checkbox("force crosshair", &vars.visuals.force_crosshair));

				effects_child->add_element(new c_checkbox("kill fade", &vars.visuals.kill_effect));

				effects_child->add_element(new c_slider("world fov", &vars.misc.worldfov, 90, 145, "%.0f"));

				effects_child->add_element(new c_slider("viewmodel fov", &vars.misc.viewmodelfov, 68, 145, "%.0f"));

				effects_child->add_element(new c_slider("aspect ratio", &vars.visuals.aspect_ratio, 0, 250, "%.0f"));

				effects_child->initialize_elements();
			}
			window->add_element(effects_child);
		}

		window->add_tab(new c_tab("misc", tab_t::misc, window)); {
			auto main_child = new c_child("main", tab_t::misc, window);
			main_child->set_size(Vector2D(250, 255));
			main_child->set_position(Vector2D(0, 0)); {

				main_child->add_element(new c_checkbox("anti untrusted", &vars.misc.antiuntrusted));

				main_child->add_element(new c_checkbox("bunny hop", &vars.misc.bunnyhop));

				main_child->add_element(new c_checkbox("knife bot", &vars.misc.knifebot));

				main_child->add_element(new c_checkbox("clant tag", &vars.visuals.clantagspammer));

				main_child->add_element(new c_checkbox("watermark",
					&vars.visuals.watermark));

				main_child->add_element(new c_checkbox("hold firing animation",
					&vars.misc.hold_firinganims));

				main_child->add_element(new c_text("viewmodel offset"));

				main_child->add_element(new c_slider("", &vars.misc.viewmodel_x, -50.f, 50.f, "x: %.0f"));

				main_child->add_element(new c_slider("", &vars.misc.viewmodel_y, -50.f, 50.f, "y: %.0f"));

				main_child->add_element(new c_slider("", &vars.misc.viewmodel_z, -50.f, 50.f, "z: %.0f"));

				main_child->add_element(new c_checkbox("buy bot",
					&vars.misc.autobuy.enable));

				main_child->add_element(new c_combo("primary weapon",
					&vars.misc.autobuy.main, {
						"auto sniper",
						"scout",
						"awp"
					}, []() { return vars.misc.autobuy.enable; }));

				main_child->add_element(new c_combo("secondary weapon",
					&vars.misc.autobuy.pistol, {
						"dual beretta",
						"heavy pistol"
					}, []() { return vars.misc.autobuy.enable; }));

				main_child->add_element(new c_multicombo("other",
					&vars.misc.autobuy.misc, {
						"head helmet",
						"other helmet",
						"he grenade",
						"molotov",
						"smoke",
						"taser",
						"defuse kit",
					}, []() { return vars.misc.autobuy.enable; }));

				main_child->initialize_elements();
			}
			window->add_element(main_child);

			auto other_child = new c_child("misc", tab_t::misc, window);
			other_child->set_size(Vector2D(250, 255));
			other_child->set_position(Vector2D(0, 265)); {
				//other_child->add_element(new c_button("unload", []() { csgo->DoUnload = true; }));
				other_child->add_element(new c_button("spoof sv cheats", []() {
					ConVar* sv_cheats = interfaces.cvars->FindVar(hs::sv_cheats::s().c_str());
					*(int*)((DWORD)&sv_cheats->m_fnChangeCallbacks + 0xC) = 0;
					sv_cheats->SetValue(1);
					}));
				other_child->add_element(new c_button("full update", []() { csgo->client_state->ForceFullUpdate(); }));
				other_child->add_element(new c_button("unlock hidden cvars", EnableHiddenCVars));
				other_child->initialize_elements();
			}
			window->add_element(other_child);

			reinit_config();
		}

		window->set_active_tab_index(-1); // любой неиспользовавшийся индекс таба, для удобства я использую -1, так как его я использовать не буду
		initialized = true;

		{
			auto undef_child = new c_child("Welcome", tab_t::undefined, window);

			undef_child->set_position(Vector2D(0, 0));
			undef_child->set_size(Vector2D(508, 520));

			undef_child->add_element(new c_text(u8"ВНИМАНИЕ!", need_warning, color_t(220, 35, 35)));


			std::stringstream ss;
			ss << "Hello" << csgo->username << ", welcome back\n";


			undef_child->add_element(new c_text(ss.str(), nullptr, color_t(255, 255, 255, 255))); // add steam name
			undef_child->add_element(new c_text(u8"\n unisense.pw v2 [debug]", nullptr, color_t(255, 255, 255, 255)));
			undef_child->add_element(new c_text(u8"\n Developers by. BURY | Recode", nullptr, color_t(139, 0, 255)));
			// undef_child->add_element(new c_text(u8"Changelogs\n-new undetect hooking(minhook library)\n-sv_pure bypass")); 


			undef_child->initialize_elements();
			window->add_element(undef_child);
		}

		window->set_active_tab_index(tab_t::undefined);
		initialized = true;
	}

	if (!vars.menu.open) {
		if (window->get_transparency() > 0.f)
			window->decrease_transparency(animation_speed * 80.f);
	}

	ImGui::GetIO().MouseDrawCursor = window->get_transparency() > 0;
	POINT mp;
	GetCursorPos(&mp);
	ScreenToClient(GetForegroundWindow(), &mp);
	g_mouse.x = mp.x;
	g_mouse.y = mp.y;
	if (should_reinit_weapon_cfg) {
		reinit_weapon_cfg();
		should_reinit_weapon_cfg = false;
	}
	if (should_reinit_chams) {
		reinit_chams();
		should_reinit_chams = false;
	}
	if (should_reinit_config) {
		reinit_config();
		should_reinit_config = false;
	}

	window->render();
	if (window->g_hovered_element) {
		if (window->g_hovered_element->type == c_elementtype::input_text)
			ImGui::SetMouseCursor(ImGuiMouseCursor_TextInput);
	}
	csgo->scroll_amount = 0;
}

void c_menu::update_binds()
{
	for (auto e : window->elements) {
		if (e->type == c_elementtype::child) {
			for (auto el : ((c_child*)e)->elements) {
				if (el->type == c_elementtype::checkbox) {
					auto c = (c_checkbox*)el;
					auto binder = c->bind;
					if (binder) {
						binder->key = std::clamp<unsigned int>(binder->key, 0, 255);

						if (binder->type == 2 && binder->key > 0) {
							if (window->key_updated(binder->key)) {
								*(bool*)c->get_ptr() = !(*(bool*)c->get_ptr());
							}
						}
						else if (binder->type == 1 && binder->key > 0) {
							*(bool*)c->get_ptr() = csgo->key_pressed[binder->key];
						}
						else if (binder->type == 3 && binder->key > 0) {
							*(bool*)c->get_ptr() = !csgo->key_pressed[binder->key];
						}
						binder->active = *(bool*)c->get_ptr();
					}
				}
				else if (el->type == c_elementtype::keybind) {
					auto c = (c_keybind*)el;
					auto binder = ((c_keybind*)el)->bind;
					if (binder) {
						binder->key = std::clamp<unsigned int>(binder->key, 0, 255);

						if (binder->type == 2 && binder->key > 0) {
							if (window->key_updated(binder->key)) {
								binder->active = !binder->active;
							}
						}
						else if (binder->type == 1 && binder->key > 0) {
							binder->active = csgo->key_pressed[binder->key];
						}
						else if (binder->type == 3 && binder->key > 0) {
							binder->active = !csgo->key_pressed[binder->key];
						}
						else if (binder->type == 0)
							binder->active = false;
						else if (binder->type == 4)
							binder->active = true;
					}
				}
			}
		}
	}
}

bool override_default() {
	return vars.ragebot.enable && (vars.ragebot.active_index == 0 || vars.ragebot.weapon[vars.ragebot.active_index].enable);
}

void c_menu::reinit_weapon_cfg()
{
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "aimbot cfg") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}
	auto cfg_child = new c_child("aimbot cfg", tab_t::rage, window);
	cfg_child->set_size(Vector2D(250, 520));
	cfg_child->set_position(Vector2D(258, 0)); {
		cfg_child->add_element(new c_combo("weapon", &vars.ragebot.active_index, {
			"default",
			"scar",
			"scout",
			"awp",
			"rifles",
			"pistols",
			"heavy pistols"
			}, enable_rage, [](int) { g_Menu->should_reinit_weapon_cfg = true; }));

		cfg_child->add_element(new c_checkbox("override default", &vars.ragebot.weapon[vars.ragebot.active_index].enable,
			[]() { return enable_rage() && vars.ragebot.active_index > 0; }));

		cfg_child->add_element(new c_slider("hitchance", &vars.ragebot.weapon[vars.ragebot.active_index].hitchance, 0, 100, "%.0f%%",
			override_default));

		cfg_child->add_element(new c_checkbox("quick stop", &vars.ragebot.weapon[vars.ragebot.active_index].quickstop,
			[]() { return override_default(); }));

		//cfg_child->add_element(new c_combo("quick stop", &vars.ragebot.weapon[vars.ragebot.active_index].quickstop, {
		//	"off",
		//	"full",
		//	"slide",
		//}, override_default));

		cfg_child->add_element(new c_multicombo("", &vars.ragebot.weapon[vars.ragebot.active_index].quickstop_options, {
			"between shots",
			"only when lethal",
			},
			[]() {
				return override_default() && vars.ragebot.weapon[vars.ragebot.active_index].quickstop;
			}));

		cfg_child->add_element(new c_slider("minimum damage", &vars.ragebot.weapon[vars.ragebot.active_index].mindamage, 0, 120, "%.0f hp",
			override_default));

		cfg_child->add_element(new c_slider("minimum damage [override]", &vars.ragebot.weapon[vars.ragebot.active_index].mindamage_override, 0, 120, "%.0f hp",
			[]() { return override_default() && vars.ragebot.override_dmg->key > 0; }));

		cfg_child->add_element(new c_multicombo("hitboxes", &vars.ragebot.weapon[vars.ragebot.active_index].hitscan, {
			"head",
			"neck",
			"upper chest",
			"chest",
			"stomach",
			"pelvis",
			"arms",
			"legs",
			"feet",
			}, override_default));

		cfg_child->add_element(new c_checkbox("enable multipoint", &vars.ragebot.weapon[vars.ragebot.active_index].multipoint, override_default));

		cfg_child->add_element(new c_slider("head scale", &vars.ragebot.weapon[vars.ragebot.active_index].pointscale_head,
			0, 100, "%.0f%%", []() { return override_default() && vars.ragebot.weapon[vars.ragebot.active_index].multipoint; }));

		cfg_child->add_element(new c_slider("body scale", &vars.ragebot.weapon[vars.ragebot.active_index].pointscale_body,
			0, 100, "%.0f%%", []() { return override_default() && vars.ragebot.weapon[vars.ragebot.active_index].multipoint; }));

		cfg_child->add_element(new c_slider("body aim when hp lower than", &vars.ragebot.weapon[vars.ragebot.active_index].baim_under_hp,
			0, 100, "%.0f hp", []() { return override_default(); }));

		cfg_child->add_element(new c_multicombo("override hitbox if", &vars.ragebot.weapon[vars.ragebot.active_index].baim, {
			"in air",
			"unresolved",
			"lethal"
			},
			override_default));

		cfg_child->add_element(new c_slider("max missed shot count", &vars.ragebot.weapon[vars.ragebot.active_index].max_misses,
			0, 5, "%.0f", []() {
				return override_default()
					&& (vars.ragebot.weapon[vars.ragebot.active_index].baim & 2);
			}));

		cfg_child->add_element(new c_multicombo("hitbox on override", &vars.ragebot.weapon[vars.ragebot.active_index].hitscan_baim, {
			"chest",
			"stomach",
			"pelvis",
			"legs",
			"feet",
			},
			[]() { return
			override_default()
			&& (vars.ragebot.weapon[vars.ragebot.active_index].baim > 0 ||
				vars.ragebot.weapon[vars.ragebot.active_index].baim_under_hp > 0
				|| vars.ragebot.force_body->key > 0);
			}));

		cfg_child->add_element(new c_checkbox("adaptive body aim", &vars.ragebot.weapon[vars.ragebot.active_index].adaptive_baim,
			[]() {
				return override_default();
			}));
		cfg_child->initialize_elements();
	}
	window->add_element(cfg_child);
}

void c_menu::reinit_config() {
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "configs") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}

	RefreshConfigs();
	auto config_child = new c_child("configs", tab_t::misc, window);
	config_child->set_size(Vector2D(250, 520));
	config_child->set_position(Vector2D(258, 0)); {
		config_child->add_element(new c_listbox("configs", &vars.menu.active_config_index, ConfigList, 150.f));
		config_child->add_element(new c_input_text("config name", &vars.menu.active_config_name, false));

		config_child->add_element(new c_button("load", []() {
			Config.Load(ConfigList[vars.menu.active_config_index]);
			}, []() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->add_element(new c_button("save", []() {
			Config.Save(ConfigList[vars.menu.active_config_index]);
			}, []() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->add_element(new c_button("refresh", []() { g_Menu->should_reinit_config = true; }));

		config_child->add_element(new c_button("create", []() {
			string add;
			if (vars.menu.active_config_name.find(".json") == -1)
				add = ".json";
			Config.Save(vars.menu.active_config_name + add);
			g_Menu->should_reinit_config = true;
			vars.menu.active_config_name.clear();
			}));

		config_child->add_element(new c_button("reset to default", []() { Config.ResetToDefault(); },
			[]() { return ConfigList.size() > 0 && vars.menu.active_config_index >= 0; }));

		config_child->initialize_elements();
	}
	window->add_element(config_child);
}

void c_menu::reinit_chams() {
	for (int i = 0; i < window->elements.size(); i++) {
		auto& e = window->elements[i];
		if (((c_child*)e)->get_title() == "models") {
			window->elements.erase(window->elements.begin() + i);
			break;
		}
	}
	auto cfg_child = new c_child("models", tab_t::esp, window);
	cfg_child->set_size(Vector2D(250, 520));
	cfg_child->set_position(Vector2D(0, 0)); {
		cfg_child->add_element(new c_combo("category", &vars.visuals.active_chams_index, {
				"enemy",
				"shadow",
				"local",
				"desync",
				"arms",
				"weapon",
				"glow"
			}, nullptr, [](int) { g_Menu->should_reinit_chams = true; }));

		switch (vars.visuals.active_chams_index)
		{
		case 0: { // enemy
			cfg_child->add_element(new c_colorpicker(&vars.visuals.chamscolor,
				color_t(0, 121, 254, 255), []() { return vars.visuals.chams; }));
			cfg_child->add_element(new c_checkbox("on enemy", &vars.visuals.chams));

			cfg_child->add_element(new c_combo("overlay", &vars.visuals.overlay, {
					"off",
					"glow outline",
					"glow fade",
				}, []() { return vars.visuals.chams; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.glow_col,
				color_t(255, 255, 255, 0), []() { return vars.visuals.chams && vars.visuals.overlay > 0; }));
			cfg_child->add_element(new c_text("overlay color", []() { return vars.visuals.chams && vars.visuals.overlay > 0; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.chamscolor_xqz,
				color_t(0, 100, 255, 255), []() { return vars.visuals.chamsxqz; }));
			cfg_child->add_element(new c_checkbox("through walls", &vars.visuals.chamsxqz));

			cfg_child->add_element(new c_combo("overlay", &vars.visuals.overlay_xqz, {
					"off",
					"glow outline",
					"glow fade",
				}, []() { return vars.visuals.chamsxqz; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.glow_col_xqz,
				color_t(255, 255, 255, 0), []() { return vars.visuals.chamsxqz && vars.visuals.overlay_xqz > 0; }));
			cfg_child->add_element(new c_text("overlay color", []() { return vars.visuals.chamsxqz && vars.visuals.overlay_xqz > 0; }));

			cfg_child->add_element(new c_combo("material", &vars.visuals.chamstype, {
					"normal",
					"flat",
					"metallic",
				}, []() { return vars.visuals.chams || vars.visuals.chamsxqz; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.metallic_clr,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));
			cfg_child->add_element(new c_text("metallic", []()
				{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));
			cfg_child->add_element(new c_colorpicker(&vars.visuals.metallic_clr2,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));
			cfg_child->add_element(new c_text("phong", []()
				{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));

			cfg_child->add_element(new c_slider("", &vars.visuals.phong_exponent, 0, 100, "exponent: %.0f",
				[]()
				{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));
			cfg_child->add_element(new c_slider("", &vars.visuals.phong_boost, 0, 100, "boost: %.0f%%",
				[]()
				{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.rim, -100.f, 100.f, "%.0f",
				[]()
				{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));
			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.pearlescent, -100.f, 100.f, "%.0f%%",
				[]()
				{ return vars.visuals.chamstype == 2 && (vars.visuals.chams || vars.visuals.chamsxqz); }));

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.chams_brightness, 0.f, 300.f, "%.0f%%",
				[]()
				{ return vars.visuals.chams || vars.visuals.chamsxqz; }));
		}
			  break;
		case 1: // history
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[history].clr,
				color_t(255, 255, 255, 255), []() { return vars.visuals.misc_chams[history].enable; }));
			cfg_child->add_element(new c_checkbox("on history", &vars.visuals.misc_chams[history].enable));
			cfg_child->add_element(new c_checkbox("interpolated", &vars.visuals.interpolated_bt));
			cfg_child->add_element(new c_combo("material", &vars.visuals.misc_chams[history].material, {
					"normal",
					"flat",
					"metallic",
				}, []() { return vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[history].metallic_clr,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_text("metallic", []()
				{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[history].metallic_clr2,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_text("phong", []()
				{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[history].phong_exponent, 0, 100, "exponent: %.0f",
				[]()
				{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[history].phong_boost, 0, 100, "boost: %.0f%%",
				[]()
				{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.misc_chams[history].rim, -100, 100, "%.0f",
				[]()
				{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.misc_chams[history].pearlescent, -100.f, 100.f, "%.0f",
				[]()
				{ return vars.visuals.misc_chams[history].material == 2 && vars.visuals.misc_chams[history].enable; }));

			{
				cfg_child->add_element(new c_combo("overlay", &vars.visuals.misc_chams[history].overlay, {
					"off",
					"glow outline",
					"glow fade",
					}, []() { return vars.visuals.misc_chams[history].enable; }));

				cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[history].glow_clr,
					color_t(255, 255, 255, 255), []() { return vars.visuals.misc_chams[history].enable && vars.visuals.misc_chams[history].overlay > 0; }));
				cfg_child->add_element(new c_text("overlay color", []() { return vars.visuals.misc_chams[history].enable && vars.visuals.misc_chams[history].overlay > 0; }));
			}

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.misc_chams[history].chams_brightness, 0.f, 300.f, "%.0f%%",
				[]()
				{ return vars.visuals.misc_chams[history].enable; }));

		}
		break;
		case 2: // local
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.localchams_color,
				color_t(255, 255, 255, 255), []() { return vars.visuals.localchams; }));
			cfg_child->add_element(new c_checkbox("on local", &vars.visuals.localchams));
			cfg_child->add_element(new c_checkbox("interpolated", &vars.visuals.interpolated_model));

			cfg_child->add_element(new c_combo("material", &vars.visuals.localchamstype, {
					"normal",
					"flat",
					"metallic",
				}, []() { return vars.visuals.localchams; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.local_chams.metallic_clr,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_text("metallic", []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.local_chams.metallic_clr2,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_text("phong", []()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.local_chams.phong_exponent, 0, 100, "exponent: %.0f",
				[]()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.local_chams.phong_boost, 0, 100, "boost: %.0f%%",
				[]()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.local_chams.rim, -100, 100, "%.0f",
				[]()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.local_chams.pearlescent, -100.f, 100.f, "%.0f",
				[]()
				{ return vars.visuals.localchamstype == 2 && vars.visuals.localchams; }));

			{
				cfg_child->add_element(new c_combo("overlay", &vars.visuals.local_chams.overlay, {
					"off",
					"glow outline",
					"glow fade",
					}, []() { return vars.visuals.localchams; }));

				cfg_child->add_element(new c_colorpicker(&vars.visuals.local_glow_color,
					color_t(255, 255, 255, 255), []() { return vars.visuals.localchams && vars.visuals.local_chams.overlay > 0; }));
				cfg_child->add_element(new c_text("overlay color", []() { return vars.visuals.localchams && vars.visuals.local_chams.overlay > 0; }));
			}

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.local_chams_brightness, 0.f, 300.f, "%.0f%%",
				[]()
				{ return vars.visuals.localchams; }));

			cfg_child->add_element(new c_checkbox("blend on scope", &vars.visuals.blend_on_scope));

			cfg_child->add_element(new c_slider("", &vars.visuals.blend_value, 0.f, 100.f, "%.0f",
				[]() { return vars.visuals.blend_on_scope; }));

		}
		break;
		case 3: // desync
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[desync].clr,
				color_t(255, 255, 255, 255), []() { return vars.visuals.misc_chams[desync].enable; }));
			cfg_child->add_element(new c_checkbox("on desync", &vars.visuals.misc_chams[desync].enable));
			cfg_child->add_element(new c_checkbox("interpolated", &vars.visuals.interpolated_dsy));

			cfg_child->add_element(new c_combo("material", &vars.visuals.misc_chams[desync].material, {
					"normal",
					"flat",
					"metallic",
				}, []() { return vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[desync].metallic_clr,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_text("metallic", []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[desync].metallic_clr2,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_text("phong", []()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[desync].phong_exponent, 0, 100, "exponent: %.0f",
				[]()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[desync].phong_boost, 0, 100, "boost: %.0f%%",
				[]()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.misc_chams[desync].rim, -100, 100, "%.0f",
				[]()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.misc_chams[desync].pearlescent, -100.f, 100.f, "%.0f",
				[]()
				{ return vars.visuals.misc_chams[desync].material == 2 && vars.visuals.misc_chams[desync].enable; }));

			{
				cfg_child->add_element(new c_combo("overlay", &vars.visuals.misc_chams[desync].overlay, {
					"off",
					"glow outline",
					"glow fade",
					}, []() { return vars.visuals.misc_chams[desync].enable; }));

				cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[desync].glow_clr,
					color_t(255, 255, 255, 255), []() { return vars.visuals.misc_chams[desync].enable && vars.visuals.misc_chams[desync].overlay > 0; }));
				cfg_child->add_element(new c_text("overlay color", []() { return vars.visuals.misc_chams[desync].enable && vars.visuals.misc_chams[desync].overlay > 0; }));
			}

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.misc_chams[desync].chams_brightness, 0.f, 300.f, "%.0f%%",
				[]()
				{ return vars.visuals.misc_chams[desync].enable; }));

		}
		break;
		case 4: // arms
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[arms].clr,
				color_t(255, 255, 255, 255), []() { return vars.visuals.misc_chams[arms].enable; }));
			cfg_child->add_element(new c_checkbox("on arms", &vars.visuals.misc_chams[arms].enable));

			cfg_child->add_element(new c_combo("material", &vars.visuals.misc_chams[arms].material, {
					"normal",
					"flat",
					"metallic",
				}, []() { return vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[arms].metallic_clr,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_text("metallic", []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[arms].metallic_clr2,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_text("phong", []()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[arms].phong_exponent, 0.f, 100.f, "exponent: %.0f",
				[]()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[arms].phong_boost, 0.f, 10.f, "boost: %.0f%%",
				[]()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.misc_chams[arms].rim, -100.f, 100.f, "%.0f",
				[]()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.misc_chams[arms].pearlescent, -100.f, 100.f, "%.0f",
				[]()
				{ return vars.visuals.misc_chams[arms].material == 2 && vars.visuals.misc_chams[arms].enable; }));

			{
				cfg_child->add_element(new c_combo("overlay", &vars.visuals.misc_chams[arms].overlay, {
					"off",
					"glow outline",
					"glow fade",
					}, []() { return vars.visuals.misc_chams[arms].enable; }));

				cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[arms].glow_clr,
					color_t(255, 255, 255, 255), []() { return vars.visuals.misc_chams[arms].enable && vars.visuals.misc_chams[arms].overlay > 0; }));
				cfg_child->add_element(new c_text("overlay color", []() { return vars.visuals.misc_chams[arms].enable && vars.visuals.misc_chams[arms].overlay > 0; }));
			}

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.misc_chams[arms].chams_brightness, 0.f, 300.f, "%.0f%%",
				[]()
				{ return vars.visuals.misc_chams[arms].enable; }));

		}
		break;
		case 5: // weapon
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[weapon].clr,
				color_t(255, 255, 255, 255), []() { return vars.visuals.misc_chams[weapon].enable; }));
			cfg_child->add_element(new c_checkbox("on weapon", &vars.visuals.misc_chams[weapon].enable));

			cfg_child->add_element(new c_combo("material", &vars.visuals.misc_chams[weapon].material, {
					"normal",
					"flat",
					"metallic",
				}, []() { return vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[weapon].metallic_clr,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_text("metallic", []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[weapon].metallic_clr2,
				color_t(255, 255, 255, 255), []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_text("phong", []()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[weapon].phong_exponent, 0.f, 100.f, "exponent: %.0f",
				[]()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_slider("", &vars.visuals.misc_chams[weapon].phong_boost, 0.f, 100.f, "boost: %.0f%%",
				[]()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_slider("rim light", &vars.visuals.misc_chams[weapon].rim, -100.f, 100.f, "%.0f",
				[]()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			cfg_child->add_element(new c_slider("pearlescent", &vars.visuals.misc_chams[weapon].pearlescent, -100.f, 100.f, "%.0f",
				[]()
				{ return vars.visuals.misc_chams[weapon].material == 2 && vars.visuals.misc_chams[weapon].enable; }));

			{
				cfg_child->add_element(new c_combo("overlay", &vars.visuals.misc_chams[weapon].overlay, {
					"off",
					"glow outline",
					"glow fade",
					}, []() { return vars.visuals.misc_chams[weapon].enable; }));

				cfg_child->add_element(new c_colorpicker(&vars.visuals.misc_chams[weapon].glow_clr,
					color_t(255, 255, 255, 255), []() { return vars.visuals.misc_chams[weapon].enable && vars.visuals.misc_chams[weapon].overlay > 0; }));
				cfg_child->add_element(new c_text("overlay color", []() { return vars.visuals.misc_chams[weapon].enable && vars.visuals.misc_chams[weapon].overlay > 0; }));
			}

			cfg_child->add_element(new c_slider("brightness", &vars.visuals.misc_chams[weapon].chams_brightness, 0.f, 300.f, "%.0f%%",
				[]()
				{ return vars.visuals.misc_chams[weapon].enable; }));

		}
		break;
		case 6: // glow
		{
			cfg_child->add_element(new c_colorpicker(&vars.visuals.glow_color,
				color_t(255, 0, 255, 150), []()
				{ return vars.visuals.glow; }));

			cfg_child->add_element(new c_checkbox("on entity", &vars.visuals.glow));

			cfg_child->add_element(new c_colorpicker(&vars.visuals.local_glow_clr,
				color_t(10, 255, 100, 150), []()
				{ return vars.visuals.local_glow; }));

			cfg_child->add_element(new c_checkbox("on local", &vars.visuals.local_glow));

			cfg_child->add_element(new c_combo("glow style", &vars.visuals.glowtype, {
				"normal",
				"pulsating",
				}, []() { return vars.visuals.glow || vars.visuals.local_glow; }));
		}
		break;

		}
	}
	cfg_child->initialize_elements();
	window->add_element(cfg_child);
}

c_menu* g_Menu = new c_menu();