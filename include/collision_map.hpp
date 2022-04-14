#ifndef BASE_MAP_HPP
#define BASE_MAP_HPP

#include <vector>

class BaseMap {
    public:
        BaseMap(int width, int height, int tile_size);

        void draw(int start_x, int start_y, int end_x, int end_y);
        void set_blocked(int tile_x, int tile_y, bool blocked);

        const int get_width() { return width; }
        const int get_height() { return height; }
        const int get_tile_size() { return tile_size; }

    private:
        struct MapTile {
            bool blocked = false;
        };

    private:
        int width, height, tile_size;
        std::vector<MapTile> tiles;
};

#endif
