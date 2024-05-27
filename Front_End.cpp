/* 
 * Copyright 2023 University of Toronto
 *
 * Permission is hereby granted, to use this software and associated 
 * documentation files (the "Software") in course work at the University 
 * of Toronto, or for personal use. Other uses are prohibited, in 
 * particular the distribution of the Software either publicly or to third 
 * parties.
 *
 * The above copyright notice and this permission notice shall be included in 
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include "ezgl/application.hpp"
#include "ezgl/graphics.hpp"


#include <cmath>
#include <vector>
#include <string>
#include <map>
#include <set>

#include <sstream>
#include <bits/stdc++.h>
#include <chrono>

/*  
 * Global Variables:
 * 
 * The following variables and data structures are used to preprocess and reduce complexity
 * Most of the data structures are loaded in m1 when loading map
 */

struct Map_UI {
    int zoom_level;
    int scale;
};

Map_UI my_map;
extern std::vector <std::string> sel_menu;
std::string map_name;

extern std::set<std::string> processed_street_names;
extern std::unordered_map <std::string, std::vector<StreetIdx>> name_and_id;
extern std::vector <ezgl::point2d> subway_node;
extern std::vector <ezgl::point2d> traffic_signal_point;
extern std::vector <ezgl::point2d> stop_sign_points;
extern std::vector <ezgl::point2d> subwayline_points;

extern std::vector <Street_Segment_Data> Street_Segments;
extern std::unordered_map <OSMID, const OSMNode*> OSMnode_map;
extern std::vector<std::vector<StreetSegmentIdx>>street_segment_of_intersection;
extern std::vector <Street_Data> Streets;

extern std::vector <Intersection_Data> Intersections;
extern std::vector <Feature_Data> Features;
extern std::vector<POI_Data> POIs;

template <class to_be_print>
void print(to_be_print a) {
    std::cout << a << std::endl;
}

extern double max_lat;
extern double min_lat;
extern double max_lon;
extern double min_lon;
extern double lat_ave;

extern float x_from_lon(float lon);
extern float y_from_lat(float lat);
extern float lat_from_y(float y);
extern float lon_from_x(float x);


//return duration
auto time_test(std::chrono::high_resolution_clock::time_point start);
std::chrono::high_resolution_clock::time_point time_mark();
void duration(std::chrono::high_resolution_clock::time_point start);
void duration(std::chrono::high_resolution_clock::time_point start, std::string message);
//set up different zoom levels
void zoom_level_definer(double bound_distance);
//find the middle pt between two xy points
ezgl::point2d calculate_mid_coordinate_LatLon_to_Pt2d(double first_coor_x, double first_coor_y, double second_coor_x, double second_coor_y);
//create find intersection of two street button 
double calculate_angle(double x1, double y1, double x2, double y2);
//create find intersection of two street button 
void initial_setup(ezgl::application* application, bool /*new window*/);
//create buttons for street searching
void street_button(ezgl::application* application, bool /*new window*/);

std::string info;
void combo_box_cbk(GtkComboBoxText* self, ezgl::application* app);

std::vector <StreetSegmentIdx> path;
std::pair<IntersectionIdx, IntersectionIdx> intersect_ids;

bool night = false;
bool finish = false;

int count_intersections = 0;
/*  
 * The following functions are used to draw and display features on map
 * 
 */
void drawMap();
void draw_main_canvas(ezgl::renderer *g);
//
void street_name1_text(GtkWidget* /*widget*/, ezgl::application* application);
//
void street_name2_text(GtkWidget* /*widget*/, ezgl::application* application);
void draw_intersections(ezgl::renderer *g);
void highlight_intersection_when_click(ezgl::application* app, GdkEventButton* event, double x, double y);
void draw_features(ezgl::renderer *g);
void draw_POIs(ezgl::renderer *g);
void draw_Street(ezgl::renderer *g);
void display_street_name(int street_seg_id, ezgl::renderer *g);
void display_onewaystreet_direction(int street_seg_id, ezgl::renderer *g);

//night mode
void toggle_night(GtkWidget* /*widget*/, ezgl::application* application);
//show distance scale on map
void show_scale(ezgl::renderer *g);
void draw_street_name(ezgl::renderer *g);

/*  
 * The following functions are helper functions that draw features on map. These functions
 * are call inside the functions above to reduce redundant code
 * 
 */

//draw complete street segement
void draw_street_helper(int street_seg_id, ezgl::renderer *g);
//draw each feature
void draw_feature_helper(int feature_id, ezgl::renderer *g);
//draw subway lines
void draw_subway_line(ezgl::renderer *g);
//draw stations
void draw_subway_station(ezgl::renderer *g);
//display stop signs
void draw_stop_sign(ezgl::renderer *g);
//display traffic lights
void draw_traffic_signal_point(ezgl::renderer *g);
//function to clear all highlighted intersections


/*  
 * The following functions are callback functions that respond to any button pressed by the users
 * 
 */
void clear_intersections(GtkWidget* /*widget*/, ezgl::application* application);
void find_intersection_of_2streets(std::string partial_street1, std::string partial_street2, ezgl::application* application);
void key_pressed(ezgl::application *application, GdkEventKey */*event*/, char *key_name);
void draw_path(GtkWidget* /*widget*/, ezgl::application* application);
void create_instruction(GtkWidget* /*widget*/, ezgl::application* app);

//A instruction to guide users how to use the software

void create_instruction(GtkWidget* /*widget*/, ezgl::application* app) {
    app->create_popup_message("INSTRUCTION", "Up, Down, Right Left Arrow: control map to move in desired direction by clicking\n"
            "\n"
            "Zoom In, Out, Fit: zoom in, out to control the zoom level of the map, zoom fit to go back to zoom level 1 by clicking\n"
            "\n"
            "Drop Down Menu: select the desired country to look at the map by clicking the drop box\n"
            "\n"
            "Find: clicking find button to check the intersection\n"
            "\n"
            "Night Mode: click to switch to night mode, click again to switch back to day mode\n"
            "\n"
            "Click the square boxes representing intersections to highlight and get location of intersections\n"
            "\n"
            "Clear Intersection: clear all the highlighted intersections\n"
            "\n"
            "Proceed: end map\n"
            "\n"
            "Type in starting street in street 1 and destination street in street 2 to get intersections between two streets by pressing enter, click intersection to select starting and ending point\n");
}

