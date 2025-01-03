#include "bezier_bullet_path.h"

float _calc_lifetime(Ref<Curve2D> curve, BezierBulletPath::SamplingMode mode, float speed) {
    ERR_FAIL_COND_V_MSG(!curve.is_valid(), 0, "Assigned curve is invalid. Cannot calculate lifetime.");

    switch (mode) {
        case (BezierBulletPath::SamplingMode::CONSTANT_SPEED): {
            return curve->get_baked_length()/speed;
        } break;

        case (BezierBulletPath::SamplingMode::VARIABLE_SPEED): {
            return (curve->get_point_count()-1)/speed;
        } break;
    }

    return 0;
}

float BezierBulletPath::get_lifetime() const {
    if (continue_on_complete || speed == 0) { return 0; }

    return _calc_lifetime(curve, mode, speed);
}

Vector2 BezierBulletPath::get_step(float p_time, float p_delta) const {
    ERR_FAIL_COND_V_MSG(!curve.is_valid(), Vector2(0,0), "Assigned curve is invalid. Cannot calculate step.");

    switch (mode) {
        case (SamplingMode::CONSTANT_SPEED): {
            if (speed != 0 && p_time + p_delta > _calc_lifetime(curve, mode, speed) && continue_on_complete) {
                return p_delta*speed*(curve->samplef(curve->get_point_count() - 1) - curve->samplef(curve->get_point_count() - 1 - CMP_EPSILON)).normalized();
            }
            return curve->sample_baked(speed*(p_time + p_delta)) - curve->sample_baked(speed*p_time);
        } break;

        case (SamplingMode::VARIABLE_SPEED): {
            if (speed != 0 && p_time + p_delta > _calc_lifetime(curve, mode, speed) && continue_on_complete) {
                return p_delta*speed/CMP_EPSILON*(curve->samplef(curve->get_point_count() - 1) - curve->samplef(curve->get_point_count() - 1 - CMP_EPSILON));
            }
            return curve->samplef(speed*(p_time + p_delta)) - curve->samplef(speed*p_time);
        } break;
    }

    return Vector2(0,0);
}

Vector2 BezierBulletPath::get_direction(float p_time) const {
    return get_step(p_time, CMP_EPSILON).normalized();
}

Vector2 BezierBulletPath::get_velocity(float p_time) const {
    return get_step(p_time, CMP_EPSILON)/CMP_EPSILON;
}

float BezierBulletPath::get_rotation(float p_time) const {
    return get_step(p_time, CMP_EPSILON).angle(); 
}

float BezierBulletPath::get_speed(float p_time) const {
    if (mode == SamplingMode::CONSTANT_SPEED) { return speed; }

    return get_velocity(p_time).length();
}

Ref<BulletPath> BezierBulletPath::rotated(float p_rotation) const {
    ERR_FAIL_COND_V_MSG(!curve.is_valid(), Ref<BezierBulletPath>(this), "Assigned curve is invalid. Cannot rotate path."); 

    Vector2 init_point = curve->get_point_position(0);
    Ref<Curve2D> new_curve = Ref<Curve2D>(memnew(Curve2D()));
    for (int i = 0; i < curve->get_point_count(); i++) {
        new_curve->add_point(
            (curve->get_point_position(i) - init_point).rotated(p_rotation),
            curve->get_point_in(i).rotated(p_rotation),
            curve->get_point_out(i).rotated(p_rotation)
        );
    }

    return Ref<BezierBulletPath>(memnew(BezierBulletPath(new_curve, mode, speed, continue_on_complete)));
}

Ref<BulletPath> BezierBulletPath::xformed(const Transform2D p_transform) const {
    ERR_FAIL_COND_V_MSG(!curve.is_valid(), Ref<BezierBulletPath>(this), "Assigned curve is invalid. Cannot transform path."); 

    Vector2 init_point = curve->samplef(0);
    Ref<Curve2D> new_curve = Ref<Curve2D>(memnew(Curve2D()));
    for (int i = 0; i < curve->get_point_count(); i++) {
        new_curve->add_point(
            p_transform.xform(curve->get_point_position(i) - init_point),
            p_transform.xform(curve->get_point_in(i)),
            p_transform.xform(curve->get_point_out(i))
        );
    }

    return Ref<BezierBulletPath>(memnew(BezierBulletPath(new_curve, mode, speed, continue_on_complete)));
}

