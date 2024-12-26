#include "bullet_server.h"

#include "scene/resources/world_2d.h"
#include "scene/main/viewport.h"

void BulletServer::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			if (Engine::get_singleton()->is_editor_hint()) {
				return;
			}
			if (relay_autoconnect) {
				BulletServerRelay *relay = Object::cast_to<BulletServerRelay>(Engine::get_singleton()->get_singleton_object("BulletServerRelay"));
				relay->connect("bullet_spawn_requested", Callable(this, "spawn_bullet"));
				relay->connect("volley_spawn_requested", Callable(this, "spawn_volley"));
			}
			set_process(true);
			set_physics_process(true);
			_update_play_area();
			_init_bullets();
		} break;

		case NOTIFICATION_PROCESS: {
			//update bullet canvasitems
			for (int i = 0; i < live_bullets.size(); i++) {
				Bullet *bullet = live_bullets[i];
				RS::get_singleton()->canvas_item_set_transform(bullet->get_ci_rid(), bullet->get_transform());
			}
		} break;

		case NOTIFICATION_PHYSICS_PROCESS: {
			if (Engine::get_singleton()->is_editor_hint()) {
				return;
			}
			_update_play_area();
			_process_bullets(get_physics_process_delta_time());
		} break;

		case NOTIFICATION_EXIT_TREE: {
			_uninit_bullets();
		} break;

		default:
			break;
	}
}

void BulletServer::_process_bullets(float delta) {
	ERR_FAIL_COND(!is_inside_tree());
	Vector<int> bullet_indices_to_clear;
	PhysicsDirectSpaceState2D* space_state = get_viewport()->find_world_2d()->get_direct_space_state();
	Dictionary collision_info = Dictionary();
	Array popped_bullets = Array();

	for (int i = 0; i < live_bullets.size(); i++) {
		Bullet* bullet = live_bullets[i];
		RS::get_singleton()->canvas_item_set_draw_index(bullet->get_ci_rid(), i);

		if (bullet->is_popped()) {
			bullet_indices_to_clear.push_back(i);
		} else if (max_lifetime >= 0.001 && bullet->get_time() > max_lifetime) {
			bullet->pop(Bullet::POPPED_LIFETIME_SERVER);
			bullet_indices_to_clear.push_back(i);
		} else if (play_area_mode == INFINITE || play_area_rect.has_point(bullet->get_position())) {
			bullet->update(delta);
			_handle_collisions(bullet, space_state, collision_info);
		} else if (_bullet_trajectory_valid(bullet->get_position(), bullet->get_direction())) {
			bullet->update(delta);
		} else {
			bullet->pop(Bullet::POPPED_OUT_OF_BOUNDS);
			bullet_indices_to_clear.push_back(i);
		}
	}

	for (int i = 0; i < bullet_indices_to_clear.size(); i++) {
		Bullet* bullet = live_bullets[bullet_indices_to_clear[i] - i];

		Dictionary dict = Dictionary();
		dict["damage"] = bullet->get_damage();
		dict["time"] = bullet->get_time();
		dict["reason_popped"] = bullet->get_state();
		dict["position"] = bullet->get_position();
		dict["path"] = bullet->get_path();
		dict["texture"] = bullet->get_texture();
		dict["custom_data"] = bullet->get_custom_data();
		popped_bullets.append(dict);

		live_bullets.remove_at(bullet_indices_to_clear[i] - i);
		dead_bullets.insert(0, bullet);
	}

	if (!collision_info.is_empty()) {
		emit_signal("collisions_detected", collision_info);
	}

	if (!popped_bullets.is_empty()) {
		emit_signal("bullets_popped", popped_bullets);
	}
}

void BulletServer::_handle_collisions(Bullet* bullet, PhysicsDirectSpaceState2D* space_state, Dictionary out) {
	if (!bullet->can_collide()) {
		return;
	}
	Vector<PhysicsDirectSpaceState2D::ShapeResult> results = Vector<PhysicsDirectSpaceState2D::ShapeResult>();
	results.resize(max_collisions_per_bullet);

	Ref<BulletTexture> b_tex = bullet->get_texture();
	PhysicsDirectSpaceState2D::ShapeParameters shape_params = PhysicsDirectSpaceState2D::ShapeParameters();

	shape_params.shape_rid = b_tex->get_collision_shape()->get_rid();
	shape_params.transform = bullet->get_transform();
	shape_params.motion = Vector2(0, 0);
	shape_params.margin = 0.0;
	shape_params.exclude = HashSet<RID>();
	shape_params.collision_mask = b_tex->get_collision_mask();
	shape_params.collide_with_bodies = b_tex->get_collision_detect_bodies();
	shape_params.collide_with_areas = b_tex->get_collision_detect_areas();


	int collisions = space_state->intersect_shape(shape_params, results.ptrw(), results.size());
	if (collisions > 0) {
		Array list = Array();
		for (int i = 0; i < collisions; i++) {
			Dictionary dict = Dictionary();
			dict["rid"] = results[i].rid;
			dict["collider_id"] = results[i].collider_id;
			dict["collider"] = results[i].collider;
			dict["shape"] = results[i].shape;
			list.append(dict);
		}
		out[bullet] = list;

		if (pop_on_collide) {
			bullet->pop(Bullet::POPPED_COLLIDE);
		}
	}
}

