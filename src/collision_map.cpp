#include <algorithm>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>

#include <collision_map.hpp>

CollisionMap::CollisionMap(int width, int height, int tile_size) :
    width(width), height(height), tile_size(tile_size), collision_tiles(width * height) {}

void CollisionMap::set_blocked(int tile_x, int tile_y, bool blocked) {
    if (tile_x >= 0 && tile_y >= 0 && tile_x < width && tile_y < height) {
        collision_tiles[width * tile_y + tile_x] = blocked;
    }
}
