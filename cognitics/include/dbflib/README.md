# dbflib

Library for reading .dbf files

## Usage

```cpp
#include "DBaseFile.h"
#include <iostream>

int main() {

    DBaseFile dbf = DBaseFile();

    try {
        dbf->openFile("./test/ANSPRECH.DBF");
    } catch(std::runtime_error& e) {
        std::cout << e.what() << std::endl;
    }

    dbf->stat();
    return 0;
}
```

## Installation

```
git clone https://github.com/fschutt/dbflib
cd dbflib
make && make install
```

This will generate a `libdbf.so` file in the root folder, then install it on the system.
On Windows, you have to copy the file manually and put the headers where your system can find them.

## Documentation

This repository uses doxygen syntax for documentation.

Please refer to http://www.doxygen.org/ for usage instructions.
