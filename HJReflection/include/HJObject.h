//
// Created by 何振邦 on 2021/10/3.
//

#ifndef REFLECTION_HJOBJECT_H
#define REFLECTION_HJOBJECT_H

#include "HJClass.h"
#define HJOBJECT __attribute__((annotate("HJObject")))
#define HJPROPERTY __attribute__((annotate("HJProperty")))

//Following macros insert automatically generated codes for HJObject
#define HJOBJECT_GET_CLASS_INFO(CLASSNAME,SUPERCLASSNAME) \
std::string CLASSNAME::getClassName() const { \
	return #CLASSNAME; \
} \
const HJClass* CLASSNAME::getClass() const { \
	return HJClass::getClassInstance(#CLASSNAME); \
} \
std::string CLASSNAME::getSuperName() const { \
	return #SUPERCLASSNAME; \
} \
const HJClass* CLASSNAME::getSuperClass() const { \
	return HJClass::getClassInstance(#SUPERCLASSNAME); \
}

#define HJOBJECT_SETVALUE_IMPL(CLASSNAME,SUPERCLASSNAME) \
bool CLASSNAME::setValue(const std::string& _mName, const HJVariant& _value){ \
    auto classInfo=CLASSNAME::getClass(); \
	if(classInfo){ \
		auto info=classInfo->getReflectInfo(_mName); \
		if(info.isValid()){ \
			HJMetaType::convert(_value.getMetaType(),_value.getData(),info.metaType,reinterpret_cast<char*>(this)+info.offset); \
			return true; \
		}else{ \
			SUPERCLASSNAME::setValue(_mName,_value); \
		} \
	}else{ \
		std::cerr<<"ERROR: can't set value, because the classInfo of class "<<#CLASSNAME<<" is nullptr!"<<std::endl; \
		return false; \
	} \
	return false; \
}

#define HJOBJECT_GETVALUE_IMPL(CLASSNAME,SUPERCLASSNAME) \
HJVariant CLASSNAME::getValue(const std::string& _mName) const { \
    auto classInfo=CLASSNAME::getClass(); \
	if(classInfo){ \
		auto info=classInfo->getReflectInfo(_mName); \
		if(info.isValid()){ \
			return {info.metaType,reinterpret_cast<const char*>(this)+info.offset}; \
		}else{ \
			return SUPERCLASSNAME::getValue(_mName); \
		} \
	}else{ \
		std::cerr<<"ERROR: can't get value in HJVariant, because the classInfo of class "<<#CLASSNAME<<" is nullptr!"<<std::endl; \
		return {}; \
	}                                                       \
	return {};                                              \
}

#define HJOBJECT_MEMBER_ACCESS_IMPL(CLASSNAME,SUPERCLASSNAME) \
	HJOBJECT_SETVALUE_IMPL(CLASSNAME,SUPERCLASSNAME)      \
	HJOBJECT_GETVALUE_IMPL(CLASSNAME,SUPERCLASSNAME)

#define HJOBJECT_BODY \
	public:                                \
		virtual std::string getClassName() const; \
		virtual const HJClass* getClass()const;\
        virtual std::string getSuperName() const; \
        virtual const HJClass* getSuperClass() const; \
        virtual bool setValue(const std::string& _mName, const HJVariant& _value); \
        virtual HJVariant getValue(const std::string& _mName) const; \
	private:

//we will generate a NewObject.generated.h header from any HJObject subclass
//the subclass declare of HJObject must contain macro HJOBJECT_REFLECTION_DECLARE
class HJObject {
	HJOBJECT_BODY
public:
	HJObject()=default;
	static HJObject* instantiateObject(const std::string& _className, void* _parameter);
	//---any sub class of HJObject must implement following functions---
	//virtual bool serialize(const std::string& serializationInfo);	//keep for now
	
	//Called by HJClass::instantiate(const std::string& _className, void* _parameter)
	//instantiate a HJObject subclass instance via parameters(optional)
	static HJObject* instantiate(void* _parameter = nullptr); //User-must-implement
};




#endif //REFLECTION_HJOBJECT_H
