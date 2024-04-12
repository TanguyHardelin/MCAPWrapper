# MCAPWrapper
This project aim to provide a simple wrapper for using MCAP both in live and playback. 
[MCAP description]
[Foxglove studio link]


All functions are thread safe.

## Build and install 
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