void BulletServer::_init_bullets() {
	for (int i = 0; i < bullet_pool_size; i++) {
		_create_bullet();
	}
}

void BulletServer::_create_bullet() {
	ERR_FAIL_COND(!is_inside_tree());
	Bullet *bullet = memnew(Bullet);
	RS::get_singleton()->canvas_item_set_parent(bullet->get_ci_rid(), get_viewport()->find_world_2d()->get_canvas());
	dead_bullets.insert(0, bullet);
}

void BulletServer::_uninit_bullets() {
	for (int i = 0; i < live_bullets.size(); i++) {
		memdelete(live_bullets[i]);
	}
	for (int i = 0; i < dead_bullets.size(); i++) {
		memdelete(dead_bullets[i]);
	}
}

void BulletServer::_update_play_area() {
	if (play_area_mode != VIEWPORT) {
		return;
	}
	
	ERR_FAIL_COND(!is_inside_tree());
	play_area_rect = get_viewport()->get_visible_rect().grow(play_area_margin);
}

int _compute_out_code(const Rect2 &p_rect, const Vector2 &p_pos) {
	int out_code = 0;

	if (p_pos.y < p_rect.position.y) {
		out_code |= 1;
	} else if (p_pos.y > p_rect.position.y + p_rect.size.y) {
		out_code |= 2;
	}

	if (p_pos.x < p_rect.position.x) {
		out_code |= 4;
	} else if (p_pos.x > p_rect.position.x + p_rect.size.x) {
		out_code |= 8;
	}

	return out_code;
}

bool BulletServer::_bullet_trajectory_valid(const Vector2 &p_pos, const Vector2 &p_dir) const {
	if (play_area_mode == INFINITE) { return true; }

	int clip_pos = 	_compute_out_code(play_area_rect, p_pos);

	if (clip_pos == 0) { return true; }
	if (!play_area_allow_incoming) { return false; }
	
	int clip_dest = _compute_out_code(play_area_rect, p_pos + play_area_max_incoming_dist*p_dir);

	if ((clip_pos & clip_dest) != 0) { return false; }

	return true;
}

void BulletServer::spawn_bullet(const Ref<BulletPath> &p_path, const Vector2 &p_position, const Vector2 &p_direction, const Ref<BulletTexture> &p_texture, const Dictionary &p_custom_data) {
	if (!_bullet_trajectory_valid(p_position, p_direction)) {
		return;
	}

	Bullet *bullet;

	if (dead_bullets.size() > 0) {
		bullet = dead_bullets.get(dead_bullets.size() - 1);
		dead_bullets.remove_at(dead_bullets.size() - 1);
	} else {
		bullet = live_bullets.get(live_bullets.size() - 1);
		live_bullets.remove_at(live_bullets.size() - 1);
	}

	bullet->spawn(p_path, p_position, p_direction, p_texture, p_custom_data);
	RS::get_singleton()->canvas_item_set_draw_index(bullet->get_ci_rid(), 0);
	live_bullets.insert(0, bullet);
}

void BulletServer::spawn_volley(const Ref<BulletPath> &p_path, const Vector2 &p_origin, const Array &p_volley, const Ref<BulletTexture> &p_texture, const Dictionary &p_custom_data) {
	for (int i = 0; i < p_volley.size(); i++) {
		Dictionary shot = p_volley[i];
		spawn_bullet(p_path, p_origin + shot["position"], shot["direction"], p_texture, p_custom_data);
	}
}

void BulletServer::clear_bullets() {
	for (int i = 0; i < live_bullets.size(); i++) {
		live_bullets[i]->pop(Bullet::POPPED_REQUESTED);
	}
}

Array BulletServer::get_live_bullets() {
	Array bullets;
	for (int i = 0; i < live_bullets.size(); i++) {
		bullets.push_back(live_bullets[i]);
	}
	return bullets;
}

int BulletServer::get_live_bullet_count() {
	return live_bullets.size();
}

