//
// Created by 何振邦 on 2021/12/11.
//

#include "HJReflectionTool.h"
#include "clang-c/Index.h"
#include <set>
#include <regex>
#include <filesystem>
#include <utility>

void HJReflectClassInfo::genPropertyReflectInfo(const HJPropertyInfo& info, std::ostream &stream) const{
	stream<< "ReflectInfo(\""<<info.name
	      <<"\", offsetof("<<name<<","<<info.name<<")"
	      <<",HJMetaType::fromType<"<<info.type<<">())";
}
void HJReflectClassInfo::genClassReflectionContent(std::ostream &stream) const{
	std::stringstream reflectInfoStr;
	for(int i=0;i<properties.size();++i){
		reflectInfoStr<<"\t\t\t";
		genPropertyReflectInfo(properties[i],reflectInfoStr);
		if(i!=properties.size()-1)reflectInfoStr<<",\n";
	}
	std::string suffix=std::to_string(location.line)+"_"+std::to_string(location.column);
	std::string helperName=name+"RegisterHelper_"+suffix;
	stream<<"struct "<<helperName<<" {\n"
	        "\t"<<helperName<<"(){\n"
	        "\t\tconst std::vector<ReflectInfo> reflectInfoArray{\n"<<reflectInfoStr.str()<<"};\n"
	        "\t\tHJClass::registerClass(\""<<name<<"\", new HJClass(\""<<name<<"\",reflectInfoArray,\""<<superName<<"\",&"<<name<<"::instantiate));\n"
	        "\t}\n"
			"} "<<helperName<<"_Instance;"<<std::endl;

	stream<<"HJOBJECT_GET_CLASS_INFO("<<name<<", "<<superName<<")\n";
	stream<<"HJOBJECT_MEMBER_ACCESS_IMPL("<<name<<", "<<superName<<")\n";
}

