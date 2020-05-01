#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include <map.hpp>

BaseMap::BaseMap(int width, int height, int tile_size) :
    width(width), height(height), tile_size(tile_size), tiles(width * height) {}

void BaseMap::draw(int start_x, int start_y, int end_x, int end_y) {
    for (int y = std::max(start_y / tile_size, 0); y < height && y * tile_size < end_y; y++) {
        for (int x = std::max(start_x / tile_size, 0); x < width && x * tile_size < end_x; x++) {
            al_draw_rectangle(x * tile_size - start_x, y * tile_size - start_y,
                    (x + 1) * tile_size - start_x, (y + 1) * tile_size - start_y,
                    al_map_rgb(225, 225, 225), 1.0f);
            if (tiles[width * y + x].blocked) {
                al_draw_filled_rectangle(x * tile_size + 1 - start_x, y * tile_size + 1 - start_y,
                        (x + 1) * tile_size - 1 - start_x, (y + 1) * tile_size - 1 - start_y,
                        al_map_rgb(225, 0, 0));
            }
        }
    }
}

void BaseMap::set_blocked(int tile_x, int tile_y, bool blocked) {
    if (tile_x >= 0 && tile_y >= 0 && tile_x < width && tile_y < height) {
        tiles[width * tile_y + tile_x].blocked = blocked;
    }
}
