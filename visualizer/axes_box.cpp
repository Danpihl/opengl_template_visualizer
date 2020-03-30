#include "visualizer/axes_box.h"

arl::Vec3Dd AxesBox::getScale() const
{
    return max_vec - min_vec;
}

arl::Point3Dd AxesBox::getCenter() const
{
    return (min_vec + max_vec) / 2.0;
}

arl::Point3Dd AxesBox::getMin() const
{
    return min_vec;
}

arl::Point3Dd AxesBox::getMax() const
{
    return max_vec;
}

void AxesBox::setCenter(const arl::Point3Dd &new_center)
{
    const arl::Point3Dd c = getCenter();
    min_vec = min_vec - c + new_center;
    max_vec = max_vec - c + new_center;
}

void AxesBox::setMin(const arl::Point3Dd &new_min)
{
    min_vec = new_min;
}

void AxesBox::setMax(const arl::Point3Dd &new_max)
{
    max_vec = new_max;
}

void AxesBox::moveCenter(const arl::Vec3Dd &delta_vec)
{
    min_vec = min_vec + delta_vec;
    max_vec = max_vec + delta_vec;
}

void AxesBox::moveMin(const arl::Vec3Dd &delta_vec)
{
    min_vec = min_vec + delta_vec;
}

void AxesBox::moveMax(const arl::Vec3Dd &delta_vec)
{
    max_vec = max_vec + delta_vec;
}
