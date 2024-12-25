#include "bullet_texture.h"

void BulletTexture::set_texture(const Ref<Texture2D> &p_texture) {
	texture = p_texture;
}

Ref<Texture2D> BulletTexture::get_texture() const {
	return texture;
}

void BulletTexture::set_modulate(const Color &p_color) {
	modulate = p_color;
}

Color BulletTexture::get_modulate() const {
	return modulate;
}

void BulletTexture::set_light_mask(int p_mask) {
	light_mask = p_mask;
}

int BulletTexture::get_light_mask() const {
	return light_mask;
}

void BulletTexture::set_material(const Ref<Material> &p_material) {
	material = p_material;
}

Ref<Material> BulletTexture::get_material() const {
	return material;
}

void BulletTexture::set_collision_shape(const Ref<Shape2D> &p_shape) {
	collision_shape = p_shape;
}

Ref<Shape2D> BulletTexture::get_collision_shape() const {
	return collision_shape;
}

void BulletTexture::set_collision_mask(int p_mask) {
	collision_mask = p_mask;
}

int BulletTexture::get_collision_mask() const {
	return collision_mask;
}

void BulletTexture::set_collision_detect_bodies(bool p_enabled) {
	collision_detect_bodies = p_enabled;
}

bool BulletTexture::get_collision_detect_bodies() const {
	return collision_detect_bodies;
}

void BulletTexture::set_collision_detect_areas(bool p_enabled) {
	collision_detect_areas = p_enabled;
}

bool BulletTexture::get_collision_detect_areas() const {
	return collision_detect_areas;
}

void BulletTexture::set_face_direction(bool p_enabled) {
	face_direction = p_enabled;
}

bool BulletTexture::get_face_direction() const {
	return face_direction;
}

void BulletTexture::set_rotation(float p_radians) {
	rotation = p_radians;
}

float BulletTexture::get_rotation() const {
	return rotation;
}

void BulletTexture::set_rotation_degrees(float p_degrees) {
	rotation = Math::deg_to_rad(p_degrees);
}

float BulletTexture::get_rotation_degrees() const {
	return Math::rad_to_deg(rotation);
}

void BulletTexture::set_scale(Vector2 p_scale) {
	scale = p_scale;
}

Vector2 BulletTexture::get_scale() const {
	return scale;
}

void BulletTexture::_bind_methods() {
	ClassDB::bind_method(D_METHOD("set_texture", "texture"), &BulletTexture::set_texture);
	ClassDB::bind_method(D_METHOD("get_texture"), &BulletTexture::get_texture);

	ClassDB::bind_method(D_METHOD("set_modulate", "modulate"), &BulletTexture::set_modulate);
	ClassDB::bind_method(D_METHOD("get_modulate"), &BulletTexture::get_modulate);

	ClassDB::bind_method(D_METHOD("set_light_mask", "light_mask"), &BulletTexture::set_light_mask);
	ClassDB::bind_method(D_METHOD("get_light_mask"), &BulletTexture::get_light_mask);

	ClassDB::bind_method(D_METHOD("set_material", "material"), &BulletTexture::set_material);
	ClassDB::bind_method(D_METHOD("get_material"), &BulletTexture::get_material);

	ClassDB::bind_method(D_METHOD("set_collision_shape", "collision_shape"), &BulletTexture::set_collision_shape);
	ClassDB::bind_method(D_METHOD("get_collision_shape"), &BulletTexture::get_collision_shape);

	ClassDB::bind_method(D_METHOD("set_collision_mask", "collision_mask"), &BulletTexture::set_collision_mask);
	ClassDB::bind_method(D_METHOD("get_collision_mask"), &BulletTexture::get_collision_mask);

	ClassDB::bind_method(D_METHOD("set_collision_detect_bodies", "enabled"), &BulletTexture::set_collision_detect_bodies);
	ClassDB::bind_method(D_METHOD("get_collision_detect_bodies"), &BulletTexture::get_collision_detect_bodies);

	ClassDB::bind_method(D_METHOD("set_collision_detect_areas", "enabled"), &BulletTexture::set_collision_detect_areas);
	ClassDB::bind_method(D_METHOD("get_collision_detect_areas"), &BulletTexture::get_collision_detect_areas);

	ClassDB::bind_method(D_METHOD("set_face_direction", "enabled"), &BulletTexture::set_face_direction);
	ClassDB::bind_method(D_METHOD("get_face_direction"), &BulletTexture::get_face_direction);

	ClassDB::bind_method(D_METHOD("set_rotation", "radians"), &BulletTexture::set_rotation);
	ClassDB::bind_method(D_METHOD("get_rotation"), &BulletTexture::get_rotation);

	ClassDB::bind_method(D_METHOD("set_rotation_degrees", "degrees"), &BulletTexture::set_rotation_degrees);
	ClassDB::bind_method(D_METHOD("get_rotation_degrees"), &BulletTexture::get_rotation_degrees);

	ClassDB::bind_method(D_METHOD("set_scale", "scale"), &BulletTexture::set_scale);
	ClassDB::bind_method(D_METHOD("get_scale"), &BulletTexture::get_scale);

	ADD_GROUP("Appearance", "");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "texture", PROPERTY_HINT_RESOURCE_TYPE, "Texture2D"), "set_texture", "get_texture");
	ADD_PROPERTY(PropertyInfo(Variant::COLOR, "modulate"), "set_modulate", "get_modulate");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "light_mask", PROPERTY_HINT_LAYERS_2D_RENDER), "set_light_mask", "get_light_mask");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "material", PROPERTY_HINT_RESOURCE_TYPE, "Material"), "set_material", "get_material");
	ADD_GROUP("Collision", "collision_");
	ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "collision_shape", PROPERTY_HINT_RESOURCE_TYPE, "Shape2D"), "set_collision_shape", "get_collision_shape");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "collision_mask", PROPERTY_HINT_LAYERS_2D_PHYSICS), "set_collision_mask", "get_collision_mask");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collision_detect_bodies"), "set_collision_detect_bodies", "get_collision_detect_bodies");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "collision_detect_areas"), "set_collision_detect_areas", "get_collision_detect_areas");
	ADD_GROUP("Transform", "");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "face_direction"), "set_face_direction", "get_face_direction");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rotation", PROPERTY_HINT_RANGE, "", PROPERTY_USAGE_NO_EDITOR), "set_rotation", "get_rotation");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rotation_degrees", PROPERTY_HINT_RANGE, "-360,360,0.1,or_less,or_greater", PROPERTY_USAGE_EDITOR), "set_rotation_degrees", "get_rotation_degrees");
	ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "scale"), "set_scale", "get_scale");
}

BulletTexture::BulletTexture() {
	texture = Ref<Texture2D>();
	modulate = Color(1, 1, 1, 1);
	light_mask = 1;
	material = Ref<Material>();
	collision_shape = Ref<Shape2D>();
	collision_mask = 1;
	collision_detect_bodies = true;
	collision_detect_areas = true;
	face_direction = false;
	rotation = 0.0;
	scale = Vector2(1, 1);
}

BulletTexture::~BulletTexture() {
	// cleanup goes here
}
