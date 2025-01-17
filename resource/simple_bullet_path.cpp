#include "simple_bullet_path.h"

void SimpleBulletPath::set_lifetime(float p_time) {
    lifetime = p_time;
}

float SimpleBulletPath::get_lifetime() const {
    return lifetime;
}

Vector2 SimpleBulletPath::get_step(float p_time, float p_delta) const {
    return p_delta*(get_velocity(p_time + p_delta) + get_velocity(p_time))/2;
}

Vector2 SimpleBulletPath::get_direction(float p_time) const {
    return Vector2(1, 0).rotated(get_rotation(p_time));
}

Vector2 SimpleBulletPath::get_velocity(float p_time) const {
    return get_direction(p_time)*get_speed(p_time);
}

float SimpleBulletPath::get_rotation(float p_time) const {
    return init_rotation + curve_rate*p_time;
}

float SimpleBulletPath::get_speed(float p_time) const {
    return CLAMP(init_speed + linear_acceleration*p_time, min_speed, max_speed);
}

Ref<BulletPath> SimpleBulletPath::rotated(float p_rotation) const {
    return Ref<SimpleBulletPath>(memnew(SimpleBulletPath(
        init_speed, 
        init_rotation + p_rotation,
        lifetime,
        linear_acceleration, 
        max_speed, 
        min_speed,
        curve_rate
    )));
}

Ref<BulletPath> SimpleBulletPath::xformed(const Transform2D &p_transform) const {
    Vector2 new_dir = p_transform.basis_xform(get_direction(0));
    float delta = new_dir.length();

    return Ref<SimpleBulletPath>(memnew(SimpleBulletPath(
        init_speed*delta, 
        new_dir.angle(),
        lifetime,
        linear_acceleration*delta, 
        max_speed*delta, 
        min_speed*delta,
        curve_rate
    )));
}

void SimpleBulletPath::set_init_speed(float p_speed) {
    init_speed = p_speed;
}
float SimpleBulletPath::get_init_speed() const {
    return init_speed;
}

void SimpleBulletPath::set_init_rotation(float p_dir) {
    init_rotation = p_dir;
}
float SimpleBulletPath::get_init_rotation() const {
    return init_rotation;
}

void SimpleBulletPath::set_linear_acceleration(float p_accel) {
    linear_acceleration = p_accel;
}
float SimpleBulletPath::get_linear_acceleration() const {
    return linear_acceleration;
}

void SimpleBulletPath::set_max_speed(float p_max_speed) {
    max_speed = p_max_speed;
}
float SimpleBulletPath::get_max_speed() const {
    return max_speed;
}

void SimpleBulletPath::set_min_speed(float p_min_speed) {
    min_speed = p_min_speed;
}
float SimpleBulletPath::get_min_speed() const {
    return min_speed;
}

void SimpleBulletPath::set_curve_rate(float p_curve_rate) {
    curve_rate = p_curve_rate;
}
float SimpleBulletPath::get_curve_rate() const {
    return curve_rate;
}

