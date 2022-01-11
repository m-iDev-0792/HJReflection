//
// Created by 何振邦 on 2021/10/3.
//

#ifndef REFLECTION_HJCLASS_H
#define REFLECTION_HJCLASS_H

#include "HJVariant.h"
#include <utility>
#include <vector>
//#define REFLECT_INFO_RELOCATE_VIA_OFFSET
struct ReflectInfo{
	using RELOCATE_TYPE=std::function<void*(void*)>;
	HJMetaType metaType;
	std::string name;
	long offset=0;
	RELOCATE_TYPE relocator;
	ReflectInfo(const std::string& _name,long _offset,const HJMetaType& _metaType):
	name(_name),offset(_offset),metaType(_metaType){}
	ReflectInfo(const std::string& _name,long _offset,const HJMetaType& _metaType,RELOCATE_TYPE _relocator):
			name(_name),offset(_offset),metaType(_metaType),relocator(std::move(_relocator)){}
	ReflectInfo()=default;
	inline bool isValid()const{
		return metaType.getType()!=HJMetaType::INVALID && !name.empty();
	}
	inline void* relocate(void* _pThis)const{
#ifdef REFLECT_INFO_RELOCATE_VIA_OFFSET
		return reinterpret_cast<char*>(_pThis)+offset;
#else
		return relocator(_pThis);
#endif
	}
	inline const void* relocate(const void* _pThis)const{
#ifdef REFLECT_INFO_RELOCATE_VIA_OFFSET
		return reinterpret_cast<const char*>(_pThis)+offset;
#else
		return relocator(const_cast<void*>(_pThis));
#endif
	}
};
#define MAKE_REFLECT_INFO(name,type,classType) ReflectInfo(#name,offsetof(classType,name),HJMetaType::fromType<type>(),[](void* _pThis)->void*{return &reinterpret_cast<classType*>(_pThis)->name;})
#define MAKE_REFLECT_INFO_NO_RELOCATOR(name,type,classType) ReflectInfo(#name,offsetof(classType,name),HJMetaType::fromType<type>())

class HJObject;
class HJClass {
	friend HJObject;
public:
	using InstatiateFuncType=HJObject* (*)(void*);
	InstatiateFuncType instantiateFunc;
	HJClass(const std::string &_name, const std::vector<ReflectInfo> &_reflectInfos,
	        const std::string &_superClassName, InstatiateFuncType _instFuncPtr=nullptr);
	
	inline const std::string& getName() const{ return name;}
	inline const std::string& getSuperName() const {return superClassName;}
	inline const HJClass* getSuperClass() const{
		auto& registeredClassesMap=HJClass::getClassMap();
		auto s=registeredClassesMap.find(superClassName);
		return s==registeredClassesMap.end()? nullptr:s->second;
	}

	inline ReflectInfo getReflectInfo(const std::string& _name) const{
		auto search=reflectInfosMap.find(_name);
		if(search!=reflectInfosMap.end())return search->second;
		else return {};
	}
	inline size_t getReflectInfoNum() const {return reflectInfosMap.size();}
	std::vector<ReflectInfo> getAllReflectInfo() const{
		std::vector<ReflectInfo> temp;
		for(auto r:reflectInfosMap){
			temp.emplace_back(r.second);
		}
		return temp;
	}

	inline bool isInstantiable() const{return nullptr!=instantiateFunc;}
	bool isSubClass(const std::string& _super)const;

	//for instantiate Object via name
	static HJObject* instantiate(const std::string& _className, void* _parameter);
	static const HJClass* getClassInstance(const std::string& _className);
	static bool registerClass(const std::string& _className,const HJClass* _class);
	static bool unregisterClass(const std::string& _className);
protected:
	static std::map<std::string,const HJClass*>& getClassMap(){
		//Save all class info of all the subclasses of HJObject
		//registeredClassesMap will be filled before main function!
		static std::map<std::string,const HJClass*> registeredClassesMap;
		return registeredClassesMap;
	}
	std::string name;
	std::string superClassName;

	//Record reflection info for a certain class derived from HJObject
	std::map<std::string,ReflectInfo> reflectInfosMap;
};


#endif //REFLECTION_HJCLASS_H
