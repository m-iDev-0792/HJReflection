#ifndef REFLECTION_HJVARIANT_H
#define REFLECTION_HJVARIANT_H

#include <type_traits>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <unordered_map>
#include <functional>
#include <utility>

class HJMetaType{
public:
	using CONVERT_FUNC=std::function<void(const void*,void*)>;
	enum Type{
		INVALID,
		INT,
		UINT,
		LONG,
		ULONG,
		FLOAT,
		DOUBLE,
		BOOL,
		CHAR,
		UCHAR,
		CUSTOM
	};

	static bool convert(HJMetaType fromType,const void* from,HJMetaType toType, void* to);

	//Get HJMetaType::Type via input metaType T.
	//The target platform is set to 64bit machine, be careful with the metaType size in other platforms!
	template<typename T> static Type getHJMetaTypeEnum(){
		using DT=std::decay_t<T>;
		if(std::is_same_v<DT,int>)return HJMetaType::INT;
		if(std::is_same_v<DT,unsigned int>)return HJMetaType::UINT;
		else if(std::is_same_v<DT,long>)return HJMetaType::LONG;
		else if(std::is_same_v<DT,unsigned long>)return HJMetaType::ULONG;
		else if(std::is_same_v<DT,float>)return HJMetaType::FLOAT;
		else if(std::is_same_v<DT,double>)return HJMetaType::DOUBLE;
		else if(std::is_same_v<DT,bool>)return HJMetaType::BOOL;
		else if(std::is_same_v<DT,char>)return HJMetaType::CHAR;
		else if(std::is_same_v<DT,unsigned char>)return HJMetaType::UCHAR;
		else return HJMetaType::CUSTOM;
	}

	//Create a HJMetaType from a type T. A valid HJMetaType should only be created from this function.
	template<typename T> static inline HJMetaType fromType(){
		using DT=std::decay_t<T>;
		auto nameIter=typeNameMap.find(typeid(DT).hash_code());
		if(nameIter==typeNameMap.end())typeNameMap[typeid(DT).hash_code()]= typeid(DT).name();
		return HJMetaType(getHJMetaTypeEnum<DT>(),sizeof(DT), typeid(DT).hash_code());
	}
	static HJMetaType fromType(Type _type);

	HJMetaType(){
		type=HJMetaType::INVALID;
		size=0;
		hashCode=0;
	}
	bool operator == (const HJMetaType& t) const{
		return hashCode==t.hashCode;
	}
	bool operator != (const HJMetaType& t) const{
		return !(*this==t);
	}
	template<typename T> inline bool isType(){
		return this->hashCode== typeid(T).hash_code();
	}
	inline size_t getSize()const{return size;}
	inline Type getType()const{return type;}
	inline size_t getHashCode()const {return hashCode;}
	//This function is provided for debugging, the type name isn't same as type text
	inline std::string getName()const {
		auto nameIter=typeNameMap.find(hashCode);
		if(nameIter!=typeNameMap.end())return nameIter->second;
		else return "INVALID";
	}

	template<typename FromType,typename ToType> static inline void registerConvertFunc(CONVERT_FUNC func){
		registerConvertFunc(typeid(FromType).hash_code(), typeid(ToType).hash_code(), std::move(func));
	}
	static void registerConvertFunc(size_t fromHash,size_t toHash,CONVERT_FUNC func);
private:
	HJMetaType(Type _t,size_t _s,size_t _hc):type(_t),size(_s),hashCode(_hc){}
private:
	static std::unordered_map<size_t,std::string> typeNameMap;
private:
	size_t size;//size in byte
	size_t hashCode;//0 means invalid type
	Type type;
};

#define HJVARIANT_MAX_INTERNAL_BUFFER_SIZE 32


class HJVariant{
private:
	template<typename T> static constexpr bool canUseInternalSpace(){
		return sizeof(T) <= HJVARIANT_MAX_INTERNAL_BUFFER_SIZE;
	}
	//Free allocated space and reset allocatedSize
	inline void freeAllocated(){
		if(allocatedSize>0){
			free(data.allocated);
			allocatedSize=0;
		}
	}
	//Allocated space to store data, this function doesn't set useInternalSpace=false
	inline void allocate(size_t _size){
		freeAllocated();
		data.allocated= malloc(_size);
		allocatedSize=_size;
	}

public:
	//Get value in target metaType safely, return a default value of metaType T when convert fail
	template<typename T>
	T getValue() const {
		//check whether sizeof(T) is larger than used size?
		auto targetType=HJMetaType::fromType<T>();
		const void* realSpace=useInternalSpace?&data.internal:data.allocated;
		if(metaType == targetType){
			return *reinterpret_cast<const T*>(realSpace);
		}
		T t{};
		HJMetaType::convert(metaType, realSpace, targetType, &t);	//try to convert
		return t;
	}
	inline void getValue(void* _pValue) const {
		getValue(_pValue,metaType);
	}
	void getValue(void* _pValue, HJMetaType _metaType) const;

