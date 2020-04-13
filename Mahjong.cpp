#include <algorithm>
#include <utility>
#include <vector>
#include <string>
#include <unordered_map>
#include <cstring>
#include <string.h>
#include <iostream>
#include <set>
#include <functional>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <map>
#include <numeric>
#include <sstream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include "jsoncpp/json.h"

using namespace std;

Json::Value inputValue, outputValue;
Json::Value promptsForDisplay;
int quan;
string lastTile;
string lastOp;
//"DRAW" "PLAY" "PENG" "GANG" "CHI"
string tileCHI, tilePENG, tileGANG;
bool currGANG = false, lastGANG = false;
//是否为大明杠 与开表示牌有关
bool lastBUGANG = false, currBUGANG = false;
//是否为补杠 与抢杠、岭上开花有关
bool lastANGANG = false, currANGANG = false;
//是否为暗杠 与国士无双可以抢暗杠有关
bool isMingPai = false;
//之前回合是否有鸣牌行为发生 与天和地和有关。九种九牌推不倒
int roundStage = -2;
//-2:通知位置
//-1:发牌
//0-3:玩家摸牌
//4-7:玩家打出牌后，通知所有玩家
//8-11:玩家杠牌，通知所有玩家
//12-15:玩家立直，通知所有玩家
int numRestTiles = 70;
bool isPlayDrawnTileOnly = false;

bool is_same_tile(const string & a, const string & b);
vector<vector<int>> string_to_34_tiles(const vector<string>& vec_str, bool five_red);
bool is_multi_mian_zi(const string s, int hasPair);

struct FanFu {
	vector<int> names;
	int fan;
	int fu;
	int score;
	FanFu();
	FanFu(vector<int> n, int fa, int f, int s);
};
struct Tiles {
	string type; // KEZI, SHUNZI, JIANG
	vector<string> part;
	int fu; // 只有刻子我们才算符数
	Tiles();
	Tiles(string t, vector<string> p, int f);

};

void back(vector<vector<Tiles>>& res, vector<Tiles>& vecTiles, vector<int>& num, int N, bool hasPair, char cType);
vector<vector<Tiles>> get_combination_of_type(vector<int>& num, char cType);
vector<vector<Tiles>> get_all_combinations(vector<vector<int>>& nums);
int get_ting_type(vector<string> vec_s, string p, int hasPair);
bool is_yao_jiu(string t);
int get_tile_type(char c);
int get_tile_num(char c);

enum YiZhongType
{
	GUO_SHI_WU_SHUANG_SHI_SAN_MIAN = 0,
	SI_AN_KE_DAN_QI = 1,
	CHUN_ZHENG_JIU_LIAN_BAO_DENG = 2,
	DA_SI_XI = 3,

	GUO_SHI_WU_SHUANG = 4,
	SI_AN_KE = 5,
	JIU_LIAN_BAO_DENG = 6,

	TIAN_HU = 7,
	DI_HU = 8,
	ZI_YI_SE = 9,
	LV_YI_SE = 10,
	QING_LAO_TOU = 11,
	DA_SAN_YUAN = 12,
	SI_GANG_ZI = 13,
	XIAO_SI_XI = 14,

	QING_YI_SE = 15,

	ER_BEI_KOU = 16,
	CHUN_QUAN_DAI_YAO_JIU = 17,
	HUN_YI_SE = 18,

	QI_DUI_ZI = 19,
	SHUANG_LI_ZHI = 20,
	HUN_QUAN_DAI_YAO_JIU = 21,
	SAN_SE_TONG_SHUN = 22,
	DUI_DUI_HU = 23,
	SAN_AN_KE = 24,
	SAN_GANG_ZI = 25,
	SAN_SE_TONG_KE = 26,
	HUN_LAO_TOU = 27,
	XIAO_SAN_YUAN = 28,
	YI_QI_TONG_GUAN = 29,

	LI_ZHI = 30,
	YI_FA = 31,
	MEN_QIAN_QING_ZI_MO = 32,
	PING_HU = 33,
	DUAN_YAO_JIU = 34,
	YI_BEI_KOU = 35,
	YI_PAI_BAI = 36,
	YI_PAI_FA = 37,
	YI_PAI_ZHONG = 38,
	LING_SHANG_KAI_HUA = 39,
	QIANG_GANG = 40,
	HAI_DI_LAO_YUE = 41,
	HE_DI_MO_YU = 42,
	YI_PAI_CHANG_FENG = 43,
	YI_PAI_MENG_FENG = 44,

	LIU_JU_MAN_GUAN = 45,

	BAO_PAI = 46,
	HONG_BAO_PAI = 47,
	LI_BAO_PAI = 48
};
enum FengQuanType
{
	EAST = 0,
	SOUTH = 1,
	WEST = 2,
	NORTH = 3,
};
enum MianZiType
{
	SHUN_ZI = 0,
	KE_ZI = 1,
	GANG_ZI = 2,
};
// 两面、嵌张、边张、双碰、单骑
enum TingPaiType
{
	LIANG_MIAN = 0b1,
	SHUANG_PENG = 0b10,
	DAN_JI = 0b100,
	QIAN_ZHANG = 0b1000,
	BIAN_ZHANG = 0b10000,
};
enum ActionType
{
	CHI = 0,
	PENG = 1,
	GANG = 2,
	BUGANG = 3,
	ANGANG = 4,
	LIZHI = 5,
	RONG = 6,
	TSUMO = 7

};

struct Player {
	int pos; // “东南西北”，编号依次为0~3
	int changFeng; // 每圈开始的时候都要更新一下
	int numLiZhi; // 从自己打出的第几张牌开始立直
	bool isLiZhi; // 是否立直
	bool isYiFa;
	bool isZhenTing[3]; // 根据是否打出过听牌判断是否振听
						// [0]舍张振听——自己打过了要听的牌，大多数是因为没役，无法荣和除非换听牌
						// [1]同巡振听——三家打了要听的牌，但是没立直也没役，只要自家摸切就可以解除
						// [2]立直振听——立直了，但是漏过了能胡的牌，无法荣和只能自摸
	vector<string> handTiles; // 未打出的手里的牌
	vector<vector<string> > anGangTiles; // 暗杠牌
	vector<vector<string> > mingTiles; // 鸣牌
	vector<pair<int, string> > mingTilesOffer; // 鸣牌时，牌的提供者以及牌是什么

	vector<string> outTiles; // 已经打出的牌
	vector<string> ownTiles; // 手上包括了暗杠、鸣牌在内的所有牌

							 // vector<vector<Tiles> > partitionTiles; // 所有的手牌（注意是手牌“未打出的手里的牌”的拆牌方式）

							 // person put out c
	int check_hu_prerequisite(const string c, int cPlayer);

	void initalize(int p, const vector<string> & hc);

	vector<string> get_own_tiles(bool reCalculate = false);

	bool delete_tile(const string c);
};

