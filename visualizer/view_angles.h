#ifndef VIEW_ANGLES_H_
#define VIEW_ANGLES_H_

#include <arl/math/math.h>

class ViewAngles
{
private:
    double azimuth_;
    double elevation_;

    void setSnapAngles();
    double calcElevationSnapAngle() const;
    double calcAzimuthSnapAngle() const;

public:
    ViewAngles();
    ViewAngles(const double azimuth, const double elevation);
    void setAngles(const double azimuth, const double elevation);
    void changeAnglesWithDelta(const double azimuth, const double elevation);
    double getAzimuth() const;
    double getElevation() const;

    arl::AxisAngled getAngleAxis() const;
    arl::Matrixd getRotationMatrix() const;
};

#endif
