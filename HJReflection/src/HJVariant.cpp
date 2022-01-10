//
// Created by 何振邦 on 2021/11/22.
//

#include "HJVariant.h"
#include <map>
#include <utility>
#define TYPE_STATIC_CAST(fromType,toType) *reinterpret_cast<toType*>(to)=static_cast<toType>(*reinterpret_cast<const fromType*>(from));
#define MAKE_STATIC_CONVERTOR(FromType,ToType) HJMetaType::getConvertorMap()[ConvertorInfo::fromTypes<FromType,ToType>()]=[](const void* from,void* to){TYPE_STATIC_CAST(FromType,ToType)};

[[maybe_unused]] struct ConvertorInstantiateHelper{
	ConvertorInstantiateHelper(){
		MAKE_STATIC_CONVERTOR(int, unsigned int)
		MAKE_STATIC_CONVERTOR(int, long)
		MAKE_STATIC_CONVERTOR(int, unsigned long)
		MAKE_STATIC_CONVERTOR(int, float)
		MAKE_STATIC_CONVERTOR(int, double)
		MAKE_STATIC_CONVERTOR(int, bool)
		MAKE_STATIC_CONVERTOR(int, char)
		MAKE_STATIC_CONVERTOR(int, unsigned char)
		MAKE_STATIC_CONVERTOR(unsigned int, int)
		MAKE_STATIC_CONVERTOR(unsigned int, long)
		MAKE_STATIC_CONVERTOR(unsigned int, unsigned long)
		MAKE_STATIC_CONVERTOR(unsigned int, float)
		MAKE_STATIC_CONVERTOR(unsigned int, double)
		MAKE_STATIC_CONVERTOR(unsigned int, bool)
		MAKE_STATIC_CONVERTOR(unsigned int, char)
		MAKE_STATIC_CONVERTOR(unsigned int, unsigned char)
		MAKE_STATIC_CONVERTOR(long, int)
		MAKE_STATIC_CONVERTOR(long, unsigned int)
		MAKE_STATIC_CONVERTOR(long, unsigned long)
		MAKE_STATIC_CONVERTOR(long, float)
		MAKE_STATIC_CONVERTOR(long, double)
		MAKE_STATIC_CONVERTOR(long, bool)
		MAKE_STATIC_CONVERTOR(long, char)
		MAKE_STATIC_CONVERTOR(long, unsigned char)
		MAKE_STATIC_CONVERTOR(unsigned long, int)
		MAKE_STATIC_CONVERTOR(unsigned long, unsigned int)
		MAKE_STATIC_CONVERTOR(unsigned long, long)
		MAKE_STATIC_CONVERTOR(unsigned long, float)
		MAKE_STATIC_CONVERTOR(unsigned long, double)
		MAKE_STATIC_CONVERTOR(unsigned long, bool)
		MAKE_STATIC_CONVERTOR(unsigned long, char)
		MAKE_STATIC_CONVERTOR(unsigned long, unsigned char)
		MAKE_STATIC_CONVERTOR(float, int)
		MAKE_STATIC_CONVERTOR(float, unsigned int)
		MAKE_STATIC_CONVERTOR(float, long)
		MAKE_STATIC_CONVERTOR(float, unsigned long)
		MAKE_STATIC_CONVERTOR(float, double)
		MAKE_STATIC_CONVERTOR(float, bool)
		MAKE_STATIC_CONVERTOR(float, char)
		MAKE_STATIC_CONVERTOR(float, unsigned char)
		MAKE_STATIC_CONVERTOR(double, int)
		MAKE_STATIC_CONVERTOR(double, unsigned int)
		MAKE_STATIC_CONVERTOR(double, long)
		MAKE_STATIC_CONVERTOR(double, unsigned long)
		MAKE_STATIC_CONVERTOR(double, float)
		MAKE_STATIC_CONVERTOR(double, bool)
		MAKE_STATIC_CONVERTOR(double, char)
		MAKE_STATIC_CONVERTOR(double, unsigned char)
		MAKE_STATIC_CONVERTOR(bool, int)
		MAKE_STATIC_CONVERTOR(bool, unsigned int)
		MAKE_STATIC_CONVERTOR(bool, long)
		MAKE_STATIC_CONVERTOR(bool, unsigned long)
		MAKE_STATIC_CONVERTOR(bool, float)
		MAKE_STATIC_CONVERTOR(bool, double)
		MAKE_STATIC_CONVERTOR(bool, char)
		MAKE_STATIC_CONVERTOR(bool, unsigned char)
		MAKE_STATIC_CONVERTOR(char, int)
		MAKE_STATIC_CONVERTOR(char, unsigned int)
		MAKE_STATIC_CONVERTOR(char, long)
		MAKE_STATIC_CONVERTOR(char, unsigned long)
		MAKE_STATIC_CONVERTOR(char, float)
		MAKE_STATIC_CONVERTOR(char, double)
		MAKE_STATIC_CONVERTOR(char, bool)
		MAKE_STATIC_CONVERTOR(char, unsigned char)
		MAKE_STATIC_CONVERTOR(unsigned char, int)
		MAKE_STATIC_CONVERTOR(unsigned char, unsigned int)
		MAKE_STATIC_CONVERTOR(unsigned char, long)
		MAKE_STATIC_CONVERTOR(unsigned char, unsigned long)
		MAKE_STATIC_CONVERTOR(unsigned char, float)
		MAKE_STATIC_CONVERTOR(unsigned char, double)
		MAKE_STATIC_CONVERTOR(unsigned char, bool)
		MAKE_STATIC_CONVERTOR(unsigned char, char)
	}
} ConvertorInstantiateHelperInstance;

