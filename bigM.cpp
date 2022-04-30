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

void optimise(map<string, int> &objective, vector<map<string, int>> &constraints) {
	
	vector<map<string, int>> copyConstraints(constraints);
	for (auto &var: copyConstraints) {
		var.erase(var.find("equality")->first);
	}
	for(auto basic: basicKeys){
		
		if(objective.at(basic)!=0){
			map<string,int> temp;
			for(auto item: copyConstraints){
				if(item.at(basic)==1)
					temp.insert(item.begin(), item.end());
			}
			int val=objective.at(basic);
			map<string, int>::iterator jt = temp.begin();
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

int pivot(map<string, int> it, string enterKey) {
	return it.at(enterKey);
}

pair<string, int> findEntryWithLargestValue(map<string, int> it)
{
	pair<string, int> entryWithMaxValue
		= make_pair("x", 0);
	//remove solution key
		it.erase(it.find("sol")->first);
	// Iterate in the map to find the required entry
	map<string, int>::iterator currentEntry;
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
		return entryWithMaxValue;
	else
		entryWithMaxValue.first = "stop";
	return entryWithMaxValue;
}

map<string, int> optimalityConition(string enteringVariable, vector<map<string, int>> constraints) {
	vector <pair<map<string, int>, int>> v;
	for (auto item : constraints) {
		pair<map<string, int>, int> entry
			= make_pair(item, item.at("sol") / item.at(enteringVariable));
		v.push_back(entry);
	}
	pair<map<string, int>, int> max
		= v.at(0);
	for (auto & item : v) {
		if (item.second<max.second)
			max = item;
	}
	return max.first;
}

void generate(map<string, int> &objective, vector<map<string, int>> &constraints) {
	pair<string, int> enteringVariable = findEntryWithLargestValue(objective);
	string enterKey = enteringVariable.first;
	map<string,int> leavingVariable=optimalityConition(enterKey,constraints);
	vector<map<string,int>>::iterator it;
	it = find (constraints.begin(), constraints.end(), leavingVariable);
	map<string,int> temp=*it;
	map<string, int>::iterator currentRow;
	for (currentRow = temp.begin(); currentRow!= temp.end();++currentRow)
		currentRow->second/= pivot(temp,enterKey);
	for(auto constraint: constraints){
		if(constraint!=item){
			for (map<string, int>::iterator iter = temp.begin(); iter!= temp.end();++iter){
				iter->second-=pivot(constraint,enterKey);
			}
		}
	}
}

void standardForm(map<string, int> &objective, vector<map<string, int>> &constraints) {

	for (auto & item : constraints) {
		if (item.find("equality")->second == 0) {
			++penalty;
			item.insert(pair<string, int>("R" + to_string(penalty), 1));
			for (auto &other : constraints) {
				if (other != item)
					other.insert(pair<string, int>("R"+to_string(penalty),0));
			}

		}
		else if (item.find("equality")->second == 1) {
			++penalty;
			++surplus;
			item.insert(pair<string, int>("R" + to_string(penalty), 1));
			item.insert(pair<string, int>("SU"+to_string(surplus), -1));
			for (auto &other : constraints) {
				if (other != item)
					other.insert(pair<string, int>("R" + to_string(penalty), 0));
					other.insert(pair<string, int>("SU" + to_string(surplus), 0));
			}

		}
		else if (item.find("equality")->second == -1) {
			++slacks;
			item.insert(pair<string, int>("SL" + to_string(slacks), 1));
			for (auto &other : constraints) {
				if (other != item)
				other.insert(pair<string, int>("SL" + to_string(slacks), 0));
			}

		}
	}
	for (int i = 0; i < penalty; i++) 
		objective.insert(pair<string, int>("R" + to_string(i + 1), 100));
	
		for (int i = 0; i<slacks; i++)
		objective.insert(pair<string, int>("SU" + to_string(i + 1), 0));

	for (int i = 0; i<surplus; i++)
		objective.insert(pair<string, int>("SL" + to_string(i + 1), 0));

	for (std::map<string, int>::iterator it = objective.begin(); it != objective.end(); ++it)
		it->second = -1 * it->second;

	objective.insert(pair<string, int>("sol", 0));

}
int main() {

	map<string, int> objective{ { "x1",4 } ,{ "x2",1 } };
	vector<map<string, int>> constraints;
	
	map<string, int> constraint1{ { "x1",3 } ,{ "x2",1 },{ "equality",0 },{ "sol", 3 } };
	map<string, int> constraint2{ { "x1",4 } ,{ "x2",3 },{ "equality",1 } ,{ "sol", 6 } };
	map<string, int> constraint3{ { "x1",1 } ,{ "x2",2 },{ "equality",-1 } ,{ "sol", 4 } };
	constraints.push_back(constraint1);
	constraints.push_back(constraint2);
	constraints.push_back(constraint3);
	standardForm(objective, constraints);
	initialise();
	optimise(objective, constraints);
	generate(objective,constraints);
	for (auto it = objective.begin(); it != objective.end(); ++it) {
		cout << it->first << ":" << it->second<<"\n";
	}
	for(auto item: constraints){
		cout<<"constraint\n";
		for (auto it = item.begin(); it != item.end(); ++it) {
		
		cout << it->first << ":" << it->second<<"\n";
	}
	}
	
}
