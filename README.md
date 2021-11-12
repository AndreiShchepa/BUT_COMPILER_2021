# VUT_IFJ_2021

## Fast run for scanner testing
```
chmod +x start.sh
./start.sh file_with_code list_of_params
```

## List of params and their meaning
RULES   - enable rules printing from parser

SCANNER - run only scanner

EXPR    - enable debug for expressions processing

## Tests
```shell
cd build
cmake ..
make -j16
./scannerTests
```

## CodeCoverage
**scannerTests**
```shell
make scannerTests_coverage
open scannerTests_coverage/index.html
```
