#ifndef BEZIERBULLETPATH_H
#define BEZIERBULLETPATH_H

#include "scene/resources/curve.h"

#include "bullet_path.h"

class BezierBulletPath : public BulletPath {
public:
    enum SamplingMode {
        CONSTANT_SPEED,
        VARIABLE_SPEED,
    };

private:
    GDCLASS(BezierBulletPath, BulletPath);

    SamplingMode mode;
    float speed;
    Ref<Curve2D> curve;
    bool continue_on_complete;

protected:
	static void _bind_methods();

public:
	virtual float get_lifetime() const override;

    virtual Vector2 get_step(float p_time, float p_delta) const override;

    virtual Vector2 get_direction(float p_time) const override;
    virtual Vector2 get_velocity(float p_time) const override;

    virtual float get_rotation(float p_time) const override;
    virtual float get_speed(float p_time) const override;

    virtual Ref<BulletPath> rotated(float p_rotation) const override;
    virtual Ref<BulletPath> xformed(const Transform2D &p_transform) const override;

    void set_continue_on_complete(bool p_continue);
    bool get_continue_on_complete() const;

    void set_curve(const Ref<Curve2D> &p_curve);
    Ref<Curve2D> get_curve() const;

    void set_sampling_mode(BezierBulletPath::SamplingMode p_mode);
    BezierBulletPath::SamplingMode get_sampling_mode() const;

    void set_speed_modifier(float p_speed);
    float get_speed_modifier() const;

    BezierBulletPath();
    BezierBulletPath(const BezierBulletPath *p_other);
    BezierBulletPath(const Ref<Curve2D> &p_curve, BezierBulletPath::SamplingMode p_mode, float p_speed, bool p_continue);
    ~BezierBulletPath();
};

VARIANT_ENUM_CAST(BezierBulletPath::SamplingMode);

#endif