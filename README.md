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

int main(int argc, char ** argv){
    std::cout << "MCAP SAMPLE ! " << std::endl;
    return 0;
}
```

## Use Foxglove-studio
### Live


### Playback