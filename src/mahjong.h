#pragma once
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
#include "json/json.h"

using namespace std;

extern int quan;
extern string lastTile;
extern string lastOp;
//"DRAW" "PLAY" "PENG" "GANG" "CHI"
extern string tileCHI;
extern bool lastGANG;
extern bool currGANG;
extern bool lastBUGANG;
//上一回合是否为补杠
extern bool currBUGANG;
//当前回合是否为补杠 与抢杠、岭上开花有关
extern bool lastANGANG;
//上一回合是否为暗杠
extern bool currANGANG;
//当前回合是否为暗杠 与国士无双可以抢暗杠有关
extern bool isMingPai;
//之前回合是否有鸣牌行为发生 与地和、九种九牌有关

extern int roundStage;
//-2:通知位置
//-1:发牌
//0-3:玩家摸牌
//4-7:玩家打出牌后，通知所有玩家
//8-12:玩家杠牌，通知所有玩家
// 13-16:玩家立直，通知所有玩家

extern int numRestTiles;
// 还剩下多少张牌

// void init();

bool isTileSame(const string & a, const string & b);

vector<vector<int>> string_to_34_tiles(const vector<string>& vec_str, bool five_red);

int typeMianZi(const string s);

bool isMultiMianZi(const string s, int hasPair);

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

	bool isEqual(const Tiles & b);
};

void back(vector<vector<Tiles>>& res, vector<Tiles>& vecTiles, vector<int>& num, int N, bool hasPair, char cType);

vector<vector<Tiles>> getCombinationOfType(vector<int>& num, char cType);

vector<vector<Tiles>> getAllCombinations(vector<vector<int>>& nums);

int checkTingType(vector<string> vec_s, string p, int hasPair);

bool isYaoJiu(string t);

int getTypeOfTile(char c);

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

// 


struct Player {
	int pos; // “东南西北”，编号依次为0~3
	int changFeng; // 每圈开始的时候都要更新一下
	int numLiZhi; // 从自己打出的第几张牌开始立直
	bool isLiZhi; // 是否立直
	bool isYiFa;
	bool isLiuJuManGuan; // 是否能够满贯流局
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
	int checkHuPrerequisite(const string c, int cPlayer);

	void initalize(int p, const vector<string> & hc);

	vector<string> retOwnTiles(bool reCalculate = false);

	bool deleteTile(const string c);
};

using YiZhongCheckFunction = std::function< FanFu (const vector<Tiles>& partition, const Player& p, string t, int pid) >; 
// vector<vector<Tiles>>& res, 

struct YiZhongChecker
{
	const char *name;
	/*bool isMenQianYi;
	bool isShiXiaYi;
	int FanShu;*/
	YiZhongType id;
	YiZhongCheckFunction checker;
};

extern YiZhongChecker specialYiZhong[3], yakumanYiZhong[13], usualYiZhong[29];

struct Event
{
	string tile; // 牌
	int doerID; // 行为发起者
	int fromID; // 牌来自于谁
	int act; // 吃 碰 杠 加杠 立直
};

int getScore(int fan, int fu, bool isYakuman);

FanFu getHighFanFu(vector<FanFu>& vecF);

vector<FanFu> checkHu(int player, const string c, int offer, bool finish);
