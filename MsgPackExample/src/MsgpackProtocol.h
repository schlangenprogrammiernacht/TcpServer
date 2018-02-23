#pragma once

#include <stdint.h>
#include <vector>
#include <msgpack.hpp>
#include "Snake.h"

namespace MsgpackProtocol
{
    enum
    {
        MESSAGE_TYPE_GAME_INFO = 0x00,
        MESSAGE_TYPE_WORLD_UPDATE = 0x01,

        MESSAGE_TYPE_TICK = 0x10,

        MESSAGE_TYPE_BOT_SPAWN = 0x20,
        MESSAGE_TYPE_BOT_KILL = 0x21,
        MESSAGE_TYPE_BOT_MOVE = 0x22,

        MESSAGE_TYPE_FOOD_SPAWN = 0x30,
        MESSAGE_TYPE_FOOD_CONSUME = 0x31,
        MESSAGE_TYPE_FOOD_DECAY = 0x32,

        MESSAGE_TYPE_PLAYER_INFO = 0xF0,
    };

    static constexpr const uint8_t PROTOCOL_VERSION = 1;

    struct SnakeSegment
    {
        double pos_x;
        double pos_y;
    };

    struct Bot
    {
        uint64_t id;
        std::string name;
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

    struct GameInfoMessage
    {
        double world_size_x = 0;
        double world_size_y = 0;
        double food_decay_per_frame = 0;
    };

    struct PlayerInfoMessage
    {
        uint64_t player_id; // id der von dieser Verbindung gesteuerten Schlange
    };

    struct TickMessage
    {
        uint64_t frame_id; // frame counter since start of server
    };

    struct WorldUpdateMessage
    {
        std::vector<Bot> bots;
        std::vector<Food> food;
    };

    struct BotSpawnMessage
    {
        Bot new_bot;
    };

    struct BotMoveItem
    {
        uint64_t bot_id;
        std::vector<SnakeSegment> new_segments;
        uint32_t current_length;
        uint32_t current_segment_radius;
    };

    struct BotMoveMessage
    {
        std::vector<BotMoveItem> items;
    };

    struct BotKillMessage
    {
        uint64_t killer_id;
        uint64_t victim_id; // victim is deleted in this frame
    };

    struct FoodSpawnMessage
    {
        std::vector<Food> new_food;
    };

    struct FoodConsumItem
    {
        uint64_t food_id; // food is deleted in this frame
        uint64_t bot_id; // bot consuming the food
    };

    struct FoodConsumeMessage
    {
        std::vector<FoodConsumItem> items;
    };

    struct FoodDecayMessage
    {
        std::vector<uint64_t> food_ids; // food is deleted in this frame
    };

}

namespace msgpack {
MSGPACK_API_VERSION_NAMESPACE(MSGPACK_DEFAULT_API_NS) {
namespace adaptor {

template <> struct pack<MsgpackProtocol::GameInfoMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::GameInfoMessage const& v) const
    {
        o.pack_array(5);
        o.pack(MsgpackProtocol::PROTOCOL_VERSION);
        o.pack(static_cast<int>(MsgpackProtocol::MESSAGE_TYPE_GAME_INFO));
        o.pack(v.world_size_x);
        o.pack(v.world_size_y);
        o.pack(v.food_decay_per_frame);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::PlayerInfoMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::PlayerInfoMessage const& v) const
    {
        o.pack_array(3);
        o.pack(MsgpackProtocol::PROTOCOL_VERSION);
        o.pack(static_cast<int>(MsgpackProtocol::MESSAGE_TYPE_PLAYER_INFO));
        o.pack(v.player_id);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::TickMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::TickMessage const& v) const
    {
        o.pack_array(3);
        o.pack(MsgpackProtocol::PROTOCOL_VERSION);
        o.pack(static_cast<int>(MsgpackProtocol::MESSAGE_TYPE_TICK));
        o.pack(v.frame_id);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::WorldUpdateMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::WorldUpdateMessage const& v) const
    {
        o.pack_array(4);
        o.pack(MsgpackProtocol::PROTOCOL_VERSION);
        o.pack(static_cast<int>(MsgpackProtocol::MESSAGE_TYPE_WORLD_UPDATE));
        o.pack(v.bots);
        o.pack(v.food);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::BotSpawnMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::BotSpawnMessage const& v) const
    {
        o.pack_array(3);
        o.pack(MsgpackProtocol::PROTOCOL_VERSION);
        o.pack(static_cast<int>(MsgpackProtocol::MESSAGE_TYPE_BOT_SPAWN));
        o.pack(v.new_bot);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::BotMoveMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::BotMoveMessage const& v) const
    {
        o.pack_array(3);
        o.pack(MsgpackProtocol::PROTOCOL_VERSION);
        o.pack(static_cast<int>(MsgpackProtocol::MESSAGE_TYPE_BOT_MOVE));
        o.pack(v.items);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::BotMoveItem>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::BotMoveItem const& v) const
    {
        o.pack_array(4);
        o.pack(v.bot_id);
        o.pack(v.new_segments);
        o.pack(v.current_length);
        o.pack(v.current_segment_radius);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::BotKillMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::BotKillMessage const& v) const
    {
        o.pack_array(4);
        o.pack(MsgpackProtocol::PROTOCOL_VERSION);
        o.pack(static_cast<int>(MsgpackProtocol::MESSAGE_TYPE_BOT_KILL));
        o.pack(v.killer_id);
        o.pack(v.victim_id);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::FoodSpawnMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::FoodSpawnMessage const& v) const
    {
        o.pack_array(3);
        o.pack(MsgpackProtocol::PROTOCOL_VERSION);
        o.pack(static_cast<int>(MsgpackProtocol::MESSAGE_TYPE_FOOD_SPAWN));
        o.pack(v.new_food);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::FoodConsumeMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::FoodConsumeMessage const& v) const
    {
        o.pack_array(3);
        o.pack(MsgpackProtocol::PROTOCOL_VERSION);
        o.pack(static_cast<int>(MsgpackProtocol::MESSAGE_TYPE_FOOD_CONSUME));
        o.pack(v.items);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::FoodConsumItem>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::FoodConsumItem const& v) const
    {
        o.pack_array(2);
        o.pack(v.food_id);
        o.pack(v.bot_id);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::FoodDecayMessage>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::FoodDecayMessage const& v) const
    {
        o.pack_array(3);
        o.pack(MsgpackProtocol::PROTOCOL_VERSION);
        o.pack(static_cast<int>(MsgpackProtocol::MESSAGE_TYPE_FOOD_DECAY));
        o.pack(v.food_ids);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::Bot>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::Bot const& v) const
    {
        o.pack_array(5);
        o.pack(v.id);
        o.pack(v.name);
        o.pack(v.segment_radius);
        o.pack(v.snake_segments);
        o.pack(v.color);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::SnakeSegment>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::SnakeSegment const& v) const
    {
        o.pack_array(2);
        o.pack(v.pos_x);
        o.pack(v.pos_y);
        return o;
    }
};

template <> struct pack<MsgpackProtocol::Food>
{
    template <typename Stream> msgpack::packer<Stream>& operator()(msgpack::packer<Stream>& o, MsgpackProtocol::Food const& v) const
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