using YiZhongCheckFunction = std::function< FanFu(const vector<Tiles>& partition, const Player& p, string t, int pid) >;
struct YiZhongChecker
{
	const char *name;
	YiZhongType id;
	YiZhongCheckFunction checker;
};
YiZhongChecker specialYiZhong[] = {
	{
		"国士无双十三面", GUO_SHI_WU_SHUANG_SHI_SAN_MIAN, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (!(is_yao_jiu(t)) || p.mingTiles.size() != 0 || p.anGangTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	string test = "";
	for (auto & x : p.handTiles) {
		if (!(is_yao_jiu(x)))
			return FanFu({}, 0, 0, 0);
		test += x;
	}
	if (test != "B1B9T1T9W1W9Z1Z2Z3Z4Z5Z6Z7")
		return FanFu({}, 0, 0, 0);
	return FanFu({ GUO_SHI_WU_SHUANG_SHI_SAN_MIAN }, 26, 0, 0);
}
	},
	{
		"国士无双", GUO_SHI_WU_SHUANG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (!(is_yao_jiu(t)) || p.mingTiles.size() != 0 || p.anGangTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	string test = "";
	for (auto & x : p.handTiles) {
		if (!(is_yao_jiu(x)))
			return FanFu({}, 0, 0, 0);
		test += x;
	}
	if (test == "B1B9T1T9W1W9Z1Z2Z3Z4Z5Z6Z7")
		return FanFu({}, 0, 0, 0);

	vector<string> hc = p.handTiles;
	hc.push_back(t);
	vector<string>::iterator it = unique(hc.begin(), hc.end());
	hc.erase(it, hc.end());
	sort(hc.begin(), hc.end());
	test = "";
	for (auto & x : hc) {
		test += x;
	}
	if (test != "B1B9T1T9W1W9Z1Z2Z3Z4Z5Z6Z7")
		return FanFu({}, 0, 0, 0);
	return FanFu({ GUO_SHI_WU_SHUANG }, 13, 0, 0);
}
	},
	{
		"七对子", QI_DUI_ZI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.anGangTiles.size() || p.mingTiles.size())
		return FanFu({}, 0, 0, 0);
	auto hc = p.handTiles;
	hc.push_back(t);
	auto nums = string_to_34_tiles(hc, false);
	int cnt = 0;
	for (int i = 0; i < 4; i++) {
		for (int j = 1; j < 10; j++) {
			if (nums[i][j] == 2)
				cnt++;
		}
	}
	if (cnt != 7)
		return FanFu({}, 0, 0, 0);
	return FanFu({ QI_DUI_ZI }, 2, 25, 0);
}
	},

};
YiZhongChecker yakumanYiZhong[] = {
	{
		"四暗刻单骑", SI_AN_KE_DAN_QI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.mingTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	int tmp = 0;
	string last = "";
	for (auto & x : p.handTiles) {
		if (last == x)
			tmp++;
		else {
			if (tmp == 2 || tmp == 4 || (tmp == 1 && last != t))
				return FanFu({}, 0, 0, 0);
			tmp = 1;
			last = x;
		}
	}
	return FanFu({ SI_AN_KE_DAN_QI }, 26, 0, 0);
}
	},
	{
		"纯正九莲宝灯", CHUN_ZHENG_JIU_LIAN_BAO_DENG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.mingTiles.size() != 0 || p.anGangTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	int type_of_tiles;
	for (type_of_tiles = 0; type_of_tiles < 3; type_of_tiles++)
		if (nums[type_of_tiles][10] == 13)
			break;
	string strT = "BTWZ";
	if (type_of_tiles == 3 || t[0] != strT[type_of_tiles])
		return FanFu({}, 0, 0, 0);
	if (nums[type_of_tiles][1] != 3 || nums[type_of_tiles][9] != 3)
		return FanFu({}, 0, 0, 0);
	for (int i = 2; i <= 8; i++)
		if (nums[type_of_tiles][i] != 1)
			return FanFu({}, 0, 0, 0);
	return FanFu({ CHUN_ZHENG_JIU_LIAN_BAO_DENG }, 26, 0, 0);
}
	},
	{
		"大四喜", DA_SI_XI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 大四喜需要有东南西北四种刻子 Z 1234每种牌都需要有3张及以上
	// 另外还需要有将牌，将牌可以是任意的
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	for (int idx = 1; idx <= 4; idx++)
		if (nums[3][idx] < 3)
			return FanFu({}, 0, 0, 0);
	int num_of_pair = 0;
	for (int type_of_tiles = 0; type_of_tiles < 3; type_of_tiles++)
		for (int idx = 1; idx <= 9; idx++)
			num_of_pair += (nums[type_of_tiles][idx] == 2);
	for (int idx = 5; idx <= 7; idx++)
		num_of_pair += (nums[3][idx] == 2);
	return num_of_pair == 1 ? FanFu({ DA_SI_XI }, 26, 0, 0) : FanFu({}, 0, 0, 0);
}
	},
	{
		"四暗刻", SI_AN_KE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.mingTiles.size() != 0 || p.pos != pid)
		return FanFu({}, 0, 0, 0);
	// 要和四暗刻单骑区分开来，手里一定有两张t；这种情况下只能自摸，否则以三暗刻算
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, false);
	int type_of_tile = get_tile_type(t[0]), num_of_tile = get_tile_num(t[1]);
	if (nums[type_of_tile][num_of_tile] != 3)
		return FanFu({}, 0, 0, 0);

	int countKeZi = 0, countPair = 0;
	for (int type_of_tiles = 0; type_of_tiles < 4; type_of_tiles++)
		for (int idx = 1; idx <= 9; idx++) {
			if (nums[type_of_tiles][idx] >= 3)
				countKeZi++;
			else if (nums[type_of_tiles][idx] == 2)
				countPair++;
			else if (nums[type_of_tiles][idx] == 1)
				return FanFu({}, 0, 0, 0);
		}
	if (countKeZi == 4 && countPair == 1)
		return FanFu({ SI_AN_KE }, 13, 0, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"九莲宝灯", JIU_LIAN_BAO_DENG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.mingTiles.size() != 0 || p.anGangTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	int type_of_tiles = get_tile_type(t[0]);
	if (nums[type_of_tiles][10] != 13 || type_of_tiles == 3)
		return FanFu({}, 0, 0, 0);
	bool flag = true;

	if (nums[type_of_tiles][1] != 3 || nums[type_of_tiles][9] != 3)
		flag = false;
	for (int i = 2; i <= 8; i++)
		if (nums[type_of_tiles][i] != 1)
			flag = false;
	if (flag)
		return FanFu({}, 0, 0, 0);

	hc.push_back(t);
	nums = string_to_34_tiles(hc, true);
	if ((nums[type_of_tiles][1] > 3 && nums[type_of_tiles][9] > 3) ||
		nums[type_of_tiles][1] < 3 || nums[type_of_tiles][9] < 3)
		return FanFu({}, 0, 0, 0);
	else if (nums[type_of_tiles][1] == 3 && nums[type_of_tiles][9] == 3) {
		int cnt = 0;
		for (int i = 2; i <= 8; i++) {
			if (nums[type_of_tiles][i] == 0 || nums[type_of_tiles][i] > 2)
				return FanFu({}, 0, 0, 0);
			else if (nums[type_of_tiles][i] == 2)
				cnt++;
		}
		if (cnt != 1)
			return FanFu({}, 0, 0, 0);
	}
	else {
		for (int i = 2; i <= 8; i++)
			if (nums[type_of_tiles][i] != 1)
				return FanFu({}, 0, 0, 0);
	}
	return FanFu({ JIU_LIAN_BAO_DENG }, 13, 0, 0);
}
	},
	{
		"天和", TIAN_HU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 第一巡亲家自摸胡牌
	if (p.pos != 0 || p.pos != pid || p.mingTiles.size() != 0 || p.anGangTiles.size() != 0 || p.outTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	return FanFu({ TIAN_HU }, 13, 0, 0);
}
	},
	{
		"地和", DI_HU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 第一巡闲家在没有人鸣牌时自摸胡牌
	if (p.pos == 0 || p.pos != pid || p.mingTiles.size() != 0 || p.anGangTiles.size() != 0 || p.outTiles.size() != 0 || isMingPai)
		return FanFu({}, 0, 0, 0);
	return FanFu({ DI_HU }, 13, 0, 0);
}
	},
	{
		"字一色", ZI_YI_SE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 字一色由 东南西北白发中 七种牌组成
	if (t[0] != 'Z')
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	for (int type_of_tiles = 0; type_of_tiles < 3; type_of_tiles++)
		if (nums[type_of_tiles][10])
			return FanFu({}, 0, 0, 0);
	// check QI_DUI_ZI
	if (p.anGangTiles.size() == 0 && p.mingTiles.size() == 0) {
		bool flag = true;
		for (int idx = 1; idx <= 7; idx++)
			if (nums[3][idx] != 2)
				flag = false;
		if (flag)
			return FanFu({ ZI_YI_SE }, 13, 0, 0);
	}
	// check usual
	int countKeZi = 0, countPair = 0;
	for (int idx = 1; idx <= 7; idx++) {
		if (nums[3][idx] >= 3)
			countKeZi++;
		else if (nums[3][idx] == 2)
			countPair++;
		else if (nums[3][idx] == 1)
			return FanFu({}, 0, 0, 0);
	}
	if (countKeZi == 4 && countPair == 1)
		return FanFu({ ZI_YI_SE }, 13, 0, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"绿一色", LV_YI_SE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 绿一色由 T2 T3 T4 T6 T8 Z6 条23468和发 六种牌组成 (type_of_tiles = 1 or 3)
	// 绿一色肯定不能是七对子，只能是常见胡牌牌型

	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	if (nums[1][1] || nums[1][5] || nums[1][7] || nums[1][9]
		|| nums[0][10] || nums[2][10] || (nums[3][6] != nums[3][10]))
		return FanFu({}, 0, 0, 0);

	return FanFu({ LV_YI_SE }, 13, 0, 0);
}
	},
	{
		"清老头", QING_LAO_TOU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 清老头由 B1 B9 T1 T9 W1 W9 六种牌组成，也不可能是七对子
	// 没有顺子，只能是刻子
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	if (nums[3][10]) // 不能有字牌
		return FanFu({}, 0, 0, 0);
	int countKeZi = 0, countPair = 0;
	for (int type_of_tiles = 0; type_of_tiles < 3; type_of_tiles++) {
		if (nums[type_of_tiles][1] + nums[type_of_tiles][9] != nums[type_of_tiles][10])
			return FanFu({}, 0, 0, 0);
		countKeZi += (nums[type_of_tiles][1] >= 3) + (nums[type_of_tiles][9] >= 3);
		countPair += (nums[type_of_tiles][1] == 2) + (nums[type_of_tiles][9] == 2);
		if (nums[type_of_tiles][1] == 1 || nums[type_of_tiles][9] == 1)
			return FanFu({}, 0, 0, 0);
	}
	return FanFu({ QING_LAO_TOU }, 13, 0, 0);
}
	},
	{
		"大三元", DA_SAN_YUAN, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 收集了三元牌的三组刻子或杠子，另外还有一个面子，然后是雀头
	// 三元牌不能做雀头
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][5] < 3 || nums[3][6] < 3 || nums[3][7] < 3)
		return FanFu({}, 0, 0, 0);
	return FanFu({ DA_SAN_YUAN }, 13, 0, 0);
}
	},
	{
		"四杠子", SI_GANG_ZI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 进行了四次杠操作，暗杠、大明杠均可，所以手里只有单张牌了
	if (p.handTiles.size() != 1)
		return FanFu({}, 0, 0, 0);
	int timesGang = p.anGangTiles.size();
	for (auto & x : p.mingTiles) {
		if (x.size() == 4)
			timesGang++;
		else
			return FanFu({}, 0, 0, 0);
	}
	if (timesGang == 4 && p.handTiles[0] == t)
		return FanFu({ SI_GANG_ZI }, 13, 0, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"小四喜", XIAO_SI_XI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 东南西北其中的3种以刻子（或者杠子）存在，剩下的一种作雀头的和了形式。
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	int countKeZi = 0, countPair = 0;
	for (int idx = 1; idx <= 4; idx++) {
		countKeZi += nums[3][idx] >= 3;
		countPair += nums[3][idx] == 2;
		if (nums[3][idx] == 1)
			return FanFu({}, 0, 0, 0);
	}
	if (countKeZi == 3 && countPair == 1)
		return FanFu({ XIAO_SI_XI }, 13, 0, 0);
	return FanFu({}, 0, 0, 0);
}
	},

};
YiZhongChecker usualYiZhong[] = {
	{
		"清一色", QING_YI_SE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 清一色不能含有字牌，所以只考虑BTW。七对子已经考虑在其中
	int baseFan = 6;
	if (p.mingTiles.size() > 0)
		baseFan--;
	char c = t[0];
	if (c == 'Z')
		return FanFu({}, 0, 0, 0);
	int typeT = get_tile_type(c);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[typeT][10] != hc.size())
		return FanFu({}, 0, 0, 0);
	return FanFu({ QING_YI_SE }, baseFan, 0, 0);
}
	},
	{
		"二杯口", ER_BEI_KOU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.anGangTiles.size() != 0 || p.mingTiles.size() != 0 || partition.empty())
		return FanFu({}, 0, 0, 0);
	int type_of_tiles = 0;
	vector<vector<string>> vecTiles(3); // 取每个顺子的第一张

	for (auto & x : partition) {
		if (x.type == "KEZI")
			return FanFu({}, 0, 0, 0);
		if (x.type == "JIANG")
			continue;
		type_of_tiles = get_tile_type(x.part[0][0]);
		vecTiles[type_of_tiles].push_back(x.part[0]);
	}

	for (int i = 0; i < 3; i++) {
		if (vecTiles[i].size() == 2) {
			if (vecTiles[i][0] != vecTiles[i][1])
				return FanFu({}, 0, 0, 0);
		}
		else if (vecTiles[i].size() == 4) {
			sort(vecTiles[i].begin(), vecTiles[i].end());
			if (vecTiles[i][0] != vecTiles[i][1] || vecTiles[i][2] != vecTiles[i][3])
				return FanFu({}, 0, 0, 0);
		}
		else
			return FanFu({}, 0, 0, 0);
	}
	return FanFu({ ER_BEI_KOU }, 3, 0, 0);
}
	},
	{
		"纯全带幺九", CHUN_QUAN_DAI_YAO_JIU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 面子只能是111(1),123,999(9),789，雀头只能是11,99，不可能是七对子
	// 和清老头区别开来，纯全带幺九一定含有顺子
	if (t[0] == 'Z' || partition.empty())
		return FanFu({}, 0, 0, 0);
	int baseFan = 3;
	if (p.mingTiles.size())
		baseFan--;

	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][10])
		return FanFu({}, 0, 0, 0);

	int type_of_tile = get_tile_type(t[0]);

	for (auto & tiles : p.anGangTiles)
		if (!is_yao_jiu(tiles[0]))
			return FanFu({}, 0, 0, 0);
	for (auto & tiles : p.mingTiles)
		if (!is_yao_jiu(tiles[0]) && !is_yao_jiu(tiles[2]))
			return FanFu({}, 0, 0, 0);

	int fu = 0;
	int cntShunZi = 0;
	for (auto & x : partition) {
		cntShunZi += x.type == "SHUNZI";
		if (x.type == "JIANG" || x.type == "KEZI") {
			if (!is_yao_jiu(x.part[0]))
				return FanFu({}, 0, 0, 0);
			if (x.type == "KEZI") {
				if (pid == p.pos || x.part[0] != t)
					fu += 8;
				else if (nums[type_of_tile][get_tile_num(t[1])] == 4)
					fu += 8;
				else // 刻子形成如果是来自他人，则算为明刻
					fu += 4;
			}
		}
		else if (!is_yao_jiu(x.part[0]) && !is_yao_jiu(x.part[2]))
			return FanFu({}, 0, 0, 0);
	}
	if (cntShunZi == 0)
		return FanFu({}, 0, 0, 0);
	return FanFu({ CHUN_QUAN_DAI_YAO_JIU }, baseFan, fu, 0);
}
	},
	{
		"混一色", HUN_YI_SE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	// 去除清一色
	if (nums[3][10] == 0)
		return FanFu({}, 0, 0, 0);
	int baseFan = 3;
	if (p.mingTiles.size())
		baseFan--;

	int type_of_tiles; // 确定染的是哪一个色
	for (type_of_tiles = 0; type_of_tiles < 3; type_of_tiles++) {
		if (nums[type_of_tiles][10])
			break;
	}
	int tmp = hc.size() - nums[3][10];
	if (tmp != nums[type_of_tiles][10])
		return FanFu({}, 0, 0, 0);

	// 有可能是七对子
	if (p.mingTiles.size() == 0 && p.anGangTiles.size() == 0) {
		int cnt = 0;
		for (int j = 1; j <= 9; j++)
			if (nums[type_of_tiles][j] == 2)
				cnt++;
		for (int j = 1; j <= 7; j++)
			if (nums[3][j] == 2)
				cnt++;
		if (cnt == 7)
			return FanFu({ HUN_YI_SE }, 3, 0, 0);
	}

	int fu = 0;

	for (auto & x : partition) {
		if (x.type == "KEZI") {
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (t[0] != 'Z' && nums[type_of_tiles][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	return FanFu({ HUN_YI_SE }, baseFan, fu, 0);
}
	},
	{
		"双立直", SHUANG_LI_ZHI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 待判断
	// 在第一巡手上牌已经听牌，且之前没有人鸣牌
	if (p.mingTiles.size() != 0 || p.anGangTiles.size() != 0 || isMingPai || !p.isLiZhi)
		return FanFu({}, 0, 0, 0);
	if (p.numLiZhi == 1)
		return FanFu({ SHUANG_LI_ZHI }, 2, 0, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"混全带幺九", HUN_QUAN_DAI_YAO_JIU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 需要和纯全带幺九、混老头等分开来。混全带幺九+七对子=混老头
	int baseFan = 2;
	if (p.mingTiles.size())
		baseFan--;

	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	// 去除纯全带。混全一定存在顺子
	if (nums[3][10] == 0 || partition.empty())
		return FanFu({}, 0, 0, 0);
	// 去除混老头
	int cntShunZi = 0;
	for (auto & x : partition)
		cntShunZi += x.type == "SHUNZI";
	if (cntShunZi == 0)
		return FanFu({}, 0, 0, 0);

	for (auto & tiles : p.anGangTiles)
		if (!is_yao_jiu(tiles[0]))
			return FanFu({}, 0, 0, 0);
	for (auto & tiles : p.mingTiles)
		if (!is_yao_jiu(tiles[0]) && !is_yao_jiu(tiles[2]))
			return FanFu({}, 0, 0, 0);

	int type_of_tile = get_tile_type(t[0]);

	int fu = 0;
	for (auto & x : partition) {
		if (x.type == "JIANG" || x.type == "KEZI") {
			if (!is_yao_jiu(x.part[0]))
				return FanFu({}, 0, 0, 0);
			if (x.type == "KEZI") {
				if (pid == p.pos || x.part[0] != t)
					fu += 8;
				else if (type_of_tile != 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
					fu += 8;
				else // 刻子形成如果是来自他人，则算为明刻
					fu += 4;
			}
		}
		else if (!is_yao_jiu(x.part[0]) && !is_yao_jiu(x.part[2]))
			return FanFu({}, 0, 0, 0);
	}
	return FanFu({ HUN_QUAN_DAI_YAO_JIU }, baseFan, fu, 0);
}
	},
	{
		"三色同顺", SAN_SE_TONG_SHUN, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	int baseFan = 2;
	if (p.mingTiles.size())
		baseFan--;
	vector<vector<int>> nMin(3, vector<int>());
	int type_of_tile, fu = 0;
	for (auto & x : p.mingTiles) {
		type_of_tile = get_tile_type(x[0][0]);
		if (type_of_tile == 3)
			continue;
		if (x[0][1] != x[1][1])
			nMin[type_of_tile].push_back(get_tile_num(x[0][1]));
	}
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	for (auto & x : partition) {
		type_of_tile = get_tile_type(x.part[0][0]);
		if (type_of_tile == 3)
			continue;
		if (x.type == "SHUNZI")
			nMin[type_of_tile].push_back(get_tile_num(x.part[0][1]));
		if (x.type == "KEZI") {
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	int tmp;
	for (int i = 0; i < 3; i++) {
		if (nMin[i].size() == 0 || nMin[i].size() > 2)
			return FanFu({}, 0, 0, 0);
		if (nMin[i].size() == 1)
			tmp = nMin[i][0];
	}
	for (int i = 0; i < 3; i++) {
		auto it = find(nMin[i].begin(), nMin[i].end(), tmp);
		if (it == nMin[i].end())
			return FanFu({}, 0, 0, 0);
	}
	return FanFu({ SAN_SE_TONG_SHUN }, baseFan, fu, 0);
}
	},
	{
		"对对和", DUI_DUI_HU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	int countPairs = 0;
	for (auto & part : partition) {
		countPairs += part.type == "JIANG";
	}
	if (countPairs >= 2)
		return FanFu({}, 0, 0, 0);

	int fu = 0;
	for (auto & tiles : p.mingTiles) {
		if (tiles[0][1] != tiles[1][1])
			return FanFu({}, 0, 0, 0);
	}

	for (auto & x : partition) {
		if (x.type == "SHUNZI")
			return FanFu({}, 0, 0, 0);
		if (x.type == "KEZI") {
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	return FanFu({ DUI_DUI_HU }, 2, fu, 0);
}
	},
	{
		"三暗刻", SAN_AN_KE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 三暗刻发生在anGangTiles和handTiles中
	// 另外还有一个面子和对子
	if (p.mingTiles.size() > 1 || partition.empty())
		return FanFu({}, 0, 0, 0);
	int countAnKe = p.anGangTiles.size(), fu = 0, type_of_tile = get_tile_type(t[0]);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	for (auto & x : partition) {
		if (x.type == "SHUNZI" || x.type == "JIANG")
			continue;
		if (pid == p.pos || x.part[0] != t) {
			countAnKe++;
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
		}
		else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4) {
			countAnKe++;
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
		}
		else // 刻子形成如果是来自他人，则算为明刻
			fu += 2 + 2 * is_yao_jiu(x.part[0]);
	}
	if (countAnKe == 3)
		return FanFu({ SAN_AN_KE }, 2, fu, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"三杠子", SAN_GANG_ZI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	// 三杠子要在鸣牌和暗杠里找。
	int countGangZi = p.anGangTiles.size(), fu = 0, type_of_tile = get_tile_type(t[0]);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	for (auto & x : p.mingTiles) {
		if (x.size() == 4)
			countGangZi++;
	}
	for (auto & x : partition) {
		if (x.type == "SHUNZI" || x.type == "JIANG")
			continue;
		if (pid == p.pos || x.part[0] != t)
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
		else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
		else // 刻子形成如果是来自他人，则算为明刻
			fu += 2 + 2 * is_yao_jiu(x.part[0]);
	}
	if (countGangZi == 3)
		return FanFu({ SAN_GANG_ZI }, 2, fu, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"三色同刻", SAN_SE_TONG_KE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	// 和三色同顺类似，需要找到最少花色的一种牌
	int fu = 0, type_of_tile;
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	vector<vector<int> > cntNum(3, vector<int>());
	for (auto & x : p.anGangTiles) {
		type_of_tile = get_tile_type(x[0][0]);
		if (type_of_tile == 3)
			continue;
		cntNum[type_of_tile].push_back(get_tile_num(x[0][1]));
	}
	for (auto & x : p.mingTiles) {
		type_of_tile = get_tile_type(x[0][0]);
		if (type_of_tile == 3 || x[0][1] != x[1][1])
			continue;
		cntNum[type_of_tile].push_back(get_tile_num(x[0][1]));
	}
	for (auto & x : partition) {
		if (x.type == "SHUNZI" || x.type == "JIANG")
			continue;
		type_of_tile = get_tile_type(x.part[0][0]);
		if (type_of_tile < 3)
			cntNum[type_of_tile].push_back(get_tile_num(x.part[0][1]));
		if (pid == p.pos || x.part[0] != t)
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
		else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
		else // 刻子形成如果是来自他人，则算为明刻
			fu += 2 + 2 * is_yao_jiu(x.part[0]);
	}
	int tmp = 0;
	for (int i = 0; i < 3; i++) {
		if (cntNum[i].size() == 0 || cntNum[i].size() > 2)
			return FanFu({}, 0, 0, 0);
		if (cntNum[i].size() == 1)
			tmp = cntNum[i][0];
	}
	for (int i = 0; i < 3; i++) {
		auto it = find(cntNum[i].begin(), cntNum[i].end(), tmp);
		if (it == cntNum[i].end())
			return FanFu({}, 0, 0, 0);
	}
	return FanFu({ SAN_SE_TONG_KE }, 2, fu, 0);
}
	},
	{
		"混老头", HUN_LAO_TOU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 混老头只能用BTW的19和字牌
	int fu = 0, type_of_tile;
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][10] == 0) // 去除清老头
		return FanFu({}, 0, 0, 0);
	// 有可能是七对子
	if (p.mingTiles.size() == 0 && p.anGangTiles.size() == 0) {
		int cnt = 0;
		for (int i = 0; i < 3; i++)
			cnt += (nums[i][1] == 2) + (nums[i][9] == 2);
		for (int i = 1; i <= 7; i++)
			cnt += (nums[3][i] == 2);
		if (cnt == 7)
			return FanFu({ HUN_LAO_TOU }, 2, 0, 0);
	}

	for (int i = 0; i < 3; i++)
		if (nums[i][1] + nums[i][9] != nums[i][10])
			return FanFu({}, 0, 0, 0);
	for (auto & x : partition) {
		if (x.type == "KEZI") {
			type_of_tile = get_tile_type(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 8;
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 8;
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 4;
		}
	}
	return FanFu({ HUN_LAO_TOU }, 2, fu, 0);
}
	},
	{
		"小三元", XIAO_SAN_YUAN, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	// 小三元指的是白发中有两个刻子，剩下的一个当雀头。
	int fu = 0, type_of_tile;
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	int countKeZi = 0, countPair = 0;
	for (int idx = 5; idx <= 7; idx++) {
		countKeZi += nums[3][idx] >= 3;
		countPair += nums[3][idx] == 2;
	}

	for (auto & x : partition) {
		if (x.type == "SHUNZI" || x.type == "JIANG")
			continue;
		type_of_tile = get_tile_type(x.part[0][0]);
		if (pid == p.pos || x.part[0] != t)
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
		else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
		else // 刻子形成如果是来自他人，则算为明刻
			fu += 2 + 2 * is_yao_jiu(x.part[0]);
	}

	if (countKeZi == 2 && countPair == 1)
		return FanFu({ XIAO_SAN_YUAN }, 2, fu, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"一气通贯", YI_QI_TONG_GUAN, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	// 判断比较很简单。是数量最多的花色，至少有9张。不可能出现这个花色的暗杠，因为顺子需要
	int baseFan = 2, type_of_tile, fu = 0, obj;
	if (p.mingTiles.size())
		baseFan--;
	vector<int> cntNum;
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	for (obj = 0; obj < 3; obj++)
		if (nums[obj][10] >= 9)
			break;
	if (obj == 3)
		return FanFu({}, 0, 0, 0);

	for (auto & x : p.mingTiles) {
		type_of_tile = get_tile_type(x[0][0]);
		if (type_of_tile == 3 || x[0][1] == x[1][1] || obj != type_of_tile)
			continue;
		cntNum.push_back(get_tile_num(x[0][1]));
	}
	for (auto & x : partition) {
		type_of_tile = get_tile_type(x.part[0][0]);
		if (x.type == "JIANG")
			continue;
		else if (x.type == "SHUNZI") {
			if (obj == type_of_tile)
				cntNum.push_back(get_tile_num(x.part[0][1]));
		}
		else {
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	for (int i = 1; i <= 7; i += 3)
		if (find(cntNum.begin(), cntNum.end(), i) == cntNum.end())
			return FanFu({}, 0, 0, 0);
	return FanFu({ YI_QI_TONG_GUAN }, baseFan, fu, 0);
}
	},
	{
		"立直", LI_ZHI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.isLiZhi) {
		if (p.mingTiles.size() == 0 && p.anGangTiles.size() == 0 && !isMingPai && p.numLiZhi == 1)
			return FanFu({}, 0, 0, 0);
		int type_of_tile, fu = 0;
		vector<string> hc = p.ownTiles;
		hc.push_back(t);
		vector<vector<int> > nums = string_to_34_tiles(hc, true);
		for (auto & x : partition) {
			type_of_tile = get_tile_type(x.part[0][0]);
			if (x.type != "KEZI")
				continue;
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
		return FanFu({ LI_ZHI }, 1, fu, 0);
	}

	return FanFu({}, 0, 0, 0);
}
	},
	{
		"一发", YI_FA, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 要先加入胡牌判断。并且需要查看立直、出牌历史
	// 不与岭上开花复合
	if (!p.isLiZhi || !p.isYiFa)
		return FanFu({}, 0, 0, 0);
	return FanFu({ YI_FA }, 1, 0, 0);// 一发一定有立直，所以不重复计算符数
}
	},
	{
		"门前清自摸", MEN_QIAN_QING_ZI_MO, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.pos != pid || p.mingTiles.size() > 0)
		return FanFu({}, 0, 0, 0);
	int type_of_tile, fu = 0;
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	for (auto & x : partition) {
		type_of_tile = get_tile_type(x.part[0][0]);
		if (x.type != "KEZI")
			continue;
		if (pid == p.pos || x.part[0] != t)
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
		else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
		else // 刻子形成如果是来自他人，则算为明刻
			fu += 2 + 2 * is_yao_jiu(x.part[0]);
	}
	return FanFu({ MEN_QIAN_QING_ZI_MO }, 1, fu, 0);
}
	},
	{
		"平和", PING_HU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {

	// 四副顺子及序数牌或客风牌作将牌，胡牌时包含主风牌、场风牌和箭牌的将牌会增加符数，所以不能有
	// 同样需要加入胡牌判断。最后必须是顺子的两面听，不能是边张、单钓中张或者是单骑听雀头。
	// 在算符的时候，如果是门清自摸，则自摸的2符必须不计
	if (p.anGangTiles.size() != 0 || partition.empty() || t[0] == 'Z') // 平和不能有暗杠 因为需要计算符数 t不能是字牌
		return FanFu({}, 0, 0, 0);
	for (auto & x : partition) {
		if (x.type == "KEZI")
			return FanFu({}, 0, 0, 0);
	}

	// 没有确定门前清是为了计算符数。wiki上：平和在门前清时算一番；在副露下算两符
	for (auto & x : p.mingTiles) {
		if (is_same_tile(x[0], x[1]))
			return FanFu({}, 0, 0, 0);
	}

	// Z-p.pos,Z-p.changFeng,Z567的都不能有
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][10] >= 3 || nums[3][5] || nums[3][6] || nums[3][7] || nums[3][p.pos + 1] || nums[3][p.changFeng + 1])
		return FanFu({}, 0, 0, 0);

	int tmp = get_tile_type(t[0]);
	// 不能是边张、单钓中张或者是单骑听雀头。
	int res = get_ting_type(hc, t, 1);
	if ((res & 1) != 1)
		return FanFu({}, 0, 0, 0);
	if (p.pos == pid)
		return FanFu({ PING_HU }, 1, 20, 0);
	else
		return FanFu({ PING_HU }, 1, 30, 0);
}
	},
	{
		"断幺九", DUAN_YAO_JIU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	if (nums[3][10] || nums[0][1] || nums[1][1] || nums[2][1] || nums[0][9] || nums[1][9] || nums[2][9])
		return FanFu({}, 0, 0, 0);

	int type_of_tile, fu = 0;

	// 有可能是七对子
	if (p.mingTiles.size() == 0 && p.anGangTiles.size() == 0) {
		int cnt = 0;
		for (int i = 0; i < 3; i++)
			for (int j = 2; j <= 8; j++)
				cnt += (nums[i][j] == 2);
		if (cnt == 7)
			return FanFu({ DUAN_YAO_JIU }, 1, 0, 0);
	}

	for (auto & x : partition) {
		type_of_tile = get_tile_type(x.part[0][0]);
		if (x.type != "KEZI")
			continue;
		if (pid == p.pos || x.part[0] != t)
			fu += 4;
		else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
			fu += 4;
		else // 刻子形成如果是来自他人，则算为明刻
			fu += 2;
	}
	return FanFu({ DUAN_YAO_JIU }, 1, fu, 0);
}
	},
	{
		"一杯口", YI_BEI_KOU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 要求门前清
	if (p.mingTiles.size() != 0 || p.anGangTiles.size() > 2 || partition.empty())
		return FanFu({}, 0, 0, 0);
	int type_of_tile, fu = 0;
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	vector<vector<int> > cntNum(3, vector<int>());
	for (auto & x : partition) {
		type_of_tile = get_tile_type(x.part[0][0]);
		if (x.type == "SHUNZI")
			cntNum[type_of_tile].push_back(get_tile_num(x.part[0][1]));
		if (x.type == "KEZI") {
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	int cnt = 0;
	for (int i = 0; i < 3; i++) {
		set<int> st(cntNum[i].begin(), cntNum[i].end());
		for (auto & val : st)
			cnt += count(cntNum[i].begin(), cntNum[i].end(), val) >= 2;
	}
	if (cnt == 1) // 和两杯口区分开来
		return FanFu({ YI_BEI_KOU }, 1, fu, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"役牌白", YI_PAI_BAI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][5] < 3)
		return FanFu({}, 0, 0, 0);
	int type_of_tile, fu = 0;
	for (auto & x : partition) {
		if (x.type == "KEZI") {
			type_of_tile = get_tile_type(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	return FanFu({ YI_PAI_BAI }, 1, fu, 0);
}
	},
	{
		"役牌发", YI_PAI_FA, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][6] < 3)
		return FanFu({}, 0, 0, 0);
	int type_of_tile, fu = 0;
	for (auto & x : partition) {
		if (x.type == "KEZI") {
			type_of_tile = get_tile_type(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	return FanFu({ YI_PAI_FA }, 1, fu, 0);
}
	},
	{
		"役牌中", YI_PAI_ZHONG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][7] < 3)
		return FanFu({}, 0, 0, 0);
	int type_of_tile, fu = 0;
	for (auto & x : partition) {
		if (x.type == "KEZI") {
			type_of_tile = get_tile_type(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	return FanFu({ YI_PAI_ZHONG }, 1, fu, 0);
}
	},
	{
		"岭上开花", LING_SHANG_KAI_HUA, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 岭上开花必须是自己杠的牌
	// t是开杠以后得的牌
	if (!(lastBUGANG || lastANGANG || currGANG) || pid != p.pos || partition.empty()) // 岭上开花一定不是七对子
		return FanFu({}, 0, 0, 0);
	int fu = 0;
	for (auto & x : partition)
		if (x.type == "KEZI")
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
	return FanFu({ LING_SHANG_KAI_HUA }, 1, fu, 0);
}
	},
	{
		"抢杠", QIANG_GANG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// 还要另外判断是否胡牌，t是别人加杠，而非大明杠和暗杠下。pid需要和p.pos不一样
	if (p.pos == pid || !lastBUGANG || partition.empty())
		return FanFu({}, 0, 0, 0);

	vector<string> hc = p.ownTiles;
	if (find(hc.begin(), hc.end(), t) != hc.end())// 抢杠肯定不可能形成刻子
		return FanFu({}, 0, 0, 0);

	int fu = 0;
	for (auto & x : partition)
		if (x.type == "KEZI")
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
	return FanFu({ QIANG_GANG }, 1, fu, 0);
}
	},
	{
		"海底捞月", HAI_DI_LAO_YUE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.pos != pid || numRestTiles != 0)	// t是自己摸的最后一张牌
		return FanFu({}, 0, 0, 0);

	int fu = 0;
	for (auto & x : partition)
		if (x.type == "KEZI")
			fu += 4 + 4 * is_yao_jiu(x.part[0]);
	return FanFu({ HAI_DI_LAO_YUE }, 1, fu, 0);
}
	},
	{
		"河底摸鱼", HE_DI_MO_YU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.pos == pid || numRestTiles != 0) // t是别人摸的最后一张牌
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	int type_of_tile, fu = 0;
	for (auto & x : partition) {
		if (x.type == "KEZI") {
			type_of_tile = get_tile_type(x.part[0][0]);
			if (x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	return FanFu({ HE_DI_MO_YU }, 1, fu, 0);
}
	},
	{
		"役牌场风", YI_PAI_CHANG_FENG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][p.changFeng + 1] < 3)
		return FanFu({}, 0, 0, 0);
	int type_of_tile, fu = 0;
	for (auto & x : partition) {
		if (x.type == "KEZI") {
			type_of_tile = get_tile_type(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	return FanFu({ YI_PAI_CHANG_FENG }, 1, fu, 0);
}
	},
	{
		"役牌门风", YI_PAI_MENG_FENG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][p.pos + 1] < 3)
		return FanFu({}, 0, 0, 0);
	int type_of_tile, fu = 0;
	for (auto & x : partition) {
		if (x.type == "KEZI") {
			type_of_tile = get_tile_type(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][get_tile_num(t[1])] == 4)
				fu += 4 + 4 * is_yao_jiu(x.part[0]);
			else // 刻子形成如果是来自他人，则算为明刻
				fu += 2 + 2 * is_yao_jiu(x.part[0]);
		}
	}
	return FanFu({ YI_PAI_MENG_FENG }, 1, fu, 0);
}
	},
};