//The function is used to detect "Enter" key pressed by the user, and get the street name typed in the research bar

void key_pressed(ezgl::application *application, GdkEventKey */*event*/, char *key_name) {
    if (strcmp(key_name, "Return") == 0) {
        //get the GTK object using the text id
        GtkEntry *street_1_entry = GTK_ENTRY(application->get_object("streetName1"));
        const gchar* street_1_input = gtk_entry_get_text(street_1_entry);
        gtk_entry_set_width_chars(street_1_entry, 6);
        // convert Gchar to string
        std::string street_1_string = std::string(g_strdup(street_1_input));

        //get the GTK object using the text id
        GtkEntry *street_2_entry = GTK_ENTRY(application->get_object("streetName2"));
        const gchar* street_2_input = gtk_entry_get_text(street_2_entry);
        // convert Gchar to string
        std::string street_2_string = std::string(g_strdup(street_2_input));
        //std::cout << street_2_string << "\n";


        std::cout << street_1_input << street_2_input << std::endl;
        find_intersection_of_2streets(street_1_input, street_2_input, application);
    }
}

void find_intersection_of_2streets(std::string partial_street1, std::string partial_street2, ezgl::application* application) {
    bool find = false;

    std::vector<StreetIdx> street1_ids = findStreetIdsFromPartialStreetName(partial_street1); //find and store the street name 1 input by user
    std::vector<StreetIdx> street2_ids = findStreetIdsFromPartialStreetName(partial_street2); //find and store the street name 2 input by user

    //find intersections between two streets, and set the boolean to true to display that permanently
    for (auto it1 = street1_ids.begin(); it1 < street1_ids.end(); it1++) {
        for (auto it2 = street2_ids.begin(); it2 < street2_ids.end(); it2++) {
            std::vector<IntersectionIdx> common = findIntersectionsOfTwoStreets(*it1, *it2);
            for (auto it3 = common.begin(); it3 < common.end(); it3++) {
                find = true;
                Intersections[*it3].two_street = true;

            }
        }
    }


    //use popup message to show user that the intersections are found
    std::ostringstream oss;
    if (find == true) {
        oss << "The intersections between " << partial_street1 << " and " << partial_street2 << " are highlighted, click to select your starting point/destination" << std::endl;
    } else {
        oss << "The intersections between " << partial_street1 << " and " << partial_street2 << " are not found" << std::endl;
    }

    std::string var = oss.str();
    char* arr = new char[var.length() + 1];
    strcpy(arr, var.c_str());

    application->refresh_drawing();
    //display intersection information of two streets at status bar
    application->create_popup_message("Intersections Between Two Streets", arr);
    delete[] arr;
    arr = nullptr;
}

//clear all the highlighted intersections

void clear_intersections(GtkWidget* /*widget*/, ezgl::application* application) {
    count_intersections = 0;
    for (int intersect_id = 0; intersect_id < Intersections.size(); intersect_id++) {
        Intersections[intersect_id].highlighted = false;
        Intersections[intersect_id].two_street = false;
    }
    for (int street_seg = 0; street_seg < getNumStreetSegments(); street_seg++) {
        Street_Segments[street_seg].drawn = false;
        Street_Segments[street_seg].flipped = false;
    }
    application->refresh_drawing();
}

/*  
 * The following functions are used to set up buttons on the menu bar.
 * 
 */
void initial_setup(ezgl::application* application, bool /*new window*/) {
    application->create_combo_box_text("box", 6, combo_box_cbk, sel_menu);
    //application->create_button("Find", 7, find_intersection_of_2streets);
    application->create_button("Night Mode", 8, toggle_night);
    application->create_button("Clear Intersections", 9, clear_intersections);
    application->create_button("Show Path", 10, draw_path);
    application->create_button("Instruction", 15, create_instruction);
}


//the function draw path between selected intersections

