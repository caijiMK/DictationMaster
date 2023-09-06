// Think twice, code once.
#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <queue>
#include <random>
#include <string>
#include <tuple>
#include <type_traits>
#include <unordered_map>
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

struct piece {
	enum Language : int { EN = 0, CN = 1 };
	static constexpr int LanguageCount = 2;
	string content[LanguageCount];
	int val;

	piece() : content{}, val(0) {}
	explicit piece(const char **str, int cnt = LanguageCount, int val = 0)
		: val(val) {
		for (int i = 0; i < cnt; ++i) {
			content[i] = str[i];
		}
	}
};
struct dictionary {
  private:
	vector<piece> data;

	int valueStable;
	int valueIncrease;
	int valueDecrease;

  public:
	dictionary() : valueStable(), valueIncrease(), valueDecrease() {}
	dictionary(int valueStable, int valueIncrease, int valueDecrease,
			   vector<piece> data = {})
		: data(std::move(data)), valueStable(valueStable),
		  valueIncrease(valueIncrease), valueDecrease(valueDecrease) {}

	auto fileInput(const char *filename) -> bool;
	auto fileOutput(const char *filename, int limit = -1) const -> bool;

	auto randomSelect(int floor = 0, int ceil = 0x3f3f3f3f) -> piece & {
		vector<piece *> res;
		for (auto &now : data) {
			if (now.val >= floor && now.val <= ceil) {
				res.push_back(&now);
			}
		}
		return *res[uniform_int_distribution<size_t>(0, res.size())(gen)];
	}
	[[nodiscard]] auto randomSelect(int floor = 0, int ceil = 0x3f3f3f3f) const
		-> const piece & {
		vector<const piece *> res;
		for (const auto &now : data) {
			if (now.val >= floor && now.val <= ceil) {
				res.push_back(&now);
			}
		}
		return *res[uniform_int_distribution<size_t>(0, res.size())(gen)];
	}
	auto maxiumSelect() -> piece & {
		piece *res = data.data();
		for (auto &now : data) {
			if (now.val > res->val) {
				res = &now;
			}
		}
		return *res;
	}
	[[nodiscard]] auto maxiumSelect() const -> const piece & {
		const piece *res = data.data();
		for (const auto &now : data) {
			if (now.val > res->val) {
				res = &now;
			}
		}
		return *res;
	}

	void setValueToStable() {
		for (auto &now : data) {
			now.val = valueStable;
		}
	}

	auto find(const piece &x) const -> size_t {
		return ::find(data.begin(), data.end(), x) - data.begin();
	}
	auto size() const -> size_t { return data.size(); }

	void setValueToStable(size_t pos) { data[pos].val = valueStable; }
	void increaseValue(size_t pos) { data[pos].val += valueIncrease; }
	void decreaseValue(size_t pos) { data[pos].val -= valueDecrease; }

	void setValueStable(int val) { valueStable = val; }
	void setValueIncrease(int val) { valueIncrease = val; }
	void setValueDecrease(int val) { valueDecrease = val; }

	auto getPiece(size_t pos) -> piece & { return data[pos]; }
	[[nodiscard]] auto getPiece(size_t pos) const -> const piece & {
		return data[pos];
	}

	[[nodiscard]] auto getValueSum() const -> int {
		int res = 0;
		for (const auto &now : data) {
			res += now.val;
		}
		return res;
	}
};

auto dictionary::fileInput(const char *filename) -> bool {
	data.clear();
	ifstream din(filename);
	int language = 0;
	while (!din.eof()) {
		char input = static_cast<char>(din.get());
		if (input == '\r') {
			continue;
		}
		if (input == '\n') {
			data.emplace_back("", "", 0);
			language = 0;
			continue;
		}
		if (input == ':') {
			data.back().content[language].erase(
				data.back().content[language].find_last_not_of(' ') + 1);
			data.back().content[++language] = "";
		}
	}
	return true;
}
auto dictionary::fileOutput(const char *filename, int limit) const -> bool {
	ofstream dout(filename);
	for (auto now : data) {
		bool flg = false;
		for (int i = 0;
			 i < (limit == -1
					  ? piece::LanguageCount
					  : min(limit, static_cast<int>(piece::LanguageCount)));
			 ++i) {
			if (flg) {
				dout << " : ";
			}
			else {
				flg = true;
			}
			dout << now.content[i];
		}
		dout << "\n";
	}
	return true;
}

enum varType {
	Int,
	Float,
	Double,
	Char,
	Str,	 // string
	ListStr, // vector<string>
};
class Practitioner {
	dictionary dictionarySelected;

	double score;

  public:
	Practitioner() = default;
	explicit Practitioner(dictionary dictionarySelected)
		: dictionarySelected(std::move(dictionarySelected)) {}

	auto loadPreferenceFromFile(const char *filename) -> bool {}
	auto dumpPreferenceToFile(const char *filename) const -> bool {}

