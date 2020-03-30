#include "visualizer/view_angles.h"

ViewAngles::ViewAngles()
    : azimuth_(0.0),
      elevation_(0.0)
{
}

ViewAngles::ViewAngles(const double azimuth, const double elevation)
{
    setAngles(azimuth, elevation);
}

void ViewAngles::setAngles(const double azimuth, const double elevation)
{
    azimuth_ = std::min(azimuth, M_PI);
    azimuth_ = std::max(azimuth_, -M_PI);
    elevation_ = std::min(elevation, M_PI / 2.0);
    elevation_ = std::max(elevation_, -M_PI / 2.0);
}

void ViewAngles::changeAnglesWithDelta(const double delta_azimuth, const double delta_elevation)
{
    double new_azimuth;
    double new_elevation;

    if ((getAzimuth() + delta_azimuth) > M_PI)
    {
        new_azimuth = -M_PI + std::fabs((getAzimuth() + delta_azimuth) - M_PI);
    }
    else if ((getAzimuth() + delta_azimuth) < -M_PI)
    {
        new_azimuth = M_PI - std::fabs((getAzimuth() + delta_azimuth) + M_PI);
    }
    else
    {
        new_azimuth = getAzimuth() + delta_azimuth;
    }

    new_elevation = getElevation() + delta_elevation;
    new_elevation = std::min(new_elevation, M_PI / 2.0);
    new_elevation = std::max(new_elevation, -M_PI / 2.0);

    setAngles(new_azimuth, new_elevation);
}

double ViewAngles::getAzimuth() const
{
    return azimuth_;
}

double ViewAngles::getElevation() const
{
    return elevation_;
}

arl::AxisAngled ViewAngles::getAngleAxis() const
{
    const arl::Matrixd rotation_mat =
        arl::rotationMatrixX(getElevation()) * arl::rotationMatrixY(getAzimuth());
    return arl::rotationMatrixToAxisAngle(rotation_mat);
}

arl::Matrixd ViewAngles::getRotationMatrix() const
{
    return arl::rotationMatrixX(getElevation()) * arl::rotationMatrixY(getAzimuth());
}
