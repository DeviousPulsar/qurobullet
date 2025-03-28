#ifndef SIMPLEBULLETPATH_H
#define SIMPLEBULLETPATH_H

#include "bullet_path.h"

class SimpleBulletPath : public BulletPath {
    GDCLASS(SimpleBulletPath, BulletPath);

    float lifetime;
    float init_speed;
    float init_rotation;
    float linear_acceleration;
    float max_speed;
    float min_speed;
	float curve_rate;

protected:
	static void _bind_methods();

public:
    void set_lifetime(float p_time);
    virtual float get_lifetime() const override;

    virtual Vector2 get_step(float p_time, float p_delta) const override;
    
    virtual Vector2 get_direction(float p_time) const override;
    virtual Vector2 get_velocity(float p_time) const override;

    virtual float get_rotation(float p_time) const override;
    virtual float get_speed(float p_time) const override;

    virtual Ref<BulletPath> rotated(float p_rotation) const override;
    virtual Ref<BulletPath> xformed(const Transform2D &p_transform) const override;

    void set_init_speed(float p_speed);
    float get_init_speed() const;

    void set_init_rotation(float p_rotation);
    float get_init_rotation() const;

    void set_linear_acceleration(float p_accel);
    float get_linear_acceleration() const;

    void set_max_speed(float p_max_speed);
    float get_max_speed() const;

    void set_min_speed(float p_min_speed);
    float get_min_speed() const;

    void set_curve_rate(float p_curve_rate);
    float get_curve_rate() const;

    SimpleBulletPath();
    SimpleBulletPath(const SimpleBulletPath *p_other);
    SimpleBulletPath(float p_speed, float p_dir, float p_lifetime = 0, float p_accel = 0, 
            float p_max_speed = 500, float p_min_speed = -500, float p_curve_rate = 0);
    ~SimpleBulletPath();
};

#endif