Array BulletServer::get_live_bullet_positions() {
	Array bullet_positions;
	for (int i = 0; i < live_bullets.size(); i++) {
		bullet_positions.push_back(live_bullets[i]->get_position());
	}
	return bullet_positions;
}

void BulletServer::set_bullet_pool_size(int p_size) {
	if (p_size > -1)
		bullet_pool_size = p_size;
	if (!is_inside_tree() || Engine::get_singleton()->is_editor_hint())
		return;
	while (int(dead_bullets.size() + live_bullets.size()) < bullet_pool_size)
		_create_bullet();
	while (int(dead_bullets.size() + live_bullets.size()) > bullet_pool_size) {
		Bullet *bullet;
		if (dead_bullets.size() > 0) {
			bullet = dead_bullets.get(dead_bullets.size() - 1);
			dead_bullets.remove_at(dead_bullets.size() - 1);
		} else {
			bullet = live_bullets.get(live_bullets.size() - 1);
			live_bullets.remove_at(live_bullets.size() - 1);
		}
		RS::get_singleton()->free(bullet->get_ci_rid());
		memdelete(bullet);
	}
}

int BulletServer::get_bullet_pool_size() const {
	return bullet_pool_size;
}

void BulletServer::set_max_lifetime(float p_time) {
	max_lifetime = p_time;
}

float BulletServer::get_max_lifetime() const {
	return max_lifetime;
}

void BulletServer::set_pop_on_collide(bool p_enabled) {
	pop_on_collide = p_enabled;
}

bool BulletServer::get_pop_on_collide() const {
	return pop_on_collide;
}

void BulletServer::set_max_collisions_per_bullet(int p_count) {
	max_collisions_per_bullet = p_count;
}

int BulletServer::get_max_collisions_per_bullet() const {
	return max_collisions_per_bullet;
}

void BulletServer::set_play_area_mode(AreaMode p_mode) {
	play_area_mode = p_mode;
	notify_property_list_changed();
}

BulletServer::AreaMode BulletServer::get_play_area_mode() const {
	return play_area_mode;
}

void BulletServer::set_play_area_rect(const Rect2 &p_rect) {
	play_area_rect = p_rect;
}

Rect2 BulletServer::get_play_area_rect() const {
	return play_area_rect;
}

void BulletServer::set_play_area_margin(float p_margin) {
	play_area_margin = p_margin;
}

float BulletServer::get_play_area_margin() const {
	return play_area_margin;
}

void BulletServer::set_play_area_allow_incoming(bool p_enabled) {
	play_area_allow_incoming = p_enabled;
}

bool BulletServer::get_play_area_allow_incoming() const {
	return play_area_allow_incoming;
}

void BulletServer::set_play_area_max_incoming_dist(float p_dist) {
	play_area_max_incoming_dist = p_dist;
}

float BulletServer::get_play_area_max_incoming_dist() const {
	return play_area_max_incoming_dist;
}

void BulletServer::set_relay_autoconnect(bool p_enabled) {
	relay_autoconnect = p_enabled;
}

bool BulletServer::get_relay_autoconnect() const {
	return relay_autoconnect;
}

void BulletServer::_validate_property(PropertyInfo &property) const {
	if (property.name == "play_area_rect" && play_area_mode != MANUAL) {
		property.usage = PROPERTY_USAGE_STORAGE;
	}

	if (property.name == "play_area_margin" && play_area_mode != VIEWPORT) {
		property.usage = PROPERTY_USAGE_STORAGE;
	}
}

