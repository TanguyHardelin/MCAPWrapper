# MCAPWrapper
According to [foxglove](https://foxglove.dev) MCAP is a standardized container format for storing heterogeneous robotics data. It allow unified data format for recording robotics data and allow futher debug. By combining it with [foxglove-studio](https://foxglove.dev/download) it provide a powerfull tool for visualize and debug robotics projects.

This project aim to provide a wrapper for writing MCAP in a simplified way. With this project it should be possible to write MCAP and use foxglove in minutes instead of hours.

## Build and install 
Install dependencies:
```bash
sudo apt install libwebsocketpp-dev
```
Build and install repository
```bash
git clone https://github.com/TanguyHardelin/MCAPWrapper.git
cd MCAPWrapper
mkdir build && cd build && sudo make install
```

## Get started
Some example are provided in the [example folder](https://github.com/TanguyHardelin/MCAPWrapper/tree/main/example). The following code provide quick get started sample for using foxglove studio with this wrapper.

```cpp
#include <iostream>
#include <chrono>
#include <thread>
#include "json.hpp"
#include "MCAPWriter.h"

int main(int argc, char **argv){
    // Open MCAP writer:
    mcap_wrapper::open_file_connexion("toto.mcap");
    // Write data into file:
    for(unsigned i=0; i<20; i++){
        // Create data
        nlohmann::json test_json;
        test_json["x"] = i;
        test_json["a"] = "toto";
        test_json["b"] = 42.3698;
        test_json["c"] = -1157.0;
        test_json["d"] = nlohmann::json();
        test_json["d"]["a"] = i * 150.0;
        test_json["d"]["b"] = "foo";
        test_json["d"]["c"] = 0;

        // Put it to file
        mcap_wrapper::write_JSON_to_all("test_json", test_json.dump(), std::chrono::system_clock::now().time_since_epoch().count());

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    // Close file
    mcap_wrapper::close_file_connexion("toto.mcap");

    return 0;
}
```

Other examples are available in [examples folder](https://github.com/TanguyHardelin/MCAPWrapper/tree/main/examples)


# TODO
- Test on compression runtime
- Test on sync
- Async compression