int get_basepoint(int fan, int fu, bool isYakuman);
int get_score(int fan, int fu, bool isYakuman);
FanFu get_high_fan_fu(vector<FanFu>& vecF);
vector<FanFu> check_HU(int player, const string c, int offer, bool finish);

string ActionName[10] = { "CHI", "PENG", "GANG", "BUGANG", "ANGANG", "LIZHI", "RONG", "TSUMO" };

string FanNames[50] = { "国士无双十三面","四暗刻单骑","纯正九莲宝灯","大四喜","国士无双","四暗刻","九莲宝灯","天和","地和","字一色","绿一色","清老头","大三元","四杠子","小四喜","清一色","二杯口","纯全带幺九","混一色","七对子","双立直","混全带幺九","三色同顺","对对和","三暗刻","三杠子","三色同刻","混老头","小三元","一气通贯","立直","一发","门前清自摸","平和","断幺九","一杯口","役牌白","役牌发","役牌中","岭上开花","抢杠","海底捞月","河底摸鱼","役牌场风","役牌门风","流局满贯","宝牌","红宝牌","里宝牌" };

bool is_same_tile(const string & a, const string & b) {
	if (a.length() != 2 || b.length() != 2)
		return false;
	char val_a = a[1], val_b = b[1];
	if (val_a == '0')
		val_a = '5';
	if (val_b == '0')
		val_b = '5';
	if (a[0] == b[0] && val_a == val_b)
		return true;
	return false;
}
vector<vector<int> > string_to_34_tiles(const vector<string> & vec_str, bool five_red) {
	vector<vector<int> > res(4, vector<int>(11, 0));
	char c;
	int type_of_tiles, idx;
	for (auto & x : vec_str) {
		c = x[0];
		type_of_tiles = get_tile_type(c);
		idx = five_red ? x[1] - '0' : get_tile_num(x[1]);
		res[type_of_tiles][idx]++;
	}
	for (int i = 0; i < 4; i++)
		res[i][10] = accumulate(res[i].begin(), res[i].end(), 0);
	return res;
}

bool is_multi_mian_zi(const string s, int hasPair) {
	// s都是“数字,hasPair为允许s中有对子的个数
	// 拆牌返回vector
	int l = s.length();
	if (l == 0 && hasPair == 0)
		return true;
	if (l == 2 && hasPair == 1)
		return s[0] == s[1];
	if (l < hasPair * 2 || (l - hasPair * 2) % 3 != 0)
		return false;
	int a[12] = { 0 }, maxN = 0;
	bool f[12][6][6][2] = { false };
	f[0][0][0][0] = true;

	for (int i = 0; i < s.length(); i++) {
		a[s[i] - '0']++;
		maxN = max(maxN, s[i] - '0');
	}

	for (int i = 1; i <= 10; i++)
		for (int j = 0; j <= a[i - 1]; j++)
			for (int k = 0; k <= a[i]; k++) {
				if (k >= 2)	f[i][j][k][1] |= f[i][j][k - 2][0];
				if (k >= 3)	f[i][j][k][0] |= f[i][j][k - 3][0];
				if (k >= 3)	f[i][j][k][1] |= f[i][j][k - 3][1];
				if (j >= k && (i == 1 || a[i - 2] >= k))
					f[i][j][k][0] |= f[i - 1][(i >= 2 ? a[i - 2] - k : 0)][j - k][0];
				if (j >= k && (i == 1 || a[i - 2] >= k))
					f[i][j][k][1] |= f[i - 1][(i >= 2 ? a[i - 2] - k : 0)][j - k][1];

			}
	return f[maxN][a[maxN - 1]][a[maxN]][hasPair];
}

Tiles::Tiles() {}

Tiles::Tiles(string t, vector<string> p, int f) {
	type = t;
	part = p;
	fu = f;
}

FanFu::FanFu() {}
FanFu::FanFu(vector<int> n, int fa, int f, int s) {
	names = n;
	fan = fa, fu = f, score = s;
}

