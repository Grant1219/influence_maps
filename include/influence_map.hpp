#ifndef INFLUENCE_MAP_HPP
#define INFLUENCE_MAP_HPP

#include <list>
#include <vector>
#include <string>

class InfluenceMap {
    public:
        InfluenceMap(std::string name, int width, int height, int tile_size, float strength, float decay, float momentum);

        void add_influence(int tile_x, int tile_y);
        void remove_influence(int tile_x, int tile_y);

        void recalculate();
        void print_values();

        const std::string& get_name() const { return name; }
        const std::vector<float>& get_influence_map() const { return influence_map; }
        const int get_width() { return width; }
        const int get_height() { return height; }
        const int get_tile_size() { return tile_size; }
        const float get_strength() { return strength; }
        const float get_decay() { return decay; }
        const float get_momentum() { return momentum; }

    private:
        struct InfluenceSource {
            int x, y;
        };

    private:
        std::string name;
        int width, height, tile_size;
        float strength, decay, momentum;
        std::list<InfluenceSource> influence_sources;
        std::vector<float> influence_map, influence_buffer;
};

#endif
