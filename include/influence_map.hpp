#ifndef INFLUENCE_MAP_HPP
#define INFLUENCE_MAP_HPP

#include <list>
#include <vector>
#include <string>

class InfluenceMap {
    public:
        InfluenceMap(std::string name, int width, int height, int tile_size, float strength, float decay, float momentum);

        void draw(int start_x, int start_y, int end_x, int end_y);

        void add_influence(int tile_x, int tile_y);
        void remove_influence(int tile_x, int tile_y);

        void recalculate();
        void print_values();

        const std::string& get_name() const { return name; }

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