void back(vector<vector<Tiles> > & res, vector<Tiles> & vecTiles, vector<int> & num, int N, bool hasPair, char cType) { // hasPair表示需要从num中找到合适的一个雀头
	int i;
	for (i = 1; i <= N; i++)
		if (num[i] != 0)
			break;
	if (i == N + 1 && !hasPair) {
		res.push_back(vecTiles);
		return;
	}
	vector<string> part;
	if (cType == 'Z') {
		if (num[i] % 3 == 1) return;
		else if (num[i] == 2) {
			num[i] = 0;
			string tmp;
			tmp.push_back(cType); tmp.push_back(i + '0');
			vecTiles.push_back(Tiles("JIANG", vector<string>{tmp, tmp}, 0));
			back(res, vecTiles, num, N, false, cType);
			num[i] = 2; vecTiles.pop_back();
		}
		else if (num[i] == 3) {
			num[i] = 0;
			string tmp;
			tmp.push_back(cType); tmp.push_back(i + '0');
			vecTiles.push_back(Tiles("KEZI", vector<string>{tmp, tmp, tmp}, 0));
			back(res, vecTiles, num, N, false, cType);
			num[i] = 3; vecTiles.pop_back();
		}
	}
	else {
		// 先确定雀头
		if (hasPair) {
			for (int j = i; j <= N; j++) {
				if (num[j] >= 2) {
					string tmp;
					tmp.push_back(cType); tmp.push_back(j + '0');
					vecTiles.push_back(Tiles("JIANG", vector<string>{tmp, tmp}, 0)); // TODO
					num[j] -= 2;
					back(res, vecTiles, num, N, false, cType);
					num[j] += 2; vecTiles.pop_back();
				}
			}
		}
		else {
			if (num[i] >= 3) {
				string tmp;
				tmp.push_back(cType); tmp.push_back(i + '0');
				vecTiles.push_back(Tiles("KEZI", vector<string>{tmp, tmp, tmp}, 0));
				num[i] -= 3;
				back(res, vecTiles, num, N, false, cType);
				num[i] += 3;
				vecTiles.pop_back();
			}
			if (i + 2 <= N) {
				int minNum = min(min(num[i], num[i + 1]), num[i + 2]);
				if (minNum == 0) return;
				part.clear();
				string tmp;
				tmp.push_back(cType); tmp.push_back(i + '0');
				for (int k = 0; k < 3; k++) {
					tmp[1] = i + '0' + k;
					part.push_back(tmp);
				}
				for (int k = 0; k < minNum; k++)
					vecTiles.push_back(Tiles("SHUNZI", part, 0));
				num[i] -= minNum; num[i + 1] -= minNum; num[i + 2] -= minNum;
				back(res, vecTiles, num, N, false, cType);
				num[i] += minNum; num[i + 1] += minNum; num[i + 2] += minNum;
				for (int k = 0; k < minNum; k++) vecTiles.pop_back();
			}
		}
	}
}
// 返回一种花色的牌的拆牌结果
vector<vector<Tiles>> get_combination_of_type(vector<int> & num, char cType) {
	vector<vector<Tiles>> res;
	vector<Tiles> vecTiles;
	res.clear(); vecTiles.clear();
	if (num[10] == 0)
		return res;
	vector<int> a = num;
	bool hasPair = a[10] % 3 == 2;
	a[10] = 0;
	int N;
	for (int i = 0; i < a.size(); i++)
		if (a[i] != 0)
			N = i;
	back(res, vecTiles, a, N, hasPair, cType);
	return res;
}
vector<vector<Tiles>> get_all_combinations(vector<vector<int>> & nums) {
	int sumAll = nums[0][10] + nums[1][10] + nums[2][10] + nums[3][10];
	string strType = "BTWZ";
	vector<vector<vector<Tiles>>> allTypedTiles;
	vector<vector<Tiles>> blank(1, vector<Tiles>());
	for (int i = 0; i <= 3; i++) {
		auto tmp = get_combination_of_type(nums[i], strType[i]);
		if (tmp.size() == 0)
			continue;
		allTypedTiles.push_back(tmp);
	}
	int k = 4 - allTypedTiles.size();
	for (int i = 0; i < k; i++)
		allTypedTiles.push_back(blank);

	vector<vector<Tiles>> res;// 每个花色都组合一遍
	for (auto & tiles_0 : allTypedTiles[0])
		for (auto & tiles_1 : allTypedTiles[1])
			for (auto & tiles_2 : allTypedTiles[2])
				for (auto & tiles_3 : allTypedTiles[3]) {
					vector<Tiles> tmp;
					for (auto & x : tiles_0)
						tmp.push_back(x);
					for (auto & x : tiles_1)
						tmp.push_back(x);
					for (auto & x : tiles_2)
						tmp.push_back(x);
					for (auto & x : tiles_3)
						tmp.push_back(x);
					res.push_back(tmp);
				}
	bool flag = false;
	for (auto & partition : res) {
		int pairCnt = 0, cnt = 0;
		for (auto & x : partition) {
			pairCnt += x.type == "JIANG";
			cnt += x.type == "JIANG" ? 2 : 3;
		}
			
		if (pairCnt == 1 && cnt == sumAll)
			flag = true;
	}
	if (!flag)
		res = {};

	return res;
}
// 听牌类型检测 两面 0b1 、双碰 0b10 、嵌张 0b1000 、边张 0b10000、单骑 0b100
int get_ting_type(vector<string> vec_s, string c, int hasPair) { // vec_s包含了p,p代表的是新加入的牌
	if (vec_s.size() == 0)
		return 0;

	string s, p = c;
	if (p[0] != 'Z' && p[1] == '0')
		p[1] = '5';
	vector<int> nums(15, 0);
	for (auto & x : vec_s) {
		if (x[0] == p[0]) {
			s.push_back(x[1] == '0' ? '5' : x[1]);
			nums[get_tile_num(x[1])]++;
		}
	}
	sort(s.begin(), s.end());

	if ((vec_s.size() - hasPair * 2) % 3 != 0 || hasPair < 0 || s.length() % 3 == 1)
		return -1;
	hasPair = (s.length() % 3) == 2;

	int res = 0;

	// check 双碰
	if (hasPair && nums[p[1] - '0'] >= 3) {
		string tmp = s;
		auto it = tmp.find_first_of(p);
		tmp.replace(it, 3, "");
		bool flag = false;
		for (int i = 1; i <= 9; i++) {
			if (flag) break;
			char c = i + '0';
			if (c == p[1]) continue;
			if (nums[i] >= 2) {
				string t = tmp;
				auto it2 = t.find_first_of(c);
				t.replace(it2, 2, "");
				if (is_multi_mian_zi(tmp, hasPair - 1))
					flag = true;
			}
		}
		if (flag)
			res |= 0b10;
	}
	// check 单骑
	if (hasPair && nums[p[1] - '0'] >= 2) {
		string tmp = s;
		auto it = tmp.find_first_of(p);
		tmp.replace(it, 2, "");
		if (is_multi_mian_zi(tmp, hasPair - 1))
			res |= 0b100;
	}
	if (p[0] == 'Z')
		return res;
	// check 嵌张
	if (p[1] != '1' && p[1] != '9' && nums[p[1] - '1'] > 0 && nums[p[1] + 1 - '0'] > 0) {
		string tmp = s;
		for (int k = 0; k < 3; k++) {
			char c = p[1] - 1 + k;
			auto it = tmp.find_first_of(c);
			tmp.replace(it, 1, "");
		}
		if (is_multi_mian_zi(tmp, hasPair))
			res |= 0b1000;
	}
	// check 边张
	if (p[1] == '3' || p[1] == '7') {
		int i = p[1] == '3' ? -1 : 1;
		if (nums[p[1] - '0' + i] > 0 && nums[p[1] - '0' + i * 2] > 0) {
			char c = p[1], c2;
			string tmp = s;
			for (int k = 0; k < 3; k++) {
				c2 = c + i * k;
				auto it = tmp.find_first_of(c2);
				tmp.replace(it, 1, "");
			}
			if (is_multi_mian_zi(tmp, hasPair))
				res |= 0b10000;
		}
	}
	// 两面 0b1
	bool flag = false;
	if (p[1] >= '1' && p[1] <= '6' && nums[p[1] - '0' + 1] > 0 && nums[p[1] - '0' + 2] > 0) {
		string tmp = s;
		for (int k = 0; k < 3; k++) {
			auto it = tmp.find_first_of(p[1] + k);
			tmp.replace(it, 1, "");
		}
		if (is_multi_mian_zi(tmp, hasPair))
			flag = true;
	}
	if (p[1] >= '4' && p[1] <= '9' && nums[p[1] - '0' - 1] > 0 && nums[p[1] - '0' - 2] > 0) {
		string tmp = s;
		for (int k = 0; k < 3; k++) {
			auto it = tmp.find_first_of(p[1] - k);
			tmp.replace(it, 1, "");
		}
		if (is_multi_mian_zi(tmp, hasPair))
			flag = true;
	}
	if (flag)
		res |= 0b1;
	return res;
}
bool is_yao_jiu(string t) {
	int l = t.length();
	if (t[0] == 'Z')
		return true;
	for (int i = 0; i < l / 2; i++)
		if (t[2 * i + 1] == '1' || t[2 * i + 1] == '9')
			return true;
	return false;
}
int get_tile_type(char c) {
	string types = "BTWZ";
	auto idx = types.find(c);
	if (idx == string::npos)
		return -1;
	else
		return ((int)idx);
}
int get_tile_num(char c) {
	return c == '0' ? 5 : c - '0';
}

int get_score(int fan, int fu, bool isYakuman) {
	int bp = 0;
	if (isYakuman) bp = fan / 13 * 8000;
	else if (fan >= 13) bp = 8000;
	else if (fan >= 11) bp = 6000;
	else if (fan >= 8) bp = 4000;
	else if (fan >= 6) bp = 3000;
	else if (fan >= 5) bp = 2000;
	else {
		bp = fu * (1 << (fan + 2));
		if (bp > 2000) bp = 2000;
	}
	return bp;
}
int get_basepoint(int fan, int fu, bool isYakuman) {

	if (isYakuman) {
		if (fan % 13 != 0) return 0;
		return fan / 13 * 32000;
	}
	else {
		if (fan >= 13) return 32000;
		else if (fan >= 11) return 24000;
		else if (fan >= 8) return 16000;
		else if (fan >= 6) return 12000;
		else if (fan >= 5) return 8000;
		else if (fan == 4) {
			if (fu >= 40) return 8000;
			else if (fu == 30) return 7700;
			else if (fu == 25) return 6400;
			else if (fu == 20) return 5200;
		}

		else if (fan == 3) {
			if (fu >= 70) return 8000;
			else if (fu == 60) return 7700;
			else if (fu == 50) return 6400;
			else if (fu == 40) return 5200;
			else if (fu == 30) return 3900;
			else if (fu == 25) return 3200;
			else if (fu == 20) return 2700;
		}
		else if (fan == 2) {
			if (fu >= 110) return 7100;
			else if (fu == 100) return 6400;
			else if (fu == 90) return 5800;
			else if (fu == 80) return 5200;
			else if (fu == 70) return 4500;
			else if (fu == 60) return 3900;
			else if (fu == 50) return 3200;
			else if (fu == 40) return 2600;
			else if (fu == 30) return 2000;
			else if (fu == 25) return 1600;
			else if (fu == 20) return 1500;
		}
		else if (fan == 1) {
			if (fu >= 110) return 3600;
			else if (fu == 100) return 3200;
			else if (fu == 90) return 2900;
			else if (fu == 80) return 2600;
			else if (fu == 70) return 2300;
			else if (fu == 60) return 2000;
			else if (fu == 50) return 1600;
			else if (fu == 40) return 1300;
			else if (fu == 30) return 1000;
		}
		return 0;
	}
}
FanFu get_high_fan_fu(vector<FanFu> & vecF) { // 这里写得其实不太好，先这样简单处理了
	int fan = 0, fu = 0, yakuman = 0;
	if (vecF.size() == 0)
		return FanFu({}, 0, 0, 0);

	bool isYakuman = false; //  需要确定役满
	vector<int> namesYakuman, namesYaku;
	for (auto & x : vecF) {
		if (x.names.size() == 0 && x.fan == 0 && x.fu == 0)
			continue;
		if (x.fan >= 13) {
			isYakuman = true;
			yakuman += x.fan;
			namesYakuman.push_back(x.names[0]);
		}
		else {
			fan += x.fan;
			fu = max(fu, x.fu);
			namesYaku.push_back(x.names[0]);
		}
	}

	if (isYakuman)
		return FanFu(namesYakuman, yakuman, 0, 0);

	for (auto & x : vecF)
		if (x.names[0] == PING_HU || x.names[0] == QI_DUI_ZI)
			fu = x.fu; // check平和、七对子的符数

	if (fan >= 13)
		fan = 13;
	return FanFu(namesYaku, fan, fu, 0);
}

vector<string> Player::get_own_tiles(bool reCalculate) {
	// reCalculate=true时计算ownTiles并返回
	if (reCalculate) {
		ownTiles.clear();
		vector<string> res;
		for (auto & x : handTiles) {
			res.push_back(x);
		}
		for (auto & tiles : anGangTiles) {
			for (auto & x : tiles) {
				res.push_back(x);
			}
		}
		for (auto & tiles : mingTiles) {
			for (auto & x : tiles) {
				res.push_back(x);
			}
		}
		ownTiles = res;
	}
	sort(ownTiles.begin(), ownTiles.end());
	return ownTiles;
}
bool Player::delete_tile(const string c)
{
	auto it = find(handTiles.begin(), handTiles.end(), c);
	if (it == handTiles.end())
		return false;
	it = handTiles.erase(it);
	return true;
}
void Player::initalize(int p, const vector<string> & hc) {
	pos = p;
	isLiZhi = false;
	numLiZhi = 25;
	memset(isZhenTing, 0, sizeof(isZhenTing));
	handTiles = hc;
}
// person put out c
int Player::check_hu_prerequisite(const string c, int offer) {
	// 分别是B饼 T条 W万 Z字（F风 J箭）
	sort(this->handTiles.begin(), this->handTiles.end());
	vector<string> hc;
	hc = this->handTiles;
	hc.push_back(c);
	vector<vector<int> > nums;
	nums = string_to_34_tiles(hc, false);

	// TODO: 这句话在写吃碰杠逻辑的时候需要注意减掉，比较影响性能。在吃碰杠的时候需要注意更改ownTiles的逻辑
	this->get_own_tiles(true);

	vector<Tiles> tmp;
	if (anGangTiles.size() == 0 && mingTiles.size() == 0) {
		// check GUO_SHI_WU_SHUANG and QI_DUI_ZI ---- return 2
		for (auto & x : specialYiZhong) {
			FanFu checkRes = x.checker(tmp, *this, c, offer);
			if (checkRes.fan > 0) {
				return x.id + 100; // 表示手牌可以以特殊牌型通过胡牌检测。但是七对子还是两杯口要在之后继续判断。
			}

		}
	}

	// check usual Hu
	int num_of_pairs = 0, N;
	if (nums[3][10] % 3 == 1)
		return 0;

	if (nums[3][10] != 0) {
		for (int idx = 1; idx <= 7; idx++) {
			if (nums[3][idx] == 0)
				continue;
			else if (nums[3][idx] % 3 == 1)
				return 0;
			else if (nums[3][idx] == 2)
				num_of_pairs++;
		}
	}

	for (int type_of_tiles = 0; type_of_tiles < 3; type_of_tiles++) {
		if (nums[type_of_tiles][10] % 3 == 1)
			return 0;
		if (nums[type_of_tiles][10] == 0)
			continue;

		bool f[12][8][8][2] = { false };
		for (N = 9; N > 0; N--)
			if (nums[type_of_tiles][N] != 0)
				break;

		f[0][0][0][0] = true;
		int i, j, k, u;
		for (i = 1; i <= N; i++)
			for (j = 0; j <= nums[type_of_tiles][i - 1]; j++)
				for (k = 0; k <= nums[type_of_tiles][i]; k++) {
					if (k >= 2)	f[i][j][k][1] |= f[i][j][k - 2][0];
					if (k >= 3)	f[i][j][k][0] |= f[i][j][k - 3][0];
					if (k >= 3)	f[i][j][k][1] |= f[i][j][k - 3][1];
					if (j >= k && (i == 1 || nums[type_of_tiles][i - 2] >= k))
						f[i][j][k][0] |= f[i - 1][(i >= 2 ? nums[type_of_tiles][i - 2] - k : 0)][j - k][0];
					if (j >= k && (i == 1 || nums[type_of_tiles][i - 2] >= k))
						f[i][j][k][1] |= f[i - 1][(i >= 2 ? nums[type_of_tiles][i - 2] - k : 0)][j - k][1];
				}
		j = nums[type_of_tiles][N - 1], k = nums[type_of_tiles][N], u = nums[type_of_tiles][10] % 3 == 2;
		num_of_pairs += u;
		if (!f[N][j][k][u])
			return 0;
	}

	if (num_of_pairs != 1)
		return 0;
	return 1;
	// check general Yi ---- return 1
	// nothing ---- return -1
}


struct Game
{
	Player players[4];
	int changFeng;
	string baoPaiZhiShiTiles[2][5]; // [0][0~5]:表示牌5张,[1][0~5]:里表示牌
	int numOfGang; // 表示开杠的次数，不能超过4次
	vector<string> mountainTiles; // 牌山 一开始有74张，包括4张岭上牌，岭上牌是从0~3

