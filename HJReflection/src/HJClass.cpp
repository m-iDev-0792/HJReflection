//
// Created by 何振邦 on 2021/10/3.
//

#include "HJClass.h"

HJClass::HJClass(const std::string &_name, const std::vector<ReflectInfo> &_reflectInfos,
                 const std::string &_superClassName, InstatiateFuncType _instFuncPtr) {
	name=_name;
	instantiateFunc=_instFuncPtr;
	superClassName=_superClassName;
	for(const auto& r:_reflectInfos){
		reflectInfosMap[r.name]=r;
	}
}

HJObject* HJClass::instantiate(const std::string& _className, void* _parameter){
	auto& registeredClassesMap=HJClass::getClassMap();
	auto searched=registeredClassesMap.find(_className);
	if(registeredClassesMap.end() == searched || searched->second== nullptr || !searched->second->isInstantiable())
		return nullptr;
	else return searched->second->instantiateFunc(_parameter);
}
const HJClass* HJClass::getClassInstance(const std::string &_className) {
	auto& registeredClassesMap=HJClass::getClassMap();
	auto searched=registeredClassesMap.find(_className);
	if(registeredClassesMap.end() == searched || searched->second== nullptr)
		return nullptr;
	else
		return searched->second;
}
bool HJClass::isSubClass(const std::string &_super) const {
	auto& registeredClassesMap=HJClass::getClassMap();
	auto sp=registeredClassesMap.find(superClassName);
	while(sp!=registeredClassesMap.end()){
		if(sp->second){
			if(sp->second->name==_super)return true;
			else sp=registeredClassesMap.find(sp->second->superClassName);
		}else{
			break;
		}
	}
	return false;
}

bool HJClass::registerClass(const std::string &_className, const HJClass *_class) {
	HJClass::getClassMap()[_className]=_class;
	return true;
}
bool HJClass::unregisterClass(const std::string &_className) {
	auto& registeredClassesMap=HJClass::getClassMap();
	auto searched=registeredClassesMap.find(_className);
	if(searched!=registeredClassesMap.end()){
		registeredClassesMap.erase(searched);
		return true;
	}else{
		return false;
	}
}