void draw_path(GtkWidget* /*widget*/, ezgl::application* application) {

    if (path.size() == 0) {
        application->create_popup_message("No path found", "Re-select your starting point and destination");
        return;
    }

    ezgl::renderer* g = application->get_renderer();

    //set variable for staring and ending point
    IntersectionIdx temp_from, temp_to;
    std::vector<ezgl::point2d> arrow; //direction arrow
    bool flip = false; //if from and to should be flip

    temp_from = intersect_ids.first;
    temp_to = intersect_ids.first;

    if (finish == false) {
        for (auto it = path.begin(); it < path.end(); it++) {
            finish = true;

            if (Street_Segments[*it].from == temp_to) {
                temp_from = Street_Segments[*it].from;
                temp_to = Street_Segments[*it].to;
                flip = false;
            } else if (Street_Segments[*it].to == temp_to) {
                temp_from = Street_Segments[*it].to;
                temp_to = Street_Segments[*it].from;
                Street_Segments[*it].flipped = true;
                flip = true;
            }


            g->set_line_width(5);
            g->set_color(151, 203, 235);
            double x1 = 0, x2 = 0, y1 = 0, y2 = 0, temp_x = 0, temp_y = 0, x3 = 0, y3 = 0, x4 = 0, y4 = 0;

            double angle;
            int i;
            //draw lines between curve points
            for (i = 0; i < Street_Segments[*it].Curve_Points_list.size() - 1; i++) {
                ezgl::point2d point1 = Street_Segments[*it].Curve_Points_list[i];
                ezgl::point2d point2 = Street_Segments[*it].Curve_Points_list[i + 1];
                g->draw_line(point1, point2);
            }

            //if from and two is not flipped
            if (flip == false) {
                if (Street_Segments[*it].Curve_Points_list.size() == 0) {
                    x1 = Street_Segments[*it].Curve_Points_list[i].x;
                    y1 = Street_Segments[*it].Curve_Points_list[i].y;
                } else {
                    x1 = Intersections[temp_from].xy_loc.x;
                    y1 = Intersections[temp_from].xy_loc.y;
                }

                x2 = Intersections[temp_to].xy_loc.x;
                y2 = Intersections[temp_to].xy_loc.y;

                //if from and two is flipped
            } else if (flip == true) {
                if (Street_Segments[*it].Curve_Points_list.size() == 0) {
                    x1 = Street_Segments[*it].Curve_Points_list[0].x;
                    y1 = Street_Segments[*it].Curve_Points_list[0].y;
                } else {
                    x1 = Intersections[temp_from].xy_loc.x;
                    y1 = Intersections[temp_from].xy_loc.y;
                }

                x2 = Intersections[temp_to].xy_loc.x;
                y2 = Intersections[temp_to].xy_loc.y;

            }
            g->set_text_rotation(0);
            angle = calculate_angle(x1, y1, x2, y2) * kDegreeToRadian;

            temp_x = x2 - 5 * cos(angle);
            temp_y = y2 - 5 * sin(angle);
            if (angle >= 0 && angle <= M_PI / 2) {
                x4 = temp_x + 3 * cos(M_PI / 2 - angle);
                y4 = temp_y - 3 * sin(M_PI / 2 - angle);
                x3 = temp_x - 3 * cos(M_PI / 2 - angle);
                y3 = temp_y + 3 * sin(M_PI / 2 - angle);

            } else if (angle >= M_PI / 2 && angle <= M_PI) {
                x4 = temp_x + 3 * cos(angle - M_PI / 2);
                y4 = temp_y + 3 * sin(angle - M_PI / 2);
                x3 = temp_x - 3 * cos(angle - M_PI / 2);
                y3 = temp_y - 3 * sin(angle - M_PI / 2);
            } else if (angle >= M_PI && angle <= 3 * M_PI / 2) {
                x4 = temp_x + 3 * cos(3 * M_PI / 2 - angle);
                y4 = temp_y - 3 * sin(3 * M_PI / 2 - angle);
                x3 = temp_x - 3 * cos(3 * M_PI / 2 - angle);
                y3 = temp_y + 3 * sin(3 * M_PI / 2 - angle);
            } else if (angle >= 3 * M_PI / 2 && angle <= 2 * M_PI) {
                x4 = temp_x + 3 * cos(2 * M_PI - angle);
                y4 = temp_y + 3 * sin(2 * M_PI - angle);
                x3 = temp_x - 3 * cos(2 * M_PI - angle);
                y3 = temp_y - 3 * sin(2 * M_PI - angle);
            }

            //store coordinates of arrows
            arrow.push_back({x2, y2});
            arrow.push_back({x3, y3});
            arrow.push_back({x4, y4});

            g->set_color(151, 203, 235);
            g->fill_poly(arrow);
            arrow.clear();

            application->flush_drawing();
            if (it != path.end() - 1) {

                //store navigation information
                std::ostringstream oss;
                oss << "Take " << Streets[Street_Segments[*(it + 1)].streetID].street_name << " at next Intersection";
                std::string var = oss.str();
                char* arr = new char[var.length() + 1];
                strcpy(arr, var.c_str());

                //show navigation in text at the status bar
                application->update_message(arr);
                delete[] arr;
                arr = nullptr;
            } else {
                finish = false;
            }

            // Add some delay
            std::chrono::milliseconds duration(500);
            std::this_thread::sleep_for(duration);

            Street_Segments[*it].drawn = true;
            application->refresh_drawing();

        }
    }

}

//the intersection is highlighted in yellow when clicked by the user

void highlight_intersection_when_click(ezgl::application* app, GdkEventButton* /*event*/, double x, double y) {

    //show highlighted intersection at zoom level of 8
    if (my_map.zoom_level > 8) {
        std::cout << "Mouse clicked at (" << x << "," << y << ")\n";

        LatLon position = LatLon(lat_from_y(y), lon_from_x(x));
        int intersect_id = findClosestIntersection(position);
        Intersections[intersect_id].highlighted = !Intersections[intersect_id].highlighted;

        std::ostringstream oss;
        //highlight
        if (Intersections[intersect_id].highlighted == true) {
            std::cout << intersect_id << std::endl;
            oss << "Intersection Selected: " << Intersections[intersect_id].name;
            count_intersections++;
            if (count_intersections == 1) {
                //store first = first intersection id
                intersect_ids.first = intersect_id;
            } else if (count_intersections == 2) {
                //store second = second intersection id
                intersect_ids.second = intersect_id;
            }
        } else { //no more should be stored
            count_intersections--;
        }

        std::string var = oss.str();
        app->refresh_drawing();

        //show intersection information at message bar
        app->update_message(var);
    }


    if (count_intersections == 2) {
        ////call the function to calculate route
        path = findPathBetweenIntersections(intersect_ids, 15);

        for (auto it : path) {
            std::cout << "calculated path" << it << std::endl;
        }


    } else if (count_intersections > 2) {
        //display error message
        app->create_popup_message("There are more than 2 intersections are selected", "Click 'Clear Intersections' to reset and re-select");
    } else {
        for (int street_seg = 0; street_seg < getNumStreetSegments(); street_seg++) {
            Street_Segments[street_seg].drawn = false;
            Street_Segments[street_seg].flipped = false;
        }
    }

}

void drawMap() {

    ezgl::application::settings settings;
    settings.main_ui_resource = "libstreetmap/resources/main.ui";
    settings.window_identifier = "MainWindow";
    settings.canvas_identifier = "MainCanvas";


    ezgl::application application(settings);
    ezgl::rectangle initial_world({x_from_lon(min_lon), y_from_lat(min_lat)},
    {
        x_from_lon(max_lon), y_from_lat(max_lat)
    });
    application.add_canvas("MainCanvas", draw_main_canvas, initial_world);

    //enable the mouse_click and button events
    application.run(initial_setup, highlight_intersection_when_click, nullptr, key_pressed);

}

