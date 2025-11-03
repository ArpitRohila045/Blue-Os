
#include <drivers/driver.h>
using namespace blueOs::drivers;
 
Driver::Driver(){}

Driver::~Driver(){}
        
void Driver::activate(){}

int Driver::reset(){
    return 0;
}

void Driver::deactivate(){
}


DriverManager::DriverManager(){
    numOfDriver = 0;
}


void DriverManager::addDriver(Driver* drv){
    drivers[numOfDriver] = drv;
    numOfDriver++;
}


void DriverManager::activateAll(){
    for(int i = 0; i < numOfDriver; i++)
        drivers[i]->activate();
}
        