	Game() {
		numOfGang = 0;
	}

	void init_mountain_tiles(int random_seed) {
		mountainTiles.clear();
		string t[4] = { "B", "T", "W", "Z" };
		for (int i = 0; i < 3; i++) {
			for (int j = 1; j < 10; j++) {
				string newTile = t[i];
				newTile.push_back('0' + j);
				for (int num = 0; num < 3; num++)
					mountainTiles.push_back(newTile);
				if (j == 5)
					newTile[1] = '0';
				mountainTiles.push_back(newTile);
			}
		}
		for (int j = 1; j < 8; j++) {
			string newTile = t[3];
			newTile.push_back('0' + j);
			for (int num = 0; num < 4; num++)
				mountainTiles.push_back(newTile);
		}
		srand(random_seed); // rand() % 4;
		random_shuffle(mountainTiles.begin(), mountainTiles.end());
	}

	void init_all(bool is_random, vector<string> hc) {
		if (!is_random) {
			mountainTiles.clear();
			mountainTiles = hc;
		}
		else {
			init_mountain_tiles(outputValue["initdata"]["srand"].asUInt());
		}
		quan = 0;
		string tmp;

		for (int j = 0; j < 2; j++) {
			for (int i = 0; i < 5; i++) {
				tmp = mountainTiles.back();
				baoPaiZhiShiTiles[j][i] = tmp;
				mountainTiles.pop_back();
			}
		}

		for (int i = 0; i < 4; i++) {
			vector<string> hc;
			int countT = 13;
			while (countT--) {
				tmp = mountainTiles.back();
				hc.push_back(tmp);
				mountainTiles.pop_back();
			}
			players[i].initalize(i, hc);
		}
	}

	string get_next_tile() {
		string res = *mountainTiles.rbegin();
		mountainTiles.pop_back();
		numRestTiles--;
		return res;
	}
	string get_gang_tile() {
		auto it = mountainTiles.begin();
		string res = *it;
		mountainTiles.erase(it);
		numRestTiles--;
		return res;
	}
	vector<string> get_ting_tiles(int pid) {
		if (pid < 0 || pid >= 4)
			return {};
		vector<string> res;
		string t[4] = { "B", "T", "W", "Z" };
		for (int type_of_tiles = 0; type_of_tiles <= 3; type_of_tiles++) {
			string tmp = t[type_of_tiles];
			tmp.push_back('0');
			for (int idx = 1; idx <= 9; idx++) {
				if (type_of_tiles == 3 && idx >= 8)
					break;
				tmp[1] = idx + '0';
				if (players[pid].check_hu_prerequisite(tmp, pid)) {
					res.push_back(tmp);
				}
			}
		}
		return res;
	}

};

Game g;

struct ExtraInfo {
	bool actionList[4][10]; // 4个人可以进行的8种行为，true表示可以，false表示不可以
	vector<vector<string> > vecstrLiZhiTiles;
	ExtraInfo() : vecstrLiZhiTiles(4, vector<string> {}) {
		for (int i = 0; i < 4; i++)
			for (int j = 0; j < 10; j++)
				actionList[i][j] = false;
	}
	// CHI = 0, PENG = 1, GANG = 2, BUGANG = 3, ANGANG = 4, LIZHI = 5, RONG = 6, TSUMO = 7
	void check(Game & g, string c, int offer, bool isDraw) { // 这个时候要确认一下是打出的牌(!isDraw)还是摸到的牌(isDraw)
															 // c不包含在任何一个player的手牌里
		int type_of_tile = get_tile_type(c[0]);
		int num_of_tile = get_tile_num(c[1]);

		vector<vector<vector<int>>> numsHandTiles;
		for (int i = 0; i < 4; i++) {
			auto & p = g.players[i];
			vector<string> hc = p.handTiles;
			vector<vector<int> > nums = string_to_34_tiles(hc, false);
			numsHandTiles.push_back(nums);
		}

		// 当只剩最后一张牌的时候，只能荣和、自摸 RONG = 6, TSUMO = 7
		// 当只剩不到四张的时候，不能立直 LIZHI = 5,
		if (!isDraw) { // 判断荣和
			for (int i = 0; i < 4; i++) {
				if (i == offer) continue;
				auto & p = g.players[i];
				if (p.isZhenTing[0] || p.isZhenTing[1] || p.isZhenTing[2]) continue;

				if (p.check_hu_prerequisite(c, offer)) {
					auto vecRes = check_HU(i, c, offer, false);
					if (!vecRes.empty()) actionList[i][RONG] = true;
				}
			}
		}
		else { // 判断自摸、立直
			auto & p = g.players[offer];
			if (p.check_hu_prerequisite(c, offer)) {
				auto vecRes = check_HU(offer, c, offer, false);
				if (!vecRes.empty()) actionList[offer][TSUMO] = true;
			}

			if (p.mingTiles.size() == 0 && !p.isLiZhi && (g.mountainTiles.size() >= 8)) { // 立直要求门清
				vector<string> vecTiles = p.handTiles;
				vecTiles.push_back(c);
				p.handTiles.push_back(c);
				for (auto & tile : vecTiles) {
					auto it = p.handTiles.begin();
					for (; it != p.handTiles.end(); it++)
						if ((*it) == tile) {
							it = p.handTiles.erase(it);
							break;
						}
					auto tingTiles = g.get_ting_tiles(offer); // 判断是否能立直，同时要计算出听什么
					if (tingTiles.size()) vecstrLiZhiTiles[offer].push_back(tile);
					p.handTiles.push_back(tile);
				}
				if (vecstrLiZhiTiles[offer].size()) actionList[offer][LIZHI] = true;
				p.delete_tile(c);
			}
		}

		if (g.mountainTiles.size() == 4)
			return;

		int rightPlayer = (offer + 1) % 4;
		if (!isDraw) {
			if (!lastANGANG && !lastBUGANG) {
				// 判断下家是否能吃
				if ((type_of_tile < 3) && (!g.players[rightPlayer].isLiZhi)) { // 立直状态不能吃
					bool flag = false;
					for (int l = num_of_tile - 2; l <= num_of_tile; l++) {
						if (l < 1 || l > 7) continue;
						flag = true;
						for (int i = l; i < l + 3; i++) {
							if (i == num_of_tile) continue;
							if (numsHandTiles[rightPlayer][type_of_tile][i] <= 0) {
								flag = false; break;
							}
						}
						if (flag) break;
					}
					if (flag) actionList[rightPlayer][CHI] = true;
				}

				// 判断四家是否能碰或大明杠
				for (int i = 0; i < 4; i++) {
					if (i == offer) continue;
					if (!g.players[i].isLiZhi && (numsHandTiles[i][type_of_tile][num_of_tile] >= 2))
						actionList[i][PENG] = true;
					if (!g.players[i].isLiZhi && (numsHandTiles[i][type_of_tile][num_of_tile] == 3)
						&& g.numOfGang < 4)
						actionList[i][GANG] = true;
				}
			}
		}
		else if (g.numOfGang < 4) {
			// BUGANG = 3, ANGANG = 4, 都是自己的牌
			// g.players[offer].handTiles.push_back(c);
			numsHandTiles[offer][type_of_tile][num_of_tile]++;
			auto & p = g.players[offer];
			for (auto & tiles : p.mingTiles) {
				if (is_same_tile(tiles[0], tiles[1])) {
					int tmp_type_of_tile = get_tile_type(tiles[0][0]);
					int tmp_num_of_tile = get_tile_num(tiles[0][1]);
					if (numsHandTiles[offer][tmp_type_of_tile][tmp_num_of_tile] == 1) {
						actionList[offer][BUGANG] = true;
						break;
					}
				}
			}

			if (p.isLiZhi) {
				if (numsHandTiles[offer][type_of_tile][num_of_tile] == 4) {
					auto ting_bef = g.get_ting_tiles(offer);
					auto oldplayer = g.players[offer];

					auto it = p.handTiles.begin();
					int n = 0;

					vector<string> tiles;
					for (; it != p.handTiles.end();) {
						if (is_same_tile(*it, c)) {
							tiles.push_back(*it);
							it = p.handTiles.erase(it);
							n++;
						}
						else
							++it;
					}

					auto ting_aft = g.get_ting_tiles(offer);
					bool flag = false;
					if (ting_aft.size() != ting_bef.size()) flag = true;
					else {
						for (int i = 0; i < ting_bef.size(); i++) {
							if (!is_same_tile(ting_bef[i], ting_aft[i]))
								flag = true;
						}
					}
					for (auto & t : ting_bef) {
						if (flag) break;
						oldplayer.handTiles.push_back(t);
						vector<string> hc = oldplayer.handTiles;
						vector<vector<int> > nums = string_to_34_tiles(hc, false);
						vector<vector<Tiles>> allCombBef = get_all_combinations(nums);
						for (auto tiles : allCombBef) { // 如果有任何一种胡牌方式里
							if (flag) break;
							for (auto & tile : tiles) { // 待暗杠的牌组成了面子，就不能暗杠
								if (flag) break;
								if (tile.type == "SHUNZI") {
									for (auto & x : tile.part) {
										if (is_same_tile(x, c)) {
											flag = true;
											break;
										}
									}
								}
							}
						}
						oldplayer.delete_tile(t);
					}

					p.handTiles.insert(p.handTiles.end(), tiles.begin(), tiles.end());
					if (!flag) actionList[offer][ANGANG] = true;
				}
			}
			else {
				for (int x = 0; x < 4; x++) {
					if (actionList[offer][ANGANG]) break;
					for (int y = 1; y <= 9; y++) {
						if (numsHandTiles[offer][x][y] == 4) {
							actionList[offer][ANGANG] = true;
							break;
						}
					}
				}
			}
		}
	}
};

void show_dora_indicators(Json::Value & v) {
	string doraIndicators = "";
	for (int i = 0; i <= g.numOfGang; i++) {
		doraIndicators += g.baoPaiZhiShiTiles[0][i];
		if (i == g.numOfGang)
			break;
		doraIndicators += " ";
	}
	v["doraIndicators"] = doraIndicators;
	outputValue["display"]["doraIndicators"] = doraIndicators;
}

void show_hidden_dora_indicators(Json::Value & v) {
	string doraIndicators = "";
	for (int i = 0; i <= g.numOfGang; i++) {
		doraIndicators += g.baoPaiZhiShiTiles[1][i];
		if (i == g.numOfGang)
			break;
		doraIndicators += " ";
	}
	v["hiddenDoraIndicators"] = doraIndicators;
	outputValue["display"]["hiddenDoraIndicators"] = doraIndicators;
}

// playerError不用改可以直接用
void playerError(int player, const string code)
{
	outputValue["display"]["action"] = code;
	outputValue["display"]["player"] = player;
	outputValue["command"] = "finish";
	int s[4] = { 0 };
	for (int i = 0; i < 4; i++) {
		if (i == player) {
			s[i] -= 32000 * 3;
		}
		else {
			s[i] += 32000;
		}
		if (g.players[i].isLiZhi) {
			s[i] -= 1000;
		}
	}


	for (int i = 0; i < 4; i++) {
		outputValue["display"]["score"][i] = s[i];
		outputValue["content"][to_string(i)] = s[i];
	}
	// outputValue["display"]["prompt"] = promptsForDisplay;
	cout << outputValue;
	exit(0);
}

// check_HU 需要给出最后的结果——多少分数、有哪些番、番数多少、符数多少
// check_HU 里不判断是否振听，只在调用之后考虑。check_HU只在满足能胡条件下，是否有役
// 进check_HU函数的player手牌中不含c
vector<FanFu> check_HU(int player, const string c, int offer, bool finish) { // 能进到这个函数的，一定是满足了能胡牌的条件的
	auto & p = g.players[player];

	vector<Tiles> tmp;
	vector<FanFu> vecFanFu, maxScoreFanFu;
	bool isGuoShi = false, isQiDuiZi = false, isYaKuMan = false;

	vector<int> namesFan;
	vector<string> hc = p.handTiles, oc = p.ownTiles;
	hc.push_back(c);
	oc.push_back(c);
	vector<vector<int> > nums = string_to_34_tiles(hc, true), nums2 = string_to_34_tiles(oc, true);


	int numRedFive = 0, numBaoPai = 0, numLiBaoPai = 0; // 计算各种宝牌的数量
	int score = 0, fan = 0, fu = 0, maxScore = 0, maxScoreFan = 0, maxScoreFu = 0;
	auto ownTiles = p.get_own_tiles(true);
	ownTiles.push_back(c);

	for (int _id = 0; _id < 3; _id++) {
		numRedFive += nums2[_id][0];
		nums[_id][5] += nums[_id][0];
		nums[_id][0] = 0;
	}

	vector<vector<Tiles>> allCombinations = get_all_combinations(nums);

	if (p.anGangTiles.empty() && p.mingTiles.empty()) {
		for (auto & yaku : specialYiZhong) {
			FanFu checkRes = yaku.checker(tmp, p, c, offer);
			if (checkRes.fan > 0) {
				if (yaku.id == GUO_SHI_WU_SHUANG || yaku.id == GUO_SHI_WU_SHUANG_SHI_SAN_MIAN) isGuoShi = true;
				else isQiDuiZi = true; // 七对子还是两杯口要在之后继续判断。
				vecFanFu.push_back(checkRes);
			}
		}
	}

	if (isGuoShi)
		isYaKuMan = true;
	else { // check Yakuman
		for (auto & yaku : yakumanYiZhong) {
			FanFu checkRes = yaku.checker(tmp, p, c, offer);
			if (checkRes.fan > 0) {
				vecFanFu.push_back(checkRes);
				isYaKuMan = true;
			}
		}
	}

	// 满足了役满的条件
	if (isYaKuMan) {
		maxScoreFanFu = vecFanFu;
		for (auto & yaku : vecFanFu)
			maxScoreFan += yaku.fan;
		maxScore = get_score(maxScoreFan, 0, true);
	}
	else { // 对于每一种拆牌都check一下常见役种
		string baopai[2] = { "234567891",  "2341675" };
		for (int i = 0; i <= g.numOfGang; i++) {
			string t = g.baoPaiZhiShiTiles[0][i];
			if (t[1] == '0') t[1] = '5';
			t[1] = baopai[t[0] == 'Z'][t[1] - '1'];
			for (auto & x : ownTiles)
				if (is_same_tile(t, x))
					numBaoPai++;
			if (g.players[player].isLiZhi) {
				string t = g.baoPaiZhiShiTiles[1][i];
				if (t[1] == '0') t[1] = '5';
				t[1] = baopai[t[0] == 'Z'][t[1] - '1'];
				for (auto & x : ownTiles)
					if (is_same_tile(t, x))
						numLiBaoPai++;
			}
		}
		// 计算符数
		int baseFu = 20;

		int tingType = get_ting_type(hc, c, 1);
		if (tingType & (QIAN_ZHANG | DAN_JI | BIAN_ZHANG)) // 胡牌时嵌张、单骑、边张时加2符
			baseFu += 2;
		baseFu += 2 * (player == offer); // 自摸时加2符
		baseFu += (player != offer) * (p.mingTiles.empty()) * 10;// 门前清荣和加10符
																 // 役牌雀头
		baseFu += 2 * (nums[3][g.players[player].pos] == 2);
		baseFu += 2 * (nums[3][g.players[player].changFeng] == 2);
		baseFu += 2 * (nums[3][5] == 2);
		baseFu += 2 * (nums[3][6] == 2);
		baseFu += 2 * (nums[3][7] == 2);

		// 刻杠类
		// 计算暗杠
		for (auto & x : g.players[player].anGangTiles) {
			if (is_yao_jiu(x[0])) baseFu += 32;
			else baseFu += 16;
		}
		// 计算明刻明杠
		for (auto & x : g.players[player].mingTiles) {
			if (x.size() == 4) {
				if (is_yao_jiu(x[0])) baseFu += 16;
				else baseFu += 8;
			}
			else if (is_same_tile(x[0], x[1])) {
				if (is_yao_jiu(x[0])) baseFu += 4;
				else baseFu += 2;
			}
		}

		vector<FanFu> tmpFanFu = vecFanFu;
		bool hasLiZhi = false;
		fan = 0, fu = 0; //  numBaoPai + numRedFive + hasLiZhi * numLiBaoPai,

		if (allCombinations.empty()) { // 只有可能是七对子了
			vector<Tiles> blank;
			for (auto & yaku : usualYiZhong) {
				FanFu checkRes = yaku.checker(blank, p, c, offer);
				if (checkRes.fan > 0) {
					tmpFanFu.push_back(checkRes);
					if (yaku.id == LI_ZHI)
						hasLiZhi = true;
				}
			}
			if (numBaoPai) tmpFanFu.push_back(FanFu({ BAO_PAI }, numBaoPai, 0, 0));
			if (numRedFive) tmpFanFu.push_back(FanFu({ HONG_BAO_PAI }, numRedFive, 0, 0));
			if (hasLiZhi && numLiBaoPai) tmpFanFu.push_back(FanFu({ LI_BAO_PAI }, numLiBaoPai, 0, 0));
			for (auto & x : tmpFanFu)
				fan += x.fan;
			fu = 25;
			score = get_score(fan, fu, false);
			maxScoreFanFu = tmpFanFu;
			maxScore = score;
			maxScoreFan = fan;
			maxScoreFu = fu;
		}
		for (auto & tiles : allCombinations) {
			int hasPingHu = 0, tmpFu = 0;
			for (auto & yaku : usualYiZhong) {
				FanFu checkRes = yaku.checker(tiles, p, c, offer);
				if (checkRes.fan > 0) {
					if (yaku.id == PING_HU) {
						if (p.mingTiles.empty() && p.anGangTiles.empty()) {
							tmpFanFu.push_back(checkRes);
							hasPingHu = 1;
							tmpFu = checkRes.fu;
						}
						else { // 平和型副露
							hasPingHu = 2;
							tmpFu = 30;
						}
					}
					else
						tmpFanFu.push_back(checkRes);
					if (yaku.id == LI_ZHI)
						hasLiZhi = true;
					if (yaku.id == ER_BEI_KOU) { // 二杯口一定有七对，但是只能存一
						auto it = tmpFanFu.begin();
						auto v = it->names;
						if (find(v.begin(), v.end(), QI_DUI_ZI) != v.end())
							tmpFanFu.erase(it);
					}
				}
			}

			if (!tmpFanFu.empty()) {
				if (numBaoPai) tmpFanFu.push_back(FanFu({ BAO_PAI }, numBaoPai, 0, 0));
				if (numRedFive) tmpFanFu.push_back(FanFu({ HONG_BAO_PAI }, numRedFive, 0, 0));
				if (hasLiZhi && numLiBaoPai) tmpFanFu.push_back(FanFu({ LI_BAO_PAI }, numLiBaoPai, 0, 0));

				for (auto & x : tmpFanFu) {
					fan += x.fan;
					fu = max(fu, x.fu);
				}
				fu += baseFu;
				if (hasPingHu) fu = tmpFu; // 平胡
				fu = fu + (10 - (fu % 10)) % 10; // 等价于int(ceil(1.0 * fu / 10) * 10); 

				score = get_score(fan, fu, false);

				if (score > maxScore) {
					maxScoreFanFu = tmpFanFu;
					maxScore = score;
					maxScoreFan = fan;
					maxScoreFu = fu;
				}
			}

		}
		// allScores.push_back(1);

		if (maxScoreFan > 13)
			maxScoreFan = 13;
	}

	// 处理一下一炮多响的情况
	if (finish) {
		if (maxScoreFanFu.empty()) {
			playerError(player, "WA");
			return {};
		}
		outputValue["display"][to_string(player)]["isYaKuMan"] = isYaKuMan;
		outputValue["display"][to_string(player)]["action"] = "HU";
		outputValue["display"]["action"] = "HU";
		outputValue["display"][to_string(player)]["player"] = player;
		outputValue["display"][to_string(player)]["fanCnt"] = maxScoreFan;
		if (!isYaKuMan)
			outputValue["display"][to_string(player)]["fuCnt"] = maxScoreFu;
		outputValue["display"][to_string(player)]["ScoreCnt"] = maxScore;

		for (auto & x : maxScoreFanFu) {
			Json::Value cFan;
			cFan["name"] = FanNames[x.names[0]];
			cFan["value"] = x.fan;
			outputValue["display"][to_string(player)]["fan"].append(cFan);
		}
		outputValue["command"] = "finish";
	}
	if (!maxScoreFanFu.empty())
		maxScoreFanFu[0].score = maxScore;
	return maxScoreFanFu;
	// outputValue["display"][to_string(i)]
}

