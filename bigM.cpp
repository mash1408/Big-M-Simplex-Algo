#include <iostream>
#include <vector>
#include <map>
#include<string>
# include <algorithm>
using namespace std;
int penalty = 0;
int slacks = 0;
int surplus = 0;
vector <string> basicKeys;
void initialise() {
	for(int i=0;i<penalty;i++)
		basicKeys.push_back("R"+to_string(i+1));
	for(int i=0;i<slacks;i++)
		basicKeys.push_back("SL"+to_string(i+1));
}
void print(map<string, float> objective, vector<map<string, float>> constraints){
	cout<<"Objective Function:"<<endl;
	for (map<string, float>::iterator iter = objective.begin(); iter!= objective.end();++iter)
		cout<<"\t"<<iter->first<<":"<<iter->second<<"\t";
		// cout<<"\t"<<iter->first<<" : "<<iter->second;
	
	cout<<endl<<"constraints"<<endl;

	for(auto item: constraints){
		cout<<endl<<"\tConstraints"<<endl;
		for (map<string, float>::iterator iter = item.begin(); iter!= item.end();++iter)
		cout<<"\t"<<iter->first<<":"<<iter->second<<"\t";
	}
	cout<<endl;
}

void optimise(map<string, float> &objective, vector<map<string, float>> &constraints) {
	
	vector<map<string, float>> copyconstraints(constraints);
	for (auto &var: copyconstraints) {
		var.erase(var.find("equality")->first);
	}
	for(auto basic: basicKeys){
		
		if(objective.at(basic)!=0){
			map<string,float> temp;
			for(auto item: copyconstraints){
				if(item.at(basic)==1)
					temp.insert(item.begin(), item.end());
			}
			float val=objective.at(basic);
			map<string, float>::iterator jt = temp.begin();
			for (auto it = objective.begin(); it != objective.end(); it++)
    		{
					if(val<0)
						it->second=it->second-val*jt->second;
					else
						it->second+=objective.at(basic)*jt->second;
					jt++;
			}
		}
	}

	
}

float pivot(map<string, float> it, string enterKey) {
	return it.at(enterKey);
}

string findEntryWithLargestValue(map<string, float> it)
{
	pair<string, float> entryWithMaxValue
		= make_pair("x", 0);
	//remove solution key
		it.erase(it.find("sol")->first);
	// Iterate in the map to find the required entry
	map<string, float>::iterator currentEntry;
	for (currentEntry = it.begin();
		currentEntry != it.end();
		++currentEntry) {
		if (currentEntry->second
			> entryWithMaxValue.second) {

			entryWithMaxValue
				= make_pair(
				currentEntry->first,
				currentEntry->second);
		}
	}
	if (entryWithMaxValue.second>0)
		return entryWithMaxValue.first;
	else
		entryWithMaxValue.first = "stop";
	return entryWithMaxValue.first;
}

map<string, float> optimalityConition(string enteringVariable, vector<map<string, float>> constraints) {
	vector <pair<map<string, float>, float>> v;
	for (auto item : constraints) {
		pair<map<string, float>, float> entry
			= make_pair(item, item.at("sol") / item.at(enteringVariable));
		v.push_back(entry);
	}
	pair<map<string, float>, float> max
		= v.at(0);
	for (auto & item : v) {
		if (item.second<max.second)
			max = item;
	}
	vector<map<string,float>>::iterator it;
	it = find (constraints.begin(), constraints.end(), max.first);
	return *it;
}

void generate(map<string, float> &objective, vector<map<string, float>> &constraints) {
	string enterKey =  findEntryWithLargestValue(objective);
	map<string,float> leavingVariable=optimalityConition(enterKey,constraints);

	
	for(auto &constraint: constraints)
		if(constraint == leavingVariable){
			for (map<string,float>::iterator currentRow = constraint.begin(); currentRow!= constraint.end();++currentRow)
				currentRow->second/= pivot(leavingVariable,enterKey);
		leavingVariable=constraint;
		}
	for(auto &constraint: constraints)
		if(constraint!=leavingVariable){
			map<string, float>::iterator jter=leavingVariable.begin();
			for (map<string, float>::iterator iter = constraint.begin(); iter!= constraint.end();++iter,++jter)
				iter->second-=pivot(constraint,enterKey)*jter->second;
		}
	map<string, float>::iterator jter=leavingVariable.begin();
	for (map<string,float>::iterator objIter = objective.begin(); objIter!= objective.end();++objIter,++jter)
				objIter->second-=pivot(objective,enterKey)*jter->second;
}

void standardForm(map<string, float> &objective, vector<map<string, float>> &constraints) {

	for (auto & item : constraints) {
		if (item.find("equality")->second == 0) {
			++penalty;
			item.insert(pair<string, float>("R" + to_string(penalty), 1));
			for (auto &other : constraints) {
				if (other != item)
					other.insert(pair<string, float>("R"+to_string(penalty),0));
			}

		}
		else if (item.find("equality")->second == 1) {
			++penalty;
			++surplus;
			item.insert(pair<string, float>("R" + to_string(penalty), 1));
			item.insert(pair<string, float>("SU"+to_string(surplus), -1));
			for (auto &other : constraints) {
				if (other != item)
					other.insert(pair<string, float>("R" + to_string(penalty), 0));
					other.insert(pair<string, float>("SU" + to_string(surplus), 0));
			}

		}
		else if (item.find("equality")->second == -1) {
			++slacks;
			item.insert(pair<string, float>("SL" + to_string(slacks), 1));
			for (auto &other : constraints) {
				if (other != item)
				other.insert(pair<string, float>("SL" + to_string(slacks), 0));
			}

		}
	}
	for (int i = 0; i < penalty; i++) 
		objective.insert(pair<string, float>("R" + to_string(i + 1), 100));
	
		for (int i = 0; i<slacks; i++)
		objective.insert(pair<string, float>("SU" + to_string(i + 1), 0));

	for (int i = 0; i<surplus; i++)
		objective.insert(pair<string, float>("SL" + to_string(i + 1), 0));

	for (std::map<string, float>::iterator it = objective.begin(); it != objective.end(); ++it)
		it->second = -1 * it->second;

	objective.insert(pair<string, float>("sol", 0));

}

int main() {

	map<string, float> objective{ { "x1",4 } ,{ "x2",1 } };
	vector<map<string, float>> constraints;
	
	map<string, float> constraint1{ { "x1",3 } ,{ "x2",1 },{ "equality",0 },{ "sol", 3 } };
	map<string, float> constraint2{ { "x1",4 } ,{ "x2",3 },{ "equality",1 } ,{ "sol", 6 } };
	map<string, float> constraint3{ { "x1",1 } ,{ "x2",2 },{ "equality",-1 } ,{ "sol", 4 } };
	constraints.push_back(constraint1);
	constraints.push_back(constraint2);
	constraints.push_back(constraint3);
	standardForm(objective, constraints);
	initialise();
	
	optimise(objective, constraints);
	print(objective,constraints);
	generate(objective,constraints);
	print(objective,constraints);
	
}