	//Get value reference in target metaType without typecast (unsafely)
	template<typename T> T& getValueRef(){
		if(sizeof(T) > metaType.getSize())std::cout << "HJVariant::getValueRef warning. "
		                                               "trying to directly cast to a metaType that larger than current metaType which may cause memory access error" << std::endl;
		return useInternalSpace?*reinterpret_cast<T*>(&data.internal):*reinterpret_cast<T*>(data.allocated);
	}
	//Set value and change metaType at same time
	template<typename T>
	void setValue(T &&_value) {
		using DT=std::decay_t<T>;
		auto _metaType=HJMetaType::fromType<DT>();
		const auto targetSize=_metaType.getSize();
		metaType=_metaType;
		//space usage priority
		//1. reuse allocated space; 2. use internal space; 3. allocate new space;
		if(targetSize<=allocatedSize){
			useInternalSpace=false;
			*reinterpret_cast<DT*>(data.allocated)=_value;
		}else if(canUseInternalSpace<DT>()){
			freeAllocated();
			useInternalSpace=true;
			*reinterpret_cast<DT*>(&data.internal)=_value;
		}else{//not enough allocated space
			freeAllocated();
			allocate(targetSize);
			useInternalSpace=false;
			*reinterpret_cast<DT*>(data.allocated)=_value;
		}
	}
	void setValue(const void* _pValue, HJMetaType _metaType);

	//Get value via reinterpret cast (unsafely)
	template<class T> T reinterpret() const{
		if(sizeof(T) > metaType.getSize())std::cout << "HJVariant::reinterpret warning. "
	    "trying to directly cast to a metaType that larger than current metaType which may cause memory access error" << std::endl;
		return useInternalSpace?*reinterpret_cast<const T*>(&data.internal):*reinterpret_cast<const T*>(data.allocated);
	}
	inline bool isValid() const{
		return metaType.getType() != HJMetaType::INVALID;
	}
	inline HJMetaType getMetaType()const{
		return metaType;
	}
	bool convertTo(HJMetaType _newMetaType);
	bool convertTo(HJMetaType::Type _newType);
	template<typename T> bool convertTo(){
		T t{};
		const void* realSpace=useInternalSpace?&data.internal:data.allocated;
		if(HJMetaType::convert(metaType,realSpace,HJMetaType::fromType<T>(),&t)){
			setValue(t);
			return true;
		}else return false;
	}
	//
	inline void convertToForce(HJMetaType::Type _newType){
		convertToForce(HJMetaType::fromType(_newType));
	}
	void convertToForce(HJMetaType _newMetaType){
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
	template<typename T> inline void convertToForce(){
		if(!convertTo<T>())setValue(T{});
	}

	HJVariant();
	HJVariant(const HJVariant& v);
	HJVariant(HJVariant&& v);
	explicit HJVariant(int _value){ setValue(_value);}
	explicit HJVariant(unsigned int _value){ setValue(_value);}
	explicit HJVariant(long _value){ setValue(_value);}
	explicit HJVariant(unsigned long _value){ setValue(_value);}
	explicit HJVariant(float _value){ setValue(_value);}
	explicit HJVariant(double _value){ setValue(_value);}
	explicit HJVariant(bool _value){ setValue(_value);}
	explicit HJVariant(char _value){ setValue(_value);}
	HJVariant(HJMetaType _metaType,const void* _pvalue){
		setValue(_pvalue,_metaType);
	}

	template<typename T> static inline HJVariant fromValue(T _value){
		return HJVariant(HJMetaType::fromType<T>(),std::addressof(_value));
	}
	~HJVariant();
	//directly copy, without type conversion
	HJVariant& operator = (const HJVariant& v);
	//directly move, without type conversion
	HJVariant& operator = (HJVariant&& v) noexcept ;
	//Simply Compare whether two HJVariants hold same value, but not compare their values are equal
	//If you really want to compare the value of two HJVariant, do v1.getValue<T>() == v2.getValue<T>()
	bool operator == (const HJVariant& v) const;
	inline bool operator != (const HJVariant& v) const{
		return !(*this==v);
	}
private:
	union{
		unsigned char internal[HJVARIANT_MAX_INTERNAL_BUFFER_SIZE];
		void* allocated;
	} data;
	HJMetaType metaType;
	bool useInternalSpace=true;
	size_t allocatedSize=0;//allocated size may not be equal to metaType.getSize()
};

#define HJV(value) HJVariant::fromValue(value)

#endif //REFLECTION_HJVARIANT_H