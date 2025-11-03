#ifndef __BLUE_OS__DRIVERS__DRIVER_H
#define __BLUE_OS__DRIVERS__DRIVER_H

namespace blueOs{
    namespace drivers{

        class Driver{
            public:
                Driver();
                ~Driver();
                // They should be pure virtual functions 
                virtual void activate();
                virtual int reset();
                virtual void deactivate();
        };

        class DriverManager{
            private:
                Driver* drivers[256];
                int numOfDriver;

            public:
                DriverManager();
                void addDriver(Driver* driver);
                void activateAll();
        };
        
    }
}

#endif