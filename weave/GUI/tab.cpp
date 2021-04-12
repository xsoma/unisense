#include "tab.h"
#include "child.h"
void c_tab::draw(Vector2D pos, Vector2D size) {
	c_child* c = (c_child*)child;
	auto alpha = (int)(c->get_transparency() * 2.55f);
	bool hovered = g_mouse.x > pos.x && g_mouse.y > pos.y
		&& g_mouse.x < pos.x + size.x && g_mouse.y < pos.y + size.y;
	if (hovered && !c->g_active_element && !c->is_holding_menu()) {
		c->g_hovered_element = this;
		if (c->is_click()) {
			c->set_active_tab_index(this->index);
			c->reset_mouse();
		}

		//if (animation < 1.f) animation += animation_speed;
	}
	else {
		//if (animation > 0.f) animation -= animation_speed;
	}

	animation = clamp(animation, 0.f, 1.f);

	auto clr = color_t(40 + animation * 10.f, 40 + animation * 10.f, 40 + animation * 10.f, alpha);
	auto clr2 = color_t(40 - animation * 10.f, 40 - animation * 10.f, 40 - animation * 10.f, alpha);
	g_Render->FilledRect(pos.x, pos.y + 1, size.x, (size.y - 2) * animation, clr);
	auto t_clr = c->get_active_tab_index() == this->index ?
		color_t(84 + 55.f * animation, 25.f - 25.f * animation, 200 + 50.f * animation, alpha) :
		color_t(200 + 55.f * animation, 200 + 55.f * animation, 200 + 55.f * animation, alpha);



	// color_t(84 + 55.f * animation, 25.f - 25.f * animation, 200 + 50.f * animation, alpha)
	g_Render->DrawString(pos.x + size.x / 2, pos.y + size.y / 2, t_clr,
		render::centered_x | render::centered_y, fonts::menu_main, get_title().c_str());
}
void c_tab::special_render() {

}
void c_tab::render() {

}