std::ofstream &HJReflectionTool::logger() {
	static std::ofstream log("HJReflectionTool.log.txt");
	return log;
}
std::ofstream &HJReflectionTool::logger(const std::string& prefix) {
	logger()<<"["<<prefix<<"]";
	return logger();
}
HJReflectionTool::HJReflectionTool(){
	clangIndex= clang_createIndex(0,0);
}
HJReflectionTool::HJReflectionTool(std::vector<std::string> _includeDirectories, const std::string& _excludedFiles):
includeDirectories(std::move(_includeDirectories)) {
	clangIndex= clang_createIndex(0,0);

}
HJReflectionTool::~HJReflectionTool() {
	if(clangIndex){
		clang_disposeIndex(clangIndex);
	}
	for(auto tu:clangTUs){
		if(tu)clang_disposeTranslationUnit(static_cast<CXTranslationUnit>(tu));
	}
}
std::set<std::string> generatedFiles;
bool isGeneratedFile(const std::string& filePath){
	return generatedFiles.find(filePath)!=generatedFiles.end();
}
void HJReflectionTool::processFileClang(const std::string &filePath) {
	logger("processFileClang")<<"Process file "<<filePath<<"\n";
	if(isExcludedFileOrDir(filePath)){
		logger("processFileClang")<<"path "<<filePath<<" is an excluded file, exit"<<std::endl;
		return;
	}
	if(isGeneratedFile(filePath)){
		logger("processFileClang")<<"path "<<filePath<<" is a generated file, exit"<<std::endl;
		return;
	}
	std::ifstream file(filePath);
	std::stringstream fileStrStream;
	fileStrStream << file.rdbuf();
	auto fileText=fileStrStream.str();
	if(fileText.find("HJOBJECT") == std::string::npos){
		logger("processFileClang")<<"current file does not contain HJOBJECT macro, exit"<<std::endl;
		return;
	}

	std::vector<char*> argsList;
	{
		std::string cppStd("-std=c++17");
		argsList.push_back(cppStd.data());
		for(auto& i:includeDirectories){
			argsList.push_back(i.data());
		}
	}
	if(clangIndex== nullptr){
		logger("processFileClang")<<"clang index is nullptr, exit"<<std::endl;
		return;
	}
	auto TU = clang_parseTranslationUnit(clangIndex,
										 filePath.c_str(),
										 argsList.data(),
										 argsList.size(),
	                                     nullptr, 0,
										 CXTranslationUnit_None);
	if (TU == nullptr){
		logger("processFileClang") << "Unable to parse file, quit" << std::endl;
		return;
	}
	clangTUs.push_back(TU);

	std::vector<CXCursor> HJObjectDecls;
	CXCursor root = clang_getTranslationUnitCursor(TU);
	clang_visitChildren(
			root,
			[](CXCursor c, CXCursor parent, CXClientData client_data)
			{
				CXSourceLocation location = clang_getCursorLocation(c);
				if( clang_Location_isFromMainFile(location) == 0 )
					return CXChildVisit_Continue;

				if(clang_getCursorKind(c)==CXCursor_ClassDecl && clang_Cursor_hasAttrs(c)){
					//this is a class declare, now check whether it has a HJOBJECT attribute
					bool flag=false;//is this a HJObject class?
					clang_visitChildren(c,[](CXCursor c, CXCursor parent, CXClientData client_data){
						if(clang_isAttribute(clang_getCursorKind(c))){
							auto spelling=clang_getCursorSpelling(c);
							std::string spellingStr(clang_getCString(spelling));
							if(spellingStr.find("HJObject")!=std::string::npos){
								*static_cast<bool*>(client_data)=true;
								clang_disposeString(spelling);
								return CXChildVisit_Break;
							}
							clang_disposeString(spelling);
						}
						return CXChildVisit_Continue;
					},&flag);

					if(flag){
						auto* pClasses=static_cast<std::vector<CXCursor>*>(client_data);
						pClasses->push_back(c);
						return CXChildVisit_Continue;//already find a HJObject class, heading to its sibling
					}
				}
				return CXChildVisit_Recurse;
			},
			&HJObjectDecls);
	struct ClassClienData{
		std::vector<std::string> baseClass;
		std::vector<CXCursor> fieldDecl;
	};
	std::vector<HJReflectClassInfo> reflectClassInfo;
	for(auto classCursor:HJObjectDecls){
		CXString classCXString= clang_getCursorSpelling(classCursor);
		std::string className(clang_getCString(classCXString));
		clang_disposeString(classCXString);
		logger("processFileClang")<<"Processing HJObject class "<<className<<std::endl;

		ClassClienData classClienData;
		//traverse class declare cursor to find super class and field declare
		clang_visitChildren(classCursor, [](CXCursor subCursor, CXCursor parent, CXClientData client_data){
			//current cursor is a base class
			if(clang_getCursorKind(subCursor)==CXCursor_CXXBaseSpecifier){
				auto baseSpelling= clang_getCursorSpelling(subCursor);
				auto baseSpellingPtr=clang_getCString(baseSpelling);
				static_cast<ClassClienData*>(client_data)->baseClass.emplace_back(baseSpellingPtr+6);
				clang_disposeString(baseSpelling);
			}
			//current cursor is a field declare
			if(clang_getCursorKind(subCursor) == CXCursor_FieldDecl && clang_Cursor_hasAttrs(subCursor)){
				bool flag=false;//current cursor subCursor points to a HJPROPERTY field declare?
				//traverse field declare to find HJProperty attribute annotation
				clang_visitChildren(subCursor, [](CXCursor c, CXCursor parent, CXClientData client_data){
					if(clang_isAttribute(clang_getCursorKind(c))){
						CXString spelling= clang_getCursorSpelling(c);
						std::string spellingStr(clang_getCString(spelling));
						clang_disposeString(spelling);
						if(spellingStr.find("HJProperty") != std::string::npos){
							*static_cast<bool*>(client_data)=true;
							return CXChildVisit_Break;
						}
					}
					return CXChildVisit_Recurse;//recursively search attribute
				}, &flag);
				if(flag){
					static_cast<ClassClienData*>(client_data)->fieldDecl.push_back(subCursor);
				}
			}
			//only search direct descendant nodes of classCursor.
			//Will this strategy miss some HJPropertys?
			return CXChildVisit_Continue;
		}, &classClienData);

		if(classClienData.baseClass.empty()){
			logger("processFileClang")<<"Error: class "<<className<<
			" is tagged as HJOBJECT, but no super class found. "<<className<<" is ignored"<<std::endl;
			continue;
		}

		HJReflectClassInfo info(className,classClienData.baseClass.front());
		{
			CXFile file; uint line,column,offset;
			auto loc = clang_getCursorLocation(classCursor);
			clang_getFileLocation(loc,&file,&line,&column,&offset);
			auto fileStr=clang_getFileName(file);
			info.location.fileName=clang_getCString(fileStr);
			info.location.column=column;
			info.location.line=line;
			info.location.offset=offset;
			clang_disposeString(fileStr);
		}

		//add HJProperty info to HJReflectClassInfo
		for(auto p:classClienData.fieldDecl){
			CXFile file; uint line,column,offset;
			auto loc = clang_getCursorLocation(p);
			clang_getFileLocation(loc,&file,&line,&column,&offset);
			auto fileStr=clang_getFileName(file);
			auto spelling=clang_getCursorSpelling(p);
			auto typeSpelling= clang_getTypeSpelling(clang_getCursorType(p));
			logger("processFileClang")<<"found HJProperty "<<clang_getCString(spelling)<<" with type "<<clang_getCString(typeSpelling)<<" at file "<<clang_getCString(fileStr)
			                          <<" line "<<line<<" column "<<column<<" offset "<<offset<<std::endl;
			HJPropertyInfo propertyInfo(clang_getCString(spelling),clang_getCString(typeSpelling));
			propertyInfo.location.fileName=clang_getCString(fileStr);
			propertyInfo.location.column=column;
			propertyInfo.location.line=line;
			propertyInfo.location.offset=offset;
			info.properties.push_back(propertyInfo);

			clang_disposeString(fileStr);
			clang_disposeString(spelling);
			clang_disposeString(typeSpelling);
		}
		reflectClassInfo.push_back(info);
	}

	//Generate code!
	if(!reflectClassInfo.empty()){
		std::string generatedFileName=filePath.substr(0,filePath.length()-2)+".generated.h";
		std::ofstream header(generatedFileName);
		if(!header.is_open()){
			logger("processFileClang")<<"Error: unable to generate header file "<<generatedFileName<<std::endl;
			return;
		}
		generatedFiles.insert(generatedFileName);
		header<<"//This is a header file generated by HJReflectionTool automatically using libclang\n"
		        "//any change may be overwritten!\n\n";
		for(auto& c:reflectClassInfo){
			header<<"//Reflection data for class \""<<c.name<<"\", inherited from \""<<c.superName<<"\"\n";
			c.genClassReflectionContent(header);
			header<<"\n";
		}
	}
}