void draw_main_canvas(ezgl::renderer *g) {

    //check drawing time
    auto startTime = time_mark();
    double current_bound = g->get_visible_world().right() - g->get_visible_world().left();
    zoom_level_definer(current_bound);
    std::cout << "Current Zoom Level is: " << my_map.zoom_level << std::endl;

    //change night mode
    if (night == true) {
        g->set_color(ezgl::BLACK, 100);
        g->fill_rectangle(g->get_visible_world());
    } else {
        g->set_color(231, 234, 236);
        g->fill_rectangle(g->get_visible_world());
    }

    //Focus on limiting these three functions by using zoom_level
    draw_features(g);
    draw_POIs(g);
    draw_Street(g);
    draw_intersections(g);
    draw_street_name(g);
    //draw_subway_line(g);
    //draw_subway_station(g);
    //draw_stop_sign(g);
    //draw_traffic_signal_point(g);
    show_scale(g);
    duration(startTime, "re_draw"); //print the redraw time
}

void show_scale(ezgl::renderer *g) {
    //change to screen coordinate
    g->set_coordinate_system(ezgl::SCREEN);

    //draw scale line
    g->set_color(ezgl::BLACK);
    g->set_line_dash(ezgl::line_dash::asymmetric_5_3);
    g->set_line_width(5);
    g->draw_line({g->get_visible_screen().right() - 130, g->get_visible_screen().top() - 20},
    {
        g->get_visible_screen().right() - 60, g->get_visible_screen().top() - 20
    });

    //calculate scale
    double world_width = g->get_visible_world().right() - g->get_visible_world().left();
    double screen_width = g->get_visible_screen().right() - g->get_visible_screen().left();
    int scale = (world_width / screen_width)*70;

    //displaying scale on canvas
    std::ostringstream oss;
    oss << scale << " m";
    std::string var = oss.str();
    char* arr = new char[var.length() + 1];
    strcpy(arr, var.c_str());
    g->set_text_rotation(0);
    g->draw_text({g->get_visible_screen().right() - 95, g->get_visible_screen().top() - 10}, arr);
    delete[] arr;
    arr = nullptr;
    g->set_coordinate_system(ezgl::WORLD);

}

void draw_subway_line(ezgl::renderer *g) {
    //go through all the subwaylines and draw the correspond subway in a city
    for (int i = 0; i < subwayline_points.size(); i++) {
        if ((abs(subwayline_points[i].x - subwayline_points[i + 1].x) < 6000 && abs(subwayline_points[i].y - subwayline_points[i + 1].y) < 1000) || (abs(subwayline_points[i].y - subwayline_points[i + 1].y) < 3000 && abs(subwayline_points[i].x - subwayline_points[i + 1].x) < 750)) {
            g->set_color(255, 192, 160);
            g->set_line_width(2);
            g->draw_line(subwayline_points[i], subwayline_points[i + 1]);
        }
    }
}

//the subway stations are shown at zoom level 8

void draw_subway_station(ezgl::renderer *g) {
    //go through subway nodes and draw the subway stations in a city
    for (int i = 0; i < subway_node.size(); i++) {
        if (my_map.zoom_level > 7) {
            if (subway_node[i].x > g->get_visible_world().left() && subway_node[i].x < g->get_visible_world().right()) {
                if (subway_node[i].y > g->get_visible_world().bottom() && subway_node[i].y < g->get_visible_world().top()) {
                    ezgl::surface* subway = ezgl::renderer::load_png("subway.png");
                    g->draw_surface(subway,{subway_node[i].x, subway_node[i].y});
                }
            }
        }
    }
}

//stop signs are shown at zoom level 12

void draw_stop_sign(ezgl::renderer *g) {
    //loop through all the stop signs and draw the stop signs in canvas
    for (int i = 0; i < stop_sign_points.size(); i++) {
        if (my_map.zoom_level == 12) {
            if (stop_sign_points[i].x > g->get_visible_world().left() && stop_sign_points[i].x < g->get_visible_world().right()) {
                if (stop_sign_points[i].y > g->get_visible_world().bottom() && stop_sign_points[i].y < g->get_visible_world().top()) {
                    ezgl::surface* stop = ezgl::renderer::load_png("stop.png");
                    g->draw_surface(stop,{stop_sign_points[i].x, stop_sign_points[i].y});
                }
            }
        }
    }

}

//traffic lights are shown at level 12

void draw_traffic_signal_point(ezgl::renderer *g) {
    //loop through all the stop signs and draw the traffic lights in canvas
    for (int i = 0; i < traffic_signal_point.size(); i++) {
        if (my_map.zoom_level == 12) {
            if (traffic_signal_point[i].x > g->get_visible_world().left() && traffic_signal_point[i].x < g->get_visible_world().right()) {
                if (traffic_signal_point[i].y > g->get_visible_world().bottom() && traffic_signal_point[i].y < g->get_visible_world().top()) {
                    ezgl::surface* light = ezgl::renderer::load_png("trafic_light.png");
                    g->draw_surface(light,{traffic_signal_point[i].x, traffic_signal_point[i].y});
                }
            }
        }
    }
}