// checkInputPASS不用改可以直接用
void checkInputPASS(const Json::Value &playerOutput, int player, bool isPLAY)
{
	if (playerOutput["verdict"] != "OK") {
		playerError(player, playerOutput["verdict"].asString());
	}
	if (playerOutput["response"] != "PASS") {
		playerError(player, "WA");
	}
	if (isPLAY && lastTile.size() == 2 && roundStage >= 4) {
		// 需要多判断一下振听情况
		auto & p = g.players[player];
		if (p.check_hu_prerequisite(lastTile, roundStage % 4)) {
			if (player == roundStage % 4)
				p.isZhenTing[0] = true; // 舍张振听
			else
				p.isZhenTing[1] = true; // 同巡振听
			p.isZhenTing[2] = p.isLiZhi; // 立直振听
		}
	}
}

// 在摸牌时的操作，后续可能还要改
void checkInputDRAW(Json::Value &playerOutput, int player)
{
	if (playerOutput["verdict"] != "OK") {
		playerError(player, playerOutput["verdict"].asString());
	}
	vector<string> outputList;
	string outputString = playerOutput["response"].asString();
	auto & p = g.players[player];
	p.isZhenTing[1] = false; // 同巡振听解除
	boost::split(outputList, outputString, boost::is_any_of(" "));
	if (outputList.size() == 1) {
		if (outputList[0] == "HU") {
			if (!p.check_hu_prerequisite(lastTile, player))
				playerError(player, "WA");
			// 自己摸牌的时候不会点别人
			// 也不用考虑振听情况，只需要有役即可
			auto vecRes = check_HU(player, lastTile, player, true);
			if (vecRes.empty()) playerError(player, "WA");
			int basepoint = outputValue["display"][to_string(player)]["ScoreCnt"].asInt(), maxScore;
			// if (p.pos == p.changFeng) maxScore = ceil(1.5 * maxScore);
			int score[4] = { 0 };
			int lizhiScore = 0;
			for (int i = 0; i < 4; i++) {
				if (g.players[i].isLiZhi) {
					lizhiScore += 1000;
					score[i] -= 1000;
				}
			}

			if (player == g.changFeng) {
				// 庄家自摸胡牌，三人各自承担2a
				maxScore = ceil(basepoint * 2 / 100.0) * 300;
				score[player] += maxScore + lizhiScore;
				for (int i = 0; i < 4; i++) {
					if (i != player) {
						score[i] -= maxScore / 3;
					}
				}
			}
			else {
				// 闲家自摸胡牌，庄家要承担一半的分数，其余两家要承担四分之一的分数。需要是100的整数倍
				score[g.changFeng] -= ceil(basepoint * 2 / 100.0) * 100;
				for (int i = 0; i < 4; i++) {
					if (i != g.changFeng && i != player) {
						score[i] -= ceil(basepoint / 100.0) * 100;
					}
				}
				maxScore = ceil(basepoint * 2 / 100.0) * 100 + ceil(basepoint / 100.0) * 200;
				score[player] += maxScore + lizhiScore;
			}

			outputValue["display"][to_string(player)]["ScoreCnt"] = maxScore;

			for (int i = 0; i < 4; i++) {
				outputValue["display"]["score"][i] = 25000 + score[i];
				outputValue["content"][to_string(i)] = 25000 + score[i];
			}

			if (g.players[player].isLiZhi) {
				Json::Value info;
				show_hidden_dora_indicators(info);
				outputValue["display"]["hiddenDoraIndicators"] = info["hiddenDoraIndicators"];
			}

			cout << outputValue;
			exit(0);
		}
	}
	else if (outputList.size() == 2) {
		auto ting_bef = g.get_ting_tiles(player);
		auto oldplayer = g.players[player];
		if (outputList[0] == "PLAY" || outputList[0] == "LIZHI")
			isPlayDrawnTileOnly = (find(p.handTiles.begin(), p.handTiles.end(), outputList[1]) == p.handTiles.end());
		p.handTiles.push_back(lastTile);
		string befTile = lastTile; // befTile 是之前抽到的牌
		lastTile = outputList[1]; // lastTile 是这次打出去的牌，要么是暗杠的牌，要么是选择进行补杠的牌
		if (outputList[0] == "PLAY") {
			if (lastBUGANG || currGANG)
				g.numOfGang += lastBUGANG + currGANG;
			if (p.isLiZhi) { // 立直了就不管看他到底是想出什么牌，随便。但是摸到就打
				lastTile = befTile;
				p.isYiFa = false;
			}
			auto curr = find(p.handTiles.begin(), p.handTiles.end(), lastTile);
			if (curr != p.handTiles.end()) {
				p.handTiles.erase(curr);
				lastOp = "PLAY";
				roundStage += 4;
				p.outTiles.push_back(lastTile);
				return;
			}
		}
		else if (outputList[0] == "ANGANG") {
			// 做暗杠需要告知是什么牌，因为有可能来一张能暗杠的牌，但是先留着之后再暗杠。
			tileGANG = lastTile;
			auto it = p.handTiles.begin();
			int n = 0;

			vector<string> tiles;
			for (; it != p.handTiles.end();) {
				if (is_same_tile(*it, lastTile)) {
					tiles.push_back(*it);
					it = p.handTiles.erase(it);
					n++;
				}
				else
					++it;
			}
			if (n != 4 || g.mountainTiles.size() <= 4 || g.numOfGang >= 4)
				playerError(player, "WA");
			if (p.isLiZhi) { // 立直后，只有不影响听牌的杠才能成立
							 // 只有不改变面子手的暗杠能成立
							 // 比如66678999s 22w 567p，来6s或9s均不能暗杠，因为杠之后再胡2w就变成

				if (!is_same_tile(befTile, lastTile)) // 立直后，只能对摸牌进行暗杠
					playerError(player, "WA");
				auto ting_aft = g.get_ting_tiles(player);
				bool flag = false;
				if (ting_aft.size() != ting_bef.size())
					flag = true;
				else {
					for (int i = 0; i < ting_bef.size(); i++) {
						if (!is_same_tile(ting_bef[i], ting_aft[i]))
							flag = true;
					}
				}
				for (auto & t : ting_bef) {
					if (flag) break;
					oldplayer.handTiles.push_back(t);
					vector<string> hc = oldplayer.handTiles;
					vector<vector<int> > nums = string_to_34_tiles(hc, false);
					vector<vector<Tiles>> allCombBef = get_all_combinations(nums);
					for (auto tiles : allCombBef) { // 如果有任何一种胡牌方式里
						if (flag) break;
						for (auto & tile : tiles) { // 待暗杠的牌组成了面子，就不能暗杠
							if (flag) break;
							if (tile.type == "SHUNZI") {
								for (auto & x : tile.part) {
									if (is_same_tile(x, lastTile)) {
										flag = true;
										break;
									}
								}
							}
						}
					}
					oldplayer.delete_tile(t);
				}

				if (flag) { // 杠会影响结果，所以不能杠，要变ANGANG为PLAY
					playerOutput["response"] = "PLAY " + lastTile;
					lastOp = "PLAY";
					roundStage = player + 4;
					p.outTiles.push_back(lastTile);
					p.isYiFa = false;
					p.handTiles.insert(p.handTiles.end(), tiles.begin(), tiles.end());
					return;
				}
				p.isYiFa = false;
			}
			isMingPai = true;
			currANGANG = true;
			g.numOfGang++;
			p.anGangTiles.push_back(tiles);
			roundStage = player + 8;
			return;
		}
		else if (outputList[0] == "BUGANG") { // 确认要补杠什么   && g.mountainTiles.size() > 4 && g.numOfGang < 4
			currBUGANG = true;
			// g.numOfGang++;
			tileGANG = lastTile;

			auto it = p.handTiles.begin();
			for (; it != p.handTiles.end(); it++) {
				if (is_same_tile(*it, lastTile))
					break;
			}
			if (it == p.handTiles.end() || g.mountainTiles.size() <= 4 || g.numOfGang >= 4)
				playerError(player, "WA");
			for (unsigned int i = 0; i < p.mingTiles.size(); i++) {
				auto &x = p.mingTiles[i];
				if (x.size() == 3 && is_same_tile(x[0], x[1]) && is_same_tile(x[1], lastTile)) {
					roundStage = player + 8;
					p.mingTiles[i].push_back(lastTile);
					p.handTiles.erase(it);
					return;
				}
			}
		}
		// 立直
		else if (outputList[0] == "LIZHI") {
			if (p.mingTiles.size() || (g.mountainTiles.size() < 8)) {
				playerError(player, "WA");
			}

			auto curr = find(p.handTiles.begin(), p.handTiles.end(), lastTile);
			if (curr != p.handTiles.end()) {
				p.handTiles.erase(curr);

				lastOp = "LIZHI";
				roundStage = player + 12;
				p.outTiles.push_back(lastTile);
				auto tingTiles = g.get_ting_tiles(player); // 判断是否能立直，同时要计算出听什么
				if (tingTiles.empty())
					playerError(player, "WA");
				p.isZhenTing[1] = false; // 因为有摸切，同巡振听解除
				bool flag = true; // 用来标记是否能够解除舍张振听
				for (auto & x : tingTiles)
					for (auto & y : p.outTiles)
						if (is_same_tile(x, y)) {
							p.isZhenTing[0] = p.isZhenTing[2] = true;
							flag = false;
							break;
						}
				if (flag)
					p.isZhenTing[0] = false;
				p.numLiZhi = p.outTiles.size();
				return;
			}
		}

	}
	playerError(player, "WA");
}


