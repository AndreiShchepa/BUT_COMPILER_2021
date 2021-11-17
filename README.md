# VUT_IFJ_2021

## Fast run for scanner testing
```shell
chmod +x start.sh
./start.sh file_with_code list_of_params
```
**List of params and their meaning**  
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


## Run tests
```shell
chmod +x run_tests.sh
./run_tests.sh name expected_err valgrind
```
**List of params and their meaning**  
name - your name which you use in name of files  
expected_err - what error you want to test  
valgrind - optional param for testing memory errors  

**Supporting errors:**  
0 - correct file without any errors  
2 - wrong file with syntax erors  
3..7 - wrong file with semantic errors  

