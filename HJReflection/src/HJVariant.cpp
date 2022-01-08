//
// Created by 何振邦 on 2021/11/22.
//

#include "HJVariant.h"
#include <map>
#include <utility>
#define TYPE_STATIC_CAST(fromType,toType) *reinterpret_cast<toType*>(to)=static_cast<toType>(*reinterpret_cast<const fromType*>(from));
#define TYPE_STATIC_CAST_AND_RETURN_TURE(fromType,toType) \
		TYPE_STATIC_CAST(fromType,toType) \
		return true;

#define CONVERT_FUNC(name,type) \
bool convertTo##name(HJMetaType::Type fromType, const void *from, void *to){ \
	if(HJMetaType::INT==fromType){ \
		TYPE_STATIC_CAST_AND_RETURN_TURE(int,type) \
	}else if(HJMetaType::UINT==fromType){ \
		TYPE_STATIC_CAST_AND_RETURN_TURE(unsigned int,type) \
	}else if(HJMetaType::LONG==fromType){ \
		TYPE_STATIC_CAST_AND_RETURN_TURE(long,type) \
	}else if(HJMetaType::ULONG==fromType){ \
		TYPE_STATIC_CAST_AND_RETURN_TURE(unsigned long,type) \
	}else if(HJMetaType::FLOAT==fromType){ \
		TYPE_STATIC_CAST_AND_RETURN_TURE(float,type) \
	}else if(HJMetaType::DOUBLE==fromType){ \
		TYPE_STATIC_CAST_AND_RETURN_TURE(double,type) \
	}else if(HJMetaType::BOOL==fromType){ \
		TYPE_STATIC_CAST_AND_RETURN_TURE(bool,type) \
	}else if(HJMetaType::CHAR==fromType){ \
		TYPE_STATIC_CAST_AND_RETURN_TURE(char,type) \
	}else if(HJMetaType::UCHAR==fromType){ \
		TYPE_STATIC_CAST_AND_RETURN_TURE(unsigned char,type) \
	} \
	return false; \
}

CONVERT_FUNC(INT,int)
CONVERT_FUNC(UINT,unsigned int)
CONVERT_FUNC(LONG,long)
CONVERT_FUNC(ULONG,unsigned long)
CONVERT_FUNC(FLOAT,float)
CONVERT_FUNC(DOUBLE,double )
CONVERT_FUNC(BOOL,bool)
CONVERT_FUNC(CHAR,char)
CONVERT_FUNC(UCHAR,unsigned char)
std::map<HJMetaType::Type,HJMetaType> TypeEnumToHJMetaType{{HJMetaType::INVALID, HJMetaType()}, {HJMetaType::INT, HJMetaType::fromType<int>()},
                                                           {HJMetaType::UINT,    HJMetaType::fromType<unsigned int>()}, {HJMetaType::LONG, HJMetaType::fromType<long>()},
                                                           {HJMetaType::ULONG,   HJMetaType::fromType<unsigned long>()}, {HJMetaType::FLOAT, HJMetaType::fromType<float>()},
                                                           {HJMetaType::DOUBLE,  HJMetaType::fromType<double>()}, {HJMetaType::BOOL, HJMetaType::fromType<bool>()},
                                                           {HJMetaType::CHAR,    HJMetaType::fromType<char>()}, {HJMetaType::UCHAR, HJMetaType::fromType<unsigned char>()}};
std::unordered_map<size_t,std::string> HJMetaType::typeNameMap{{typeid(int).hash_code(), typeid(int).name()},
                                                               {typeid(unsigned int).hash_code(), typeid(unsigned int).name()},
                                                               {typeid(long).hash_code(), typeid(long).name()},
                                                               {typeid(unsigned long).hash_code(), typeid(unsigned long).name()},
                                                               {typeid(float).hash_code(), typeid(float).name()},
                                                               {typeid(double).hash_code(), typeid(double).name()},
                                                               {typeid(bool).hash_code(), typeid(bool).name()},
                                                               {typeid(char).hash_code(), typeid(char).name()},
                                                               {typeid(unsigned char).hash_code(), typeid(unsigned char).name()}};
HJMetaType HJMetaType::fromType(HJMetaType::Type _type) {
	auto s=TypeEnumToHJMetaType.find(_type);
	if(s != TypeEnumToHJMetaType.end())return s->second;
	else return {};
}
struct ConvertInfo{
	size_t fromHash;
	size_t toHash;
	ConvertInfo()=default;
	ConvertInfo(size_t _fromHash,size_t _toHash):fromHash(_fromHash),toHash(_toHash){}
	bool operator < (const ConvertInfo& ci) const{
		return fromHash==ci.fromHash?toHash<ci.toHash:fromHash<ci.fromHash;
	}
	template<typename FromType,typename ToType> inline static ConvertInfo fromTypes(){
		return {typeid(FromType).hash_code(), typeid(ToType).hash_code()};
	}
};

