#include "Snake.h"
#include <cmath>

Snake::Snake(uint32_t id, Vector2d p, double heading, unsigned length)
    : Id(id)
    , Heading(heading)
{
    SetHeading(-heading);
    for (unsigned i=0; i<length; i++)
    {
        Segments.push_back(p);
        p.X += _stepX;
        p.Y += _stepY;
    }
    SetHeading(heading);
}

void Snake::SetHeading(double heading)
{
    Heading = heading;
    _stepX = Speed * cos(Heading);
    _stepY = Speed * sin(Heading);
}

void Snake::MakeStep()
{
    for (size_t i=Segments.size()-1; i>=1; i--)
    {
        Segments[i] = Segments[i-1];
    }

    Segments[0] = {
        Segments[1].X + _stepX,
        Segments[1].Y + _stepY
    };
}