HJMetaType HJMetaType::fromType(HJMetaType::Type _type) {
	if(HJMetaType::INT==_type) return HJMetaType::fromType<int>();
	else if(HJMetaType::UINT==_type) return HJMetaType::fromType<unsigned int>();
	else if(HJMetaType::LONG==_type) return HJMetaType::fromType<long>();
	else if(HJMetaType::ULONG==_type) return HJMetaType::fromType<unsigned long>();
	else if(HJMetaType::FLOAT==_type) return HJMetaType::fromType<float>();
	else if(HJMetaType::DOUBLE==_type) return HJMetaType::fromType<double>();
	else if(HJMetaType::CHAR==_type) return HJMetaType::fromType<char>();
	else if(HJMetaType::UCHAR==_type) return HJMetaType::fromType<unsigned char>();
	else if(HJMetaType::BOOL==_type) return HJMetaType::fromType<bool>();
	else return {};
}
void HJMetaType::registerConvertFunc(size_t fromHash, size_t toHash, HJMetaType::CONVERT_FUNC_TYPE func) {
	ConvertorInfo convertInfo(fromHash, toHash);
	auto& convertorMap=getConvertorMap();
	auto s=convertorMap.find(convertInfo);
	if(s != convertorMap.end())
		std::cout<<"Warning. overwrite existing convertor function for converting "<<fromHash<<" to "<<toHash<<std::endl;
	convertorMap[convertInfo]=std::move(func);
}
bool HJMetaType::assign(HJMetaType type, const void *from, void *to) {
	auto& convertorMap=getConvertorMap();
	auto iter=convertorMap.find(ConvertorInfo(type.hashCode, type.hashCode));
	if(iter!=convertorMap.end()){
		iter->second(from,to);
		return true;
	}else{
		memcpy(to,from,type.size);//memcpy may cause shallow copy
		return false;
	}
}
bool HJMetaType::convert(HJMetaType fromType, const void *from, HJMetaType toType, void *to) {
	if(fromType.type==INVALID||toType.type==INVALID)return false;
	else if(fromType==toType){
		HJMetaType::assign(fromType,from,to);
		return true;
	}else{
		ConvertorInfo convertInfo(fromType.hashCode, toType.hashCode);
		auto& convertorMap=getConvertorMap();
		auto s=convertorMap.find(convertInfo);
		if(s != convertorMap.end()){
			s->second(from,to);
			return true;
		}else return false;
	}
}