#define MAKE_STATIC_CONVERTOR(FromType,ToType) {ConvertInfo::fromTypes<FromType,ToType>(),[](const void* from,void* to){TYPE_STATIC_CAST(FromType,ToType)}}

std::map<ConvertInfo, HJMetaType::CONVERT_FUNC> HJMetaTypeConvertorMap{MAKE_STATIC_CONVERTOR(int, unsigned int),
                                                                       MAKE_STATIC_CONVERTOR(int, long),
                                                                       MAKE_STATIC_CONVERTOR(int, unsigned long),
                                                                       MAKE_STATIC_CONVERTOR(int, float),
                                                                       MAKE_STATIC_CONVERTOR(int, double),
                                                                       MAKE_STATIC_CONVERTOR(int, bool),
                                                                       MAKE_STATIC_CONVERTOR(int, char),
                                                                       MAKE_STATIC_CONVERTOR(int, unsigned char),
                                                                       MAKE_STATIC_CONVERTOR(unsigned int, int),
                                                                       MAKE_STATIC_CONVERTOR(unsigned int, long),
                                                                       MAKE_STATIC_CONVERTOR(unsigned int,
                                                                                             unsigned long),
                                                                       MAKE_STATIC_CONVERTOR(unsigned int, float),
                                                                       MAKE_STATIC_CONVERTOR(unsigned int, double),
                                                                       MAKE_STATIC_CONVERTOR(unsigned int, bool),
                                                                       MAKE_STATIC_CONVERTOR(unsigned int, char),
                                                                       MAKE_STATIC_CONVERTOR(unsigned int,
                                                                                             unsigned char),
                                                                       MAKE_STATIC_CONVERTOR(long, int),
                                                                       MAKE_STATIC_CONVERTOR(long, unsigned int),
                                                                       MAKE_STATIC_CONVERTOR(long, unsigned long),
                                                                       MAKE_STATIC_CONVERTOR(long, float),
                                                                       MAKE_STATIC_CONVERTOR(long, double),
                                                                       MAKE_STATIC_CONVERTOR(long, bool),
                                                                       MAKE_STATIC_CONVERTOR(long, char),
                                                                       MAKE_STATIC_CONVERTOR(long, unsigned char),
                                                                       MAKE_STATIC_CONVERTOR(unsigned long, int),
                                                                       MAKE_STATIC_CONVERTOR(unsigned long,
                                                                                             unsigned int),
                                                                       MAKE_STATIC_CONVERTOR(unsigned long, long),
                                                                       MAKE_STATIC_CONVERTOR(unsigned long, float),
                                                                       MAKE_STATIC_CONVERTOR(unsigned long, double),
                                                                       MAKE_STATIC_CONVERTOR(unsigned long, bool),
                                                                       MAKE_STATIC_CONVERTOR(unsigned long, char),
                                                                       MAKE_STATIC_CONVERTOR(unsigned long,
                                                                                             unsigned char),
                                                                       MAKE_STATIC_CONVERTOR(float, int),
                                                                       MAKE_STATIC_CONVERTOR(float, unsigned int),
                                                                       MAKE_STATIC_CONVERTOR(float, long),
                                                                       MAKE_STATIC_CONVERTOR(float, unsigned long),
                                                                       MAKE_STATIC_CONVERTOR(float, double),
                                                                       MAKE_STATIC_CONVERTOR(float, bool),
                                                                       MAKE_STATIC_CONVERTOR(float, char),
                                                                       MAKE_STATIC_CONVERTOR(float, unsigned char),
                                                                       MAKE_STATIC_CONVERTOR(double, int),
                                                                       MAKE_STATIC_CONVERTOR(double, unsigned int),
                                                                       MAKE_STATIC_CONVERTOR(double, long),
                                                                       MAKE_STATIC_CONVERTOR(double, unsigned long),
                                                                       MAKE_STATIC_CONVERTOR(double, float),
                                                                       MAKE_STATIC_CONVERTOR(double, bool),
                                                                       MAKE_STATIC_CONVERTOR(double, char),
                                                                       MAKE_STATIC_CONVERTOR(double, unsigned char),
                                                                       MAKE_STATIC_CONVERTOR(bool, int),
                                                                       MAKE_STATIC_CONVERTOR(bool, unsigned int),
                                                                       MAKE_STATIC_CONVERTOR(bool, long),
                                                                       MAKE_STATIC_CONVERTOR(bool, unsigned long),
                                                                       MAKE_STATIC_CONVERTOR(bool, float),
                                                                       MAKE_STATIC_CONVERTOR(bool, double),
                                                                       MAKE_STATIC_CONVERTOR(bool, char),
                                                                       MAKE_STATIC_CONVERTOR(bool, unsigned char),
                                                                       MAKE_STATIC_CONVERTOR(char, int),
                                                                       MAKE_STATIC_CONVERTOR(char, unsigned int),
                                                                       MAKE_STATIC_CONVERTOR(char, long),
                                                                       MAKE_STATIC_CONVERTOR(char, unsigned long),
                                                                       MAKE_STATIC_CONVERTOR(char, float),
                                                                       MAKE_STATIC_CONVERTOR(char, double),
                                                                       MAKE_STATIC_CONVERTOR(char, bool),
                                                                       MAKE_STATIC_CONVERTOR(char, unsigned char),
                                                                       MAKE_STATIC_CONVERTOR(unsigned char, int),
                                                                       MAKE_STATIC_CONVERTOR(unsigned char,
                                                                                             unsigned int),
                                                                       MAKE_STATIC_CONVERTOR(unsigned char, long),
                                                                       MAKE_STATIC_CONVERTOR(unsigned char,
                                                                                             unsigned long),
                                                                       MAKE_STATIC_CONVERTOR(unsigned char, float),
                                                                       MAKE_STATIC_CONVERTOR(unsigned char, double),
                                                                       MAKE_STATIC_CONVERTOR(unsigned char, bool),
                                                                       MAKE_STATIC_CONVERTOR(unsigned char, char)};