void draw_intersections(ezgl::renderer *g) {
    for (int intersect_id = 0; intersect_id < Intersections.size(); intersect_id++) {
        float width = 0;

        //highlight the intersection between two streets
        if (Intersections[intersect_id].two_street == true) {
            g->set_color(ezgl::PINK, 100);
            width = 1000 / (my_map.zoom_level);

            float height = width;
            Intersections[intersect_id].xy_loc.x = x_from_lon(Intersections[intersect_id].position.longitude());
            Intersections[intersect_id].xy_loc.y = y_from_lat(Intersections[intersect_id].position.latitude());
            float x = Intersections[intersect_id].xy_loc.x - width / 2;
            float y = Intersections[intersect_id].xy_loc.y - height / 2;

            g->fill_rectangle({x, y}, width, height);

        }

        //enable select intersections and draw all intersections at level 8
        if (my_map.zoom_level > 8) {
            if (Intersections[intersect_id].highlighted == true) {
                g->set_color(ezgl::YELLOW);
                width = 100 / (my_map.zoom_level);
            } else {
                g->set_color(ezgl::GREY_75);
                width = 4;
            }


            //find the position of intersection and draw
            float height = width;
            Intersections[intersect_id].xy_loc.x = x_from_lon(Intersections[intersect_id].position.longitude());
            Intersections[intersect_id].xy_loc.y = y_from_lat(Intersections[intersect_id].position.latitude());
            float x = Intersections[intersect_id].xy_loc.x - width / 2;
            float y = Intersections[intersect_id].xy_loc.y - height / 2;

            g->fill_rectangle({x, y}, width, height);
        }
    }
}

//POI points will be shown at level 11, and names are displayed at level 12

void draw_POIs(ezgl::renderer *g) {
    float width = 2;
    float height = width;
    g->set_color(ezgl::BLACK);
    for (int POI_id = 0; POI_id < getNumPointsOfInterest() && my_map.zoom_level > 10; POI_id++) {

        //check the bound of POI and only draw the POIs that are in bound
        if (POIs[POI_id].POI_point.x > g->get_visible_world().left() && POIs[POI_id].POI_point.x < g->get_visible_world().right()) {
            if (POIs[POI_id].POI_point.y > g->get_visible_world().bottom() && POIs[POI_id].POI_point.y < g->get_visible_world().top()) {

                float x = POIs[POI_id].POI_point.x;
                float y = POIs[POI_id].POI_point.y;
                g->fill_rectangle({x, y}, width, height);


                if (my_map.zoom_level > 11) {
                    g->set_color(ezgl::BLACK);
                    g->set_text_rotation(0);
                    g->draw_text({x, y + 4}, POIs[POI_id].name);
                }
            }
        }

    }
}

//draw the feature of a specific feature id

void draw_feature_helper(int feature_id, ezgl::renderer *g) {

    for (int feature_points = 0; feature_points < Features[feature_id].num_points - 1; feature_points++) {
        if (!Features[feature_id].is_shape || Features[feature_id].Feature_Points_list.size() < 1) {
            g->draw_line(Features[feature_id].Feature_Points_list[feature_points], Features[feature_id].Feature_Points_list[feature_points + 1]);
        }
    }

    //fill the bound if the feature has any area
    if (Features[feature_id].is_shape && Features[feature_id].Feature_Points_list.size() > 1) {
        g->fill_poly(Features[feature_id].Feature_Points_list);
    }
}

//draw all the features

void draw_features(ezgl::renderer *g) {

    //set different color for different feature types and display them at different zoom level
    for (int feature_id = 0; feature_id < Features.size(); feature_id++) {

        if (Features[feature_id].type == 10) {
            //GLACIER
            g->set_color(245, 240, 228);
            draw_feature_helper(feature_id, g);
        }

        if (my_map.zoom_level > 5) {
            if (Features[feature_id].type == 9) {
                //STREAM
                g->set_color(152, 188, 248);
                draw_feature_helper(feature_id, g);
            }
        }

        if (Features[feature_id].type == 8) {
            //GOLFCOURSE

            g->set_color(147, 211, 164);
            draw_feature_helper(feature_id, g);
        }

        if (Features[feature_id].type == 7) {
            //GREENSPACE
            g->set_color(207, 234, 215);
            draw_feature_helper(feature_id, g);
        }

        if (my_map.zoom_level > 6) {
            if (Features[feature_id].type == 6) {
                //BUILDING
                g->set_color(201, 201, 198);
                draw_feature_helper(feature_id, g);
            }
        }

        if (Features[feature_id].type == 5) {
            //ISLAND
            g->set_color(207, 234, 215);
            draw_feature_helper(feature_id, g);
        }

        if (Features[feature_id].type == 4) {
            //RIVER
            g->set_color(152, 188, 248);
            draw_feature_helper(feature_id, g);
        }

        if (Features[feature_id].type == 3) {
            //LAKE
            g->set_color(152, 188, 248);
            draw_feature_helper(feature_id, g);
        }

        if (Features[feature_id].type == 2) {
            //BEACH
            g->set_color(254, 239, 195);
            draw_feature_helper(feature_id, g);
        }

        if (Features[feature_id].type == 1) {
            //PARK
            g->set_color(167, 218, 181);
            draw_feature_helper(feature_id, g);
        }

        if (Features[feature_id].type == 0) {
            g->set_color(ezgl::WHITE);
            draw_feature_helper(feature_id, g);
        }
    }
}

//draw street names

void draw_street_name(ezgl::renderer *g) {
    //for each street_segments
    for (int street_seg_id = 0; street_seg_id < getNumStreetSegments(); street_seg_id++) {
        if (my_map.zoom_level >= 10) {
            display_onewaystreet_direction(street_seg_id, g);

            //check the bound of the canvas, display street name if the street segment is in bound
            if (Intersections[Street_Segments[street_seg_id].from].xy_loc.x > g->get_visible_world().left() &&
                    Intersections[Street_Segments[street_seg_id].from].xy_loc.x < g->get_visible_world().right()) {
                if (Intersections[Street_Segments[street_seg_id].from].xy_loc.y > g->get_visible_world().bottom() &&
                        Intersections[Street_Segments[street_seg_id].from].xy_loc.y < g->get_visible_world().top()) {
                    if (Streets[Street_Segments[street_seg_id].streetID].name_displayed != true) {
                        display_street_name(street_seg_id, g);
                    }
                }
            }
        }
    }
}

