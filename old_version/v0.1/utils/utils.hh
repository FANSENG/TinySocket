#ifndef UTILS_HH
#define UTILS_HH

#include <iostream>
#include <vector>
using namespace std;


vector<string> split(string str, const char split){
    vector<string> res;
    if (str == "") return res;
	string strs = str + split;
	size_t pos = strs.find(split);
 
	while (pos != strs.npos)
	{
		string temp = strs.substr(0, pos);
		res.push_back(temp);
		strs = strs.substr(pos + 1, strs.size());
		pos = strs.find(split);
    }
    return res;
}

void handShakeSetParm(string &str, int &SYN, int &ACK, int &seq, int &ack){
    vector<string> res = split(str, '.');
    if(res.size() != 4) return; 
    SYN = stoi(res[0]);
    ACK = stoi(res[1]);
    seq = stoi(res[2]);
    ack = stoi(res[3]);
}

void cutString(string &str, const char ch){
    while(str.at(str.size() - 1) == ch) str.pop_back();
}

#endif // UTILS_HH