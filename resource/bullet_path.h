#ifndef BULLETPATH_H
#define BULLETPATH_H

#include "core/io/resource.h"
#include "core/math/transform_2d.h"

class BulletPath : public Resource {
    GDCLASS(BulletPath, Resource);

public:
	virtual float get_lifetime() const = 0;

    virtual Vector2 get_step(float p_time, float p_delta) const = 0;

    virtual Vector2 get_direction(float p_time) const = 0;
    virtual Vector2 get_velocity(float p_time) const = 0;

    virtual float get_rotation(float p_time) const = 0;
    virtual float get_speed(float p_time) const = 0;

    virtual Ref<BulletPath> rotated(float p_rotation) const = 0;
    virtual Ref<BulletPath> xformed(const Transform2D &p_transform) const = 0;
};

#endif