//draw all the street at different level

void draw_Street(ezgl::renderer *g) {

    //calculate the relative ratio of street segment to the bound
    double original_bound = x_from_lon(max_lon) - x_from_lon(min_lon);
    double current_bound = g->get_visible_world().right() - g->get_visible_world().left();
    double ratio = (original_bound / current_bound) / my_map.zoom_level;


    g->set_line_cap(ezgl::line_cap::round);
    //for each street_segments, determine if the road is a major road or a minor road and draw them at different level
    for (int street_seg_id = 0; street_seg_id < getNumStreetSegments(); street_seg_id++) {

        if (my_map.zoom_level <= 3) {
            if (Street_Segments[street_seg_id].speedLimit >= 20) {
                g->set_line_width(5 * ratio);
                g->set_color(245, 218, 156);
                draw_street_helper(street_seg_id, g);
            }
        } else if (my_map.zoom_level < 6) {
            if (Street_Segments[street_seg_id].speedLimit < 20 && Street_Segments[street_seg_id].speedLimit >= 13) {
                g->set_line_width(3 * ratio);
                g->set_color(245, 247, 246);
                draw_street_helper(street_seg_id, g);
            } else if (Street_Segments[street_seg_id].speedLimit >= 20) {
                g->set_line_width(5 * ratio);
                g->set_color(245, 218, 156);
                draw_street_helper(street_seg_id, g);
            }

        } else if (my_map.zoom_level < 9) {
            if (Street_Segments[street_seg_id].speedLimit < 13) {
                g->set_line_width(1 * ratio);
                g->set_color(ezgl::WHITE);
            } else if (Street_Segments[street_seg_id].speedLimit < 20) {
                g->set_line_width(2 * ratio);
                g->set_color(245, 247, 246);

            } else {
                g->set_line_width(5 * ratio);
                g->set_color(245, 218, 156);

            }

            //draw_street_helper is a much ez way to draw a full street_segment, with or without curvepoints
            draw_street_helper(street_seg_id, g);

        } else {
            if (Street_Segments[street_seg_id].speedLimit < 13) {
                g->set_line_width(0.3 * ratio);
                g->set_color(ezgl::WHITE);
            } else if (Street_Segments[street_seg_id].speedLimit < 20) {
                g->set_line_width(1 * ratio);
                g->set_color(245, 247, 246);

            } else {
                g->set_line_width(5 * ratio);
                g->set_color(245, 218, 156);

            }

            //draw_street_helper is a much ez way to draw a full street_segment, with or without curvepoints
            draw_street_helper(street_seg_id, g);

        }
    }

    for (int street_seg_id = 0; street_seg_id < getNumStreetSegments(); street_seg_id++) {

        if (Street_Segments[street_seg_id].drawn == true) {
            std::vector<ezgl::point2d> arrow; //direction arrow
            double x1 = 0, x2 = 0, y1 = 0, y2 = 0, temp_x = 0, temp_y = 0, x3 = 0, y3 = 0, x4 = 0, y4 = 0;
            double angle;
            IntersectionIdx temp_from = Street_Segments[street_seg_id].from;
            IntersectionIdx temp_to = Street_Segments[street_seg_id].to;

            if (Street_Segments[street_seg_id].flipped == true) {
                temp_to = Street_Segments[street_seg_id].from;
                temp_from = Street_Segments[street_seg_id].to;
            }


            g->set_line_width(5);
            g->set_color(151, 203, 235);
            int i;
            //draw lines between curve points
            for (i = 0; i < Street_Segments[street_seg_id].Curve_Points_list.size() - 1; i++) {
                ezgl::point2d point1 = Street_Segments[street_seg_id].Curve_Points_list[i];
                ezgl::point2d point2 = Street_Segments[street_seg_id].Curve_Points_list[i + 1];
                g->draw_line(point1, point2);
            }

            if (Street_Segments[street_seg_id].flipped == false) {
                if (Street_Segments[street_seg_id].Curve_Points_list.size() == 0) {
                    x1 = Street_Segments[street_seg_id].Curve_Points_list[i].x;
                    y1 = Street_Segments[street_seg_id].Curve_Points_list[i].y;
                } else {
                    x1 = Intersections[temp_from].xy_loc.x;
                    y1 = Intersections[temp_from].xy_loc.y;
                }

                x2 = Intersections[temp_to].xy_loc.x;
                y2 = Intersections[temp_to].xy_loc.y;

                //if from and two is flipped
            } else if (Street_Segments[street_seg_id].flipped == true) {
                if (Street_Segments[street_seg_id].Curve_Points_list.size() == 0) {
                    x1 = Street_Segments[street_seg_id].Curve_Points_list[0].x;
                    y1 = Street_Segments[street_seg_id].Curve_Points_list[0].y;
                } else {
                    x1 = Intersections[temp_from].xy_loc.x;
                    y1 = Intersections[temp_from].xy_loc.y;
                }

                x2 = Intersections[temp_to].xy_loc.x;
                y2 = Intersections[temp_to].xy_loc.y;

            }

            g->set_text_rotation(0);
            angle = calculate_angle(x1, y1, x2, y2) * kDegreeToRadian;

            temp_x = x2 - 5 * cos(angle);
            temp_y = y2 - 5 * sin(angle);
            if (angle >= 0 && angle <= M_PI / 2) {
                x4 = temp_x + 3 * cos(M_PI / 2 - angle);
                y4 = temp_y - 3 * sin(M_PI / 2 - angle);
                x3 = temp_x - 3 * cos(M_PI / 2 - angle);
                y3 = temp_y + 3 * sin(M_PI / 2 - angle);

            } else if (angle >= M_PI / 2 && angle <= M_PI) {
                x4 = temp_x + 3 * cos(angle - M_PI / 2);
                y4 = temp_y + 3 * sin(angle - M_PI / 2);
                x3 = temp_x - 3 * cos(angle - M_PI / 2);
                y3 = temp_y - 3 * sin(angle - M_PI / 2);
            } else if (angle >= M_PI && angle <= 3 * M_PI / 2) {
                x4 = temp_x + 3 * cos(3 * M_PI / 2 - angle);
                y4 = temp_y - 3 * sin(3 * M_PI / 2 - angle);
                x3 = temp_x - 3 * cos(3 * M_PI / 2 - angle);
                y3 = temp_y + 3 * sin(3 * M_PI / 2 - angle);
            } else if (angle >= 3 * M_PI / 2 && angle <= 2 * M_PI) {
                x4 = temp_x + 3 * cos(angle- 3 * M_PI/2);
                y4 = temp_y + 3 * sin(angle- 3 * M_PI/2);
                x3 = temp_x - 3 * cos(angle- 3 * M_PI/2);
                y3 = temp_y - 3 * sin(angle- 3 * M_PI/2);
            }

            //store coordinates of arrows
            arrow.push_back({x2, y2});
            arrow.push_back({x3, y3});
            arrow.push_back({x4, y4});

            g->set_color(151, 203, 235);
            g->fill_poly(arrow);
            arrow.clear();
        }
    }
    for (int street_id = 0; street_id < getNumStreets(); street_id++) {
        Streets[street_id].name_displayed = false;
    }


}

void display_street_name(int street_seg_id, ezgl::renderer *g) {

    if (Streets[Street_Segments[street_seg_id].streetID].street_name != "<unknown>") {
        //draw name on each segment only once
        if (Street_Segments[street_seg_id].numCurvePoints == 0) {
            double x1 = Intersections[Street_Segments[street_seg_id].from].xy_loc.x;
            double y1 = Intersections[Street_Segments[street_seg_id].from].xy_loc.y;
            double x2 = Intersections[Street_Segments[street_seg_id].to].xy_loc.x;
            double y2 = Intersections[Street_Segments[street_seg_id].to].xy_loc.y;

            ezgl::point2d mid_pt = calculate_mid_coordinate_LatLon_to_Pt2d(x1, y1, x2, y2);

            double degree = calculate_angle(x1, y1, x2, y2);

            g->set_color(ezgl::BLACK);
            g->set_font_size(8);
            g->set_text_rotation(degree);
            g->draw_text(mid_pt, Streets[Street_Segments[street_seg_id].streetID].street_name);

        } else {
            int curve_pt = Street_Segments[street_seg_id].numCurvePoints;
            double x1 = Street_Segments[street_seg_id].Curve_Points_list[curve_pt / 2].x;
            double y1 = Street_Segments[street_seg_id].Curve_Points_list[curve_pt / 2].y;
            double x2 = Street_Segments[street_seg_id].Curve_Points_list[curve_pt / 2 + 1].x;
            double y2 = Street_Segments[street_seg_id].Curve_Points_list[curve_pt / 2 + 1].y;

            ezgl::point2d mid_pt = calculate_mid_coordinate_LatLon_to_Pt2d(x1, y1, x2, y2);

            double degree = calculate_angle(x1, y1, x2, y2);

            g->set_color(ezgl::BLACK);
            g->set_font_size(10);
            g->set_text_rotation(degree);
            g->draw_text(mid_pt, Streets[Street_Segments[street_seg_id].streetID].street_name);
        }

        //use global boolean to make sure each street name only draw once
        Streets[Street_Segments[street_seg_id].streetID].name_displayed = true;
    }
}

void display_onewaystreet_direction(int street_seg_id, ezgl::renderer *g) {
    double text_height = 3;
    double text_width = 3;
    g->set_color(ezgl::GREY_55);
    std::string direction = ">";

    if (Street_Segments[street_seg_id].oneWay == true) {
        //find the from and to position of the oneway street
        IntersectionIdx from = Street_Segments[street_seg_id].from;
        LatLon coord_from = getIntersectionPosition(from);
        double first_intersect_pt_x = coord_from.longitude();
        double first_intersect_pt_y = coord_from.latitude();

        IntersectionIdx to = Street_Segments[street_seg_id].to;
        LatLon coord_to = getIntersectionPosition(to);
        double last_intersect_pt_x = coord_to.longitude();
        double last_intersect_pt_y = coord_to.latitude();

        //no curve pt
        if (Street_Segments[street_seg_id].numCurvePoints == 0) {
            ezgl::point2d xy_mid = calculate_mid_coordinate_LatLon_to_Pt2d(first_intersect_pt_x, first_intersect_pt_y, last_intersect_pt_x, last_intersect_pt_y);
            double degree = calculate_angle(first_intersect_pt_x, first_intersect_pt_y, last_intersect_pt_x, last_intersect_pt_y);


            g->set_text_rotation(degree);
            g->draw_text(xy_mid, direction, text_width, text_height);


        } else { //curve pt > 0
            int num_curve_pt = Street_Segments[street_seg_id].numCurvePoints;
            double first_curve_pt_x = Street_Segments[street_seg_id].Curve_Points_list[0].x;
            double first_curve_pt_y = Street_Segments[street_seg_id].Curve_Points_list[0].y;

            ezgl::point2d first_coor_drawn = calculate_mid_coordinate_LatLon_to_Pt2d(first_intersect_pt_x, first_intersect_pt_y, first_curve_pt_x, first_curve_pt_y);
            double degree_first_coor = calculate_angle(first_intersect_pt_x, first_intersect_pt_y, first_curve_pt_x, first_curve_pt_y);

            g->set_text_rotation(degree_first_coor);
            g->draw_text(first_coor_drawn, direction, text_width, text_height);

            for (int num_curve = 0; num_curve < num_curve_pt - 1; num_curve++) {
                double curve_start_x = Street_Segments[street_seg_id].Curve_Points_list[num_curve].x;
                double curve_start_y = Street_Segments[street_seg_id].Curve_Points_list[num_curve].y;
                double curve_end_x = Street_Segments[street_seg_id].Curve_Points_list[num_curve + 1].x;
                double curve_end_y = Street_Segments[street_seg_id].Curve_Points_list[num_curve + 1].y;

                ezgl::point2d curve_mid_pt = calculate_mid_coordinate_LatLon_to_Pt2d(curve_start_x, curve_start_y, curve_end_x, curve_end_y);
                double curve_degree = calculate_angle(curve_start_x, curve_start_y, curve_end_x, curve_end_y);

                g->set_text_rotation(curve_degree);
                g->draw_text(curve_mid_pt, direction, text_width, text_height);

            }
            double last_curve_pt_x = Street_Segments[street_seg_id].Curve_Points_list[num_curve_pt - 1].x;
            double last_curve_pt_y = Street_Segments[street_seg_id].Curve_Points_list[num_curve_pt - 1].y;

            ezgl::point2d last_coor_drawn = calculate_mid_coordinate_LatLon_to_Pt2d(last_curve_pt_x, last_curve_pt_y, last_intersect_pt_x, last_intersect_pt_y);
            double degree_last_coor = calculate_angle(last_curve_pt_x, last_curve_pt_y, last_intersect_pt_x, last_intersect_pt_y);

            g->set_text_rotation(degree_last_coor);
            g->draw_text(last_coor_drawn, direction, text_width, text_height);

        }


    }
}

//use combo box for user to select different map

void combo_box_cbk(GtkComboBoxText* self, ezgl::application* app) {
    auto text = gtk_combo_box_text_get_active_text(self);
    std::string textcast = text;

    //no map selected, return
    g_free(text);
    if (textcast == "") {
        return;
    } else {
        //map_name = text;
        //select the map according to the text in combo box
        app->update_message(gtk_combo_box_text_get_active_text(self));
        std::string prefix = "/cad2/ece297s/public/maps/";
        std::string postfix = ".streets.bin";
        prefix.append(textcast);
        prefix.append(postfix);

        closeMap();
        std::thread re_load(loadMap, prefix);
        re_load.join();

        ezgl::rectangle new_world({x_from_lon(min_lon), y_from_lat(min_lat)},
        {
            x_from_lon(max_lon), y_from_lat(max_lat)
        });
        app->change_canvas_world_coordinates("MainCanvas", new_world);
        app->refresh_drawing();

    }
}

//toggle the boolean of button to change between night mode and day mode

void toggle_night(GtkWidget* /*widget*/, ezgl::application* application) {
    night = !night;
    application->refresh_drawing();

}

//calculate the angle of two points with respect to x coordinate

double calculate_angle(double x1, double y1, double x2, double y2) {

    double angle = 0;

    //check if they have the same coordinate
    if (x1 == x2) {
        if (y2 > y1) {
            angle = M_PI / 2;
        } else if (y2 < y1) {
            angle = (-1) * M_PI / 2;
        }
    } else {
        angle = atan((y2 - y1) / (x2 - x1));
    }

    //make the angle between -pi/2 to pi/2 to 0 to 2pi
    if ((x2 - x1) < 0 && (y2 - y1) >= 0) {
        angle = M_PI + angle;
    } else if ((x2 - x1) < 0 && (y2 - y1) <= 0) {
        angle = M_PI + angle;
    } else if ((x2 - x1) > 0 && (y2 - y1) <= 0) {
        angle = 2 * M_PI + angle;
    }

    //change radian to degree
    return angle / kDegreeToRadian;
}

//find the midpoint between two points

ezgl::point2d calculate_mid_coordinate_LatLon_to_Pt2d(double first_coor_x, double first_coor_y, double second_coor_x, double second_coor_y) {

    double mid_x = (first_coor_x + second_coor_x) / 2.0;
    double mid_y = (first_coor_y + second_coor_y) / 2.0;

    ezgl::point2d mid_pt = ezgl::point2d(mid_x, mid_y);

    return mid_pt;
}

//draw a street segment

void draw_street_helper(int street_seg_id, ezgl::renderer *g) {
    for (int i = 0; i < Street_Segments[street_seg_id].Curve_Points_list.size() - 1; i++) {
        ezgl::point2d point1 = Street_Segments[street_seg_id].Curve_Points_list[i];
        ezgl::point2d point2 = Street_Segments[street_seg_id].Curve_Points_list[i + 1];
        g->draw_line(point1, point2);
    }
}

//helper functions to caculate the draw time

std::chrono::high_resolution_clock::time_point time_mark() {
    return std::chrono::high_resolution_clock::now();
}

auto time_test(std::chrono::high_resolution_clock::time_point start) {
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(end - start);
    return duration;
}

void duration(std::chrono::high_resolution_clock::time_point start) {
    std::cout << "The duration between time_mark was:    " << time_test(start).count() << " s " << std::endl;
}

void duration(std::chrono::high_resolution_clock::time_point start, std::string message) {
    std::cout << "The duration of " << message << " is   " << time_test(start).count() << " s " << std::endl;
}


//set the zoom level according to the distance between the bounds of the canvas

void zoom_level_definer(double bound_distance) {
    if (bound_distance < 250) {
        my_map.zoom_level = 12;
    } else if (bound_distance < 500) {
        my_map.zoom_level = 11;
    } else if (bound_distance < 650) {
        my_map.zoom_level = 10;
    } else if (bound_distance < 1100) {
        my_map.zoom_level = 9;
    } else if (bound_distance < 1800) {
        my_map.zoom_level = 8;
    } else if (bound_distance < 3000) {
        my_map.zoom_level = 7;
    } else if (bound_distance < 5000) {
        my_map.zoom_level = 6;
    } else if (bound_distance < 9000) {
        my_map.zoom_level = 5;
    } else if (bound_distance < 15000) {
        my_map.zoom_level = 4;
    } else if (bound_distance < 25000) {
        my_map.zoom_level = 3;
    } else if (bound_distance < 40000) {
        my_map.zoom_level = 2;
    } else {
        my_map.zoom_level = 1;
    }
}