void BulletServer::_bind_methods() {
	ClassDB::bind_method(D_METHOD("spawn_bullet", "path", "position", "direction", "texture", "custom_data"), &BulletServer::spawn_bullet);
	ClassDB::bind_method(D_METHOD("spawn_volley", "path", "position", "volley",  "texture", "custom_data"), &BulletServer::spawn_volley);
	ClassDB::bind_method(D_METHOD("clear_bullets"), &BulletServer::clear_bullets);

	ClassDB::bind_method(D_METHOD("get_live_bullets"), &BulletServer::get_live_bullets);
	ClassDB::bind_method(D_METHOD("get_live_bullet_count"), &BulletServer::get_live_bullet_count);
	ClassDB::bind_method(D_METHOD("get_live_bullet_positions"), &BulletServer::get_live_bullet_positions);

	ClassDB::bind_method(D_METHOD("set_bullet_pool_size", "size"), &BulletServer::set_bullet_pool_size);
	ClassDB::bind_method(D_METHOD("get_bullet_pool_size"), &BulletServer::get_bullet_pool_size);

	ClassDB::bind_method(D_METHOD("set_pop_on_collide", "enabled"), &BulletServer::set_pop_on_collide);
	ClassDB::bind_method(D_METHOD("get_pop_on_collide"), &BulletServer::get_pop_on_collide);

	ClassDB::bind_method(D_METHOD("set_max_lifetime", "time"), &BulletServer::set_max_lifetime);
	ClassDB::bind_method(D_METHOD("get_max_lifetime"), &BulletServer::get_max_lifetime);

	ClassDB::bind_method(D_METHOD("set_max_collisions_per_bullet", "count"), &BulletServer::set_max_collisions_per_bullet);
	ClassDB::bind_method(D_METHOD("get_max_collisions_per_bullet"), &BulletServer::get_max_collisions_per_bullet);

	ClassDB::bind_method(D_METHOD("set_play_area_mode", "mode"), &BulletServer::set_play_area_mode);
	ClassDB::bind_method(D_METHOD("get_play_area_mode"), &BulletServer::get_play_area_mode);

	ClassDB::bind_method(D_METHOD("set_play_area_rect", "rect"), &BulletServer::set_play_area_rect);
	ClassDB::bind_method(D_METHOD("get_play_area_rect"), &BulletServer::get_play_area_rect);

	ClassDB::bind_method(D_METHOD("set_play_area_margin", "margin"), &BulletServer::set_play_area_margin);
	ClassDB::bind_method(D_METHOD("get_play_area_margin"), &BulletServer::get_play_area_margin);

	ClassDB::bind_method(D_METHOD("set_play_area_allow_incoming", "allow_incoming"), &BulletServer::set_play_area_allow_incoming);
	ClassDB::bind_method(D_METHOD("get_play_area_allow_incoming"), &BulletServer::get_play_area_allow_incoming);

	ClassDB::bind_method(D_METHOD("set_play_area_max_incoming_dist", "max_incoming_dist"), &BulletServer::set_play_area_max_incoming_dist);
	ClassDB::bind_method(D_METHOD("get_play_area_max_incoming_dist"), &BulletServer::get_play_area_max_incoming_dist);

	ClassDB::bind_method(D_METHOD("set_relay_autoconnect", "relay_autoconnect"), &BulletServer::set_relay_autoconnect);
	ClassDB::bind_method(D_METHOD("get_relay_autoconnect"), &BulletServer::get_relay_autoconnect);

	ADD_PROPERTY(PropertyInfo(Variant::INT, "bullet_pool_size", PROPERTY_HINT_RANGE, "1,5000,1,or_greater"), "set_bullet_pool_size", "get_bullet_pool_size");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_lifetime", PROPERTY_HINT_RANGE, "0,300,0.01,or_greater"), "set_max_lifetime", "get_max_lifetime");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "pop_on_collide"), "set_pop_on_collide", "get_pop_on_collide");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "max_collisions_per_bullet", PROPERTY_HINT_RANGE, "1,256,1,or_greater"), "set_max_collisions_per_bullet", "get_max_collisions_per_bullet");
	
	ADD_GROUP("Play Area", "play_area_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "play_area_mode", PROPERTY_HINT_ENUM, "Viewport,Manual,Infinite"), "set_play_area_mode", "get_play_area_mode");
	ADD_PROPERTY(PropertyInfo(Variant::RECT2, "play_area_rect"), "set_play_area_rect", "get_play_area_rect");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "play_area_margin", PROPERTY_HINT_RANGE, "0,300,0.01,or_less,or_greater"), "set_play_area_margin", "get_play_area_margin");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "play_area_allow_incoming"), "set_play_area_allow_incoming", "get_play_area_allow_incoming");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "play_area_max_incoming_dist", PROPERTY_HINT_RANGE, "0,10000,10,or_greater"), "set_play_area_max_incoming_dist", "get_play_area_max_incoming_dist");

	ADD_GROUP("Relay", "relay_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "relay_autoconnect"), "set_relay_autoconnect", "get_relay_autoconnect");

	ADD_SIGNAL(MethodInfo("collisions_detected", PropertyInfo(Variant::DICTIONARY, "collisions")));
	ADD_SIGNAL(MethodInfo("bullets_popped", PropertyInfo(Variant::ARRAY, "bullets")));
	BIND_ENUM_CONSTANT(VIEWPORT);
	BIND_ENUM_CONSTANT(MANUAL);
	BIND_ENUM_CONSTANT(INFINITE);
}

BulletServer::BulletServer() {
	bullet_pool_size = 1500;
	max_lifetime = 0.0;
	max_collisions_per_bullet = 32;
	play_area_allow_incoming = true;
	play_area_mode = VIEWPORT;
	play_area_margin = 0;
	play_area_rect = Rect2();
	play_area_max_incoming_dist = 2000;
	pop_on_collide = true;
	relay_autoconnect = true;
}

BulletServer::~BulletServer() {
}