void HJReflectionTool::processPath(const std::string &path) {
	namespace fs=std::filesystem;
	fs::directory_entry p(path);
	if(!p.is_directory()){
		logger("processPath")<<"path "<<path<<" is not a directory, exit"<<std::endl;
		return;
	}
	if(isExcludedFileOrDir(path)){
		logger("processPath")<<"path "<<path<<" is an excluded path, exit"<<std::endl;
		return;
	}
	for (const auto & entry : fs::directory_iterator(path)){
		if(entry.is_directory()){
			processPath(entry.path());
		}else if(entry.is_regular_file() && entry.path().has_extension() && entry.path().extension()==".h"){
			processFileClang(entry.path());
		}
	}
}

void HJReflectionTool::addIncludePath(const std::string &pathStr) {
	std::string str=pathStr;
	for(auto& c:str){
		if(c==';')c=' ';
	}
	std::stringstream ss(str);
	std::string path;
	namespace fs=std::filesystem;
	while(ss>>path){
		fs::directory_entry p(path);
		if(p.is_directory()){
			includeDirectories.push_back("-I"+path);
			logger("addIncludePath")<<"add an include directory = "<<path<<std::endl;
		}
	}
}
std::set<std::string> excludedDir;
std::set<std::string> excludedFile;
void HJReflectionTool::addExcludedPath(const std::string &pathStr) {
	std::string str=pathStr;
	for(auto& c:str){
		if(c==';')c=' ';
	}
	std::stringstream ss(str);
	std::string path;
	namespace fs=std::filesystem;
	while(ss>>path){
		fs::directory_entry p(path);
		if(p.is_directory()){
			logger("addExcludedPath")<<"added an excluded directory = "<<path<<std::endl;
			excludedDir.insert(path);
		}else if(p.is_regular_file()){
			logger("addExcludedPath")<<"added an excluded = "<<path<<std::endl;
			excludedFile.insert(path);
		}
	}
}

bool HJReflectionTool::isExcludedFileOrDir(const std::string &path) const {
	if(excludedFile.find(path)!=excludedFile.end())return true;
	else if(excludedDir.find(path)!=excludedDir.end())return true;
	else return false;
}