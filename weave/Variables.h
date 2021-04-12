#pragma once
#include "DLL_MAIN.h"
#include "json/json.h"


using namespace std;

enum removals
{
	vis_recoil = 0,
	vis_smoke,
	flash,
	scope,
	zoom,
	post_processing,
	fog,
	shadow,
};

enum visual_indicators
{
	fake = 0, // 1
	lag_comp, // 2
	fake_duck, // 4
	override_dmg,// 8
	force_baim, // 16
	double_tap, // 32
	manual_antiaim // 64
};

enum chams_type
{
	arms,
	weapon,
	//sleeve,
	history,
	desync
};

enum weap_type {
	def,
	scar,
	scout,
	_awp,
	rifles,
	pistols,
	heavy_pistols
};
struct CWeaponConfig {
	bool enable;
	int mindamage;
	int mindamage_override;
	int hitchance;

	int hitchancetype;
	bool multipoint;
	bool quickstop;
	int quickstop_options;
	int pointscale_head;
	int pointscale_body;
	int pointscale_limbs;

	unsigned int hitscan;
	bool adaptive_baim;
	unsigned int hitscan_baim;

	unsigned int baim;
	int baim_under_hp;

	int max_misses;
};

struct c_bind;
struct CGlobalVariables
{
	struct {
		float fov;
		bool rcs_fov_enabled;
		bool rcs;
		float rcs_fov;
		float rcs_x;
		float rcs_y;
		float rcs_start;
		bool rcs_smooth_enabled;
		float rcs_smooth;
		float smooth;
		int priority;
		int weapon;
		bool legitenable;
		c_bind* legitkey = new c_bind();
		int smooth_type;
		int rcs_type;
		bool check_smoke;
		bool check_jump;
		bool check_flash;
		float kill_delay;
		float silent_fov;
		int aim_type;
		int hitbox;
		bool deathmatch;
		int fov_type;
		bool silent;
		bool humanize;
		float curviness;
		float shot_delay;
	} legitbot;

	struct {
		bool enableaa;
		int deslegit = 0;
	} legitaa;
	struct
	{
		bool enable;
		bool autoscope;
		bool disable_dt_delay;
		bool autoshoot;
		bool delayshot;
		bool hitchance_consider_hitbox;
		bool posadj;
		bool backshoot_bt;
		bool resolver;
		bool onshot_invert;
		bool onshot_resolver;
		int base_angle;
		bool shotrecord;
		bool dt_instant;

		color_t shot_clr;
		c_bind* override_dmg = new c_bind();
		c_bind* force_body = new c_bind();
		c_bind* double_tap = new c_bind();
		c_bind* hide_shots = new c_bind();

		// scar, scout, awp, rifles, pistols, heavy pistols
		int dt_speed_boost;
		int dt_delay;
		CWeaponConfig weapon[7];
		int dt_charge_delay;

		bool extend_backtrack;
		int active_index;
	} ragebot;
	struct
	{
		bool enable;
		int desync;
		bool jitter;
		bool lagsync;
		bool disable_fakelag_when_stand;
		c_bind* inverter = new c_bind();

		bool attarget;
		int pitch;

		int fakelag;
		int fakelagfactor;
		int fakelagvariance;
		int min, max;
		int val_a, val_b;
		int jitter_low_delta;

		c_bind* fakeduck = new c_bind();
		c_bind* slowwalk = new c_bind();
		int slowwalk_speed;

		struct
		{
			bool enable;
			c_bind* left = new c_bind();
			c_bind* right = new c_bind();
			c_bind* back = new c_bind();
			c_bind* inverter = new c_bind();
		} aa_override;
		bool fakelag_onshot;
	} antiaim;
	struct
	{
		bool enable = true;
		bool teammates;
		bool dormant;
		bool box = true;
		color_t box_color;
		bool healthbar;
		bool name;
		color_t name_color;
		unsigned int flags;
		color_t flags_color;
		bool weapon;
		color_t weapon_color;

		bool fog;

		bool skeleton;
		color_t skeleton_color;

		bool ammo;
		color_t ammo_color;

		int hitmarker;
		color_t hitmarker_color;
		bool hitmarker_sound;

		unsigned int remove;
		int scope_thickness;

		bool glow;
		color_t glow_color;
		int glowtype;

		bool DURKA;

		bool local_glow;
		color_t local_glow_clr;


		unsigned int indicators;

		bool innacuracyoverlay;
		int innacuracyoverlaytype;
		color_t innacuracyoverlay_color;
		int innacuracyoverlay_rotatingspeed;
		int innacuracyoverlay_alpha;
		bool bullet_tracer;
		color_t bullet_tracer_color;
		bool bullet_impact;
		bool bullet_tracer_local;
		color_t bullet_tracer_local_color;
		color_t bullet_impact_color;
		color_t client_impact_color;
		float impacts_size;
		int bullet_tracer_type;
		int aspect_ratio;

		bool shot_multipoint;
		//bool removesmoke, removeflash, removescope, removezoom;
		bool snipercrosshair;
		bool clantagspammer;
		bool watermark;
		int transparentprops;
		//bool removevisualrecoil;
		bool chams;
		bool chamsteam;
		bool chamsxqz;
		int overlay;
		int overlay_xqz;
		color_t chamscolor;
		color_t chamscolor_xqz;
		color_t chamscolor_glow;
		color_t chamscolor_glow_xqz;
		//bool backtrackchams;
		int chamstype;
		bool aaindicator, aaindicator2;
		bool autowallcrosshair;
		bool lbyindicator;
		bool showgrenades;
		bool showdroppedweapons;
		bool showbomb;

