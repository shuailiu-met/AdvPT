#include "Race.h"
#include <iostream>

Race::Race(){
    // create DataAcc object
    data = new DataAcc("res/unit_db.csv", "res/parameters.csv");

    // set simulation time to zero
    currentTime = 0;

    // init resources
    //workers = data->getParameter("WORKERS_START", false);
    minerals = data->getParameter("MINERALS_START", true);
    vespene = data->getParameter("VESPENE_START", true);

    workers = 0;
    worker_minerals = 0;
    worker_vespene = 0;

    supply = 0;
    supply_used = 0;
    vespene_buildings = 0;
}


int Race::advanceOneTimeStep(){
    if(currentTime == 0){
        // JSON header
        addEventsToJSON(true);
    }
    currentTime ++;

    // stop simulating it if takes more than 1000 seconds
    // see restrictions in 'project_part2.pdf'
    if(currentTime > 1000){
        invalidateJSON();
        outputJSON();
        return 0;
    }
    // TODO: restriction from pdf: there can be at most 2 vespine buildings per base!
    // Does per base mean "per base building" or max 2 in one simulation?


    // Do all neccessary actions (see header file)
    updateResources();
    advanceBuildingProcess();
    // check if special ability was activated
    // only one action at a timestep possible
    // -> no building process possible anymore
    // see restrictions in 'project_part2.pdf'
    int ret = specialAbility();
    if(!ret)
        ret = startBuildingProcess();
    if(ret == -2){
        // building can never be build!
        invalidateJSON();
        outputJSON();
        return 0;
    }

    distributeWorkers();

    // add all events added at this timestep to json
    addEventsToJSON();

    if((building.size() + future.size()) == 0)
        outputJSON();
    return building.size() + future.size();
}

// implement worker distribution strategy here
void Race::distributeWorkers(){
    // as much workers as possible to vespene
    worker_vespene = (workers <= vespene_buildings * 3) ? workers : vespene_buildings * 3;
    worker_minerals = workers - worker_vespene;
    // normally we need more minerals than we need vespine
    // => redistribute if we have less minerals
    // TODO play aroud with this and see which distribution has good results
    /*if(minerals < 2*vespene){
        worker_minerals += worker_vespene;
        worker_vespene = 0;
    }*/
}

void Race::invalidateJSON(){
    // in the init element valid is stored in minerals
    // because minerals is unused
    json[0].minerals = 0;
}

void Race::addEvent(std::string type, std::string name, std::string i1, std::string i2, int id){
    // check for multiple build finish events from the same producer
    // TODO not tested yet -> not needed in Protoss
    if(type == "build-end"){
        for(std::vector<Event>::iterator it = events.begin(); it != events.end(); it++) {
            if(it->type == "build-end" && it->info1 == i1 && it->name == name){
                it->ids.push_back(id);
                return;
            }
        }
    }
    // IDs must be printed as array
    Event ev;
    ev.type = type;
    ev.name = name;
    if(type == "build-start"){
        ev.info1 = i1;
    }else if(type == "build-end"){
        ev.info1 = i1;
        ev.ids.push_back(id);
    }else if(type == "special"){
        ev.info1 = i1;
        ev.info2 = i2;
    }else{
        // unsupported
        assert(false);
    }
    events.push_back(ev);
}

void Race::addEventsToJSON(bool init){
    if(events.size() == 0 && !init)
        return;
    if(init){
        JSON js;
        std::vector<Event> evs;
        Event ev;

        ev.name = finished.front().getName();

        for(std::list<Unit>::iterator it = finished.begin(); it != finished.end(); it++) {
            if(ev.ids.size() > 0 && ev.name == it->getName()){
                ev.ids.push_back(it->getId());
            }else if(ev.ids.size() == 0){
                ev.ids.push_back(it->getId());
            }else{
                js.events.push_back(ev);
                ev = Event();
                ev.name = it->getName();
                ev.ids.push_back(it->getId());
            }
        }
        js.events.push_back(ev);
        js.time = currentTime;
        // minerals member is used in the init element to indicate
        // if the JSON is valid or not!!!
        js.minerals = 1;
        json.push_back(js);
    }else{
        JSON js;
        js.time = currentTime;
        js.work_minerals = worker_minerals;
        js.work_vespene = worker_vespene;
        js.minerals = minerals;
        js.vespene = vespene;
        js.supply_used = supply_used;
        js.supply = supply;
        js.events = events;

        json.push_back(js);
        events.clear();
    }
}

