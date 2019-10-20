#include "mahjong.h"
/*
void init() {

	MenQianYi[GUO_SHI_WU_SHUANG_SHI_SAN_MIAN] = true;
	MenQianYi[SI_AN_KE_DAN_QI] = true;
	MenQianYi[CHUN_ZHENG_JIU_LIAN_BAO_DENG] = true;

	MenQianYi[GUO_SHI_WU_SHUANG] = true;
	MenQianYi[SI_AN_KE] = true;
	MenQianYi[JIU_LIAN_BAO_DENG] = true;
	MenQianYi[TIAN_HU] = true;
	MenQianYi[DI_HU] = true;

	MenQianYi[ER_BEI_KOU] = true;

	MenQianYi[QI_DUI_ZI] = true;
	MenQianYi[SHUANG_LI_ZHI] = true;

	MenQianYi[LI_ZHI] = true;
	MenQianYi[YI_FA] = true;
	MenQianYi[MEN_QIAN_QING_ZI_MO] = true;
	MenQianYi[PING_HU] = true;
	MenQianYi[YI_BEI_KOU] = true;

	ShiXiaYi[QING_YI_SE] = true;

	ShiXiaYi[HUN_YI_SE] = true;
	ShiXiaYi[CHUN_QUAN_DAI_YAO_JIU] = true;

	ShiXiaYi[SAN_SE_TONG_SHUN] = true;
	ShiXiaYi[YI_QI_TONG_GUAN] = true;
	ShiXiaYi[HUN_QUAN_DAI_YAO_JIU] = true;


	FanShu[GUO_SHI_WU_SHUANG_SHI_SAN_MIAN] = 20;
	FanShu[SI_AN_KE_DAN_QI] = 20;
	FanShu[CHUN_ZHENG_JIU_LIAN_BAO_DENG] = 20;
	FanShu[DA_SI_XI] = 20;

	FanShu[GUO_SHI_WU_SHUANG] = 10;
	FanShu[SI_AN_KE] = 10;
	FanShu[JIU_LIAN_BAO_DENG] = 10;
	FanShu[TIAN_HU] = 10;
	FanShu[DI_HU] = 10;
	FanShu[ZI_YI_SE] = 10;
	FanShu[LV_YI_SE] = 10;
	FanShu[QING_LAO_TOU] = 10;
	FanShu[DA_SAN_YUAN] = 10;
	FanShu[SI_GANG_ZI] = 10;
	FanShu[XIAO_SI_XI] = 10;

	FanShu[QING_YI_SE] = 6;

	FanShu[ER_BEI_KOU] = 3;
	FanShu[CHUN_QUAN_DAI_YAO_JIU] = 3;
	FanShu[HUN_YI_SE] = 3;

	FanShu[QI_DUI_ZI] = 2;
	FanShu[SHUANG_LI_ZHI] = 2;
	FanShu[HUN_QUAN_DAI_YAO_JIU] = 2;
	FanShu[SAN_SE_TONG_SHUN] = 2;
	FanShu[DUI_DUI_HU] = 2;
	FanShu[SAN_AN_KE] = 2;

	FanShu[SAN_GANG_ZI] = 2;
	FanShu[SAN_SE_TONG_KE] = 2;
	FanShu[HUN_LAO_TOU] = 2;
	FanShu[XIAO_SAN_YUAN] = 2;
	FanShu[YI_QI_TONG_GUAN] = 2;

	FanShu[LI_ZHI] = 1;
	FanShu[YI_FA] = 1;
	FanShu[MEN_QIAN_QING_ZI_MO] = 1;
	FanShu[PING_HU] = 1;
	FanShu[DUAN_YAO_JIU] = 1;
	FanShu[YI_BEI_KOU] = 1;
	FanShu[YI_PAI_BAI] = 1;
	FanShu[YI_PAI_FA] = 1;
	FanShu[YI_PAI_ZHONG] = 1;
	FanShu[LING_SHANG_KAI_HUA] = 1;

	FanShu[LING_SHANG_KAI_HUA] = 1;
	FanShu[QIANG_GANG] = 1;
	FanShu[HAI_DI_LAO_YUE] = 1;
	FanShu[HE_DI_MO_YU] = 1;
	FanShu[YI_PAI_CHANG_FENG] = 1;
	FanShu[YI_PAI_MENG_FENG] = 1;

	FanShu[LIU_JU_MAN_GUAN] = 10;

}
*/
bool isTileSame(const string & a, const string & b) {
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
		type_of_tiles = getTypeOfTile(c);
		idx = (!five_red && x[1] == '0') ? 5 : x[1] - '0';
		res[type_of_tiles][idx]++;
	}
	for (int i = 0; i < 4; i++)
		res[i][10] = accumulate(res[i].begin(), res[i].end(), 0);
	return res;
}

