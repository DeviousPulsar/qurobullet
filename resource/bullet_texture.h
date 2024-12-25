#ifndef BULLETTEXTURE_H
#define BULLETTEXTURE_H

#include "core/io/resource.h"
#include "scene/resources/material.h"
#include "scene/resources/2d/shape_2d.h"
#include "scene/resources/texture.h"

class BulletTexture : public Resource {
	GDCLASS(BulletTexture, Resource);

private:
	Ref<Texture2D> texture;
	Color modulate;
	int light_mask;
	Ref<Material> material;
	Ref<Shape2D> collision_shape;
	int collision_mask;
	bool collision_detect_bodies;
	bool collision_detect_areas;
	bool face_direction;
	float rotation;
	Vector2 scale;

protected:
	static void _bind_methods();

public:
	void set_texture(const Ref<Texture2D> &p_texture);
	Ref<Texture2D> get_texture() const;

	void set_modulate(const Color &p_color);
	Color get_modulate() const;

	void set_light_mask(int p_mask);
	int get_light_mask() const;

	void set_material(const Ref<Material> &p_material);
	Ref<Material> get_material() const;

	void set_collision_shape(const Ref<Shape2D> &p_shape);
	Ref<Shape2D> get_collision_shape() const;

	void set_collision_mask(int p_mask);
	int get_collision_mask() const;

	void set_collision_detect_bodies(bool p_enabled);
	bool get_collision_detect_bodies() const;

	void set_collision_detect_areas(bool p_enabled);
	bool get_collision_detect_areas() const;

	void set_face_direction(bool p_enabled);
	bool get_face_direction() const;

	void set_rotation(float p_radians);
	float get_rotation() const;

	void set_rotation_degrees(float p_degrees);
	float get_rotation_degrees() const;

	void set_scale(Vector2 p_scale);
	Vector2 get_scale() const;

	BulletTexture();
	~BulletTexture();
};

#endif