		bool out_of_fov;
		int out_of_fov_distance;
		int out_of_fov_size;
		color_t out_of_fov_color;

		color_t arrow22_clr;
		int chams_brightness;
		int local_chams_brightness;
		bool interpolated_model;
		bool interpolated_dsy;
		bool interpolated_bt;
		struct MiscChams_t
		{
			bool enable;
			int material;
			color_t clr;
			color_t glow_clr;

			int chams_brightness;

			// metallic
			int phong_exponent;
			int phong_boost;
			int rim;
			color_t metallic_clr;
			color_t metallic_clr2;

			// meme
			int pearlescent;
			int overlay;
		};
		int active_chams_index;
		MiscChams_t misc_chams[4];

		// local player esp
		bool localchams;
		bool localpulsatingeffect;
		int localchamstype;
		color_t local_glow_color;
		color_t localchams_color;
		color_t localchams_color_glow;
		bool fullbright;
		bool nightmode;
		//bool removepostprocessing;
		int nightmode_val = 100;
		color_t nightmode_color;
		color_t nightmode_prop_color;
		color_t fog_color;
		color_t nightmode_skybox_color;
		int thirdperson_dist = 120;

		bool chamsragdoll;
		int phong_exponent;
		int phong_boost;
		int rim;
		int pearlescent;
		color_t metallic_clr;
		color_t metallic_clr2;
		color_t glow_col;
		color_t glow_col_xqz;

		int points_size;

		struct
		{
			int phong_exponent;
			int phong_boost;
			int rim;
			color_t metallic_clr;
			color_t metallic_clr2;
			int pearlescent;
			int overlay;
		}local_chams;

		bool override_hp;
		color_t hp_color;

		int prop, wall;

		bool show_points;
		color_t points_color;
		bool nadepred;
		color_t nadepred_color = color_t(255, 255, 255, 255);
		bool eventlog;
		color_t eventlog_color;
		bool speclist;
		bool force_crosshair;

		bool disable_thirdperson_with_granade;
		bool kill_effect;

		int anim_id;
		bool blend_on_scope;
		int blend_value;
	} visuals;

	struct
	{
		bool bunnyhop;
		c_bind* airstuckkey = new c_bind();
		c_bind* thirdperson_bind = new c_bind();
		bool knifebot;
		bool antiuntrusted = true;
		bool hold_firinganims;
		bool removelocalplayermomentum = false;
		int viewmodelfov = 68;
		int viewmodel_x, viewmodel_y, viewmodel_z;
		int worldfov = 90;

		struct {
			bool enable;
			int main;
			int pistol;
			unsigned int misc;
		}autobuy;
	} misc;


	struct
	{
		std::string active_config_name;
		int active_config_index;
		color_t color;
		bool open;
		int y;
		bool guiinited;
		int current_tab;
	} menu;


};

extern CGlobalVariables vars;

typedef Json::Value json_t;

class CConfig
{
private:

	string GetModuleFilePath(HMODULE hModule);
	string GetModuleBaseDir(HMODULE hModule);

public:
	vector<std::string> ConfigList;
	void ReadConfigs(LPCTSTR lpszFileName);
	void ResetToDefault();
	bool init = false;
	CConfig()
	{
		Setup();
	}
	void SaveColor(color_t color, const char* name, json_t* json) {
		auto& j = *json;
		j[name]["red"] = color.get_red();
		j[name]["green"] = color.get_green();
		j[name]["blue"] = color.get_blue();
		j[name]["alpha"] = color.get_alpha();
	}
	void SaveBind(c_bind* bind, const char* name, json_t* json) {
		auto& j = *json;
		j[name]["key"] = bind->key;
		j[name]["type"] = bind->type;
		j[name]["active"] = bind->active;
	}
	void LoadBool(bool* pointer, const char* name, json_t json) {
		if (json.isMember(name)) *pointer = json[name].asBool();
	}
	void LoadInt(int* pointer, const char* name, json_t json) {
		if (json.isMember(name)) *pointer = json[name].asInt();
	}
	void LoadUInt(unsigned int* pointer, const char* name, json_t json) {
		if (json.isMember(name)) *pointer = json[name].asUInt();
	}
	void LoadFloat(float* pointer, const char* name, json_t json) {
		if (json.isMember(name)) *pointer = json[name].asFloat();
	}
	void LoadColor(color_t* pointer, const char* name, json_t json) {
		if (!json.isMember(name))
			return;
		auto location = json[name];
		if (location.isMember("red") && location.isMember("green")
			&& location.isMember("blue") &&location.isMember("alpha")) {
			pointer->set_red(location["red"].asInt());
			pointer->set_green(location["green"].asInt());
			pointer->set_blue(location["blue"].asInt());
			pointer->set_alpha(location["alpha"].asInt());
		}
	}
	void LoadBind(c_bind* pointer, const char* name, json_t json) {
		if (!json.isMember(name))
			return;
		auto location = json[name];
		if (location.isMember("key") && location.isMember("type")) {
			pointer->key = location["key"].asUInt();
			pointer->type = location["type"].asUInt();
			pointer->active = location["active"].asBool();
		}
	}
	void Setup();

	void Save(string cfg_name);
	void Load(string cfg_name);
};

extern CConfig Config;