#ifndef BULLET_H
#define BULLET_H

#include "resource/bullet_path.h"
#include "resource/bullet_texture.h"
#include "core/math/transform_2d.h"
#include "core/object/object.h"
#include "scene/resources/world_2d.h"

class Bullet : public Object {
public:
	enum State {
		LIVE,
		UNINITIALIZED,
		POPPED_OUT_OF_BOUNDS,
		POPPED_LIFETIME_SERVER,
		POPPED_LIFETIME_BULLET,
		POPPED_COLLIDE,
		POPPED_REQUESTED
	};

private:
	GDCLASS(Bullet, Object);

	float damage;

	float time;
	Bullet::State state;

	Vector2 position;

	Ref<BulletPath> path;
	Ref<BulletTexture> texture;

	RID ci_rid;

	Dictionary custom_data;

	void _update_appearance(const Ref<BulletTexture> &p_texture = NULL);

protected:
	static void _bind_methods();

public:
	void spawn(const Ref<BulletPath> &p_path, const Vector2 &p_init_pos, const Vector2 &p_init_dir, 
			const Ref<BulletTexture> &p_texture, const Dictionary &p_custom_data);

	void update(float delta);

	void request_pop();
	void pop(Bullet::State p_pop_reason);
	bool is_popped();
	Bullet::State get_state();

	bool can_collide();

	void set_time(float p_time);
	float get_time() const;

	void set_damage(float p_amount);
	float get_damage() const;

	void set_texture(const Ref<BulletTexture> &p_texture);
	Ref<BulletTexture> get_texture() const;

	void set_path(const Ref<BulletPath> &p_path);
	Ref<BulletPath> get_path() const;

	void set_position(const Vector2 &p_position);
	Vector2 get_position() const;

	Vector2 get_direction() const;
	Vector2 get_velocity() const;
	float get_rotation() const;
	float get_speed() const;

	Transform2D get_transform() const;

	void set_ci_rid(const RID &p_rid);
	RID get_ci_rid() const;

	void set_custom_data(const Dictionary &p_custom_data);
	Dictionary get_custom_data() const;

	Bullet();
	~Bullet();
};

VARIANT_ENUM_CAST(Bullet::State);
#endif