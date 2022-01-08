#include "HJReflectionTool.h"

using namespace std;
int main(int argc, char *argv[]) {
	if(argc<=1){
		cout<<"Error: No processing path inputted\nUsage: HJReflectionTool path [-I<includePath1;includePath2..>] [-E<excludedPath1;excludedPath2..>]"<<endl;
	}else{
		HJReflectionTool tool;
		for(int i=2;i<argc;++i){
			if(strlen(argv[i])>=3&&argv[i][0]=='-'){
				if(argv[i][1]=='I'){
					tool.addIncludePath(&argv[i][2]);
					continue;
				}else if(argv[i][1]=='E'){
					tool.addExcludedPath(&argv[i][2]);
					continue;
				}
			}
			std::cout<<"unknown command "<<argv[i]<<std::endl;
		}
		tool.processPath(argv[1]);
	}

	return 0;
}