void Race::outputJSON(){
    size_t count = 0;
    for(std::vector<JSON>::iterator it = json.begin(); it != json.end(); it++) {
        if(it->time == 0){
            std::cout << "{\n";
            Unit cur = finished.front();
            std::cout << "\t\"game\": \"" << data->getAttributeString(cur.getName(), DataAcc::race) << "\",\n";
            // in the init Element the valid bit is stored in minerals member
            // saves an extra mostly unused member
            if(it->minerals == 0){
                std::cout << "\t\"buildlistValid\": " << it->minerals << "\n";
                std::cout << "}\n";
                return;
            }
            std::cout << "\t\"buildlistValid\": " << it->minerals << ",\n";
            std::cout << "\t\"initialUnits\": {\n";

            size_t i1 = 0;
            for(std::vector<Event>::iterator it2 = it->events.begin(); it2 != it->events.end(); it2++) {
                size_t i2 = 0;
                std::cout << "\t\t\"" << it2->name << "\": [\n";
                for(std::vector<int>::iterator it3 = it2->ids.begin(); it3 != it2->ids.end(); it3++) {
                    i2 ++;
                    if(i2 == it2->ids.size()){
                        std::cout << "\t\t\t\"" << it2->name << "_" << *it3 << "\"\n";
                    }else{
                        std::cout << "\t\t\t\"" << it2->name << "_" << *it3 << "\",\n";
                    }
                }
                i1++;
                if(i1 == it->events.size()){
                    std::cout << "\t\t]\n";
                }else{
                    std::cout << "\t\t],\n";
                }
            }

            std::cout << "\t},\n";
            std::cout << "\t\"messages\": [\n";

        }else{

            std::cout << "\t\t{\n";
            std::cout << "\t\t\t\"time\": " << it->time << ",\n";
            std::cout << "\t\t\t\"status\": {\n";

            std::cout << "\t\t\t\t\"workers\": {\n";
            std::cout << "\t\t\t\t\t\"minerals\": " << it->work_minerals << ",\n";
            std::cout << "\t\t\t\t\t\"vespene\": " << it->work_vespene << "\n";
            std::cout << "\t\t\t\t},\n";

            std::cout << "\t\t\t\t\"resources\": {\n";
            std::cout << "\t\t\t\t\t\"minerals\": " << (it->minerals / FIXEDPOINT_FACTOR) << ",\n";
            std::cout << "\t\t\t\t\t\"vespene\": " << (it->vespene / FIXEDPOINT_FACTOR) << ",\n";
            std::cout << "\t\t\t\t\t\"supply-used\": " << it->supply_used << ",\n";
            std::cout << "\t\t\t\t\t\"supply\": " << it->supply << "\n";
            std::cout << "\t\t\t\t}\n";
            std::cout << "\t\t\t},\n";


            std::cout << "\t\t\t\"events\": [\n";
            size_t i1 = 0;
            for(std::vector<Event>::iterator it2 = it->events.begin(); it2 != it->events.end(); it2++) {
                std::cout << "\t\t\t\t{\n";
                std::cout << "\t\t\t\t\t\"type\": " << "\"" << it2->type << "\",\n";
                std::cout << "\t\t\t\t\t\"name\": " << "\"" << it2->name << "\",\n";
                if(it2->type == "build-start"){
                    std::cout << "\t\t\t\t\t\"producerID\": " << "\"" << it2->info1 << "\"\n";
                }else if(it2->type == "special"){
                    std::cout << "\t\t\t\t\t\"targetBuilding\": " << "\"" << it2->info1 << "\",\n";
                    std::cout << "\t\t\t\t\t\"triggeredBy\": " << "\"" << it2->info2 << "\"\n";
                }else if(it2->type == "build-end"){
                    std::cout << "\t\t\t\t\t\"producerID\": " << "\"" << it2->info1 << "\",\n";
                    std::cout << "\t\t\t\t\t\"producedIDs\": [\n";

                    size_t i2 = 0;
                    for(std::vector<int>::iterator it3 = it2->ids.begin(); it3 != it2->ids.end(); it3++) {
                        i2 ++;
                        if(i2 == it2->ids.size()){
                            std::cout << "\t\t\t\t\t\t\"" << it2->name << "_" << *it3 << "\"\n";
                        }else{
                            std::cout << "\t\t\t\t\t\t\"" << it2->name << "_" << *it3 << "\",\n";
                        }
                    }
                    std::cout << "\t\t\t\t\t]\n";
                }
                i1 ++;
                if(i1 == it->events.size()){
                    std::cout << "\t\t\t\t}\n";
                }else{
                    std::cout << "\t\t\t\t},\n";
                }
            }
            std::cout << "\t\t\t]\n";

            count ++;
            if(count == (json.size()-1)){
                std::cout << "\t\t}\n";
            }else{
                std::cout << "\t\t},\n";
            }
        }
    }
    std::cout << "\t]\n";
    std::cout << "}\n";
}
