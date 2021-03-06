# HJReflection
![](logo.png)
HJReflection is a simple tool that adds reflection for C++ code.
Like Qt MOC and Unreal Header Tool, HJReflection reads C++ header/source files and generates additional reflection meta data for classes/properties that are tagged with certain macros.

Example of constructing a reflectable class HJTest

in header file:
```cpp
class HJOBJECT HJTest : public HJObject{ //HJOBJECT macro indicates HJTest is a reflectable class
	HJOBJECT_BODY // HJOBJECT_BODY macro add additional members
public:
	HJTest()=default;
	HJPROPERTY int id = 10010; //HJPROPERTY indicates id is a reflectable property
	HJPROPERTY std::string name;
	HJPROPERTY float grade;
	static HJObject* instantiate(void* _parameter = nullptr); //User-must-implement instantiate function
};
```

in source file:
```cpp
#include "HJTest.h"
#include "HJTest.generated.h" //a header file automatically generated by HJReflectionTool
HJObject* HJTest::instantiate(void* _parameter){
	return new HJTest();
}
```

in main.cpp
```cpp
HJObject* test=HJObject::instantiateObject("HJTest", nullptr);
cout<<"Object test is a "<<test->getClassName()<<endl;
dynamic_cast<HJTest*>(test)->name="huajige";
dynamic_cast<HJTest*>(test)->grade=59.9;
auto ID=test->getValue("id");
auto NAME=test->getValue("name");
cout<<NAME.getValue<string>()<<" has id = "<<ID.getValue<int>()<<endl;
HJVariant GRADE(60.0f);
test->setValue("grade",GRADE);
cout<<"student "<<dynamic_cast<HJTest*>(test)->name<<" has grade  = "<<dynamic_cast<HJTest*>(test)->grade<<endl;
```
you got output
```shell
Object test is a HJTest
huajige has id = 10010
student huajige has grade  = 60
```

configure CMakeLists.txt to run HJReflectionTool automatically before building the target
```cmake
#add sub projects
add_subdirectory(HJReflectionTool)
add_subdirectory(HJReflection)

#configure Reflection test project
add_executable(Reflection main.cpp HJTest.cpp ${HJREFLECTION_SRC_FILES})
target_include_directories(Reflection PUBLIC ${HJREFLECTION_INCLUDE_DIR})

set(EXCLUDED_PATH ${HJREFLECTION_EXCLUDED_FILES} ${HJREFLECTIONTOOL_SOURCE_DIR} "and other files your don't wish to parse")
#let libclang to find stl lib and other system include files
set(SYSTEM_INCLUDE_PATH "the directories of standard libraries and other header files in your computer")
set(INCLUDE_DIR ${HJREFLECTION_INCLUDE_DIR} ${SYSTEM_INCLUDE_PATH} "and other header files your wish to parse")
# Method 1 to call HJReflectionTool automatically
add_custom_target(HJReflectionToolTarget ALL
        COMMAND HJReflectionTool "${CMAKE_SOURCE_DIR}" "-E${EXCLUDED_PATH}" "-I${INCLUDE_DIR}"
        DEPENDS HJReflectionTool
        WORKING_DIRECTORY "${HJREFLECTIONTOOL_BINARY_DIR}"
        COMMENT "using add_custom_target to run HJReflectionTool to process path ${CMAKE_SOURCE_DIR}"
        VERBATIM
        )
add_dependencies(Reflection HJReflectionToolTarget)


# Method 2 to call HJReflectionTool automatically
#add_custom_command(TARGET Reflection PRE_BUILD
#        COMMAND HJReflectionTool "${CMAKE_SOURCE_DIR}" "-E${EXCLUDED_PATH}" "-I${INCLUDE_DIR}"
#        VERBATIM
#        WORKING_DIRECTORY "${HJREFLECTIONTOOL_BINARY_DIR}"
#        COMMENT "using add_custom_command to run HJReflectionTool to process path ${CMAKE_SOURCE_DIR}"
#        DEPENDS HJReflectionTool)
```