#include "force_bullet_path.h"

void ForceBulletPath::set_lifetime(float p_time) {
    lifetime = p_time;
}
float ForceBulletPath::get_lifetime() const {
    return lifetime;
}

Vector2 ForceBulletPath::get_step(float p_time, float p_delta) const {
    return p_delta*(get_velocity(p_time + p_delta) + get_velocity(p_time))/2;
}

Vector2 ForceBulletPath::get_direction(float p_time) const {
    return get_velocity(p_time).normalized();
}

Vector2 ForceBulletPath::get_velocity(float p_time) const {
    Vector2 velocity = init_velocity + p_time*force;
    return velocity.limit_length(max_speed);
}

float ForceBulletPath::get_rotation(float p_time) const {
    return get_velocity(p_time).angle();
}

float ForceBulletPath::get_speed(float p_time) const {
    return get_velocity(p_time).length();
}

Ref<BulletPath> ForceBulletPath::rotated(float p_rotation) const {
    Ref<ForceBulletPath> new_path = Ref<BulletPath>(memnew(ForceBulletPath(this)));
    
    new_path->init_velocity = init_velocity.rotated(p_rotation);

    if(rotate_forces_on_transform) {
        new_path->force = new_path->force.rotated(p_rotation);
    }

    return new_path;
}

Ref<BulletPath> ForceBulletPath::xformed(const Transform2D &p_transform) const {
    Ref<ForceBulletPath> new_path = Ref<BulletPath>(memnew(ForceBulletPath(this)));
    
    new_path->init_velocity = p_transform.basis_xform(init_velocity);

    if(rotate_forces_on_transform) {
        new_path->force = p_transform.basis_xform(new_path->force);
    }

    return new_path;
}

void ForceBulletPath::set_init_velocity(const Vector2 &p_velocity) {
    init_velocity = p_velocity;
}
Vector2 ForceBulletPath::get_init_velocity() const {
    return init_velocity;
}

void ForceBulletPath::set_init_speed(float p_speed) {
    init_velocity = init_velocity.normalized()*p_speed;
}
float ForceBulletPath::get_init_speed() const {
    return init_velocity.length();
}

void ForceBulletPath::set_init_rotation(float p_dir) {
    init_velocity = Vector2::from_angle(p_dir)*get_init_speed();
}
float ForceBulletPath::get_init_rotation() const {
    return init_velocity.angle();
}

void ForceBulletPath::set_max_speed(float p_max_speed) {
    max_speed = p_max_speed;
}
float ForceBulletPath::get_max_speed() const {
    return max_speed;
}

void ForceBulletPath::set_force(const Vector2 &p_force) {
    force = p_force;
}
Vector2 ForceBulletPath::get_force() const {
    return force;
}

void ForceBulletPath::set_rotate_forces_on_transform(bool p_rotate) {
    rotate_forces_on_transform = p_rotate;
}
bool ForceBulletPath::get_rotate_forces_on_transform() const {
    return rotate_forces_on_transform;
}

Ref<ForceBulletPath> ForceBulletPath::apply_force(const Vector2 &p_force, float p_time) {
    Ref<ForceBulletPath> new_path = Ref<BulletPath>(memnew(ForceBulletPath()));

    new_path->force = force + p_force;
    new_path->init_velocity = get_velocity(p_time);
    new_path->lifetime = (lifetime == 0)? 0 : MIN(lifetime - p_time, 0.001);
    new_path->max_speed = max_speed;

    return new_path;
}

Ref<ForceBulletPath> ForceBulletPath::apply_impulse(const Vector2 &p_impulse, float p_time) {
    Ref<ForceBulletPath> new_path = Ref<BulletPath>(memnew(ForceBulletPath(this)));

    new_path->init_velocity = get_velocity(p_time) + p_impulse;
    new_path->lifetime = (lifetime == 0)? 0 : MIN(lifetime - p_time, 0.001);

    return new_path;
}

