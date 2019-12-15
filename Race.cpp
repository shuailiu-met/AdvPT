#include "Race.h"
#include <iostream>

Race::Race(std::vector<std::string> buildings){
    // create DataAcc object
    data = new DataAcc("res/unit_db.csv", "res/parameters.csv");

    // set simulation time to zero
    currentTime = 0;

    // build buildings and add them to list
    for(std::vector<std::string>::iterator it = buildings.begin(); it != buildings.end(); it++) {
        Unit u = data->getUnit(*it);
        future.push_back(u);
    }

    // init resources
    workers = data->getParameter("WORKERS_START", true);
    minerals = data->getParameter("MINERALS_START", true);
    vespene = data->getParameter("VESPENE_START", true);

    worker_minerals = 0;
    worker_vespene = 0;

    supply = 0;
    supply_used = 0;
}


int Race::advanceOneTimeStep(){
    if(currentTime == 0){
        // JSON header
        addEventsToJSON(true);
    }
    currentTime ++;

    // Do all neccessary actions (see header file)
    updateResources();
    advanceBuildingProcess();
    specialAbility();
    int ret = startBuildingProcess();
    if(ret == -2){
        // building can never be build!
        // exit process
        return ret;
    }
    distributeWorkers();

    // just some testing stuff for now
    //std::cout << "Workers: " << workers << ", Minerals: " << minerals;
    //std::cout << ", Vespene: " << vespene << ", Supply: " << supply << "\n";

    /*for(std::list<Unit>::iterator it = future.begin(); it != future.end(); it++) {
        std::cout << (*it).getName() << " : ";
        std::cout << (*it).getTimeLeft() << "\n";
    }
    for(std::list<Unit>::iterator it = building.begin(); it != building.end(); it++) {
        std::cout << (*it).getName() << " : ";
        std::cout << (*it).getTimeLeft() << "\n";
    }*/

    addEventsToJSON();

    if((building.size() + future.size()) == 0)
        outputJSON();
    return building.size() + future.size();
}

// implement worker distribution strategy here
void Race::distributeWorkers(){
    // TODO implement
    // placeholder to test simulation
    worker_minerals = 6;
    worker_vespene = 6;
}

void Race::addEvent(std::string type, std::string name, std::string i1, std::string i2, std::vector<int> *ids){
    Event ev;
    ev.type = type;
    ev.name = name;
    if(type == "build-start"){
        ev.info1 = i1;
    }else if(type == "build-end"){
        ev.info1 = i1;
        ev.ids = *ids;
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
    if(events.size() == 0)
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
            //std::cout << "\t\"buildlistValid\": 1,\n";
            Unit cur = finished.front();
            std::cout << "\t\"game\": \"" << data->getAttributeString(cur.getName(), DataAcc::race) << "\",\n";
            std::cout << "\t\"initialUnits\": {\n";

            size_t i1 = 0;
            for(std::vector<Event>::iterator it2 = it->events.begin(); it2 != it->events.end(); it2++) {
                size_t i2 = 0;
                std::cout << "\t\t\"" << it2->name << "\": [\n";
                for(std::vector<int>::iterator it3 = it2->ids.begin(); it3 != it2->ids.end(); it3++) {
                    i2 ++;
                    if(i2 == it2->ids.size()){
                        std::cout << "\t\t\t\"" << it2->name << "_" << *it3 << "\",\n";
                    }else{
                        std::cout << "\t\t\t\"" << it2->name << "_" << *it3 << "\"\n";
                    }
                }
                i1++;
                if(i2 == it->events.size()){
                    std::cout << "\t\t]\n";
                }else{
                    std::cout << "\t\t],\n";
                }
            }

            std::cout << "\t},\n";

        }else{

            std::cout << "\t\"messages\": [\n";
            std::cout << "\t\t{\n";
            std::cout << "\t\t\t\"time\": " << it->time << ",\n";
            std::cout << "\t\t\t\"status\": {\n";

            std::cout << "\t\t\t\t\"workers\": {\n";
            std::cout << "\t\t\t\t\t\"minerals\": " << it->work_minerals << ",\n";
            std::cout << "\t\t\t\t\t\"vespene\": " << it->work_vespene << "\n";
            std::cout << "\t\t\t\t\"},\n";

            std::cout << "\t\t\t\t\"resources\": {\n";
            std::cout << "\t\t\t\t\t\"minerals\": " << it->minerals << ",\n";
            std::cout << "\t\t\t\t\t\"vespene\": " << it->vespene << ",\n";
            std::cout << "\t\t\t\t\t\"supply-used\": " << it->supply_used << ",\n";
            std::cout << "\t\t\t\t\t\"supply\": " << it->supply << "\n";
            std::cout << "\t\t\t\t\"}\n";
            std::cout << "\t\t\t\"},\n";


            std::cout << "\t\t\t\"events\": [\n";
            size_t i1 = 0;
            for(std::vector<Event>::iterator it2 = it->events.begin(); it2 != it->events.end(); it2++) {
                std::cout << "\t\t\t\t{\n";
                std::cout << "\t\t\t\t\"type\": " << "\"" << it2->type << "\",\n";
                std::cout << "\t\t\t\t\"name\": " << "\"" << it2->name << "\",\n";
                if(it2->type == "build-start"){
                    std::cout << "\t\t\t\t\"producerID\": " << "\"" << it2->name << "_" << it2->info1 << "\",\n";
                }else if(it2->type == "special"){
                    std::cout << "\t\t\t\t\"targetBuilding\": " << "\"" << it2->info1 << "\",\n";
                    std::cout << "\t\t\t\t\"triggeredBy\": " << "\"" << it2->info2 << "\",\n";
                }else if(it2->type == "build-end"){
                    std::cout << "\t\t\t\t\"producerID\": " << "\"" << it2->name << "_" << it2->info1 << "\",\n";
                    std::cout << "\t\t\t\t\"producerIDs\": [\n";

                    size_t i2 = 0;
                    for(std::vector<int>::iterator it3 = it2->ids.begin(); it3 != it2->ids.end(); it3++) {
                        i2 ++;
                        if(i2 == it2->ids.size()){
                            std::cout << "\t\t\t\t\t\"" << it2->name << "_" << *it3 << "\"\n";
                        }else{
                            std::cout << "\t\t\t\t\t\"" << it2->name << "_" << *it3 << "\",\n";
                        }
                    }
                    std::cout << "\t\t\t\t]\n";
                }
                i1 ++;
                if(i1 == it->events.size()){
                    std::cout << "\t\t\t},\n";
                }else{
                    std::cout << "\t\t\t}\n";
                }
            }

            count ++;
            if(count == json.size()){
                std::cout << "\t\t}\n";
            }else{
                std::cout << "\t\t},\n";
            }
        }
    }
}
