#ifndef AXES_BOX_H_
#define AXES_BOX_H_

#include <arl/math/math.h>

class AxesBox
{
private:
    arl::Point3Dd min_vec;
    arl::Point3Dd max_vec;

public:
    arl::Point3Dd getCenter() const;
    arl::Point3Dd getMin() const;
    arl::Point3Dd getMax() const;
    arl::Vec3Dd getScale() const;

    void setCenter(const arl::Point3Dd &new_center);
    void setMin(const arl::Point3Dd &new_min);
    void setMax(const arl::Point3Dd &new_max);

    void moveCenter(const arl::Vec3Dd &delta_vec);
    void moveMin(const arl::Vec3Dd &delta_vec);
    void moveMax(const arl::Vec3Dd &delta_vec);

    AxesBox() = default;
};

#endif
