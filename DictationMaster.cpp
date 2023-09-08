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
	piece(const char **str, int cnt = LanguageCount, int val = 0) // NOLINT
		: val(val) {
		for (int i = 0; i < cnt; ++i) {
			content[i] = str[i];
		}
	}

	auto operator==(const piece &x) const -> bool {
		return this->content[EN] == x.content[EN];
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
	data.emplace_back();
	ifstream din(filename);
	int language = 0;
	auto formatString = [](string &x) -> void {
		x.erase(0, x.find_first_not_of(' '));
		x.erase(x.find_last_not_of(' ') + 1);
	};
	while (!din.eof()) {
		int input = din.get();
		if (input == '\r') {
			continue;
		}
		if (input == '\n') {
			formatString(data.back().content[language]);
			data.emplace_back();
			language = 0;
			continue;
		}
		if (input == ':') {
			formatString(data.back().content[language]);
			data.back().content[++language] = "";
			continue;
		}
		data.back().content[language].push_back(static_cast<char>(input));
	}
	data.pop_back();
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
		systemClear();

		int valueStable;
		int valueIncrease;
		int valueDecrease;
		cout << "请输入加权 [默认 增长 衰减] : ";
		cin >> valueStable >> valueIncrease >> valueDecrease;
		dictionarySelected.setValueStable(valueStable);
		dictionarySelected.setValueIncrease(valueIncrease);
		dictionarySelected.setValueDecrease(valueDecrease);
		dictionarySelected.setValueToStable();
		systemClear();

		while (dictionarySelected.getValueSum() != 0) {
			piece &nowPiece = gen() % 2 == 0
								  ? dictionarySelected.maxiumSelect()
								  : dictionarySelected.randomSelect(1);
			cout << nowPiece.content[piece::CN] << " (value:" << nowPiece.val
				 << ", sum:" << dictionarySelected.getValueSum() << ")\n";
			string input;
			while (input.empty()) {
				getline(cin, input);
				while (input.back() == ' ' || input.back() == '\n' ||
					   input.back() == '\r') {
					input.pop_back();
				}
			}

			if (input == "/finish") {
				break;
			}
			if (input == "/skip") {
				cout << "跳过此单词。\n";
				goto NXT;
			}

			if (input.find(nowPiece.content[piece::EN]) == 0 &&
				(input.size() == nowPiece.content[piece::EN].size() ||
				 input.substr(nowPiece.content[piece::EN].size(), 2) ==
					 " /")) {
				dictionarySelected.decreaseValue(
					dictionarySelected.find(nowPiece));
				cout << "回答正确。\n";
				if (input.find("/pass") != string::npos) {
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
			systemClear();
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