void SimpleBulletPath::_bind_methods() {
    ClassDB::bind_method(D_METHOD("set_lifetime", "time"), &SimpleBulletPath::set_lifetime);
	ClassDB::bind_method(D_METHOD("get_lifetime"), &SimpleBulletPath::get_lifetime);

    ClassDB::bind_method(D_METHOD("get_step", "time", "delta"), &SimpleBulletPath::get_step);

	ClassDB::bind_method(D_METHOD("get_direction", "time"), &SimpleBulletPath::get_direction);
    ClassDB::bind_method(D_METHOD("get_velocity", "time"), &SimpleBulletPath::get_velocity);
    ClassDB::bind_method(D_METHOD("get_rotation", "time"), &SimpleBulletPath::get_rotation);
    ClassDB::bind_method(D_METHOD("get_speed", "time"), &SimpleBulletPath::get_speed);

    ClassDB::bind_method(D_METHOD("rotated", "degrees"), &SimpleBulletPath::rotated);
    ClassDB::bind_method(D_METHOD("xformed", "transform"), &SimpleBulletPath::xformed);

    ClassDB::bind_method(D_METHOD("set_init_speed", "speed"), &SimpleBulletPath::set_init_speed);
    ClassDB::bind_method(D_METHOD("get_init_speed"), &SimpleBulletPath::get_init_speed);

    ClassDB::bind_method(D_METHOD("set_init_rotation", "direction"), &SimpleBulletPath::set_init_rotation);
    ClassDB::bind_method(D_METHOD("get_init_rotation"), &SimpleBulletPath::get_init_rotation);

    ClassDB::bind_method(D_METHOD("set_linear_acceleration", "linear_acceration"), &SimpleBulletPath::set_linear_acceleration);
    ClassDB::bind_method(D_METHOD("get_linear_acceleration"), &SimpleBulletPath::get_linear_acceleration);

    ClassDB::bind_method(D_METHOD("set_max_speed", "speed_clamp"), &SimpleBulletPath::set_max_speed);
    ClassDB::bind_method(D_METHOD("get_max_speed"), &SimpleBulletPath::get_max_speed);

    ClassDB::bind_method(D_METHOD("set_min_speed", "speed_clamp"), &SimpleBulletPath::set_min_speed);
    ClassDB::bind_method(D_METHOD("get_min_speed"), &SimpleBulletPath::get_min_speed);

    ClassDB::bind_method(D_METHOD("set_curve_rate", "curve_rate"), &SimpleBulletPath::set_curve_rate);
    ClassDB::bind_method(D_METHOD("get_curve_rate"), &SimpleBulletPath::get_curve_rate);

    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "lifetime", PROPERTY_HINT_RANGE, "0,30,0.01,or_greater"), "set_lifetime", "get_lifetime");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed", PROPERTY_HINT_RANGE, "0,500,1,or_greater"), "set_init_speed", "get_init_speed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "linear_acceleration", PROPERTY_HINT_RANGE, "-200,200,0.1,or_less,or_greater"), "set_linear_acceleration", "get_linear_acceleration");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "max_speed", PROPERTY_HINT_RANGE, "0,500,0.1,or_greater"), "set_max_speed", "get_max_speed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "min_speed", PROPERTY_HINT_RANGE, "-500,500,0.1,or_less,or_greater"), "set_min_speed", "get_min_speed");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "rotation", PROPERTY_HINT_RANGE, "-360,360,0.1,radians_as_degrees"), "set_init_rotation", "get_init_rotation");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "curve_rate", PROPERTY_HINT_RANGE, "-360,360,0.1,radians_as_degrees"), "set_curve_rate", "get_curve_rate");
}

SimpleBulletPath::SimpleBulletPath() {
    lifetime = 0;
    init_speed = 100;
    init_rotation = 0;
    linear_acceleration = 0;
    max_speed = 500;
    min_speed = -500;
    curve_rate = 0;
}

SimpleBulletPath::SimpleBulletPath(const SimpleBulletPath *p_other) {
    lifetime = p_other->get_lifetime();
    init_speed = p_other->get_init_speed();
    init_rotation = p_other->get_init_rotation();
    linear_acceleration = p_other->get_linear_acceleration();
    max_speed = p_other->get_max_speed();
    min_speed = p_other->get_min_speed();
    curve_rate = p_other->get_curve_rate();
}

SimpleBulletPath::SimpleBulletPath(float p_speed, float p_dir, float p_lifetime, float p_accel, float p_max_speed, float p_min_speed, float p_curve_rate) {
    lifetime = p_lifetime;
    init_speed = p_speed;
    init_rotation = p_dir;
    linear_acceleration = p_accel;
    max_speed = p_max_speed;
    min_speed = p_min_speed;
    curve_rate = p_curve_rate;
}

SimpleBulletPath::~SimpleBulletPath() {}