void ForceBulletPath::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_lifetime", "time"), &ForceBulletPath::set_lifetime);
	ClassDB::bind_method(D_METHOD("get_lifetime"), &ForceBulletPath::get_lifetime);

    ClassDB::bind_method(D_METHOD("get_step", "time", "delta"), &ForceBulletPath::get_step);

	ClassDB::bind_method(D_METHOD("get_direction", "time"), &ForceBulletPath::get_direction);
    ClassDB::bind_method(D_METHOD("get_velocity", "time"), &ForceBulletPath::get_velocity);
    ClassDB::bind_method(D_METHOD("get_rotation", "time"), &ForceBulletPath::get_rotation);
    ClassDB::bind_method(D_METHOD("get_speed", "time"), &ForceBulletPath::get_speed);

    ClassDB::bind_method(D_METHOD("rotated", "degrees"), &ForceBulletPath::rotated);
    ClassDB::bind_method(D_METHOD("xformed", "transform"), &ForceBulletPath::xformed);

    ClassDB::bind_method(D_METHOD("set_init_velocity", "velocity"), &ForceBulletPath::set_init_velocity);
    ClassDB::bind_method(D_METHOD("get_init_velocity"), &ForceBulletPath::get_init_velocity);

    ClassDB::bind_method(D_METHOD("set_init_speed", "speed"), &ForceBulletPath::set_init_speed);
    ClassDB::bind_method(D_METHOD("get_init_speed"), &ForceBulletPath::get_init_speed);

    ClassDB::bind_method(D_METHOD("set_init_rotation", "direction"), &ForceBulletPath::set_init_rotation);
    ClassDB::bind_method(D_METHOD("get_init_rotation"), &ForceBulletPath::get_init_rotation);

    ClassDB::bind_method(D_METHOD("set_max_speed", "max_speed"), &ForceBulletPath::set_max_speed);
    ClassDB::bind_method(D_METHOD("get_max_speed"), &ForceBulletPath::get_max_speed);

    ClassDB::bind_method(D_METHOD("set_force", "force"), &ForceBulletPath::set_force);
    ClassDB::bind_method(D_METHOD("get_force"), &ForceBulletPath::get_force);

    ClassDB::bind_method(D_METHOD("set_rotate_forces_on_transform", "rotate"), &ForceBulletPath::set_rotate_forces_on_transform);
    ClassDB::bind_method(D_METHOD("get_rotate_forces_on_transform"), &ForceBulletPath::get_rotate_forces_on_transform);

    ClassDB::bind_method(D_METHOD("apply_force", "force", "time"), &ForceBulletPath::apply_force);
    ClassDB::bind_method(D_METHOD("apply_impulse", "impulse", "time"), &ForceBulletPath::apply_impulse);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lifetime", PROPERTY_HINT_RANGE, "0,30,0.01,or_greater"), "set_lifetime", "get_lifetime");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,500,1,or_greater"), "set_init_speed", "get_init_speed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rotation", PROPERTY_HINT_RANGE, "-360,360,0.1,radians_as_degrees"), "set_init_rotation", "get_init_rotation");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_speed", PROPERTY_HINT_RANGE, "0,2000,0.1,or_greater"), "set_max_speed", "get_max_speed");
    ADD_PROPERTY(PropertyInfo(Variant::VECTOR2, "force"), "set_force", "get_force");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "rotate_forces_on_transform"), "set_rotate_forces_on_transform", "get_rotate_forces_on_transform");
}

ForceBulletPath::ForceBulletPath() {
    lifetime = 0;
    init_velocity = 100*Vector2(1,0);
    max_speed = 10000;
    force = Vector2(0,0);
    rotate_forces_on_transform = false;
}

ForceBulletPath::ForceBulletPath(const ForceBulletPath *p_other) {
    lifetime = p_other->get_lifetime();
    init_velocity = p_other->get_init_velocity();
    max_speed = p_other->get_max_speed();
    force = p_other->force;
    rotate_forces_on_transform = p_other->rotate_forces_on_transform;
}

ForceBulletPath::ForceBulletPath(float p_speed, float p_dir, const Vector2 &p_force, float p_lifetime, float p_max_speed, bool p_rotate_forces_on_transform) {
    lifetime = p_lifetime;
    init_velocity = p_speed*Vector2::from_angle(p_dir);
    max_speed = p_max_speed;
    force = p_force;
    rotate_forces_on_transform = p_rotate_forces_on_transform;
}

ForceBulletPath::~ForceBulletPath() {};