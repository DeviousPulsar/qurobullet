#include "bullet.h"

void Bullet::spawn(const Ref<BulletPath> &p_path, const Vector2 &p_init_pos, const Vector2 &p_init_dir, const Ref<BulletTexture> &p_texture, const Dictionary &p_custom_data) {
	time = 0.0;
	//popped = false;
	pop_reason = 0;

	set_texture(p_texture);
	RS::get_singleton()->canvas_item_set_visible(ci_rid, true);

	position = p_init_pos;
	set_path(p_path->rotated(p_init_dir.angle()));

	custom_data = p_custom_data;
}

void Bullet::update(float delta) {
	position += path->get_step(time, delta);
	time += delta;

	if (!Math::is_zero_approx(path->get_lifetime()) && time > path->get_lifetime()) {
		pop(100);
	}
}

void Bullet::pop(int p_pop_reason) {
	//popped = true;
	pop_reason = p_pop_reason;
	RS::get_singleton()->canvas_item_set_visible(ci_rid, false);
}

bool Bullet::is_popped() {
	return pop_reason != 0;
}

int Bullet::get_pop_reason() {
	return pop_reason;
}

bool Bullet::can_collide() {
	return !is_popped() && texture.is_valid() && !texture->get_collision_shape().is_null() && texture->get_collision_mask() != 0;
}

void Bullet::set_time(float p_time) {
	time = p_time;
}

float Bullet::get_time() const {
	return time;
}

void Bullet::set_texture(const Ref<BulletTexture> &p_texture) {
	texture = p_texture;
	_update_appearance(p_texture);
}

Ref<BulletTexture> Bullet::get_texture() const {
	return texture;
}

void Bullet::set_path(const Ref<BulletPath> &p_path) {
	path = p_path;
	time = 0;
}

Ref<BulletPath> Bullet::get_path() const {
	return path;
}

void Bullet::set_position(const Vector2 &p_position) {
	position = p_position;
}

Vector2 Bullet::get_position() const {
	return position;
}

Vector2 Bullet::get_direction() const {
	return path->get_direction(time);
}

Vector2 Bullet::get_velocity() const {
	return path->get_velocity(time);
}

float Bullet::get_rotation() const {
	return path->get_rotation(time);
}

float Bullet::get_speed() const {
	return path->get_speed(time);
}

Transform2D Bullet::get_transform() const {
	Transform2D t;
	t.set_origin(position);
	if (texture.is_valid() && path.is_valid()) {
		t.set_rotation_and_scale(get_rotation() + texture->get_rotation(), texture->get_scale());
	} else if (path.is_valid()) {
		t.set_rotation_and_scale(get_rotation(), Vector2(1, 1));
	}
	return t;
}

void Bullet::set_ci_rid(const RID &p_rid) {
	ci_rid = p_rid;
}

RID Bullet::get_ci_rid() const {
	return ci_rid;
}

void Bullet::_update_appearance(const Ref<BulletTexture> &p_texture) {
	if (p_texture.is_valid()) {
		RenderingServer *rs = RS::get_singleton();
		Ref<Texture2D> old_tex = texture.is_valid() ? texture->get_texture() : NULL;
		Ref<Texture2D> new_tex = p_texture->get_texture();

		//if (new_tex.is_null()) {
		//	rs->canvas_item_clear(ci_rid);
		//} else if (old_tex != new_tex) {
			rs->canvas_item_clear(ci_rid);
			new_tex->draw(ci_rid, -new_tex->get_size()/2);
		//}

		if (p_texture->get_material().is_valid()) {
			rs->canvas_item_set_material(ci_rid, p_texture->get_material()->get_rid());
		}
		
		rs->canvas_item_set_modulate(ci_rid, p_texture->get_modulate());
		rs->canvas_item_set_light_mask(ci_rid, p_texture->get_light_mask());
	}
}

void Bullet::_bind_methods() {
	ClassDB::bind_method(D_METHOD("spawn", "path", "position", "direction", "graphic", "custom_data"), &Bullet::spawn);

	ClassDB::bind_method(D_METHOD("update", "delta"), &Bullet::update);

	ClassDB::bind_method(D_METHOD("pop"), &Bullet::pop);
	ClassDB::bind_method(D_METHOD("is_popped"), &Bullet::is_popped);

	ClassDB::bind_method(D_METHOD("can_collide"), &Bullet::can_collide);

	ClassDB::bind_method(D_METHOD("get_time"), &Bullet::get_time);

	ClassDB::bind_method(D_METHOD("set_texture", "texture"), &Bullet::set_texture);
	ClassDB::bind_method(D_METHOD("get_texture"), &Bullet::get_texture);

	ClassDB::bind_method(D_METHOD("set_path", "path"), &Bullet::set_path);
	ClassDB::bind_method(D_METHOD("get_path"), &Bullet::get_path);

	ClassDB::bind_method(D_METHOD("set_position", "position"), &Bullet::set_position);
	ClassDB::bind_method(D_METHOD("get_position"), &Bullet::get_position);

	ClassDB::bind_method(D_METHOD("get_direction"), &Bullet::get_direction);
	ClassDB::bind_method(D_METHOD("get_velocity"), &Bullet::get_velocity);
	ClassDB::bind_method(D_METHOD("get_rotation"), &Bullet::get_rotation);
	ClassDB::bind_method(D_METHOD("get_speed"), &Bullet::get_speed);

	ClassDB::bind_method(D_METHOD("get_transform"), &Bullet::get_transform);

	ClassDB::bind_method(D_METHOD("get_ci_rid"), &Bullet::get_ci_rid);
}

Bullet::Bullet() {
	ci_rid = RS::get_singleton()->canvas_item_create();
	position = Vector2();
	texture = Ref<BulletTexture>();
	path = Ref<BulletPath>();
	//popped = true;
	pop_reason = -1;
	time = 0;
}

Bullet::~Bullet() {
	RS::get_singleton()->free(ci_rid);
}
