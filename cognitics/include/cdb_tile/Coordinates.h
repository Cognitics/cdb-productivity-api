
#pragma once

#include "Latitude.h"
#include "Longitude.h"

namespace cognitics {
namespace cdb {

class Coordinates
{
public:
    Coordinates(void);
    Coordinates(const Latitude& latitude, const Longitude& longitude);
    Coordinates(double latDouble,double lonDouble);

    Latitude latitude(void) const;
    Longitude longitude(void) const;
    bool operator==(const Coordinates& rhs) { return (_latitude == rhs._latitude) && (_longitude == rhs._longitude); }
    bool operator!=(const Coordinates& rhs) { return !(*this == rhs); } 
    bool operator<(const Coordinates& rhs)
    {
        if(_latitude < _latitude)
            return true;
        if(_latitude > _latitude)
            return false;
        if(_longitude < _longitude)
            return true;
        if(_longitude > _longitude)
            return false;
        return false;
    }
    bool operator>(const Coordinates& rhs)
    {
        if(_latitude > _latitude)
            return true;
        if(_latitude < _latitude)
            return false;
        if(_longitude > _longitude)
            return true;
        if(_longitude < _longitude)
            return false;
        return false;
    }

private:
    Latitude _latitude;
    Longitude _longitude;
};

} }
