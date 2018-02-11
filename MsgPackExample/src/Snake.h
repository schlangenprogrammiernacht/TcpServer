#pragma once
#include <vector>
#include <msgpack.hpp>

struct Vector2d
{
    double X;
    double Y;
    MSGPACK_DEFINE(X, Y)
};

class Snake
{
    public:
        double Heading = 0;
        double Speed = 5;
        std::vector<Vector2d> Segments;
        MSGPACK_DEFINE(Heading, Speed, Segments)

        Snake(Vector2d p, double heading, unsigned length);
        void SetHeading(double heading);
        void MakeStep();

    private:
        double _stepX = 0;
        double _stepY = 0;

};
