#pragma once
#include <vector>
#include <msgpack.hpp>

struct Vector2d
{
    double X;
    double Y;
    MSGPACK_DEFINE(X, Y)
};

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

template <> struct pack<Vector2d>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, Vector2d const& v) const
    {
        o.pack_map(2);
        o.pack("x");
        o.pack(v.X);
        o.pack("y");
        o.pack(v.Y);
        return o;
    }
};
} // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack

class Snake
{
    public:
        uint32_t Id = 0;
        double Heading = 0;
        double Speed = 5.0;
        std::vector<Vector2d> Segments;
        Snake(uint32_t id, Vector2d p, double heading, unsigned length);
        Snake(const Snake& other) = delete;
        void SetHeading(double heading);
        void MakeStep();

    private:
        double _stepX = 0;
        double _stepY = 0;

};


namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

template <> struct pack<Snake>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, Snake const& v) const
    {
        o.pack_map(4);
        o.pack("id");
        o.pack(v.Id);
        o.pack("Heading");
        o.pack(v.Heading);
        o.pack("Speed");
        o.pack(v.Speed);
        o.pack("Segments");
        o.pack(v.Segments);
        return o;
    }
};
} // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack
