#include <iostream>
#include <memory>
#include <string>

#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <imgui.h>
#include <imgui_impl_allegro5.h>
#include <imgui_stdlib.h>

#include <map.hpp>
#include <influence_map.hpp>

const int SCREEN_WIDTH = 1600;
const int SCREEN_HEIGHT = 1080;
const int FPS = 30;

enum class MouseEditMode {
    BLOCK_TILE,
    PLACE_INFLUENCE
};

int main(int argc, char** argv) {
    al_init();
    al_init_primitives_addon();
    al_init_image_addon();
    al_init_font_addon();
    al_install_keyboard();
    al_install_mouse();

    ALLEGRO_DISPLAY* display;
    ALLEGRO_TIMER* loop_timer;
    ALLEGRO_TIMER* influence_timer;
    ALLEGRO_EVENT_QUEUE* ev_queue;
    ALLEGRO_FONT* bitmap_font;

    display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    loop_timer = al_create_timer(1.0f / FPS);
    influence_timer = al_create_timer(0.1f);

    // setup blending for non-premultiplied alpha
    al_set_blender(ALLEGRO_ADD, ALLEGRO_ALPHA, ALLEGRO_INVERSE_ALPHA);

    ev_queue = al_create_event_queue();
    al_register_event_source(ev_queue, al_get_keyboard_event_source());
    al_register_event_source(ev_queue, al_get_mouse_event_source());
    al_register_event_source(ev_queue, al_get_display_event_source(display));
    al_register_event_source(ev_queue, al_get_timer_event_source(loop_timer));
    al_register_event_source(ev_queue, al_get_timer_event_source(influence_timer));

    // setup imgui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;

    ImGui::StyleColorsDark();
    ImGui_ImplAllegro5_Init(display);

    // setup fonts
    bitmap_font = al_create_builtin_font();

    // prepare main loop
    bool done = false;
    bool redraw = true;
    ALLEGRO_EVENT ev;
    ALLEGRO_MOUSE_STATE mouse;
    ALLEGRO_KEYBOARD_STATE keyboard;

    // demo variables
    int cam_x = 0;
    int cam_y = 0;
    int cam_x_vel = 0;
    int cam_y_vel = 0;
    bool map_exists = false;
    int tile_size = 64;
    int map_width = 50, map_height = 50;
    MouseEditMode mouse_edit_mode = MouseEditMode::BLOCK_TILE;

    std::unique_ptr<BaseMap> current_map = nullptr;

    std::string inf_map_name;
    float inf_map_strength = 5.0f, inf_map_decay = 0.5f, inf_map_momentum = 0.3f;
    std::shared_ptr<InfluenceMap> selected_inf_map = nullptr;
    std::vector<std::shared_ptr<InfluenceMap>> influence_maps;

    al_start_timer(loop_timer);
    al_start_timer(influence_timer);

    while (!done) {
        al_wait_for_event(ev_queue, &ev);
        ImGui_ImplAllegro5_ProcessEvent(&ev);

        if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
            done = true;
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_RESIZE) {
            ImGui_ImplAllegro5_InvalidateDeviceObjects();
            al_acknowledge_resize(display);
            ImGui_ImplAllegro5_CreateDeviceObjects();
        }
        else if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (ev.timer.source == loop_timer) {
                redraw = true;
            }
            else if (ev.timer.source == influence_timer) {
                for (auto& inf : influence_maps) {
                    //inf->print_values();
                    inf->recalculate();
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                    cam_y_vel = -5;
                    break;
                case ALLEGRO_KEY_DOWN:
                    cam_y_vel = 5;
                    break;
                case ALLEGRO_KEY_LEFT:
                    cam_x_vel = -5;
                    break;
                case ALLEGRO_KEY_RIGHT:
                    cam_x_vel = 5;
                    break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_UP) {
            switch (ev.keyboard.keycode) {
                case ALLEGRO_KEY_UP:
                case ALLEGRO_KEY_DOWN:
                    cam_y_vel = 0;
                    break;
                case ALLEGRO_KEY_RIGHT:
                case ALLEGRO_KEY_LEFT:
                    cam_x_vel = 0;
                    break;
            }
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
            if (mouse_edit_mode == MouseEditMode::PLACE_INFLUENCE && !io.WantCaptureMouse) {
                int tile_x = (cam_x + ev.mouse.x) / tile_size;
                int tile_y = (cam_y + ev.mouse.y) / tile_size;

                if (ev.mouse.button == 1) {
                    if (selected_inf_map != nullptr) {
                        
                        selected_inf_map->add_influence(tile_x, tile_y);
                        std::cout << "Created influence at (" << tile_x << ", " << tile_y << ")" << std::endl;
                    }
                }
                else if (ev.mouse.button == 2) {
                    if (selected_inf_map != nullptr) {
                        selected_inf_map->remove_influence(tile_x, tile_y);
                        std::cout << "Removed influence at (" << tile_x << ", " << tile_y << ")" << std::endl;
                    }
                }
            }
        }

        // logic for drawing on map
        if (mouse_edit_mode == MouseEditMode::BLOCK_TILE && !io.WantCaptureMouse) {
            if (current_map != nullptr) {
                al_get_mouse_state(&mouse);

                if (mouse.buttons & 1) {
                    int tile_x = (cam_x + mouse.x) / tile_size;
                    int tile_y = (cam_y + mouse.y) / tile_size;

                    current_map->set_blocked(tile_x, tile_y, true);
                }
                else if (mouse.buttons & 2) {
                    int tile_x = (cam_x + mouse.x) / tile_size;
                    int tile_y = (cam_y + mouse.y) / tile_size;

                    current_map->set_blocked(tile_x, tile_y, false);
                }
            }
        }

        if (redraw) {
            redraw = false;

            // move the camera
            cam_x += cam_x_vel;
            cam_y += cam_y_vel;

            // UI logic
            ImGui_ImplAllegro5_NewFrame();
            ImGui::NewFrame();

            ImGui::SetNextWindowPos(ImVec2(SCREEN_WIDTH - 300, 50));
            ImGui::Begin("Editor controls");
            ImGui::Text("Mouse mode (left/right click)");

            if (ImGui::RadioButton("Add/remove block tiles", mouse_edit_mode == MouseEditMode::BLOCK_TILE)) {
                mouse_edit_mode = MouseEditMode::BLOCK_TILE;
            }
            if (ImGui::RadioButton("Add/remove influence", mouse_edit_mode == MouseEditMode::PLACE_INFLUENCE)) {
                mouse_edit_mode = MouseEditMode::PLACE_INFLUENCE;
            }

            ImGui::End();

            ImGui::Begin("Map info");

            if (current_map != nullptr) {
                ImGui::Text("Tile size: %d", tile_size);
                ImGui::Text("Map width: %d", map_width);
                ImGui::Text("Map height: %d", map_height);

                if (ImGui::Button("Delete Map")) {
                    current_map.reset(nullptr);
                    influence_maps.clear();
                }
            }
            else {
                ImGui::Text("No map created, create one first");
                ImGui::Separator();
                ImGui::InputInt("Tile size", &tile_size);
                ImGui::InputInt("Map width", &map_width);
                ImGui::InputInt("Map height", &map_height);
                if (ImGui::Button("Create Map")) {
                    current_map = std::make_unique<BaseMap>(map_width, map_height, tile_size);
                    std::cout << "Created map with tile_size=" << tile_size <<
                        " and dimensions=(" << map_width << ", " << map_height << ")" << std::endl;
                    ImGui::SetNextWindowPos(ImVec2(60, 250));
                }
            }

            ImGui::End();

            if (current_map != nullptr) {
                ImGui::Begin("Influence maps");
                ImGui::InputText("Name", &inf_map_name);
                ImGui::SliderFloat("Strength", &inf_map_strength, 1.0f, 50.0f);
                ImGui::SliderFloat("Decay", &inf_map_decay, 0.0f, 1.0f);
                ImGui::SliderFloat("Momentum", &inf_map_momentum, 0.0f, 1.0f);
                if (ImGui::Button("Create Influence")) {
                    if (inf_map_name.size() > 0) {
                        // TODO allow customizing strength, decay, momentum
                        influence_maps.emplace_back(std::make_shared<InfluenceMap>(inf_map_name, map_width, map_height, tile_size, inf_map_strength, inf_map_decay, inf_map_momentum));
                    }
                }

                ImGui::Separator();
                ImGui::Text("Influence list");
                if (ImGui::ListBoxHeader("", influence_maps.size(), 10)) {
                    for (const auto& i : influence_maps) {
                        if (ImGui::Selectable(i->get_name().c_str(), i == selected_inf_map)) {
                            selected_inf_map = i;
                        }
                    }
                    ImGui::ListBoxFooter();
                }
                ImGui::End();
            }

            ImGui::Render();
            al_clear_to_color(al_map_rgb(0, 0, 0));

            // render the base map
            if (current_map != nullptr) {
                current_map->draw(cam_x, cam_y, cam_x + SCREEN_WIDTH, cam_y + SCREEN_HEIGHT);
            }

            // render all influence maps
            for (const auto& i : influence_maps) {
                i->draw(cam_x, cam_y, cam_x + SCREEN_WIDTH, cam_y + SCREEN_HEIGHT);
            }

            // render GUI
            ImGui_ImplAllegro5_RenderDrawData(ImGui::GetDrawData());


            al_flip_display();
        }
    }

    ImGui_ImplAllegro5_Shutdown();
    ImGui::DestroyContext();
    al_destroy_font(bitmap_font);
    al_destroy_timer(loop_timer);
    al_destroy_timer(influence_timer);
    al_destroy_event_queue(ev_queue);
    al_destroy_display(display);

    return 0;
}
