#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <cstring>
#include <algorithm>
#include <string>
#include <map>
#define MAXSTATION 500
#define MAXLINE 20
#define MAXEDGE 100100
#define TRANSFERCOST 1000
#define OO 0x7fffffff
using namespace std;

// Station
struct Station {
	int id;
	string sname;
	string lname;
}sta[MAXSTATION], statemp[MAXSTATION];
int stanum;
// Line
string lines[MAXLINE];
int isCir[MAXLINE];
int linenum;
// subway lines
typedef multimap<string, int> Subways;
Subways subways;
// for bfs
struct Edge {
	int go, dis, next;
}edge[MAXEDGE];
int head[MAXSTATION], tp;
int queue[MAXSTATION], use[MAXSTATION], dis[MAXSTATION], from[MAXSTATION];

bool cmp(const Station &a, const Station  &b) {
	return a.sname < b.sname;
}

int Init() {
	ifstream fin;
	string now;
	fin.open("beijing-subway.txt");
	if (fin.is_open()) {
		while (fin >> now) {
			//			cout << now << endl;
			if (now == "L" || now == "C") {
				linenum++;
				fin >> lines[linenum];
				isCir[linenum] = now == "C" ? 1 : 0;
			}
			else {
				stanum++;
				sta[stanum].id = stanum;
				sta[stanum].sname = now;
				sta[stanum].lname = lines[linenum];
				subways.insert(make_pair(lines[linenum], stanum));
			}
		}
	}
	else
		return -1;
	fin.close();
	return 0;
}

void AddEdge(int a, int b, int w) {
	edge[++tp].go = b;edge[tp].dis = w;edge[tp].next = head[a];head[a] = tp;
	edge[++tp].go = a;edge[tp].dis = w;edge[tp].next = head[b];head[b] = tp;
}

void BuildMap(char mode) {
	// for subway lines
	for (int ll = 1; ll <= linenum; ll++) {
		Subways::iterator beg = subways.lower_bound(lines[ll]);
		Subways::iterator end = subways.upper_bound(lines[ll]);
		int h = beg->second;
		int last = beg++->second;
		while (beg != end) {
			AddEdge(last, beg->second, 1);
			last = beg->second;
			beg++;
		}
		if (isCir[ll]) AddEdge(last, h, 1);
	}
	// for transfer stations
	for (int i = 0; i <= stanum; i++)statemp[i] = sta[i];
	sort(statemp + 1, statemp + 1 + stanum, cmp);
	if (mode == 'b') {
		for (int i = 1; i < stanum; i++) {
			for (int j = i + 1; j <= stanum; j++) {
				if (statemp[i].sname != statemp[j].sname)
					break;
				AddEdge(statemp[i].id, statemp[j].id, 0);
			}
		}
	}
	else if (mode == 'c') {
		for (int i = 1; i < stanum; i++) {
			for (int j = i + 1; j <= stanum; j++) {
				if (statemp[i].sname != statemp[j].sname)
					break;
				AddEdge(statemp[i].id, statemp[j].id, TRANSFERCOST);
			}
		}
	}
}

void Search(string src, string dst) {
	int front = 0, end = 0;
	for (int i = 0; i <= stanum; i++)
		dis[i] = OO;
	for (int i = 1; i <= stanum; i++) {
		if (sta[i].sname == src) {
			use[i] = 1;
			dis[i] = 1;
			queue[end++] = i;
		}
	}
	for (;front < end;front++) {
		int now = queue[front];
		use[now] = 0;
		for (int v = head[now]; v; v = edge[v].next) {
			if (dis[edge[v].go] > dis[now] + edge[v].dis) {
				dis[edge[v].go] = dis[now] + edge[v].dis;
				from[edge[v].go] = now;
				if (!use[edge[v].go]) {
					queue[end++] = edge[v].go;
					use[edge[v].go] = 1;
				}
			}
		}
	}
	int did = 0, mm = OO;
	for (int i = 1; i <= stanum; i++) {
		if (sta[i].sname == dst) {
			if (dis[i] < mm) {
				mm = dis[i];
				did = i;
			}
		}
	}
	cout << dis[did] % TRANSFERCOST << endl;
	int ways[MAXSTATION] = { 0 };
	while (sta[did].sname == sta[from[did]].sname)did = from[did];
	for (int i = did; i; i = from[i])
		ways[++ways[0]] = i;
	for (int i = ways[0]; i; i--) {
		cout << sta[ways[i]].sname;
		if (i > 1) {
			string l = sta[ways[i]].lname;
			while (sta[ways[i]].sname == sta[ways[i - 1]].sname)i--;
			if (l != sta[ways[i]].lname)cout << "»»³Ë" << sta[ways[i]].lname;
		}
		cout << endl;
	}
}

int main(int argc, const char *argv[]) {
	if (Init()) {
		cout << "The data file is error!\n";
		return 0;
	}
	if (argc == 1) {
		string in;
		while (true) {
			cin >> in;
			if (in == "exit") {
				cout << "Thanks for using!\n";
				return 0;
			}
			else {
				if (!subways.count(in))
					cout << "Do not have this subway line!\n";
				else {
					Subways::iterator beg = subways.lower_bound(in);
					Subways::iterator end = subways.upper_bound(in);
					while (beg != end)
						cout << sta[beg++->second].sname << " ";
					cout << endl;
				}
			}
		}
	}
	else if (argc == 4) {
		if (strcmp(argv[1], "-b") && strcmp(argv[1], "-c")) {
			cout << "Unknown command!\n";
			return 0;
		}
		BuildMap(argv[1][1]);
		Search(argv[2], argv[3]);
	}
	else
		cout << "Unknown command!\n";
	system("pause");
	return 0;
}