HJVariant::HJVariant() {
}
HJVariant::HJVariant(const HJVariant &v) {
	*this=v;
}
HJVariant::HJVariant(HJVariant&& v){
	metaType=v.metaType;
	if(v.isValid()){
		freeAllocated();
		data=v.data;
		allocatedSize=v.allocatedSize;
		useInternalSpace = v.useInternalSpace;
		if(!useInternalSpace){
			v.data.allocated= nullptr;
			v.allocatedSize=0;
			v.useInternalSpace=true;
		}
		v.metaType=HJMetaType();//set to invalid type
	}
}
HJVariant::~HJVariant() {
	freeAllocated();
}
HJVariant& HJVariant::operator=(const HJVariant &v) {
	if (this == &v)return *this;
	metaType=v.metaType;
	if(v.isValid()){
		auto targetSize=metaType.getSize();
		if(v.useInternalSpace){
			freeAllocated();
			useInternalSpace=true;
			memcpy(&data.internal,&v.data.internal,targetSize);
		}else if(targetSize <= HJVARIANT_MAX_INTERNAL_BUFFER_SIZE){
			freeAllocated();
			useInternalSpace=true;
			memcpy(&data.internal,v.data.allocated,targetSize);
		}else {
			useInternalSpace=false;
			if(allocatedSize<targetSize){//not enough size, allocate new space
				freeAllocated();
				allocate(targetSize);
			}
			memcpy(data.allocated,v.data.allocated,targetSize);
		}
	}
	return *this;
}
HJVariant& HJVariant::operator=(HJVariant &&v)  noexcept {
	if (this == &v)return *this;
	metaType=v.metaType;
	if(v.isValid()){
		freeAllocated();
		data=v.data;
		allocatedSize=v.allocatedSize;
		useInternalSpace=v.useInternalSpace;
		if(!v.useInternalSpace){
			v.data.allocated= nullptr;
			v.allocatedSize=0;
			v.useInternalSpace=true;
		}
		v.metaType=HJMetaType();//set to invalid type
	}
	return *this;
}
bool HJVariant::operator==(const HJVariant &v) const {
	if(metaType!=v.metaType)return false;
	else{
		const void* realSpace=useInternalSpace?&data.internal:data.allocated;
		const void* realSpaceV=v.useInternalSpace?&v.data.internal:v.data.allocated;
		return memcmp(realSpace,realSpaceV,metaType.getSize())==0;
	}
}
bool HJVariant::convertTo(HJMetaType _newMetaType) {
	if(_newMetaType.getType()==HJMetaType::INVALID)return false;
	else if(_newMetaType.getType() != HJMetaType::CUSTOM)//convert to basic type
		return convertTo(_newMetaType.getType());
	else{
		auto tempSpace= malloc(_newMetaType.getSize());
		const void* realSpace=useInternalSpace?&data.internal:data.allocated;
		if(HJMetaType::convert(metaType, realSpace, _newMetaType, tempSpace)){
			freeAllocated();
			useInternalSpace=false;
			data.allocated=tempSpace;//move allocated tempSpace
			allocatedSize=_newMetaType.getSize();
			metaType=_newMetaType;
			return true;
		}else{
			free(tempSpace);
			return false;
		}
	}
}
bool HJVariant::convertTo(HJMetaType::Type _newType) {
	if(!isValid()||HJMetaType::INVALID==_newType)return false;
	if(_newType==HJMetaType::INT){
		return convertTo<int>();
	}else if(_newType==HJMetaType::UINT){
		return convertTo<unsigned int>();
	}else if(_newType==HJMetaType::LONG){
		return convertTo<long>();
	}else if(_newType==HJMetaType::ULONG){
		return convertTo<unsigned long>();
	}else if(_newType==HJMetaType::FLOAT){
		return convertTo<float>();
	}else if(_newType==HJMetaType::DOUBLE){
		return convertTo<double>();
	}else if(_newType==HJMetaType::BOOL){
		return convertTo<bool>();
	}else if(_newType==HJMetaType::CHAR){
		return convertTo<char>();
	}else if(_newType==HJMetaType::UCHAR){
		return convertTo<unsigned char>();
	}else return false;
}

void HJVariant::convertToForce(HJMetaType::Type _newType) {
	if(!isValid()||HJMetaType::INVALID==_newType)return;
	if(_newType==HJMetaType::INT){
		convertToForce<int>();
	}else if(_newType==HJMetaType::UINT){
		convertToForce<unsigned int>();
	}else if(_newType==HJMetaType::LONG){
		convertToForce<long>();
	}else if(_newType==HJMetaType::ULONG){
		convertToForce<unsigned long>();
	}else if(_newType==HJMetaType::FLOAT){
		convertToForce<float>();
	}else if(_newType==HJMetaType::DOUBLE){
		convertToForce<double>();
	}else if(_newType==HJMetaType::BOOL){
		convertToForce<bool>();
	}else if(_newType==HJMetaType::CHAR){
		convertToForce<char>();
	}else if(_newType==HJMetaType::UCHAR){
		convertToForce<unsigned char>();
	}else{
		std::cout<<"Can't convert to a custom type via convertToForce(HJMetaType::Type)"<<std::endl;
	}
}
void HJVariant::convertToForce(HJMetaType _newMetaType){
	if(!convertTo(_newMetaType)){//failed to convert, just adjust space
		if(_newMetaType.getSize()<=HJVARIANT_MAX_INTERNAL_BUFFER_SIZE){
			freeAllocated();
			useInternalSpace=true;
		}else if(allocatedSize<_newMetaType.getSize()){
			allocate(_newMetaType.getSize());
			useInternalSpace=false;
		}
		//memset realSpace to 0 here?
	}
	metaType=_newMetaType;
}
void HJVariant::getValue(void *_pValue, HJMetaType _metaType) const {
	const void* realSpace=useInternalSpace?&data.internal:data.allocated;
	if(_metaType==metaType){
		memcpy(_pValue, realSpace, metaType.getSize());
	}else{
		HJMetaType::convert(metaType, realSpace, _metaType, _pValue);
	}
}