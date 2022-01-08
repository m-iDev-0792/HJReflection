//
// Created by 何振邦 on 2021/12/15.
//

#include "HJTest.h"
#include "HJTest.generated.h"

HJObject* HJTest::instantiate(void* _parameter){
	return new HJTest();
}


HJObject* HJTestSon::instantiate(void *_parameter) {
	auto p=new HJTestSon();
	p->id=10086;
	if(!_parameter)p->age=25;
	else p->age=*reinterpret_cast<int*>(_parameter);
	return p;
}