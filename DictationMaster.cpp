// Think twice, code once.
#include <tuple>
#include <cmath>
#include <random>
#include <chrono>
#include <vector>
#include <cstdio>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <algorithm>
using namespace std;

mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
void systemClear() {
#ifdef _WIN32
	system("cls");
#else
	system("clear");
#endif
	return ;
}

namespace Practice {
	int st, acval, waval, num, ac, lst = -1;
	string dictionary;
	vector<tuple<string, string, int>> v;

	void main();
}
namespace Dictation {
	vector<string> res;
	vector<int> wrong;
	vector<pair<string, string>> v;
	string dictionary;

	void main();
}

int main() {
#ifdef _WIN32
	system("chcp 65001");
	system("cls");
#endif
	puts("使用说明\n\
\n\
运行后首先输入词典名称，可以使用绝对路径或相对路径。\n\
然后输入加权，应为三个整数，分别表示每个单词的初始权值、默写正确后减少的权值和默写错误后增加的权值。\n\
\n\
默写时会显示当前单词的权值和所有单词的总权值。\n\
输入 /finish 可以直接结束默写。\n\
输入 /skip 暂时跳过这个单词。\n\
在输入完正确单词后加一个空格再输入 /pass 可以直接清空该单词权值。\n\
\n\
输入回车开始默写。");
	cin.get();
	systemClear();
	puts("请选择模式。");
	puts("1. 练习（传统模式） 2. 模拟听写");
	int x;
	scanf("%d", &x);
	systemClear();
	if (x == 1) Practice::main();
	else Dictation::main();
	return 0;
}

void Practice::main() {
	puts("请选择词库");
	cin >> dictionary;
	ifstream din(dictionary.c_str());
	puts("请输入加权");
	scanf("%d%d%d", &st, &acval, &waval);
	{
		string s;
		for (int line = 1; getline(din, s); line++) {
			if (s == "\n" || s == "\r" || s == "\r\n") continue;
			int pos = -1;
			for (int i = 0; i < (int)s.length(); i++)
				if (s[i] == ':') {pos = i; break;}
			if (pos == -1) {
				systemClear();
				printf("词典第 %d 行出错。\n程序停止运行。\n", line);
				return ;
			}
			v.emplace_back(s.substr(0, pos - 1), s.substr(pos + 2), st);
		}
	}
	systemClear();
	puts("词典加载成功。");
	puts("==预览==");
	for (auto i : v) printf("%s : %s\n", get<1>(i).c_str(), get<0>(i).c_str());
	puts("输入回车开始默写。");
	cin.get(); cin.get();
	while (1) {
		int sum = 0;
		for (auto i : v) sum += get<2>(i);
		if (sum == 0) break;
		int pos = -1;
		{
			int rnd = gen() % sum + 1, tmp = 0;
			for (int i = 0; i < (int)v.size(); i++)
				if (rnd <= tmp + get<2>(v[i])) {pos = i; break;}
				else tmp += get<2>(v[i]);
		}
		if (pos == lst && get<2>(v[pos]) != sum) continue;
		lst = pos;
		systemClear();
		printf("%s (value: %d, sum: %d)\n", get<1>(v[pos]).c_str(), get<2>(v[pos]), sum);
		string s;
		do getline(cin, s);
		while (s == "\n" || s == "\r" || s == "\r\n");
		puts("");
		while (s.back() == ' ') s.pop_back();
		if (s == get<0>(v[pos])) {
			ac++;
			get<2>(v[pos]) = max(get<2>(v[pos]) - acval, 0);
			puts("Accepted.");
		} else if (s == get<0>(v[pos]) + " /skip") {
			ac += get<2>(v[pos]);
			num += get<2>(v[pos]) - 1;
			get<2>(v[pos]) = 0;
			puts("Accepted and passed.");
		} else if (s == "/finish") break;
		else if (s == "/skip") num--;
		else if (s + "." == get<0>(v[pos])) puts("不喜欢加句号是这样的/cf/cf/cf"), ac++;
		else {
			get<2>(v[pos]) += waval;
			puts("Wrong. The answer is:");
			printf("%s\n", get<0>(v[pos]).c_str());
		}
		num++;
		puts("\n按回车继续");
		getchar();
	}
	systemClear();
	puts("默写结束！");
	printf("完成默写使用次数：%d, 正确次数：%d, 正确率: %.1lf%%\n", num, ac, (double)ac / num * 100);
	puts("按回车结束程序");
	return ;
}

void Dictation::main() {
	puts("请选择词库");
	cin >> dictionary;
	ifstream din(dictionary.c_str());
	{
		string s;
		for (int line = 1; getline(din, s); line++) {
			if (s == "\n" || s == "\r" || s == "\r\n") continue;
			int pos = -1;
			for (int i = 0; i < (int)s.length(); i++)
				if (s[i] == ':') {pos = i; break;}
			if (pos == -1) {
				systemClear();
				printf("词典第 %d 行出错。\n程序停止运行。\n", line);
				return ;
			}
			v.emplace_back(s.substr(0, pos - 1), s.substr(pos + 2));
		}
	}
	systemClear();
	puts("词典加载成功。");
	puts("==预览==");
	for (auto i : v) printf("%s : %s\n", i.second.c_str(), i.first.c_str());
	puts("输入回车开始默写。");
	cin.get(); cin.get();
	for (auto i : v) {
		systemClear();
		printf("%s\n", i.second.c_str());string s;
		do getline(cin, s);
		while (s == "\n" || s == "\r" || s == "\r\n");
		res.push_back(s);
		systemClear();
	}
	int ac = 0, num = res.size();
	for (int i = 0; i < (int)res.size(); i++)
		if (res[i] == v[i].first) ac++;
		else wrong.push_back(i);
	printf("你的得分为：%dpts.\n", (int)floor((double)ac / num * 100));
	if (ac != num) {
		puts("你错误的题目：");
		for (auto i : wrong)
			printf("%s\n你的答案为：%s\n正确答案为：%s\n\n",
				v[i].second.c_str(), res[i].c_str(), v[i].first.c_str());	
	} else puts("膜拜 AK King! /bx/bx/bx");
	return ;
}