void HJMetaType::registerConvertFunc(size_t fromHash, size_t toHash, CONVERT_FUNC func) {
	ConvertInfo convertInfo(fromHash,toHash);
	auto s=HJMetaTypeConvertorMap.find(convertInfo);
	if(s!=HJMetaTypeConvertorMap.end())
		std::cout<<"Warning. overwrite existing convertor function for converting "<<fromHash<<" to "<<toHash<<std::endl;
	HJMetaTypeConvertorMap[convertInfo]=std::move(func);
}
//todo. refactor this convert function
bool HJMetaType::convert(HJMetaType fromType, const void *from, HJMetaType toType, void *to) {
	if(fromType.type==INVALID||toType.type==INVALID)return false;
	else if(fromType==toType){
		memcpy(to,from,toType.size);
		return true;
	}
	ConvertInfo convertInfo(fromType.hashCode,toType.hashCode);
	auto s=HJMetaTypeConvertorMap.find(convertInfo);
	if(s!=HJMetaTypeConvertorMap.end()){
		s->second(from,to);
		return true;
	}else return false;

/*	if(toType.type==HJMetaType::INT){
		convertToINT(fromType.type,from,to);
	}else if(toType.type==HJMetaType::UINT){
		convertToUINT(fromType.type,from,to);
	}else if(toType.type==HJMetaType::LONG){
		convertToLONG(fromType.type,from,to);
	}else if(toType.type==HJMetaType::ULONG){
		convertToULONG(fromType.type,from,to);
	}else if(toType.type==HJMetaType::FLOAT){
		convertToFLOAT(fromType.type,from,to);
	}else if(toType.type==HJMetaType::DOUBLE){
		convertToDOUBLE(fromType.type,from,to);
	}else if(toType.type==HJMetaType::BOOL){
		convertToBOOL(fromType.type,from,to);
	}else if(toType.type==HJMetaType::CHAR){
		convertToCHAR(fromType.type,from,to);
	}else if(toType.type==HJMetaType::UCHAR){
		convertToUCHAR(fromType.type,from,to);
	}
	return false;*/
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
void HJVariant::getValue(void *_pValue, HJMetaType _metaType) const {
	const void* realSpace=useInternalSpace?&data.internal:data.allocated;
	if(_metaType==metaType){
		memcpy(_pValue, realSpace, metaType.getSize());
	}else{
		HJMetaType::convert(metaType, realSpace, _metaType, _pValue);
	}
}

void HJVariant::setValue(const void *_pValue, HJMetaType _metaType) {
	const auto targetSize=_metaType.getSize();
	metaType=_metaType;
	//space usage priority
	//1. reuse allocated space; 2. use internal space; 3. allocate new space;
	if(targetSize<=allocatedSize){
		useInternalSpace=false;
		memcpy(data.allocated, _pValue, targetSize);
	}else if(targetSize <= HJVARIANT_MAX_INTERNAL_BUFFER_SIZE){
		freeAllocated();
		useInternalSpace=true;
		memcpy(&data.internal, _pValue, targetSize);
	}else{//not enough allocated space
		freeAllocated();
		allocate(targetSize);
		useInternalSpace=false;
		memcpy(data.allocated, _pValue, targetSize);
	}
}