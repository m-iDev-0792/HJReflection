//
// Created by 何振邦 on 2021/12/15.
//

#ifndef REFLECTION_HJTEST_H
#define REFLECTION_HJTEST_H
#include "HJObject.h"

class HJOBJECT HJTest : public HJObject{
	HJOBJECT_BODY
public:
	HJTest()=default;
	HJPROPERTY int id = 10010;
	HJPROPERTY std::string name;
	HJPROPERTY float grade;
	static HJObject* instantiate(void* _parameter = nullptr); //User-must-implement
};
class UselessClass{
public:
	int uselessInt=100;
	std::string uselessStr="hello";
};
class HJOBJECT HJTestSon : public HJTest, public UselessClass{
	HJOBJECT_BODY
public:
	HJTestSon()=default;
	HJPROPERTY int age;
	HJPROPERTY std::string str;
	static HJObject* instantiate(void* _parameter = nullptr);
};


#endif //REFLECTION_HJTEST_H
