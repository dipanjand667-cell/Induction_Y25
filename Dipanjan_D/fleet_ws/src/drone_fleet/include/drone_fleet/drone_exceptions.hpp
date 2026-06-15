#include <stdexcept>
#include <string>

class DroneException : public std::runtime_error
{
public:
    DroneException(std::string msg)
        : std::runtime_error(msg)
    {
    }
};

class BatteryDepletedError : public DroneException
{
public:
    BatteryDepletedError(std::string msg)
        : DroneException(msg)
    {
    }
};

class InvalidStateError : public DroneException
{
public:
    InvalidStateError(std::string msg)
        : DroneException(msg)
    {
    }
};

class AltitudeError : public DroneException
{
public:
    AltitudeError(std::string msg)
        : DroneException(msg)
    {
    }
};
