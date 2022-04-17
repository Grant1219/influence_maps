#ifndef INFLUENCE_MAP_HPP
#define INFLUENCE_MAP_HPP

#include <list>
#include <vector>
#include <string>
#include <memory>

#include <collision_map.hpp>

class InfluenceMap {
    public:
        InfluenceMap(std::string name, std::shared_ptr<CollisionMap> collision_map, float strength, float decay, float momentum, bool collision_enabled = true);

        void add_influence(int tile_x, int tile_y);
        void remove_influence(int tile_x, int tile_y);
        void set_collision(bool enabled);

        void recalculate();

        const std::string& get_name() const { return name; }
        const std::vector<float>& get_influence_map() const { return influence_map; }
        const int get_width() { return collision_map->get_width(); }
        const int get_height() { return collision_map->get_height(); }
        const int get_tile_size() { return collision_map->get_tile_size(); }
        const float get_strength() { return strength; }
        const float get_decay() { return decay; }
        const float get_momentum() { return momentum; }

    private:
        struct InfluenceSource {
            int x, y;
        };

    private:
        std::string name;
        float strength, decay, momentum;
        bool collision_enabled;
        std::shared_ptr<CollisionMap> collision_map;
        std::list<InfluenceSource> influence_sources;
        std::vector<float> influence_map, influence_buffer;
};

#endif
