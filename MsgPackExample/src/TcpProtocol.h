#pragma once

#include <stdint.h>
#include <vector>
#include <msgpack.hpp>
#include "Snake.h"

namespace TcpProtocol
{
    struct InitMessage
    {
        uint32_t EgoId = 0;
        double Heading = 0;
    };

    struct FullWorldData
    {
        std::map<uint32_t,std::unique_ptr<Snake>>& Snakes;
    };

    struct StepData
    {
        struct SnakeStep
        {
            uint32_t Id;
            double Heading;
            double Speed;
        };
        std::vector<SnakeStep> Data;
    };

}

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

template <> struct pack<TcpProtocol::InitMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::InitMessage const& v) const
    {
        o.pack_map(3);
        o.pack("t");
        o.pack("init");
        o.pack("EgoId");
        o.pack(v.EgoId);
        o.pack("Heading");
        o.pack(v.Heading);
        return o;
    }
};

template <> struct pack<TcpProtocol::FullWorldData>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::FullWorldData const& v) const
    {
        o.pack_map(2);
        o.pack("t");
        o.pack("full");
        o.pack("Snakes");
        o.pack(v.Snakes);
        return o;
    }
};

template <> struct pack<TcpProtocol::StepData::SnakeStep>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::StepData::SnakeStep const& v) const
    {
        o.pack_map(3);
        o.pack("id");
        o.pack(v.Id);
        o.pack("Heading");
        o.pack(v.Heading);
        o.pack("Speed");
        o.pack(v.Speed);
        return o;
    }
};

template <> struct pack<TcpProtocol::StepData>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::StepData const& v) const
    {
        o.pack_map(2);
        o.pack("t");
        o.pack("step");
        o.pack("Data");
        o.pack(v.Data);
        return o;
    }
};


} // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack
