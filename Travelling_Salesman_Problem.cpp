


//pre-declearation of helper functions
bool astar(IntersectionIdx src, IntersectionIdx dst, const double turn_penalty);
std::vector<int> TraceBack(int src, int dst);

//computer the total travelling time inside a path
double computePathTravelTime(const std::vector<StreetSegmentIdx>& path, const double turn_penalty) {
    //code is hidden for academic integrity purpose
}

std::vector <StreetSegmentIdx> findPathBetweenIntersections(const std::pair<IntersectionIdx, IntersectionIdx> intersect_ids, const double turn_penalty) {

    //The larger a_offset could be, the more the alg will be like Dijstra
    a_offset = 25.5;
    
    //Delegate the start and end point
    const IntersectionIdx start_inter = intersect_ids.first;
    const IntersectionIdx end_inter = intersect_ids.second;
    
    //if the path is found, trace_back the path
    if (astar(start_inter, end_inter, turn_penalty)) {
        return TraceBack(start_inter, end_inter);
    }
    return {};

}

std::vector<int> TraceBack(int src, int dst) {

    std::list<StreetSegmentIdx> outpath;//Use of List for push_front
    std::vector<StreetSegmentIdx> result;
    IntersectionIdx searching_node = dst;
    
    while (searching_node != src) {
        //if the path is not found
        if (Intersections[searching_node].reachingEdge == -1) {
            return {}; //return a empty path
        }
        
        outpath.push_front(Intersections[searching_node].reachingEdge);
        //Determine the direction of trace back
        if (searching_node == Street_Segments[Intersections[searching_node].reachingEdge].from) {
            searching_node = Street_Segments[Intersections[searching_node].reachingEdge].to;
        } else {
            searching_node = Street_Segments[Intersections[searching_node].reachingEdge].from;
        }
    }

    //reverse the list into the vector
    for (auto n : outpath) {
        result.push_back(n);
    }

    //reset all the modified intersections 
    for (int i = 0; i < modified.size(); i++) {
        Intersections[modified[i]].traveltime = 0;
        Intersections[modified[i]].reachingEdge = -1;
    }
    modified.clear();
    
    return result;
}

bool astar(IntersectionIdx src, IntersectionIdx dst, const double turn_penalty) {

    std::priority_queue<WaveElem, std::vector<WaveElem>, std::greater < WaveElem>> wavefront; //nodes to be detected
    std::unordered_set<IntersectionIdx> checked_node; //keep track of the visited nodes

    wavefront.push(WaveElem(src, 0.0));//create the first element

    //as long as there's nodes to explore
    while (!wavefront.empty()) {

        WaveElem this_node_ele = wavefront.top();//get the node with least travel time
        wavefront.pop();

        IntersectionIdx current_node = this_node_ele.nodeID;

        //check if the node is visited
        if (checked_node.count(current_node) > 0) {
            continue;
        }
        checked_node.insert(current_node);

        //if the node is found, return
        if (current_node == dst) {
            return true;
        }

        std::vector<StreetSegmentIdx> segment_list = street_segment_of_intersection[current_node];
        for (int current_edge_count = 0; current_edge_count < segment_list.size(); current_edge_count++) {

            Street_Segment_Data this_seg_info = Street_Segments[segment_list[current_edge_count]];
            IntersectionIdx next_node;
            //check the direction of moving
            if (this_seg_info.to == current_node) {
                //if is moving in reverse direction and disobeying the regulation of oneway
                if (this_seg_info.oneWay) {
                    continue; //ignore this node
                } else {
                    next_node = this_seg_info.from;
                    //check again if the new node was explored
                    if (checked_node.count(next_node) > 0) {
                        continue;
                    }
                }
            } else {
                next_node = this_seg_info.to;
                //check again if the new node was explored before
                if (checked_node.count(next_node) > 0) {
                    continue;
                }
            }
            
            double travel_time = Street_Segments[segment_list[current_edge_count]].travel_time;
            if (this_seg_info.streetID != Street_Segments[Intersections[current_node].reachingEdge].streetID) {
                travel_time += turn_penalty; //determine if taking this route will have any turn penalties
            }

            //g_time is the actural time 
            double g_time = Intersections[current_node].traveltime + travel_time ;
            
            //if the next node was not explored or there's a better time
            if (Intersections[next_node].traveltime == 0 || g_time < Intersections[next_node].traveltime) {

                Intersections[next_node].traveltime = g_time; //keep the best time
                Intersections[next_node].reachingEdge = segment_list[current_edge_count]; //keep the path to the best time
                modified.push_back(next_node);
                
                //h_time is the estimated time
                double h_time = findDistanceBetweenTwoPoints(Intersections[next_node].position, Intersections[dst].position) / a_offset;
                wavefront.push(WaveElem(next_node, g_time + h_time));

            }
        }
    }

    return false;
}



std::vector<CourierSubPath> greedy_travelingCourier(
        const std::vector<DeliveryInf>& deliveries,
        const std::vector<IntersectionIdx>& depots,
        const float turn_penalty) {

    countm4++;

    std::unordered_set<IntersectionIdx> destinations;
    std::unordered_set<IntersectionIdx> pickups;
    std::unordered_set<IntersectionIdx> dropoffs;
    std::unordered_set<IntersectionIdx> my_depots;
    std::unordered_map<IntersectionIdx, std::unordered_set < IntersectionIdx>> pick_drop;

    std::vector<CourierSubPath> output;


    for (DeliveryInf info : deliveries) {
        destinations.insert(info.pickUp);
        pickups.insert(info.pickUp); //fetch all the pickup point
        //std::cout << " need to puckupat:" << info.pickUp << std::endl;

        destinations.insert(info.dropOff);
        //std::cout << " need to dropoff at" << info.dropOff << std::endl;

        pick_drop[info.pickUp].insert(info.dropOff); // mapped pickup to dropoff
    }

    for (IntersectionIdx depot : depots) {
        destinations.insert(depot);
        my_depots.insert(depot);

    }


    //for all the start_point

    //do not multi thread
    //#pragma omp parallel for  
    for (int i = 0; i < deliveries.size(); i++) {

        //use multi-des Dijkstra
        IntersectionIdx this_pickup = deliveries[i].pickUp;
        IntersectionIdx this_dropoff = deliveries[i].dropOff;
        if (time_map.count(this_pickup) == 0) {
            multi_dijkstra(this_pickup, destinations, turn_penalty);

        }
        if (time_map.count(this_dropoff) == 0) {
            multi_dijkstra(this_dropoff, destinations, turn_penalty);

        }




    }


    //depots
    //#pragma omp parallel for 
    for (int i = 0; i < depots.size(); i++) {

        //use multi-des Dijkstra
        IntersectionIdx this_depot = depots[i];
        multi_dijkstra(this_depot, destinations, turn_penalty);
    }


    //optimize_point 1 = change a start_position
    //At this step, all path-time should be loaded
    //int most_min_index = 0;
    int min_index = 0;
    int min_time = 100000000;
    for (int i = 0; i < depots.size(); i++) {
        //use multi-des Dijkstra
        IntersectionIdx this_depot = depots[i];
        dst_data this_satrt = time_map[this_depot].top();
        while (pickups.count(this_satrt.dst_id) == 0) {
            time_map[this_depot].pop(); //not the point we want， discard that
            this_satrt = time_map[this_depot].top(); //check the new front node  
        }
        if (this_satrt.time < min_time) {
            min_time = this_satrt.time;
            //min_index = most_min_index;
            min_index = i;
        }
    }

    //Go from Depot[0] to first pickup
    CourierSubPath multides_path; //cdeclare the subpath—obj
    multides_path.start_intersection = depots[min_index];


    dst_data this_dst = time_map[depots[min_index]].top();

    while (pickups.count(this_dst.dst_id) == 0) {
        time_map[depots[min_index]].pop(); //not the point we want， discard that
        this_dst = time_map[depots[min_index]].top(); //check the new front node
    }//keep locking the pickup


    //load the infomation of the first node
    multides_path.end_intersection = this_dst.dst_id;
    multides_path.subpath = this_dst.path;

    output.push_back(multides_path);
    IntersectionIdx point_here = this_dst.dst_id;


    //new location due to a new pickupp
    for (int dropoff_point : pick_drop[this_dst.dst_id]) {
        dropoffs.insert(dropoff_point);
    }
    pickups.erase(this_dst.dst_id); // should no longer planning to visit this pickup


    while (!pickups.empty() || !dropoffs.empty()) {

        this_dst = time_map[point_here].top();
        while (pickups.count(this_dst.dst_id) == 0 && dropoffs.count(this_dst.dst_id) == 0) { //if not found
            time_map[point_here].pop(); //not the point we want， discard that
            this_dst = time_map[point_here].top(); //check the new front node  
        }

        //At this point this_dst will be either pickup or dropoff
        if (pickups.count(this_dst.dst_id) > 0) { // if this is the pickup
         
            CourierSubPath pickup_next_path;
            pickup_next_path.start_intersection = point_here;
            pickup_next_path.end_intersection = this_dst.dst_id;
            pickup_next_path.subpath = this_dst.path;


            //dropoffs.insert(pick_drop[this_dst.dst_id]); // new location to go due to newpickupp
            for (int dropoff_point : pick_drop[this_dst.dst_id]) {
                dropoffs.insert(dropoff_point);
                
            }

            pickups.erase(this_dst.dst_id); // should nolonger go here
            point_here = this_dst.dst_id;
            output.push_back(pickup_next_path);

        } else if (dropoffs.count(this_dst.dst_id) > 0) { // this is the dropoff
      
            CourierSubPath dropoff_next_path;
            dropoff_next_path.start_intersection = point_here;
            dropoff_next_path.end_intersection = this_dst.dst_id;
            dropoff_next_path.subpath = this_dst.path;

            dropoffs.erase(this_dst.dst_id);
            point_here = this_dst.dst_id;
            output.push_back(dropoff_next_path);
        } else {
          
            time_map[point_here].pop();
        }

    }

    multi_dijkstra(point_here, destinations, turn_penalty);
    CourierSubPath lastpath;
    //optimizing point 2
    lastpath.start_intersection = point_here;
    this_dst = time_map[point_here].top(); //check the new front node
    while (my_depots.count(this_dst.dst_id) == 0) { //if not a depot, keep looking

        time_map[point_here].pop(); //not the point we want， discard that
        this_dst = time_map[point_here].top(); //check the new front node
    }//keep locking the depot
    lastpath.end_intersection = this_dst.dst_id;
    lastpath.subpath = this_dst.path;


    output.push_back(lastpath);


    time_map.clear();
    pick_drop.clear();
    pickups.clear();
    my_depots.clear();
    dropoffs.clear();
    destinations.clear();

    return output;

}

