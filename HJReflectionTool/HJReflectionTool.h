//
// Created by 何振邦 on 2021/12/11.
//

#ifndef REFLECTION_HJREFLECTIONTOOL_H
#define REFLECTION_HJREFLECTIONTOOL_H
#include <iostream>
#include <utility>
#include <vector>
#include <fstream>
#include <sstream>
#include <set>
struct Location{
	std::string fileName;
	int line;
	int column;
	int offset;
};
struct HJPropertyInfo{
	std::string type;
	std::string name;
	Location location;
	HJPropertyInfo()=default;
	HJPropertyInfo(std::string  _name,std::string  _type):
	name(std::move(_name)),type(std::move(_type)){}
};
struct HJReflectClassInfo{
	std::string name;
	std::string superName;
	std::vector<HJPropertyInfo> properties;
	Location location;
	HJReflectClassInfo()=default;
	HJReflectClassInfo(std::string  _name, std::string  _superName):
	name(std::move(_name)),superName(std::move(_superName)){}

	void genPropertyReflectInfo(const HJPropertyInfo& info, std::ostream &stream) const;
	void genClassReflectionContent(std::ostream &stream) const;
};
class HJReflectionTool {
public:
	void processFileClang(const std::string& filePath);
	void processPath(const std::string& path);
	void addExcludedPath(const std::string& pathStr);
	void addIncludePath(const std::string& pathStr);
	HJReflectionTool();
	HJReflectionTool(std::vector<std::string> _includeDirectories, const std::string& _excludedFiles);
	~HJReflectionTool();
	static std::ofstream& logger();
	static std::ofstream& logger(const std::string& prefix);
private:
	std::vector<std::string> includeDirectories;
	void* clangIndex= nullptr;
	std::vector<void*> clangTUs;
	bool isExcludedFileOrDir(const std::string& path) const;
};


#endif //REFLECTION_HJREFLECTIONTOOL_H