	auto selectDictionary(const char *filename) -> bool {
		return dictionarySelected.fileInput(filename);
	}
	void practice() {
		cout << "==预览==\n";
		for (int i = 0; i < dictionarySelected.size(); ++i) {
			auto now = dictionarySelected.getPiece(i);
			cout << now.content[piece::EN] << " : " << now.content[piece::CN]
				 << "\n";
		}

		int valueStable;
		int valueIncrease;
		int valueDecrease;
		cout << "请输入加权 [默认 增长 衰减] :";
		cin >> valueStable >> valueIncrease >> valueDecrease;
		dictionarySelected.setValueStable(valueStable);
		dictionarySelected.setValueIncrease(valueIncrease);
		dictionarySelected.setValueDecrease(valueDecrease);

		while (dictionarySelected.getValueSum() != 0) {
			piece &nowPiece = gen() % 2 == 0
								  ? dictionarySelected.maxiumSelect()
								  : dictionarySelected.randomSelect(1);
			cout << nowPiece.content[piece::CN] << " (value:" << nowPiece.val
				 << ", sum:" << dictionarySelected.getValueSum() << ")\n";
			string input;
			getline(cin, input);
			while (input.back() == ' ' || input.back() == '\n' ||
				   input.back() == '\r') {
				input.pop_back();
			}
			if (input == "/finish") {
				break;
			}
			if (input == "/skip") {
				continue;
			}

			if (input.find(nowPiece.content[piece::EN]) == 0 &&
				(input.size() == nowPiece.content[piece::EN].size() ||
				 input.substr(nowPiece.content[piece::EN].size(), 2) ==
					 " /")) {
				dictionarySelected.decreaseValue(
					dictionarySelected.find(nowPiece));
				cout << "回答正确。\n";
				if (input.find_last_of("/pass") != string::npos) {
					nowPiece.val = 0;
					cout << "权值已清空！\n";
				}
				goto NXT;
			}

			dictionarySelected.increaseValue(
				dictionarySelected.find(nowPiece));
			if (input + "." == nowPiece.content[piece::EN]) {
				cout << "不喜欢写句号是这样的/cf/cf。\n";
			}
			else {
				cout << "回答错误。\n";
			}
			cout << "正确答案是: " << nowPiece.content[piece::EN] << "\n";

		NXT:
			cout << "按下回车以继续。\n";
			cin.get();
		}
	}
} practitioner;

class Display {
	class Menu {
		class Item {
			size_t width;
			string name;
			bool visiable;

		  public:
			explicit Item(string name = "unnamedMenuItem",
						  bool visiable = true)
				: width(name.size() + 2), name(std::move(name)),
				  visiable(visiable) {}
		};
		vector<Item> items;
		int x0, y0;
		int x1, y1;
		int height;

	  public:
		explicit Menu(int x0 = 0, int y0 = 0, int x1 = 1, int y1 = -1,
					  vector<Item> items = {})
			: x0(x0), y0(y0), x1(x1), y1(y1), items(std::move(items)) {
			height = x1 - x0;
		}
	} menuMain;

  public:
	void redisplayMenu(int x0 = 0, int y0 = 0, int x1 = -1, int y1 = -1) {
		int nowx = 0;
		int nowy = 0;
	}
	void redisplay(int x0 = 0, int y0 = 0, int x1 = -1, int y1 = -1) {}
};
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
	string dictionary;
	cout << "请选择词典: \n";
	cin >> dictionary;
	practitioner.selectDictionary(dictionary.c_str());
	practitioner.practice();
	return 0;
}
/*
void Practice::main() {
	cout << "请选择词库\n";
	cin >> dictionary;
	cout << "请输入加权\n";
	cin >> st >> acval >> waval;
	inputDict(dictionary.c_str(), v);
	systemClear();
	cout << "词典加载成功。\n==预览==\n";
	for (auto i : v) {
		cout << i.content[piece::Language::CN] << " : "
			 << i.content[piece::Language::EN] << "\n";
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
		cout << v[pos].content[piece::Language::CN]
			 << " (value: " << v[pos].val << ", sum: " << sum << ")\n";
		string s;
		do {
			getline(cin, s);
		} while (s == "\n" || s == "\r" || s == "\r\n");
		cout << "\n";
		while (s.back() == ' ') {
			s.pop_back();
		}
		if (s == v[pos].content[piece::Language::EN]) {
			ac++;
			v[pos].val = max(v[pos].val - acval, 0);
			cout << "Accepted.\n";
		}
		else if (s == v[pos].content[piece::Language::EN] + " /pass") {
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
		else if (s + "." == v[pos].content[piece::Language::EN]) {
			cout << "不喜欢加句号是这样的/cf/cf/cf\n", ac++;
		}
		else {
			v[pos].val += waval;
			cout << "Wrong. The answer is:\n";
			cout << v[pos].content[piece::Language::EN] << "\n";
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
	cout << "请选择词库\n";
	cin >> dictionary;
	inputDict(dictionary.c_str(), v);
	systemClear();
	cout << "词典加载成功。\n";
	cout << "==预览==\n";
	for (const auto &i : v) {
		cout << i.content[piece::Language::CN] << " : "
			 << i.content[piece::Language::EN].c_str() << "\n";
	}
	cout << "输入回车开始默写。\n";
	cin.get();
	cin.get();
	for (const auto &i : v) {
		systemClear();
		cout << i.content[piece::Language::CN] << "\n";
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
		if (res[i] == v[i].content[piece::Language::EN]) {
			ac++;
		}
		else {
			wrong.push_back(i);
		}
	}
	cout << "你的得分为："
		 << static_cast<int>(floor(static_cast<double>(ac) / num * 100))
		 << "pts.\n";
	if (ac != num) {
		cout << "你错误的题目：\n";
		for (auto i : wrong) {
			cout << v[i].content[piece::Language::CN] << "\n";
			cout << "你的答案为：" << res[i] << "\n";
			cout << "正确答案为：" << v[i].content[piece::Language::EN].c_str()
				 << "\n";
		}
	}
	else {
		cout << "膜拜 AK King! /bx/bx/bx\n";
	}
}
*/