void BezierBulletPath::set_continue_on_complete(bool p_continue) {
    continue_on_complete = p_continue;
}

bool BezierBulletPath::get_continue_on_complete() const {
    return continue_on_complete;
}

void BezierBulletPath::set_sampling_mode(BezierBulletPath::SamplingMode p_mode) {
    mode = p_mode;
}
BezierBulletPath::SamplingMode BezierBulletPath::get_sampling_mode() const {
    return mode;
}

void BezierBulletPath::set_speed_modifier(float p_speed) {
    speed = p_speed;
}
float BezierBulletPath::get_speed_modifier() const {
    return speed;
}

void BezierBulletPath::set_curve(const Ref<Curve2D> &p_curve) {
    curve = p_curve;
}
Ref<Curve2D> BezierBulletPath::get_curve() const {
    return curve;
}

void BezierBulletPath::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_lifetime"), &BezierBulletPath::get_lifetime);

    ClassDB::bind_method(D_METHOD("get_step", "time", "delta"), &BezierBulletPath::get_step);

	ClassDB::bind_method(D_METHOD("get_direction", "time"), &BezierBulletPath::get_direction);
    ClassDB::bind_method(D_METHOD("get_velocity", "time"), &BezierBulletPath::get_velocity);
    ClassDB::bind_method(D_METHOD("get_rotation", "time"), &BezierBulletPath::get_rotation);
    ClassDB::bind_method(D_METHOD("get_speed", "time"), &BezierBulletPath::get_speed);

    ClassDB::bind_method(D_METHOD("rotated", "degrees"), &BezierBulletPath::rotated);
    ClassDB::bind_method(D_METHOD("xformed", "transform"), &BezierBulletPath::xformed);

    ClassDB::bind_method(D_METHOD("set_curve", "curve"), &BezierBulletPath::set_curve);
    ClassDB::bind_method(D_METHOD("get_curve"), &BezierBulletPath::get_curve);

    ClassDB::bind_method(D_METHOD("set_sampling_mode", "speed"), &BezierBulletPath::set_sampling_mode);
    ClassDB::bind_method(D_METHOD("get_sampling_mode"), &BezierBulletPath::get_sampling_mode);

    ClassDB::bind_method(D_METHOD("set_speed_modifier", "speed"), &BezierBulletPath::set_speed_modifier);
    ClassDB::bind_method(D_METHOD("get_speed_modifier"), &BezierBulletPath::get_speed_modifier);

    ClassDB::bind_method(D_METHOD("set_continue_on_complete", "continue"), &BezierBulletPath::set_continue_on_complete);
    ClassDB::bind_method(D_METHOD("get_continue_on_complete"), &BezierBulletPath::get_continue_on_complete);

    ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "curve", PROPERTY_HINT_RESOURCE_TYPE, "Curve2D"), "set_curve", "get_curve");
    ADD_PROPERTY(PropertyInfo(Variant::INT, "sampling_mode", PROPERTY_HINT_ENUM, "CONSTANT_SPEED,VARIABLE_SPEED"), "set_sampling_mode", "get_sampling_mode");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "speed_modifier"), "set_speed_modifier", "get_speed_modifier");
    ADD_PROPERTY(PropertyInfo(Variant::BOOL, "continue_on_complete"), "set_continue_on_complete", "get_continue_on_complete");

    BIND_ENUM_CONSTANT(CONSTANT_SPEED);
	BIND_ENUM_CONSTANT(VARIABLE_SPEED);
}

BezierBulletPath::BezierBulletPath() {
    mode = SamplingMode::VARIABLE_SPEED;
    speed = 1;
    curve = Ref<Curve2D>();
    continue_on_complete = true;
}

BezierBulletPath::BezierBulletPath(const BezierBulletPath *p_other) {
    mode = p_other->get_sampling_mode();
    speed = p_other->get_speed_modifier();
    curve = p_other->get_curve();
    continue_on_complete = p_other->get_continue_on_complete();

}

BezierBulletPath::BezierBulletPath(const Ref<Curve2D> &p_curve, SamplingMode p_mode, float p_speed, bool p_continue) {
    mode = p_mode;
    speed = p_speed;
    curve = p_curve;
    continue_on_complete = p_continue;
}

BezierBulletPath::~BezierBulletPath() {}