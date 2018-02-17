#pragma once

#include <stdint.h>
#include <vector>
#include <msgpack.hpp>
#include "Snake.h"

namespace TcpProtocol
{
    struct Message
    {
        uint8_t protocol_version;
        uint8_t message_type;
    };

    struct SnakeSegment
    {
        double pos_x;
        double pos_y;
    };

    struct Bot
    {
        uint64_t id;
        std::string name;
        double heading;
        double segment_radius;

        // snake_segments[0] = Head of Snake
        std::vector<SnakeSegment> snake_segments;
        std::vector<uint32_t> color;
    };

    struct Food {
        uint64_t id;
        double   pos_x;
        double   pos_y;
        double   value;
    };

    struct GameInfoMessage : public Message
    {
        double world_size_x;
        double world_size_y;
        double food_decay_per_frame;
    };

    struct TickMessage : public Message
    {
        uint64_t frame_id; // frame counter since start of server
    };

    struct WorldUpdateMessage : public Message
    {
        std::vector<Bot> bots;
        std::vector<Food> food;
    };

    struct BotSpawnMessage : public Message
    {
        Bot new_bot;
    };

    // kill
    struct BotKilledMessage : public Message
    {
        uint64_t killer_id;
        uint64_t victim_id; // victim is deleted in this frame
    };

    // food produce
    struct FoodSpawnMessage : public Message
    {
        std::vector<Food> new_food;
    };

    // food consumed
    struct FoodConsumedItem
    {
        uint64_t food_id; // food is deleted in this frame
        uint64_t bot_id; // bot consuming the food
    };

    struct FoodConsumedMessage : public Message
    {
        std::vector<FoodConsumedItem> items;
    };


    // move
    struct BotMoveItem
    {
        uint64_t bot_id;
        std::vector<SnakeSegment> new_segments;
        uint32_t current_length;
        uint32_t current_segment_radius;
    };

    struct BotMovedMessage : public Message
    {
        std::vector<BotMoveItem> items;
    };

}

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

template <> struct pack<TcpProtocol::GameInfoMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::GameInfoMessage const& v) const
    {
        o.pack_array(5);
        o.pack(v.protocol_version);
        o.pack(v.message_type);
        o.pack(v.world_size_x);
        o.pack(v.world_size_y);
        o.pack(v.food_decay_per_frame);
        return o;
    }
};

template <> struct pack<TcpProtocol::TickMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::TickMessage const& v) const
    {
        o.pack_array(3);
        o.pack(v.protocol_version);
        o.pack(v.message_type);
        o.pack(v.frame_id);
        return o;
    }
};

template <> struct pack<TcpProtocol::WorldUpdateMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::WorldUpdateMessage const& v) const
    {
        o.pack_array(4);
        o.pack(v.protocol_version);
        o.pack(v.message_type);
        o.pack(v.bots);
        o.pack(v.food);
        return o;
    }
};

template <> struct pack<TcpProtocol::BotSpawnMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::BotSpawnMessage const& v) const
    {
        o.pack_array(3);
        o.pack(v.protocol_version);
        o.pack(v.message_type);
        o.pack(v.new_bot);
        return o;
    }
};

template <> struct pack<TcpProtocol::BotKilledMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::BotKilledMessage const& v) const
    {
        o.pack_array(4);
        o.pack(v.protocol_version);
        o.pack(v.message_type);
        o.pack(v.killer_id);
        o.pack(v.victim_id);
        return o;
    }
};

template <> struct pack<TcpProtocol::FoodSpawnMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::FoodSpawnMessage const& v) const
    {
        o.pack_array(3);
        o.pack(v.protocol_version);
        o.pack(v.message_type);
        o.pack(v.new_food);
        return o;
    }
};

template <> struct pack<TcpProtocol::FoodConsumedMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::FoodConsumedMessage const& v) const
    {
        o.pack_array(3);
        o.pack(v.protocol_version);
        o.pack(v.message_type);
        o.pack(v.items);
        return o;
    }
};

template <> struct pack<TcpProtocol::FoodConsumedItem>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::FoodConsumedItem const& v) const
    {
        o.pack_array(2);
        o.pack(v.food_id);
        o.pack(v.bot_id);
        return o;
    }
};

template <> struct pack<TcpProtocol::BotMovedMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::BotMovedMessage const& v) const
    {
        o.pack_array(3);
        o.pack(v.protocol_version);
        o.pack(v.message_type);
        o.pack(v.items);
        return o;
    }
};

template <> struct pack<TcpProtocol::BotMoveItem>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::BotMoveItem const& v) const
    {
        o.pack_array(4);
        o.pack(v.bot_id);
        o.pack(v.new_segments);
        o.pack(v.current_length);
        o.pack(v.current_segment_radius);
        return o;
    }
};

template <> struct pack<TcpProtocol::Bot>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::Bot const& v) const
    {
        o.pack_array(6);
        o.pack(v.id);
        o.pack(v.name);
        o.pack(v.heading);
        o.pack(v.segment_radius);
        o.pack(v.snake_segments);
        o.pack(v.color);
        return o;
    }
};

template <> struct pack<TcpProtocol::SnakeSegment>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::SnakeSegment const& v) const
    {
        o.pack_array(2);
        o.pack(v.pos_x);
        o.pack(v.pos_y);
        return o;
    }
};

template <> struct pack<TcpProtocol::Food>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, TcpProtocol::Food const& v) const
    {
        o.pack_array(4);
        o.pack(v.id);
        o.pack(v.pos_x);
        o.pack(v.pos_y);
        o.pack(v.value);
        return o;
    }
};

} // namespace adaptor
} // MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS)
} // namespace msgpack
