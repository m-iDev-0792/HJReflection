#include <iostream>
#include "HJVariant.h"
#include "HJTest.h"
using namespace std;
struct MyType{
	int id;
	string name;
	float hello[123];
};
int main() {
	{
		HJObject* test=HJObject::instantiateObject("HJTest", nullptr);
		cout<<"Object test is a "<<test->getClassName()<<endl;
		dynamic_cast<HJTest*>(test)->name="huajige";
		dynamic_cast<HJTest*>(test)->grade=59.9;
		auto ID=test->getValue("id");
		auto NAME=test->getValue("name");
		cout<<NAME.getValue<string>()<<" has id = "<<ID.getValue<int>()<<endl;
		HJVariant GRADE(60.0f);
		test->setValue("grade",GRADE);
		cout<<"student "<<dynamic_cast<HJTest*>(test)->name<<" has grade  = "<<dynamic_cast<HJTest*>(test)->grade<<endl;
		return 0;
	}
	HJTest test;
	test.name="huajige";
	test.grade=59.9;
	auto ID=test.getValue("id");
	auto NAME=test.getValue("name");
	cout<<NAME.getValue<string>()<<" has id = "<<ID.getValue<int>()<<endl;
	HJVariant GRADE(60.0f);
	test.setValue("grade",GRADE);
	cout<<"student "<<test.name<<" has grade  = "<<test.grade<<endl;
	auto inst=HJClass::instantiate("HJTest", nullptr);
	cout<<"instantiated object name = "<<inst->getClassName()<<endl;

	auto hjobject=HJClass::instantiate("HJObject", nullptr);
	cout<<"instatiated hjobject name = "<<hjobject->getClassName()<<endl;

	int sonAge=100;
	auto hjtestson=HJClass::instantiate("HJTestSon", &sonAge);
	cout<<"instatiated hjtestson name = "<<hjtestson->getClassName()<<"\n";
	auto TestSonID=hjtestson->getValue("id");
	cout<<"hjtestson's id="<<TestSonID.getValue<int>()<<endl;
	cout<<"hjtestson 's age = "<<hjtestson->getValue("age").getValue<int>()<<endl;

	auto testson=dynamic_cast<HJTestSon*>(hjtestson);
	cout << "another useless str=" << static_cast<UselessClass*>(testson)->uselessStr << endl;
	return 0;
}