//检查其他玩家打出牌后，玩家的输出。所以没有暗杠、补杠请求
//1检查和牌操作
void checkInputPLAY1(Json::Value &playerOutput, int player)
{
	if (playerOutput["verdict"] != "OK") {
		playerError(player, playerOutput["verdict"].asString());
	}
	string outputString = playerOutput["response"].asString();

	vector<string> outputList;
	boost::split(outputList, outputString, boost::is_any_of(" "));

	auto & p = g.players[player];

	if (outputList.size() == 1) {
		if (outputString == "HU") {
			// 这里需要处理的是check_HU里面要不要有处理振听情况。要理清楚HU与振听的关系
			if (!p.check_hu_prerequisite(lastTile, roundStage % 4))
				playerError(player, "WA");
			auto vecRes = check_HU(player, lastTile, roundStage % 4, true);
			if (vecRes.empty()) playerError(player, "WA");
			// 在所有check_HU的地方都要check是否有振听的情况
			bool isSheZhangZhenTing = false; // 先更新一下舍张振听的情况
			for (auto & x : p.outTiles)
				if (p.check_hu_prerequisite(x, player))
					isSheZhangZhenTing = true;
			if (isSheZhangZhenTing) // 舍张振听只有自摸且有役才能解
				p.isZhenTing[0] = true;
			else
				p.isZhenTing[0] = false;
			if (p.isZhenTing[0] || p.isZhenTing[1] || p.isZhenTing[2]) {
				// 唯一处理过的地方
				outputValue["command"] = "request";
				playerOutput["response"] = "PASS";
			}
		}
		// 处理振听
		else if (outputString == "PASS") {
			if (p.check_hu_prerequisite(lastTile, roundStage % 4)) {
				if (p.isLiZhi) p.isZhenTing[2] = true; // 立直振听
				else p.isZhenTing[1] = true; // 同巡振听
			}
		}
		else if (outputString == "GANG") { }
		else
			playerError(player, "WA");
	}
}
//2检查碰牌、杠牌
bool checkInputPLAY2(const Json::Value &playerOutput, int player)
{
	vector<string> outputList;
	string outputString = playerOutput["response"].asString();
	auto & p = g.players[player];
	boost::split(outputList, outputString, boost::is_any_of(" "));
	if (outputList.size() == 1) {
		if (outputList[0] == "PASS") {
			return false;
		}
		else if (outputList[0] == "GANG" && g.mountainTiles.size() > 4 && g.numOfGang < 4) {
			isMingPai = true;
			auto it = p.handTiles.begin();
			int n = 0;
			vector<string> tiles{ lastTile };
			for (; it != p.handTiles.end();) {
				if (is_same_tile(*it, lastTile)) {
					tiles.push_back(*it);
					it = p.handTiles.erase(it);
					n++;
				}
				else
					++it;
			}
			if (n == 3) {
				p.mingTiles.push_back(tiles);
				p.mingTilesOffer.push_back(make_pair(roundStage % 4, lastTile));
				lastOp = "GANG";
				roundStage = player + 8;
				tileGANG = lastTile;
				for (int i = 0; i < 4; i++)
					if (g.players[i].isLiZhi)
						g.players[i].isYiFa = false;
				currGANG = true;
				return true;
			}
		}
		playerError(player, "WA");
	}
	else if (outputList.size() == 5) {
		if (outputList[0] == "PENG" && g.mountainTiles.size() > 4) {
			isMingPai = true;
			for (int i = 0; i < 4; i++)
				if (g.players[i].isLiZhi)
					g.players[i].isYiFa = false;
			// outputList里，第二个和第三个是要碰掉的牌，第四个string是打出的牌
			vector<string> tiles{ lastTile };
			if (lastTile != outputList[1] || !is_same_tile(lastTile, outputList[2])
				|| !is_same_tile(lastTile, outputList[3]) || is_same_tile(lastTile, outputList[4]))
				playerError(player, "WA");// 不能碰掉一样的牌，然后又打出去一张
			auto it = find(p.handTiles.begin(), p.handTiles.end(), outputList[2]);
			if (it == p.handTiles.end())
				playerError(player, "WA");
			p.handTiles.erase(it);
			tiles.push_back(outputList[2]);
			it = find(p.handTiles.begin(), p.handTiles.end(), outputList[3]);
			if (it == p.handTiles.end())
				playerError(player, "WA");
			p.handTiles.erase(it);
			tiles.push_back(outputList[3]);

			p.mingTiles.push_back(tiles);
			p.mingTilesOffer.push_back(make_pair(roundStage % 4, lastTile));
			lastOp = "PENG";
			roundStage = player + 8;

			tilePENG = outputList[1] + " " + outputList[2] + " " + outputList[3];

			lastTile = outputList[4];
			it = find(p.handTiles.begin(), p.handTiles.end(), lastTile);
			if (it == p.handTiles.end())
				playerError(player, "WA");
			p.outTiles.push_back(lastTile);
			p.handTiles.erase(it);
			roundStage = 4 + player;
			// 因为有打出去的操作，所以要判断一下振听
			p.isZhenTing[1] = p.isZhenTing[2] = false; // 打出去就可以摆脱同巡振听，且一定不是立直
			bool flag = true;
			auto tingTiles = g.get_ting_tiles(player);
			if (!tingTiles.empty()) {
				for (auto & x : tingTiles) {
					for (auto & y : p.outTiles) {
						if (is_same_tile(x, y)) {
							p.isZhenTing[0] = true;
							flag = false;
							break;
						}
					}
				}
			}
			if (flag)
				p.isZhenTing[0] = false;

			return true;
		}

		if (outputList[0] != "CHI")
			playerError(player, "WA");
	}
	return false;
}
//3检查吃牌
bool checkInputPLAY3(const Json::Value &playerOutput, int player)
{
	vector<string> outputList;
	string outputString = playerOutput["response"].asString();
	auto & p = g.players[player];
	boost::split(outputList, outputString, boost::is_any_of(" "));
	if (outputList.size() == 5) {
		// 前3个分别为吃牌组成，其中第一张牌是从上家那里吃到的（其实就是lastTile），第4个为打出的牌
		if (outputList[0] != "CHI" || (roundStage - player) % 4 != 3 || g.mountainTiles.size() <= 4) {
			playerError(player, "WA");
		}
		isMingPai = true;
		for (int i = 0; i < 4; i++)
			if (g.players[i].isLiZhi)
				g.players[i].isYiFa = false;

		if ((lastTile[0] != 'W' && lastTile[0] != 'B' && lastTile[0] != 'T') ||
			(outputList[2][0] != lastTile[0]) || (outputList[3][0] != lastTile[0]) ||
			(outputList[3] == lastTile) || (outputList[2] == lastTile) || (outputList[3] == outputList[2]))
			playerError(player, "WA");
		vector<string> tiles{ lastTile, outputList[2], outputList[3] };
		string card1 = outputList[2], card2 = outputList[3];
		auto it = find(p.handTiles.begin(), p.handTiles.end(), card1);
		if (it == p.handTiles.end())
			playerError(player, "WA");
		p.handTiles.erase(it);
		it = find(p.handTiles.begin(), p.handTiles.end(), card2);
		if (it == p.handTiles.end())
			playerError(player, "WA");
		p.handTiles.erase(it);
		bool hasRedFive = false;
		for (int i = 0; i < 3; i++) {
			if (tiles[i][1] == '0') {
				tiles[i][1] = '5';
				hasRedFive = true;
			}
		}
		vector<string> cannotOut{ lastTile };
		sort(tiles.begin(), tiles.end());
		string left = tiles[0], right = tiles[2];
		if (is_same_tile(left, lastTile) && right[1] < '9') {
			right[1]++;
			cannotOut.push_back(right);
			right[1]--;
		}
		if (is_same_tile(right, lastTile) && left[1] > '1') {
			left[1]--;
			cannotOut.push_back(left);
			left[1]++;
		}
		if (tiles[2][1] - tiles[1][1] != 1 || tiles[1][1] - tiles[0][1] != 1)
			playerError(player, "WA");
		if (hasRedFive)
			for (int i = 0; i < 3; i++)
				if (tiles[i][1] == '5')
					tiles[i][1] = '0';
		p.mingTiles.push_back(tiles);
		p.mingTilesOffer.push_back(make_pair(roundStage % 4, lastTile));
		lastOp = "CHI";
		tileCHI = lastTile;
		for (auto & x : tiles) {
			if (is_same_tile(x, lastTile))
				continue;
			tileCHI += " " + x;
		}
		for (auto & x : cannotOut)
			if (is_same_tile(x, outputList[4]))
				playerError(player, "WA"); // 不能吃掉一张，然后再打出相同的一张。或者可替代的。
		lastTile = outputList[4];
		it = find(p.handTiles.begin(), p.handTiles.end(), lastTile);
		if (it == p.handTiles.end()) {
			playerError(player, "WA");
		}
		p.handTiles.erase(it);
		p.outTiles.push_back(outputList[4]);
		roundStage = 4 + player;

		p.isZhenTing[1] = p.isZhenTing[2] = false; // 打出去就可以摆脱同巡振听，且一定不是立直
		bool flag = true;
		auto tingTiles = g.get_ting_tiles(player);
		if (!tingTiles.empty()) {
			for (auto & x : tingTiles) {
				for (auto & y : p.outTiles) {
					if (is_same_tile(x, y)) {
						p.isZhenTing[0] = true;
						flag = false;
						break;
					}
				}
			}

		}
		if (flag)
			p.isZhenTing[0] = false;
		return true;
	}

	return false;
}

//检查玩家杠牌后的回应
//用于抢杠和
void checkInputGANG(const Json::Value &playerOutput, int player)
{
	if (playerOutput["verdict"] != "OK") {
		playerError(player, playerOutput["verdict"].asString());
	}
	if (playerOutput["response"] == "PASS") {
		checkInputPASS(playerOutput, player, true); // check一下是否有判断振听的情况
		return;
	}
	if (lastBUGANG && roundStage % 4 != player && playerOutput["response"] == "HU") {
		if (!g.players[player].check_hu_prerequisite(lastTile, player))
			playerError(player, "WA");
		auto vecRes = check_HU(player, lastTile, roundStage % 4, true);
		if (vecRes.empty()) playerError(player, "WA");
	}
	// 特殊判断国士无双可以抢暗杠
	if (lastANGANG && roundStage % 4 != player && playerOutput["response"] == "HU") {
		if (is_yao_jiu(lastTile)) {
			int res = g.players[player].check_hu_prerequisite(lastTile, player) - 100;
			if (res == GUO_SHI_WU_SHUANG || res == GUO_SHI_WU_SHUANG_SHI_SAN_MIAN) {
				auto vecRes = check_HU(player, lastTile, roundStage % 4, true);
				if (vecRes.empty()) playerError(player, "WA");
			}
		}
	}
}


// roundStage
//-2:通知位置
//-1:发牌
//0-3:玩家摸牌
//4-7:玩家打出牌后，通知所有玩家
//8-12:玩家杠牌，通知所有玩家
//13-16:玩家立直，通知所有玩家
void roundOutput(Json::Value &outputValue)
{
	if (roundStage == -1) {
		outputValue["display"]["action"] = "DEAL";
		for (int i = 0; i < 4; i++) {
			for (auto & x : g.players[i].handTiles)
				outputValue["display"]["hand"][i].append(x);
		}
		for (int i = 0; i < 4; i++) {
			string outputString = "1";
			for (int j = 0; j < 13; j++)
				outputString += " " + g.players[i].handTiles[j];
			// outputValue["content"][to_string(i)] = outputString;
			Json::Value info;
			info["handTiles"] = outputString;
			show_dora_indicators(info);
			outputValue["content"][to_string(i)]["handTiles"] = info["handTiles"];
			outputValue["content"][to_string(i)]["doraIndicators"] = info["doraIndicators"];
			// outputValue["content"][to_string(i)].append(info);
		}
	}
	else if (roundStage >= 0 && roundStage < 4) {
		// 几种流局情况需要考虑一下
		if (g.mountainTiles.size() == 4) { // 牌山没得抓了
			outputValue["display"]["action"] = "HUANG";
			outputValue["command"] = "finish";
			bool liuman[4] = { true };
			int cntLiuman = 0;
			for (int i = 0; i < 4; i++) {
				auto & p = g.players[i];
				for (auto & x : p.mingTilesOffer)
					liuman[x.first] = false;
				for (auto & x : p.outTiles) {
					if (!is_yao_jiu(x)) {
						liuman[i] = false;
						break;
					}
				}
			}

			Json::Value details;
			for (int i = 0; i < 4; i++)
				cntLiuman += liuman[i];
			if (cntLiuman) {
				// 流局满贯的优先级要比听牌罚符要高
				int s[4] = { 0 };
				for (int i = 0; i < 4; i++) {
					if (g.players[i].isLiZhi)
						s[i] -= 1000;
				}


				// 计算得分
				for (int i = 0; i < 4; i++) {
					if (liuman[i]) {
						if (i == g.changFeng) {
							// 庄家的流满，需要闲家各付4000
							s[i] += 12000;
							for (int j = 0; j < 4; j++) {
								if (j == i) continue;
								s[j] -= 4000;
							}
						}
						else {
							// 闲家的流满，需要庄家付4000，闲家付2000
							s[i] += 8000;
							for (int j = 0; j < 4; j++) {
								if (j == i) continue;
								if (j == g.changFeng) s[j] -= 4000;
								else s[j] -= 2000;
							}
						}
					}
				}

				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][i] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
					if (liuman[i])
						details[to_string(i)] = "LIUMAN";
					else
						details[to_string(i)] = "NOLIUMAN";
				}

			}
			else {
				bool isTing[4] = { false };
				int s[4] = { 0 }, cntTing = 0;
				for (int i = 0; i < 4; i++) {
					if (g.players[i].isLiZhi)
						s[i] -= 1000;
				}
				for (int i = 0; i < 4; i++) {
					auto res = g.get_ting_tiles(i);
					if (!res.empty()) {
						isTing[i] = true;
						cntTing++;
					}
				}
				if (cntTing >= 1 && cntTing <= 3) {
					for (int i = 0; i < 4; i++) {
						if (isTing[i])
							s[i] += 3000 / cntTing;
						else
							s[i] -= 3000 / (4 - cntTing);
					}
				}
				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][i] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
					if (isTing[i])
						details[to_string(i)] = "TING";
					else
						details[to_string(i)] = "NOTING";
				}
			}
			outputValue["display"]["details"] = details;
			return;
		}

		// lastANGANG和lastBUGANG都是在摸牌之后进行的操作，currGANG是在杠了别人牌之后的操作，有所不同
		if (lastANGANG || lastBUGANG || currGANG) // 更新杠牌情况，为四杠散了做准备
			lastTile = g.get_gang_tile();
		else
			lastTile = g.get_next_tile();

		// lastANGANG = lastBUGANG = lastGANG = false;

		lastOp = "DRAW";
		outputValue["display"]["action"] = "DRAW";
		outputValue["display"]["player"] = roundStage;
		outputValue["display"]["tile"] = lastTile;

		ExtraInfo exinfo;
		exinfo.check(g, lastTile, roundStage % 4, true);
		string extraStr = "";
		for (int i = 0; i < 8; i++) {
			if (exinfo.actionList[roundStage % 4][i]) {
				extraStr += ActionName[i];
				if (i == LIZHI) {
					for (auto & tile : exinfo.vecstrLiZhiTiles[roundStage % 4]) {
						extraStr += " " + tile;
					}
				}
				extraStr += ",";
			}
		}
		if (extraStr.length() != 0) {
			extraStr.pop_back();
		}
		for (int i = 0; i < 4; i++) {
			Json::Value info;
			show_dora_indicators(info);
			if (roundStage % 4 == i) {
				info["action"] = "2 " + lastTile;
				if (extraStr.length() != 0) {
					promptsForDisplay[to_string(i)]["validact"] = extraStr;
				}
			}
			else {
				info["action"] = "3 " + to_string(roundStage % 4) + " DRAW";
			}
			outputValue["content"][to_string(i)]["state"] = info["action"];
			outputValue["content"][to_string(i)]["doraIndicators"] = info["doraIndicators"];
			// outputValue["content"][to_string(i)].append(info);
		}
	}
	else if (roundStage >= 4 && roundStage < 8) {
		outputValue["display"]["action"] = lastOp;
		outputValue["display"]["player"] = roundStage % 4;
		outputValue["display"]["tile"] = lastTile;

		// string lastOp;
		//"DRAW" "PLAY" "PENG" "GANG" "CHI"

		if (lastOp == "CHI") {
			outputValue["display"]["tileCHI"] = tileCHI;
		}
		else if (lastOp == "PENG") {
			outputValue["display"]["tilePENG"] = tilePENG;
		}
		else if (lastOp == "GANG" || lastOp == "ANGANG" || lastOp == "BUGANG") {}

		for (int i = 0; i < 4; i++) {
			Json::Value info;
			show_dora_indicators(info);

			if (lastOp == "CHI") { // CHI 后面会跟四张牌，前三张是组成顺子的，其中第一张是吃的牌。第四张是自己打出的牌。
				info["action"] = "3 " + to_string(roundStage % 4) + " CHI " + tileCHI + " " + lastTile;
			}
			else if (lastOp == "PENG") { // PENG 后面会跟三张牌，前两张和lastTile组成刻子，第三张是自己打出的牌。
				info["action"] = "3 " + to_string(roundStage % 4) + " PENG " + tilePENG + " " + lastTile;
			}
			else {
				info["action"] = "3 " + to_string(roundStage % 4) + " " + lastOp + " " + lastTile;
			}
			outputValue["content"][to_string(i)].clear();
			outputValue["content"][to_string(i)]["state"] = info["action"];
			outputValue["content"][to_string(i)]["doraIndicators"] = info["doraIndicators"];
			if (lastOp == "PLAY") {
				// 是否摸切
				outputValue["content"][to_string(i)]["playDrawnTile"] = isPlayDrawnTileOnly;
			}
		}

		ExtraInfo exinfo;
		string extraStr[4] = { "", "", "", "" };
		exinfo.check(g, lastTile, roundStage % 4, false);
		for (int playerID = 0; playerID < 4; playerID++) {
			if (playerID == roundStage % 4)
				continue;
			for (int i = 0; i < 8; i++) {
				if (exinfo.actionList[playerID][i]) {
					extraStr[playerID] += ActionName[i] + ",";
				}
			}
			if (extraStr[playerID].length() != 0) {
				extraStr[playerID].pop_back();
			}
		}
		for (int i = 0; i < 4; i++) {
			if (roundStage % 4 != i && extraStr[i].length() != 0) {
				promptsForDisplay[to_string(i)]["validact"] = extraStr[i];
			}
		}

	}
	else if (roundStage >= 8 && roundStage < 12) {
		string cOp = "GANG " + lastTile;
		if (lastANGANG) {
			cOp = "ANGANG " + lastTile;
			outputValue["display"]["action"] = "ANGANG";
		}
		else {
			if (lastBUGANG) {
				cOp = "BUGANG " + lastTile;
				outputValue["display"]["action"] = "BUGANG";
			}
			else
				outputValue["display"]["action"] = "GANG";
		}
		// outputValue["display"]["action"] = lastBUGANG ? "BUGANG" : "GANG";
		outputValue["display"]["player"] = roundStage % 4;
		outputValue["display"]["tile"] = lastTile;

		ExtraInfo exinfo;
		string extraStr[4] = { "", "", "", "" };
		promptsForDisplay.clear();
		if ((lastANGANG && is_yao_jiu(lastTile)) || lastBUGANG) {
			exinfo.check(g, lastTile, roundStage % 4, false);
			for (int playerID = 0; playerID < 4; playerID++) {
				if (playerID == roundStage % 4)
					continue;
				for (int i = 0; i < 8; i++) {
					if (exinfo.actionList[playerID][i]) {
						extraStr[playerID] += ActionName[i] + ",";
					}
				}
				if (extraStr[playerID].length() != 0) {
					extraStr[playerID].pop_back();
				}
			}
		}


		Json::Value info;
		show_dora_indicators(info);

		for (int i = 0; i < 4; i++) {
			outputValue["content"][to_string(i)].clear();
			outputValue["content"][to_string(i)]["doraIndicators"] = info["doraIndicators"];
			outputValue["content"][to_string(i)]["state"] = "3 " + to_string(roundStage % 4) + " " + cOp;

			if (roundStage % 4 != i && extraStr[i].length() != 0) {
				promptsForDisplay[to_string(i)]["validact"] = extraStr[i];
			}

		}

	}
	else {
		// 立直情况
		outputValue["display"]["action"] = lastOp;
		outputValue["display"]["player"] = roundStage % 4;
		outputValue["display"]["tile"] = lastTile;



		Json::Value info;
		show_dora_indicators(info);

		for (int i = 0; i < 4; i++) {
			outputValue["content"][to_string(i)].clear();
			outputValue["content"][to_string(i)]["doraIndicators"] = info["doraIndicators"];
			outputValue["content"][to_string(i)]["state"] = "3 " + to_string(roundStage % 4) + " " + lastOp + " " + lastTile;
			outputValue["content"][to_string(i)]["playDrawnTile"] = isPlayDrawnTileOnly;
		}
		ExtraInfo exinfo;
		string extraStr[4] = { "", "", "", "" };
		promptsForDisplay.clear();
		exinfo.check(g, lastTile, roundStage % 4, false);
		for (int playerID = 0; playerID < 4; playerID++) {
			if (playerID == roundStage % 4)
				continue;
			for (int i = 0; i < 8; i++) {
				if (exinfo.actionList[playerID][i]) {
					extraStr[playerID] += ActionName[i] + ",";
				}
			}
			if (extraStr[playerID].length() != 0) {
				extraStr[playerID].pop_back();
			}
		}

		for (int i = 0; i < 4; i++) {
			if (roundStage % 4 != i) {
				if (extraStr[i].length() != 0) {
					promptsForDisplay[to_string(i)]["validact"] = extraStr[i];
				}
			}
		}

	}
}