void multi_dijkstra(IntersectionIdx src, std::unordered_set<IntersectionIdx> dst_set, const double turn_penalty) {

    std::vector<IntersectionIdx> node_to_trace;
    std::vector<IntersectionIdx> node_modified;
    std::unordered_map <IntersectionIdx, StreetSegmentIdx> reaching_edge;
    std::unordered_map <IntersectionIdx, double> travel_time_map;


    std::priority_queue<WaveElem, std::vector<WaveElem>, std::greater < WaveElem>> wavefront; //nodes to be detected
    std::unordered_set<IntersectionIdx> checked_node; //keep track of the visited nodes

    wavefront.push(WaveElem(src, 0.0)); //create the first element

    //as long as there's nodes to explore
    while (!wavefront.empty()) {

        WaveElem this_node_ele = wavefront.top(); //get the node with least travel time
        wavefront.pop();

        IntersectionIdx current_node = this_node_ele.nodeID;

        //check if the node is visited
        if (checked_node.count(current_node) > 0) {
            continue;
        }
        checked_node.insert(current_node);

        //if the node is found, return
        if (dst_set.count(current_node) > 0) {

            //Node find is itself
            if (current_node == src) {
                std::vector<StreetSegmentIdx> empty_path = {};
                time_map[src].push(dst_data(src, empty_path, 1000000000));
            } else {
                node_to_trace.push_back(current_node);
                dst_set.erase(current_node);
            }

            //Node find is other points
        }

        if (dst_set.empty()) {
            break;
        }

        std::vector<StreetSegmentIdx> segment_list = street_segment_of_intersection[current_node];
        for (int current_edge_count = 0; current_edge_count < segment_list.size(); current_edge_count++) {

            Street_Segment_Data this_seg_info = Street_Segments[segment_list[current_edge_count]];
            IntersectionIdx next_node;
            //check the direction of moving
            if (this_seg_info.to == current_node) {
                //if is moving in reverse direction and disobeying the regulation of oneway
                if (this_seg_info.oneWay) {
                    continue; //ignore this node
                } else {
                    next_node = this_seg_info.from;
                    //check again if the new node was explored
                    if (checked_node.count(next_node) > 0) {
                        continue;
                    }
                }
            } else {
                next_node = this_seg_info.to;
                //check again if the new node was explored before
                if (checked_node.count(next_node) > 0) {
                    continue;
                }
            }

            double travel_time = Street_Segments[segment_list[current_edge_count]].travel_time;
            if (this_seg_info.streetID != Street_Segments[reaching_edge[current_node]].streetID) {
                travel_time += turn_penalty; //determine if taking this route will have any turn penalties
            }

            //g_time is the actural time 
            //double g_time = Intersections[current_node].traveltime + travel_time;
            double g_time = travel_time_map[current_node] + travel_time;


            if (travel_time_map[next_node] == 0 || g_time < travel_time_map[next_node]) {

                travel_time_map[next_node] = g_time; //keep the best time
                reaching_edge[next_node] = segment_list[current_edge_count]; //keep the path to the best time
                node_modified.push_back(next_node);

                wavefront.push(WaveElem(next_node, g_time));

            }




        }
    }

    IntersectionIdx dst; //this is temp node_dst

    for (int i = 0; i < node_to_trace.size(); i++) {
        dst = node_to_trace[i];


        //trace_back
        std::list<StreetSegmentIdx> outpath; //Use of List for push_front
        std::vector<StreetSegmentIdx> result;
        IntersectionIdx searching_node = dst;

        while (searching_node != src) {
            if (reaching_edge[searching_node] == -1) {
                result = {}; //return a empty path
            }

            outpath.push_front(reaching_edge[searching_node]);
            //Determine the direction of trace back
            if (searching_node == Street_Segments[reaching_edge[searching_node]].from) {
                searching_node = Street_Segments[reaching_edge[searching_node]].to;
            } else {
                searching_node = Street_Segments[reaching_edge[searching_node]].from;
            }
        }

        //reverse the list into the vector
        for (auto n : outpath) {
            result.push_back(n);
        }

        time_map[src].push(dst_data(dst, result, computePathTravelTime(result, turn_penalty)));

    }

}



