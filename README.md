The Unified Robotic Framework Common is a collection of common objects used by all Unified Robotic Framework libraries.

## Requirements
- CMake 3.10
- Python 3.6+
- C++17 compiler
    - GCC 7 or higher
    - Microsoft Visual C++ (MSVC) 2017 or higher

## Installation
#### Conan
Install conan on your system:
```
$ pip3 install conan
```
For additional information for system wide installation on different platform, please read https://docs.conan.io/en/latest/installation.html

#### Install dependencies
For installing all the dependencies, launch from the root folder of the repository the following command:

```
$ conan install . -if build/conan -s build_type=<Debug|Release|RelWithDebInfo> --build=*
```

This will install locally all the necessary dependencies for the selected build type and build the missing ones.

#### Build the project
For building the project, launch from the root folder of the repository the following command:
```
$ conan build . -if build/conan
```
The command will build the entire repository as well as execute all the automated tests.

#### Install package
Finally, the package can be installed in the system so that it can be used by other projects:
```
$ conan create . -s build_type=<Debug|Release|RelWithDebInfo>
```
The command will build the entire repository as well as execute all the automated tests and finally install the package.

## Usage
After installation, the package ```urf_common/<version>``` can be added to your conanfile requirements.
You can add the package to your CMakeLists as follows:
```
find_package(urf_common_cpp REQUIRED)
```
The following variables will be set:
- ```${urf_common_cpp_LIBRARIES}``` containing the path to the libraries
- ```${urf_common_cpp_INCLUDE_DIRS}``` containing the path to the header files

You can then link the package to your target as follows:
```
target_link_libraries(target_name ${urf_common_cpp_LIBRARIES})
target_include_directories(target_name PUBLIC ${urf_common_cpp_INCLUDE_DIRS})
```