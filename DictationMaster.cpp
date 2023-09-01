// Think twice, code once.
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>
using namespace std;

mt19937 gen(chrono::system_clock::now().time_since_epoch().count());
void systemClear() {
#ifdef _WIN32
	system("cls");
#else
	cout << "\033c";
#endif
}

namespace Practice {
	int st, acval, waval, num, ac, lst = -1;
	string dictionary;
	struct word {
		string contentEn;
		string contentCn;
		int val;
	};
	vector<word> v;
	// vector<tuple<string, string, int>> v;

	void inputDict(const char *filename);
	void main();
} // namespace Practice
namespace Dictation {
	vector<string> res;
	vector<int> wrong;
	vector<pair<string, string>> v;
	string dictionary;

	void main();
} // namespace Dictation

auto main() -> int {
#ifdef _WIN32
	system("chcp 65001");
	system("cls");
#endif
	cout << R"(使用说明

运行后首先输入词典名称，可以使用绝对路径或相对路径。
然后输入加权，应为三个整数，分别表示每个单词的初始权值、默写正确后减少的权值和默写错误后增加的权值。

默写时会显示当前单词的权值和所有单词的总权值。
输入 /finish 可以直接结束默写。
输入 /skip 暂时跳过这个单词。
在输入完正确单词后加一个空格再输入 /pass 可以直接清空该单词权值。

输入回车开始默写。
)";
	cin.get();
	systemClear();
	cout << "请选择模式。\n";
	cout << "1. 练习（传统模式） 2. 模拟听写\n";
	int x;
	cin >> x;
	systemClear();
	if (x == 1) {
		Practice::main();
	}
	else {
		Dictation::main();
	}
	return 0;
}

void Practice::inputDict(const char *filename) {
	ifstream din(filename);
	string s;
	for (int line = 1; getline(din, s); line++) {
		if (s == "\n" || s == "\r" || s == "\r\n") {
			continue;
		}
		int pos = -1;
		for (int i = 0; i < static_cast<int>(s.length()); i++) {
			if (s[i] == ':') {
				pos = i;
				break;
			}
		}
		if (pos == -1) {
			systemClear();
			cerr << "词典第 " << line << " 行出错。\n程序停止运行。\n";
			return;
		}
		v.emplace_back(s.substr(0, pos - 1), s.substr(pos + 2), st);
		size_t tmp;
		(tmp = v.back().contentCn.find('\r')) != string::npos
			? v.back().contentCn.erase(tmp)
			: "";

		(tmp = v.back().contentEn.find('\r')) != string::npos
			? v.back().contentEn.erase(tmp)
			: "";
	}
}
void Practice::main() {
	cout << "请选择词库\n";
	cin >> dictionary;
	cout << "请输入加权\n";
	cin >> st >> acval >> waval;
	inputDict(dictionary.c_str());
	systemClear();
	cout << "词典加载成功。\n==预览==\n";
	for (auto i : v) {
		cout << i.contentCn << " : " << i.contentEn << "\n";
	}
	cout << "输入回车开始默写。\n";
	cin.get();
	cin.get();
	while (true) {
		int sum = 0;
		for (auto i : v) {
			sum += i.val;
		}
		if (sum == 0) {
			break;
		}
		int pos = -1;
		{
			int rnd = gen() % sum + 1;
			int tmp = 0;
			for (int i = 0; i < static_cast<int>(v.size()); i++) {
				if (rnd <= tmp + v[i].val) {
					pos = i;
					break;
				}
				tmp += v[i].val;
			}
		}
		if (pos == lst && v[pos].val != sum) {
			continue;
		}
		lst = pos;
		systemClear();
		cout << v[pos].contentCn << " (value: " << v[pos].val
			 << ", sum: " << sum << ")\n";
		string s;
		do {
			getline(cin, s);
		} while (s == "\n" || s == "\r" || s == "\r\n");
		cout << "\n";
		while (s.back() == ' ') {
			s.pop_back();
		}
		if (s == v[pos].contentEn) {
			ac++;
			v[pos].val = max(v[pos].val - acval, 0);
			cout << "Accepted.\n";
		}
		else if (s == v[pos].contentEn + " /pass") {
			ac += v[pos].val;
			num += v[pos].val - 1;
			v[pos].val = 0;
			cout << "Accepted and passed.\n";
		}
		else if (s == "/finish") {
			break;
		}
		else if (s == "/skip") {
			num--;
		}
		else if (s + "." == v[pos].contentEn) {
			cout << "不喜欢加句号是这样的/cf/cf/cf\n", ac++;
		}
		else {
			v[pos].val += waval;
			cout << "Wrong. The answer is:\n";
			cout << v[pos].contentEn << "\n";
		}
		num++;
		cout << "\n按回车继续";
		cin.get();
	}
	systemClear();
	cout << "默写结束！\n";
	cout << "完成默写使用次数：" << num << ", 正确次数：" << ac
		 << ", 正确率: " << static_cast<double>(ac) / num * 100 << "\n";
	cout << "按回车结束程序\n";
}

void Dictation::main() {
	puts("请选择词库");
	cin >> dictionary;
	ifstream din(dictionary.c_str());
	{
		string s;
		for (int line = 1; getline(din, s); line++) {
			if (s == "\n" || s == "\r" || s == "\r\n") {
				continue;
			}
			int pos = -1;
			for (int i = 0; i < static_cast<int>(s.length()); i++) {
				if (s[i] == ':') {
					pos = i;
					break;
				}
			}
			if (pos == -1) {
				systemClear();
				printf("词典第 %d 行出错。\n程序停止运行。\n", line);
				return;
			}
			v.emplace_back(s.substr(0, pos - 1), s.substr(pos + 2));
		}
	}
	systemClear();
	puts("词典加载成功。");
	puts("==预览==");
	for (const auto &i : v) {
		printf("%s : %s\n", i.second.c_str(), i.first.c_str());
	}
	puts("输入回车开始默写。");
	cin.get();
	cin.get();
	for (const auto &i : v) {
		systemClear();
		printf("%s\n", i.second.c_str());
		string s;
		do {
			getline(cin, s);
		} while (s == "\n" || s == "\r" || s == "\r\n");
		res.push_back(s);
		systemClear();
	}
	int ac = 0;
	int num = res.size();
	for (int i = 0; i < static_cast<int>(res.size()); i++) {
		if (res[i] == v[i].first) {
			ac++;
		}
		else {
			wrong.push_back(i);
		}
	}
	printf("你的得分为：%dpts.\n",
		   static_cast<int>(floor(static_cast<double>(ac) / num * 100)));
	if (ac != num) {
		puts("你错误的题目：");
		for (auto i : wrong) {
			printf("%s\n你的答案为：%s\n正确答案为：%s\n\n",
				   v[i].second.c_str(), res[i].c_str(), v[i].first.c_str());
		}
	}
	else {
		puts("膜拜 AK King! /bx/bx/bx");
	}
}