void roundInput(Json::Value &inputValue)
{
	if (roundStage < 0) {
		for (int i = 0; i < 4; i++) {
			checkInputPASS(inputValue[to_string(i)], i, false);
		}
		roundStage++;
	}
	else if (roundStage >= 0 && roundStage < 4) {

		for (int i = 0; i < 4; i++) {
			if (roundStage != i) {
				checkInputPASS(inputValue[to_string(i)], i, false);
			}
			else {
				checkInputDRAW(inputValue[to_string(i)], i);
			}
		}


		lastGANG = currGANG;
		lastBUGANG = currBUGANG;
		lastANGANG = currANGANG;
		currGANG = currBUGANG = currANGANG = false;


	}
	else if (roundStage >= 4 && roundStage < 8) {
		// 先确认一下四杠散了
		if (g.numOfGang == 4) { // 四杠散了在打牌的那一刻就over了
			int numPlayerGang = 0; // 有几个人开了杠
			for (int id = 0; id < 4; id++) {
				auto & p = g.players[id];
				if (p.anGangTiles.size())
					numPlayerGang++;
				else {
					for (auto & x : p.mingTiles) {
						if (x.size() == 4) {
							numPlayerGang++;
							break;
						}
					}
				}
			}
			if (numPlayerGang >= 2) { // 当有两人及以上开了累计四次杠并在打出牌的那一刻，四杠散了
				outputValue["command"] = "finish";
				outputValue["display"]["action"] = "SAN";
				int s[4] = { 0 };
				for (int i = 0; i < 4; i++) {
					if (g.players[i].isLiZhi)
						s[i] -= 1000;
				}
				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][i] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
				}
				cout << outputValue;
				exit(0);
			}
		}

		for (int i = 0; i < 4; i++) {
			if (roundStage == i + 4) {
				checkInputPASS(inputValue[to_string(i)], i, true);
			}
			else {
				checkInputPLAY1(inputValue[to_string(i)], i);
			}
		}
		if (outputValue["command"].isString()) {
			string temp = outputValue["command"].asString();
			if (temp == "finish") {
				int s[4] = { 0 };
				for (int i = 0; i < 4; i++)
					if (outputValue["display"][to_string(i)].isObject())
						s[i] = outputValue["display"][to_string(i)]["ScoreCnt"].asInt();
				if (s[roundStage % 4]) exit(1); // 不知道会不会出现这种奇怪的情况 roundStage % 4 放炮的
												// 点炮情况就只需要考虑庄家分数要乘以1.5倍
												// 庄家：maxScore = ceil(1.5 * maxScore / 300) * 300;
												// 闲家：maxScore = ceil(maxScore / 200) * 100 + ceil(maxScore / 400) * 200;

												// 放铳
				for (int i = 0; i < 4; i++) {
					if (i == g.changFeng) {
						s[i] = ceil(6 * s[i] / 100.0) * 100;
					}
					else
						s[i] = ceil(4 * s[i] / 100.0) * 100;
					outputValue["display"][to_string(i)]["ScoreCnt"] = s[i];
				}

				s[roundStage % 4] = -s[0] - s[1] - s[2] - s[3]; // 有可能会出现一炮多响的情况
				int lizhiScore = 0;
				bool flag = false; // 立直的人胡牌了
				for (int i = 0; i < 4; i++) {
					if (g.players[i].isLiZhi) { // 统计有几根立直棒
						lizhiScore += 1000;
						if (s[i] > 0)
							flag = true;
					}
				}
				for (int i = 0; i < 4; i++) {
					if (s[i] > 0) { // 从东家逆时针，第一个胡牌的人获得所有点棒
						s[i] += lizhiScore;
						break;
					}
				}
				for (int i = 0; i < 4; i++) {
					if (g.players[i].isLiZhi) { // 收取立直棒的点数
						s[i] -= 1000;
					}
				}
				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][i] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
				}
				if (flag) {
					Json::Value info;
					show_hidden_dora_indicators(info);
					outputValue["display"]["hiddenDoraIndicators"] = info["hiddenDoraIndicators"];
				}

				cout << outputValue;
				exit(0);
			}
		}

		// roundInput处理的是最后一个log。
		// 考虑最后一张牌，只有PASS或者HU，没有其他情况
		if (g.mountainTiles.size() == 4) {
			roundStage = (roundStage + 1) % 4;
			return;
		}

		bool pass = true;
		for (int i = 0; i < 4; i++) {
			if (pass && roundStage != i + 4) {
				pass = !checkInputPLAY2(inputValue[to_string(i)], i);
			}
		}
		for (int i = 0; i < 4; i++) {
			if (pass && roundStage != i + 4) {
				pass = !checkInputPLAY3(inputValue[to_string(i)], i);
			}
		}
		if (pass) {
			roundStage = (roundStage + 1) % 4;
		}
	}
	else if (roundStage >= 8 && roundStage < 12) {
		// 确认一下还能不能杠
		if (g.numOfGang == 5) {
			playerError(roundStage % 4, "WA");
		}

		for (int i = 0; i < 4; i++) {
			checkInputGANG(inputValue[to_string(i)], i);
		}
		if (outputValue["command"].isString()) {
			string temp = outputValue["command"].asCString();
			if (temp == "finish") {
				int s[4] = { 0 };
				for (int i = 0; i < 4; i++)
					if (outputValue["display"][to_string(i)].isObject())
						s[i] = outputValue["display"][to_string(i)]["ScoreCnt"].asInt();
				if (s[roundStage % 4]) exit(1); // 不知道会不会出现这种奇怪的情况 roundStage % 4 放炮的
				for (int i = 0; i < 4; i++) {
					if (i == g.changFeng) {
						s[i] = ceil(6 * s[i] / 100.0) * 100;
					}
					else
						s[i] = ceil(4 * s[i] / 100.0) * 100;
					outputValue["display"][to_string(i)]["ScoreCnt"] = s[i];
				}
				s[roundStage % 4] = -s[0] - s[1] - s[2] - s[3];
				int lizhiScore = 0;
				bool flag = false;
				for (int i = 0; i < 4; i++) {
					if (g.players[i].isLiZhi) { // 统计有几根立直棒
						lizhiScore += 1000;
						if (s[i] > 0)
							flag = true;
					}
				}
				for (int i = 0; i < 4; i++) {
					if (s[i] > 0) { // 从东家逆时针，第一个胡牌的人获得所有点棒
						s[i] += lizhiScore;
						break;
					}
				}
				for (int i = 0; i < 4; i++) {
					if (g.players[i].isLiZhi) { // 收取立直棒的点数
						s[i] -= 1000;
					}
				}
				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][i] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
				}

				if (flag) {
					Json::Value info;
					show_hidden_dora_indicators(info);
					outputValue["display"]["hiddenDoraIndicators"] = info["hiddenDoraIndicators"];
				}
				cout << outputValue;
				exit(0);
			}
		}
		roundStage -= 8;
	}
	else if (roundStage >= 12 && roundStage < 16) {
		// 立直情况类似直接出牌，但是需要另外做标记

		for (int i = 0; i < 4; i++) {
			if (roundStage == i + 12) {
				checkInputPASS(inputValue[to_string(i)], i, true); // 是自己打出的牌，看看是不是ok
			}
			else {
				checkInputPLAY1(inputValue[to_string(i)], i); // 自己打的牌有没有点炮
			}
		}
		if (outputValue["command"].isString()) {
			string temp = outputValue["command"].asCString();
			if (temp == "finish") {
				int s[4] = { 0 };
				for (int i = 0; i < 4; i++)
					if (outputValue["display"][to_string(i)].isObject())
						s[i] = outputValue["display"][to_string(i)]["ScoreCnt"].asInt();
				if (s[roundStage % 4]) exit(1); // 不知道会不会出现这种奇怪的情况 roundStage % 4 放炮的
				for (int i = 0; i < 4; i++) {
					if (i == g.changFeng) {
						s[i] = ceil(6 * s[i] / 100.0) * 100;
					}
					else
						s[i] = ceil(4 * s[i] / 100.0) * 100;
					outputValue["display"][to_string(i)]["ScoreCnt"] = s[i];
				}
				s[roundStage % 4] = -s[0] - s[1] - s[2] - s[3];

				int lizhiScore = 0;
				bool flag = false;
				for (int i = 0; i < 4; i++) {
					if (g.players[i].isLiZhi) { // 统计有几根立直棒
						lizhiScore += 1000;
						if (s[i] > 0)
							flag = true;
					}
				}
				for (int i = 0; i < 4; i++) {
					if (s[i] > 0) { // 从东家逆时针，第一个胡牌的人获得所有点棒
						s[i] += lizhiScore;
						break;
					}
				}
				for (int i = 0; i < 4; i++) {
					if (g.players[i].isLiZhi) { // 收取立直棒的点数
						s[i] -= 1000;
					}
				}
				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][roundStage % 4] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
				}
				if (flag) {
					Json::Value info;
					show_hidden_dora_indicators(info);
					outputValue["display"]["hiddenDoraIndicators"] = info["hiddenDoraIndicators"];
				}
				cout << outputValue;
				exit(0);
			}
		}
		// 立直只有在他家没荣和时成立
		g.players[roundStage % 4].isLiZhi = g.players[roundStage % 4].isYiFa = true;
		g.players[roundStage % 4].numLiZhi = g.players[roundStage % 4].outTiles.size();

		bool pass = true;
		for (int i = 0; i < 4; i++) {
			if (pass && roundStage != i + 4) {
				pass = !checkInputPLAY2(inputValue[to_string(i)], i); // 自己打的牌有没有人要碰杠
			}
		}
		for (int i = 0; i < 4; i++) {
			if (pass && roundStage != i + 4) {
				pass = !checkInputPLAY3(inputValue[to_string(i)], i); // 自己打的牌下家要不要吃
			}
		}
		if (pass) {
			roundStage = (roundStage + 1) % 4;
		}
	}
	else {
		playerError(roundStage % 4, "ERR");
	}
}

template<typename T>
Json::Value toJson(vector<T> arr)
{
	Json::Value result(Json::ValueType::arrayValue);
	for (auto & x : arr)
		result.append(x);
	return result;
}

int main()
{
	roundStage = -2;
	//-2:通知位置
	//-1:发牌
	//0-3:玩家摸牌
	//4-7:玩家打出牌后，通知所有玩家
	//8-12:玩家杠牌，通知所有玩家
	//13-16:玩家立直，通知所有玩家

	// freopen("in.json", "r", stdin);
	cin >> inputValue;
	numRestTiles = 70;

	bool isRandom = true;
	vector<string> handTilesList;
	string str;
	unsigned int randSeed = time(nullptr);
	if (inputValue["initdata"].isObject()) {
		if (!inputValue["initdata"]["game"].isNull() && inputValue["initdata"]["game"]["tiles"].isString())
		{
			isRandom = false;
			str = inputValue["initdata"]["game"]["tiles"].asString();
			boost::split(handTilesList, str, boost::is_any_of(" "));
		}
		else {
			if (inputValue["initdata"]["srand"].isUInt())
				randSeed = inputValue["initdata"]["srand"].asUInt();
		}
		if (isRandom) {
			g.init_mountain_tiles(randSeed);
			for (auto & x : g.mountainTiles) {
				// TODO：需要改，因为str要的是136张手牌，但是init_all之后只有74张
				str = str + x + " ";
				handTilesList.push_back(x);
			}
			str.pop_back();
		}

		if (inputValue["initdata"]["quan"].isUInt())
			quan = inputValue["initdata"]["quan"].asUInt();
		else
			quan = 0; // 默认为速东局
	}
	else {
		g.init_mountain_tiles(randSeed);
		for (auto & x : g.mountainTiles) {
			// TODO：需要改，因为str要的是136张手牌，但是init_all之后只有74张
			str = str + x + " ";
			handTilesList.push_back(x);
		}
		str.pop_back();
		quan = 0; // 默认为速东局
	}

	outputValue["command"] = "request";
	outputValue["initdata"]["game"]["tiles"] = str;
	outputValue["initdata"]["quan"] = quan;
	outputValue["initdata"]["srand"] = randSeed;


	if (inputValue["log"].size() == 0) {
		srand(randSeed);
		outputValue["display"]["action"] = "INIT";
		outputValue["display"]["srand"] = outputValue["initdata"]["srand"] = randSeed;
		outputValue["display"]["quan"] = outputValue["initdata"]["quan"] = quan;

		for (int i = 0; i < 4; i++) {
			outputValue["content"][to_string(i)]["state"] = "0 " + to_string(i) + " " + to_string(quan);
		}
		cout << outputValue;
		return 0;
	}
	g.init_all(isRandom, handTilesList);
	for (int i = 0; i < 4; i++) {
		g.players[i].changFeng = quan;
		g.players[i].pos = i;
	}

	// TODO:
	roundInput(inputValue["log"][1]);
	for (unsigned int i = 2; i < inputValue["log"].size(); i += 2) {
		Json::Value tmp = outputValue;
		outputValue["display"].clear();

		roundOutput(tmp);
		isPlayDrawnTileOnly = false;
		roundInput(inputValue["log"][i + 1]);
	}
	promptsForDisplay.clear();
	outputValue["display"].clear();

	roundOutput(outputValue);
	outputValue["display"]["tileCnt"] = unsigned(g.mountainTiles.size() - 4);
	outputValue["display"]["prompt"] = promptsForDisplay;
	if (!outputValue["content"]["0"].isInt()) {
		for (int i = 0; i < 4; i++) {
			outputValue["content"][to_string(i)]["validact"] = promptsForDisplay[to_string(i)]["validact"];
		}
	}

	cout << outputValue;
	// cout << promptsForDisplay << endl;
	return 0;
}