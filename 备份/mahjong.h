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
#include <ctype.h>
#include <math.h>
#include <map>
#include <numeric>
#include <sstream>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include "jsoncpp/json.h"

using namespace std;

// ��initʱ�ͻ�ȷ����
//bool MenQianYi[50];
//bool ShiXiaYi[50];
//int FanShu[50];

extern int quan;
extern string lastTile;
extern string lastOp;
//"DRAW" "PLAY" "PENG" "GANG" "CHI"
extern string tileCHI;
extern bool lastGANG;
extern bool currGANG;
extern bool lastBUGANG;
//��һ�غ��Ƿ�Ϊ����
extern bool currBUGANG;
//��ǰ�غ��Ƿ�Ϊ���� �����ܡ����Ͽ����й�
extern bool lastANGANG;
//��һ�غ��Ƿ�Ϊ����
extern bool currANGANG;
//��ǰ�غ��Ƿ�Ϊ���� ���ʿ��˫�����������й�
extern bool isMingPai;
//֮ǰ�غ��Ƿ���������Ϊ���� ��غ͡����־����й�

extern int roundStage;
//-2:֪ͨλ��
//-1:����
//0-3:�������
//4-7:��Ҵ���ƺ�֪ͨ�������
//8-12:��Ҹ��ƣ�֪ͨ�������
// 13-16:�����ֱ��֪ͨ�������

extern int numRestTiles;
// ��ʣ�¶�������

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
	int fu; // ֻ�п������ǲ������
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

// ���桢Ƕ�š����š�˫��������
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
	JIA_GANG = 3,
	LIZHI = 5,
};


struct Player {
	int pos; // ���������������������Ϊ0~3
	int changFeng; // ÿȦ��ʼ��ʱ��Ҫ����һ��
	int numLiZhi; // ���Լ�����ĵڼ����ƿ�ʼ��ֱ
	bool isLiZhi; // �Ƿ���ֱ
	bool isYiFa;
	bool isLiuJuManGuan; // �Ƿ��ܹ���������
	bool isZhenTing[3]; // �����Ƿ����������ж��Ƿ�����
						// [0]�������������Լ������Ҫ�����ƣ����������Ϊû�ۣ��޷��ٺͳ��ǻ�����
						// [1]ͬѲ�����������Ҵ���Ҫ�����ƣ�����û��ֱҲû�ۣ�ֻҪ�Լ����оͿ��Խ��
						// [2]��ֱ����������ֱ�ˣ�����©�����ܺ����ƣ��޷��ٺ�ֻ������
	vector<string> handTiles; // δ������������
	vector<vector<string> > anGangTiles; // ������
	vector<vector<string> > mingTiles; // ����
	vector<pair<int, string> > mingTilesOffer; // ����ʱ���Ƶ��ṩ���Լ�����ʲô

	vector<string> outTiles; // �Ѿ��������
	vector<string> ownTiles; // ���ϰ����˰��ܡ��������ڵ�������

	// vector<vector<Tiles> > partitionTiles; // ���е����ƣ�ע�������ơ�δ�����������ơ��Ĳ��Ʒ�ʽ��

							 // person put out c
	int checkHuPrerequisite(const string c, int cPlayer);

	void initalize(int p, const vector<string> & hc);

	vector<string> retOwnTiles(bool reCalculate = false);
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
	string tile; // ��
	int doerID; // ��Ϊ������
	int fromID; // ��������˭
	int act; // �� �� �� �Ӹ� ��ֱ
};

int getScore(int fan, int fu, bool isYakuman);

FanFu getHighFanFu(vector<FanFu>& vecF);

vector<FanFu> checkHu(int player, const string c, int offer, bool finish);