int typeMianZi(const string s) {
	int l = s.length();
	if (l == 4) { // 4个
		char n = s[0];
		for (int i = 1; i < 4; i++)
			if (s[i] != n)
				return -1;
		return GANG_ZI;
	}
	else if (l == 3) { // 3个
		char n = s[0];
		if (s[1] == n && s[2] == n)
			return KE_ZI;
		if (s[1] == n + 1 && s[2] == n + 2)
			return SHUN_ZI;
	}
	return -1;
}

bool isMultiMianZi(const string s, int hasPair) {
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

bool Tiles::isEqual(const Tiles & b) {
	if (type == b.type && part.size() == b.part.size()) {
		int N = part.size();
		for (int i = 0; i < N; i++)
			if (part[i] != b.part[i])
				return false;
		return true;
	}
	return false;
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
vector<vector<Tiles>> getCombinationOfType(vector<int> & num, char cType) {
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

vector<vector<Tiles>> getAllCombinations(vector<vector<int>> & nums) {
	string strType = "BTWZ";
	vector<vector<vector<Tiles>>> allTypedTiles;
	vector<vector<Tiles>> blank(1, vector<Tiles>());
	for (int i = 0; i <= 3; i++) {
		auto tmp = getCombinationOfType(nums[i], strType[i]);
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
		int cnt = 0;
		for (auto & x : partition) 
			cnt += x.type == "JIANG";
		if (cnt == 1)
			flag = true;
	}
	if (!flag)
		res = {};

	return res;
}

// 听牌类型检测 两面 0b1 、双碰 0b10 、嵌张 0b1000 、边张 0b10000、单骑 0b100
int checkTingType(vector<string> vec_s, string c, int hasPair) { // vec_s包含了p,p代表的是新加入的牌
	if (vec_s.size() == 0)
		return 0;

	string s, p = c;
	if (p[0] != 'Z' && p[1] == '0')
		p[1] = '5';
	vector<int> nums(15, 0);
	for (auto & x : vec_s) {
		if (x[0] == p[0]) {
			s.push_back(x[1] == '0' ? '5' : x[1]);
			nums[(x[1] - '0') == 0 ? 5 : x[1] - '0']++;
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
				if (isMultiMianZi(tmp, hasPair - 1))
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
		if (isMultiMianZi(tmp, hasPair - 1))
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
		if (isMultiMianZi(tmp, hasPair))
			res |= 0b1000;
	}
	// check 边张
	if (p[1] == '3' || p[1] == '7') {
		int i = p[1] == '3' ? -1 : 1;
		if (nums[p[1] - '0' + i] > 0 && nums[p[1] - '0' + i * 2] > 0) {
			char c = p[1];
			string tmp = s;
			for (int k = 0; k < 3; k++) {
				c = c + i * k;
				auto it = tmp.find_first_of(c);
				tmp.replace(it, 1, "");
			}
			if (isMultiMianZi(tmp, hasPair))
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
		if (isMultiMianZi(tmp, hasPair))
			flag = true;
	}
	if (p[1] >= '4' && p[1] <= '9' && nums[p[1] - '0' - 1] > 0 && nums[p[1] - '0' - 2] > 0) {
		string tmp = s;
		for (int k = 0; k < 3; k++) {
			auto it = tmp.find_first_of(p[1] - k);
			tmp.replace(it, 1, "");
		}
		if (isMultiMianZi(tmp, hasPair))
			flag = true;
	}
	if (flag)
		res |= 0b1;
	return res;
}

bool isYaoJiu(string t) {
	int l = t.length();
	if (t[0] == 'Z')
		return true;
	for (int i = 0; i < l / 2; i++)
		if (t[2 * i + 1] == '1' || t[2 * i + 1] == '9')
			return true;
	return false;
}

int getTypeOfTile(char c) {
	return ((c > 'B') + (c > 'T') + (c > 'W'));
}
int getScore(int fan, int fu, bool isYakuman) {
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

FanFu getHighFanFu(vector<FanFu> & vecF) { // 这里写得其实不太好，先这样简单处理了
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



