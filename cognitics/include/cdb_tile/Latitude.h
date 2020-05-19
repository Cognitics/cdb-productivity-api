
#pragma once

namespace cognitics {
namespace cdb {

class Latitude
{
public:
    Latitude(void);
    Latitude(double value);

    double value(void) const;

    bool operator==(const Latitude& rhs) { return (_value == rhs._value); }
    bool operator!=(const Latitude& rhs) { return !(*this == rhs); } 
    bool operator<(const Latitude& rhs) { return (_value < rhs._value); }
    bool operator>(const Latitude& rhs) { return (_value > rhs._value); }

private:
    double _value;
};

} }
