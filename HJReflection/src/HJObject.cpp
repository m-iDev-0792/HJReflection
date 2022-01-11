//
// Created by 何振邦 on 2021/10/3.
//

#include "HJObject.h"

//an example of the content of HJObjectSubClass.generated.h
struct HJObjectRegisterHelper{
	HJObjectRegisterHelper(){
		const std::vector<ReflectInfo> HJObjectReflectInfoArray;
		HJClass::registerClass("HJObject",new HJClass("HJObject",
		                                              HJObjectReflectInfoArray,
		                                              "",
		                                              &HJObject::instantiate));
	}
} HJObjectRegisterHelperInstance;

std::string HJObject::getClassName() const {
	return "HJObject";
}
const HJClass* HJObject::getClass() const {
	return HJClass::getClassInstance("HJObject");
}
std::string HJObject::getSuperName() const {
	return "";
}
const HJClass* HJObject::getSuperClass() const {
	return nullptr;
}

bool HJObject::setValue(const std::string &_mName, const HJVariant &_value) {
	auto classInfo=HJObject::getClass();
	if(classInfo){
		auto info=classInfo->getReflectInfo(_mName);
		if(info.isValid()){
			HJMetaType::convert(_value.getMetaType(),_value.getData(),info.metaType,info.relocate(this));
			return true;
		}else{
			return false;
		}
	}else{
		std::cerr<<"ERROR: can't set value, because the classInfo of class HJObject is nullptr!"<<std::endl;
		return false;
	}
}

HJVariant HJObject::getValue(const std::string &_mName) const {
	auto classInfo=HJObject::getClass();
	if(classInfo){
		auto info=classInfo->getReflectInfo(_mName);
		if(info.isValid()){
			return {info.metaType,info.relocate(this)};
		}else{
			return {};
		}
	}else{
		std::cerr<<"ERROR: can't get value in HJVariant, because the classInfo of class HJObject is nullptr!"<<std::endl;
		return {};
	}
}

//-----------real contain of HJObject --------------
HJObject* HJObject::instantiate(void *_parameter) {
	return new HJObject();
}
HJObject* HJObject::instantiateObject(const std::string& _className, void* _parameter){
	return HJClass::instantiate(_className,_parameter);
}