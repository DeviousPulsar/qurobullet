#ifndef FORCEBULLETPATH_H
#define FORCEBULLETPATH_H

#include "bullet_path.h"

#include <vector>

struct ForceInfo;

class ForceBulletPath : public BulletPath {
    GDCLASS(ForceBulletPath, BulletPath);

    float lifetime;
    float max_speed;
    Vector2 init_velocity;
    Vector2 force;

    bool rotate_forces_on_transform;

    int _get_force_index(float p_time);

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

    void set_init_velocity(const Vector2 &p_velocity);
    Vector2 get_init_velocity() const;

    void set_init_speed(float p_speed);
    float get_init_speed() const;

    void set_init_rotation(float p_rotation);
    float get_init_rotation() const;

    void set_max_speed(float p_max_speed);
    float get_max_speed() const;

    void set_force(const Vector2 &p_force);
    Vector2 get_force() const;

    void set_rotate_forces_on_transform(bool p_rotate);
    bool get_rotate_forces_on_transform() const;

    Ref<ForceBulletPath> apply_force(const Vector2 &p_force, float p_time);
    Ref<ForceBulletPath> apply_impulse(const Vector2 &p_force, float p_time);

    ForceBulletPath();
    ForceBulletPath(const ForceBulletPath *p_other);
    ForceBulletPath(float p_speed, float p_dir, const Vector2 &p_force, float p_lifetime = 0, float p_max_speed = 500, bool p_rotate_forces_on_transform = false);
    ~ForceBulletPath();
};

#endif