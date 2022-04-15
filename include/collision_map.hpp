#ifndef COLLISION_MAP_HPP
#define COLLISION_MAP_HPP

#include <vector>

class CollisionMap {
    public:
        CollisionMap(int width, int height, int tile_size);

        void draw(int start_x, int start_y, int end_x, int end_y);
        void set_blocked(int tile_x, int tile_y, bool blocked);

        const std::vector<bool>& get_collision_map() const { return collision_tiles; }
        const int get_width() { return width; }
        const int get_height() { return height; }
        const int get_tile_size() { return tile_size; }

    private:
        int width, height, tile_size;
        std::vector<bool> collision_tiles;
};

#endif
