#include "mahjong.h"

YiZhongChecker specialYiZhong[] = {
	{
		"��ʿ��˫ʮ����", GUO_SHI_WU_SHUANG_SHI_SAN_MIAN, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (!(isYaoJiu(t)) || p.mingTiles.size() != 0 || p.anGangTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	string test = "";
	for (auto & x : p.handTiles) {
		if (!(isYaoJiu(x)))
			return FanFu({}, 0, 0, 0);
		test += x;
	}
	if (test != "B1B9T1T9W1W9Z1Z2Z3Z4Z5Z6Z7")
		return FanFu({}, 0, 0, 0);
	return FanFu({ GUO_SHI_WU_SHUANG_SHI_SAN_MIAN }, 26, 0, 0);
}
	},
	{
		"��ʿ��˫", GUO_SHI_WU_SHUANG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (!(isYaoJiu(t)) || p.mingTiles.size() != 0 || p.anGangTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	string test = "";
	for (auto & x : p.handTiles) {
		if (!(isYaoJiu(x)))
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
		"�߶���", QI_DUI_ZI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.anGangTiles.size() || p.mingTiles.size())
		return FanFu({}, 0, 0, 0);
	map<string, int> countNum;
	countNum.insert(make_pair(t, 1));
	for (auto & x : p.handTiles) {
		auto it = countNum.find(x);
		if (it != countNum.end())
			it->second++;
		else
			countNum.insert(make_pair(x, 1));
	}
	for (auto & x : countNum) {
		if (x.second != 2)
			return FanFu({}, 0, 0, 0);
	}
	return FanFu({ QI_DUI_ZI }, 2, 25, 0);
}
	},

};

YiZhongChecker yakumanYiZhong[] = {
	{
		"�İ��̵���", SI_AN_KE_DAN_QI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.mingTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	int tmp = 0;
	string last = "";
	bool flag = true;
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
		"������������", CHUN_ZHENG_JIU_LIAN_BAO_DENG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
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
		"����ϲ", DA_SI_XI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ����ϲ��Ҫ�ж����������ֿ��� Z 1234ÿ���ƶ���Ҫ��3�ż�����
	// ���⻹��Ҫ�н��ƣ����ƿ����������
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
		"�İ���", SI_AN_KE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.mingTiles.size() != 0 || p.pos != pid)
		return FanFu({}, 0, 0, 0);
	// Ҫ���İ��̵������ֿ��������������ֻ����������������������
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
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
		"��������", JIU_LIAN_BAO_DENG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.mingTiles.size() != 0 || p.anGangTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	int type_of_tiles = getTypeOfTile(t[0]);
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
		"���", TIAN_HU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ��һѲ�׼���������
	if (p.pos != 0 || p.pos != pid || p.mingTiles.size() != 0 || p.anGangTiles.size() != 0 || p.outTiles.size() != 0)
		return FanFu({}, 0, 0, 0);
	return FanFu({ TIAN_HU }, 13, 0, 0);
}
	},
	{
		"�غ�", DI_HU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ��һѲ�м���û��������ʱ��������
	if (p.pos == 0 || p.pos != pid || p.mingTiles.size() != 0 || p.anGangTiles.size() != 0 || p.outTiles.size() != 0 || isMingPai)
		return FanFu({}, 0, 0, 0);
	return FanFu({ DI_HU }, 13, 0, 0);
}
	},
	{
		"��һɫ", ZI_YI_SE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ��һɫ�� ���������׷��� ���������
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
		"��һɫ", LV_YI_SE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ��һɫ�� T2 T3 T4 T6 T8 Z6 ��23468�ͷ� ��������� (type_of_tiles = 1 or 3)
	// ��һɫ�϶��������߶��ӣ�ֻ���ǳ�����������

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
		"����ͷ", QING_LAO_TOU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ����ͷ�� B1 B9 T1 T9 W1 W9 ��������ɣ�Ҳ���������߶���
	// û��˳�ӣ�ֻ���ǿ���
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	if (nums[3][10]) // ����������
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
		"����Ԫ", DA_SAN_YUAN, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// �ռ�����Ԫ�Ƶ�������ӻ���ӣ����⻹��һ�����ӣ�Ȼ����ȸͷ
	// ��Ԫ�Ʋ�����ȸͷ
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][5] < 3 || nums[3][6] < 3 || nums[3][7] < 3)
		return FanFu({}, 0, 0, 0);
	return FanFu({ DA_SAN_YUAN }, 13, 0, 0);
}
	},
	{
		"�ĸ���", SI_GANG_ZI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// �������Ĵθܲ��������ܡ������ܾ��ɣ���������ֻ�е�������
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
		"С��ϲ", XIAO_SI_XI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// �����������е�3���Կ��ӣ����߸��ӣ����ڣ�ʣ�µ�һ����ȸͷ�ĺ�����ʽ��
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
		"��һɫ", QING_YI_SE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ��һɫ���ܺ������ƣ�����ֻ����BTW���߶����Ѿ�����������
	int baseFan = 6;
	if (p.mingTiles.size() > 0)
		baseFan--;
	char c = t[0];
	if (c == 'Z')
		return FanFu({}, 0, 0, 0);
	int typeT = getTypeOfTile(c);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[typeT][10] != hc.size())
		return FanFu({}, 0, 0, 0);
	return FanFu({ QING_YI_SE }, baseFan, 0, 0);
}
	},
	{
		"������", ER_BEI_KOU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.anGangTiles.size() != 0 || p.mingTiles.size() != 0 || partition.empty())
		return FanFu({}, 0, 0, 0);
	int type_of_tiles = 0;
	vector<vector<string>> vecTiles(3); // ȡÿ��˳�ӵĵ�һ��

	for (auto & x : partition) {
		if (x.type == "KEZI")
			return FanFu({}, 0, 0, 0);
		if (x.type == "JIANG")
			continue;
		type_of_tiles = getTypeOfTile(x.part[0][0]);
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
		"��ȫ���۾�", CHUN_QUAN_DAI_YAO_JIU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ����ֻ����111(1),123,999(9),789��ȸͷֻ����11,99�����������߶���
	// ������ͷ����������ȫ���۾�һ������˳��
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

	int type_of_tile = getTypeOfTile(t[0]);

	for (auto & tiles : p.anGangTiles)
		if (!isYaoJiu(tiles[0]))
			return FanFu({}, 0, 0, 0);
	for (auto & tiles : p.mingTiles)
		if (!isYaoJiu(tiles[0]) && !isYaoJiu(tiles[2]))
			return FanFu({}, 0, 0, 0);

	int fu = 0;
	int cntShunZi = 0;
	for (auto & x : partition) {
		cntShunZi += x.type == "SHUNZI";
		if (x.type == "JIANG" || x.type == "KEZI") {
			if (!isYaoJiu(x.part[0]))
				return FanFu({}, 0, 0, 0);
			if (x.type == "KEZI") {
				if (pid == p.pos || x.part[0] != t)
					fu += 8;
				else if (nums[type_of_tile][t[1] - '0'] == 4)
					fu += 8;
				else // �����γ�������������ˣ�����Ϊ����
					fu += 4;
			}
		}
		else if (!isYaoJiu(x.part[0]) && !isYaoJiu(x.part[2]))
			return FanFu({}, 0, 0, 0);
	}
	if (cntShunZi == 0)
		return FanFu({}, 0, 0, 0);
	return FanFu({ CHUN_QUAN_DAI_YAO_JIU }, baseFan, fu, 0);
}
	},
	{
		"��һɫ", HUN_YI_SE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	// ȥ����һɫ
	if (nums[3][10] == 0) 
		return FanFu({}, 0, 0, 0);
	int baseFan = 3;
	if (p.mingTiles.size())
		baseFan--;

	int type_of_tiles; // ȷ��Ⱦ������һ��ɫ
	for (type_of_tiles = 0; type_of_tiles < 3; type_of_tiles++) {
		if (nums[type_of_tiles][10])
			break;
	}
	int tmp = hc.size() - nums[3][10];
	if (tmp != nums[type_of_tiles][10])
		return FanFu({}, 0, 0, 0);
	
	// �п������߶���
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
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (t[0] != 'Z' && nums[type_of_tiles][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
	}
	return FanFu({ HUN_YI_SE }, baseFan, fu, 0);
}
	},
	{
		"˫��ֱ", SHUANG_LI_ZHI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ���ж�
	// �ڵ�һѲ�������Ѿ����ƣ���֮ǰû��������
	if (p.mingTiles.size() != 0 || p.anGangTiles.size() != 0 || isMingPai || !p.isLiZhi)
	 	return FanFu({}, 0, 0, 0);
	if (p.numLiZhi == 1)
		return FanFu({ SHUANG_LI_ZHI }, 2, 0, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"��ȫ���۾�", HUN_QUAN_DAI_YAO_JIU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ��Ҫ�ʹ�ȫ���۾š�����ͷ�ȷֿ�������ȫ���۾�+�߶���=����ͷ
	int baseFan = 2;
	if (p.mingTiles.size())
		baseFan--;

	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	// ȥ����ȫ������ȫһ������˳��
	if (nums[3][10] == 0 || partition.empty())
		return FanFu({}, 0, 0, 0);
	// ȥ������ͷ
	int cntShunZi = 0;
	for (auto & x : partition)
		cntShunZi += x.type == "SHUNZI";
	if (cntShunZi == 0)
		return FanFu({}, 0, 0, 0);

	for (auto & tiles : p.anGangTiles)
		if (!isYaoJiu(tiles[0]))
			return FanFu({}, 0, 0, 0);
	for (auto & tiles : p.mingTiles) 
		if (!isYaoJiu(tiles[0]) && !isYaoJiu(tiles[2]))
			return FanFu({}, 0, 0, 0);

	int type_of_tile = getTypeOfTile(t[0]);

	int fu = 0;
	for (auto & x : partition) {
		if (x.type == "JIANG" || x.type == "KEZI") {
			if (!isYaoJiu(x.part[0]))
				return FanFu({}, 0, 0, 0);
			if (x.type == "KEZI") {
				if (pid == p.pos || x.part[0] != t)
					fu += 8;
				else if (type_of_tile != 3 && nums[type_of_tile][t[1] - '0'] == 4)
					fu += 8;
				else // �����γ�������������ˣ�����Ϊ����
					fu += 4;
			}
		}
		else if (!isYaoJiu(x.part[0]) && !isYaoJiu(x.part[2]))
			return FanFu({}, 0, 0, 0);
	}
	return FanFu({ HUN_QUAN_DAI_YAO_JIU }, baseFan, fu, 0);
}
	},
	{
		"��ɫͬ˳", SAN_SE_TONG_SHUN, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	int baseFan = 2;
	if (p.mingTiles.size())
		baseFan--;
	vector<vector<int>> nMin(3, vector<int>()); 
	int type_of_tile, fu = 0;
	for (auto & x : p.mingTiles) {
		type_of_tile = getTypeOfTile(x[0][0]);
		if (type_of_tile == 3)
			continue;
		if (x[0][1] != x[1][1])
			nMin[type_of_tile].push_back(x[0][1] - '0');
	}
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	for (auto & x : partition) {
		type_of_tile = getTypeOfTile(x.part[0][0]);
		if (type_of_tile == 3)
			continue;
		if (x.type == "SHUNZI")
			nMin[type_of_tile].push_back(x.part[0][1] - '0');
		if (x.type == "KEZI") {
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
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
		"�ԶԺ�", DUI_DUI_HU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
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
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
	}
	return FanFu({ DUI_DUI_HU }, 2, fu, 0);
}
	},
	{
		"������", SAN_AN_KE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// �����̷�����anGangTiles��handTiles��
	// ���⻹��һ�����ӺͶ���
	if (p.mingTiles.size() > 1 || partition.empty())
		return FanFu({}, 0, 0, 0);
	int countAnKe = p.anGangTiles.size(), fu = 0, type_of_tile = getTypeOfTile(t[0]);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	for (auto & x : partition) {
		if (x.type == "SHUNZI" || x.type == "JIANG")
			continue;
		if (pid == p.pos || x.part[0] != t) {
			countAnKe++;
			fu += 4 + 4 * isYaoJiu(x.part[0]);
		}
		else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4) {
			countAnKe++;
			fu += 4 + 4 * isYaoJiu(x.part[0]);
		}
		else // �����γ�������������ˣ�����Ϊ����
			fu += 2 + 2 * isYaoJiu(x.part[0]);
	}
	if (countAnKe == 3)
		return FanFu({ SAN_AN_KE }, 2, fu, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"������", SAN_GANG_ZI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	// ������Ҫ�����ƺͰ������ҡ�
	int countGangZi = p.anGangTiles.size(), fu = 0, type_of_tile = getTypeOfTile(t[0]);
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
			fu += 4 + 4 * isYaoJiu(x.part[0]);
		else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
			fu += 4 + 4 * isYaoJiu(x.part[0]);
		else // �����γ�������������ˣ�����Ϊ����
			fu += 2 + 2 * isYaoJiu(x.part[0]);
	}
	if (countGangZi == 3)
		return FanFu({ SAN_GANG_ZI }, 2, fu, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"��ɫͬ��", SAN_SE_TONG_KE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	// ����ɫͬ˳���ƣ���Ҫ�ҵ����ٻ�ɫ��һ����
	int fu = 0, type_of_tile;
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	vector<vector<int> > cntNum(3, vector<int>());
	for (auto & x : p.anGangTiles) {
		type_of_tile = getTypeOfTile(x[0][0]);
		if (type_of_tile == 3)
			continue;
		cntNum[type_of_tile].push_back(x[0][1] - '0');
	}
	for (auto & x : p.mingTiles) {
		type_of_tile = getTypeOfTile(x[0][0]);
		if (type_of_tile == 3 || x[0][1] != x[1][1])
			continue;
		cntNum[type_of_tile].push_back(x[0][1] - '0');
	}
	for (auto & x : partition) {
		if (x.type == "SHUNZI" || x.type == "JIANG")
			continue;
		type_of_tile = getTypeOfTile(x.part[0][0]);
		if (type_of_tile < 3)
			cntNum[type_of_tile].push_back(x.part[0][1] - '0');
		if (pid == p.pos || x.part[0] != t)
			fu += 4 + 4 * isYaoJiu(x.part[0]);
		else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
			fu += 4 + 4 * isYaoJiu(x.part[0]);
		else // �����γ�������������ˣ�����Ϊ����
			fu += 2 + 2 * isYaoJiu(x.part[0]);
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
		"����ͷ", HUN_LAO_TOU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ����ͷֻ����BTW��19������
	int fu = 0, type_of_tile;
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][10] == 0) // ȥ������ͷ
		return FanFu({}, 0, 0, 0);
	// �п������߶���
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
			type_of_tile = getTypeOfTile(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 8;
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 8;
			else // �����γ�������������ˣ�����Ϊ����
				fu += 4;
		}
	}
	return FanFu({ HUN_LAO_TOU }, 2, fu, 0);
}
	},
	{
		"С��Ԫ", XIAO_SAN_YUAN, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	// С��Ԫָ���ǰ׷������������ӣ�ʣ�µ�һ����ȸͷ��
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
		type_of_tile = getTypeOfTile(x.part[0][0]);
		if (pid == p.pos || x.part[0] != t)
			fu += 4 + 4 * isYaoJiu(x.part[0]);
		else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
			fu += 4 + 4 * isYaoJiu(x.part[0]);
		else // �����γ�������������ˣ�����Ϊ����
			fu += 2 + 2 * isYaoJiu(x.part[0]);
	}

	if (countKeZi == 2 && countPair == 1)
		return FanFu({ XIAO_SAN_YUAN }, 2, fu, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"һ��ͨ��", YI_QI_TONG_GUAN, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (partition.empty())
		return FanFu({}, 0, 0, 0);
	// �жϱȽϺܼ򵥡����������Ļ�ɫ��������9�š������ܳ��������ɫ�İ��ܣ���Ϊ˳����Ҫ
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
		type_of_tile = getTypeOfTile(x[0][0]);
		if (type_of_tile == 3 || x[0][1] == x[1][1] || obj != type_of_tile)
			continue;
		cntNum.push_back(x[0][1] - '0');
	}
	for (auto & x : partition) {
		type_of_tile = getTypeOfTile(x.part[0][0]);
		if (x.type == "JIANG")
			continue;
		else if (x.type == "SHUNZI") {
			if (obj == type_of_tile)
			cntNum.push_back(x.part[0][1] - '0');
		}
		else {
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
	}
	for (int i = 1; i <= 7; i += 3)
		if (find(cntNum.begin(), cntNum.end(), i) == cntNum.end())
			return FanFu({}, 0, 0, 0);
	return FanFu({ YI_QI_TONG_GUAN }, baseFan, fu, 0);
}
	},
	{
		"��ֱ", LI_ZHI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.isLiZhi) {
		if (p.mingTiles.size() == 0 && p.anGangTiles.size() == 0 && !isMingPai && p.numLiZhi == 1)
			return FanFu({}, 0, 0, 0);
		int type_of_tile, fu = 0;
		vector<string> hc = p.ownTiles;
		hc.push_back(t);
		vector<vector<int> > nums = string_to_34_tiles(hc, true);
		for (auto & x : partition) {
			type_of_tile = getTypeOfTile(x.part[0][0]);
			if (x.type != "KEZI")
				continue;
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
		return FanFu({ LI_ZHI }, 1, fu, 0);
	}
	
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"һ��", YI_FA, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// Ҫ�ȼ�������жϡ�������Ҫ�鿴��ֱ��������ʷ
	// �������Ͽ�������
	if (!p.isLiZhi || !p.isYiFa)
		return FanFu({}, 0, 0, 0);
	return FanFu({ YI_FA }, 1, 0, 0);// һ��һ������ֱ�����Բ��ظ��������
}
	},
	{
		"��ǰ������", MEN_QIAN_QING_ZI_MO, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.pos != pid || p.mingTiles.size() > 0)
		return FanFu({}, 0, 0, 0);
	int type_of_tile, fu = 0;
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	for (auto & x : partition) {
		type_of_tile = getTypeOfTile(x.part[0][0]);
		if (x.type != "KEZI")
			continue;
		if (pid == p.pos || x.part[0] != t)
			fu += 4 + 4 * isYaoJiu(x.part[0]);
		else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
			fu += 4 + 4 * isYaoJiu(x.part[0]);
		else // �����γ�������������ˣ�����Ϊ����
			fu += 2 + 2 * isYaoJiu(x.part[0]);
	}
	return FanFu({ MEN_QIAN_QING_ZI_MO }, 1, fu, 0);
}
	},
	{
		"ƽ��", PING_HU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {

	// �ĸ�˳�Ӽ������ƻ�ͷ��������ƣ�����ʱ���������ơ������ƺͼ��ƵĽ��ƻ����ӷ��������Բ�����
	// ͬ����Ҫ��������жϡ���������˳�ӵ��������������Ǳ��š��������Ż����ǵ�����ȸͷ��
	// �������ʱ�������������������������2�����벻��
	if (p.anGangTiles.size() != 0 || partition.empty() || t[0] == 'Z') // ƽ�Ͳ����а��� ��Ϊ��Ҫ������� t����������
		return FanFu({}, 0, 0, 0);
	// û��ȷ����ǰ����Ϊ�˼��������wiki�ϣ�ƽ������ǰ��ʱ��һ�����ڸ�¶��������
	for (auto & x : p.mingTiles) {
		if (isTileSame(x[0], x[1]))
			return FanFu({}, 0, 0, 0);
	}

	// Z-p.pos,Z-p.changFeng,Z567�Ķ�������
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	if (nums[3][10] >= 3 || nums[3][5] || nums[3][6] || nums[3][7] || nums[3][p.pos + 1] || nums[3][p.changFeng + 1])
		return FanFu({}, 0, 0, 0);

	int tmp = getTypeOfTile(t[0]);
	// �����Ǳ��š��������Ż����ǵ�����ȸͷ��
	int res = checkTingType(hc, t, nums[tmp][10] % 3 == 2);
	if ((res & 1) != 1)
		return FanFu({}, 0, 0, 0);
	if(p.pos == pid)
		return FanFu({ PING_HU }, 1, 20, 0);
	else
		return FanFu({ PING_HU }, 1, 30, 0);
}
	},
	{
		"���۾�", DUAN_YAO_JIU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	if (nums[3][10] || nums[0][1] || nums[1][1] || nums[2][1] || nums[0][9] || nums[1][9] || nums[2][9])
		return FanFu({}, 0, 0, 0);

	int type_of_tile, fu = 0;

	// �п������߶���
	if (p.mingTiles.size() == 0 && p.anGangTiles.size() == 0) {
		int cnt = 0;
		for (int i = 0; i < 3; i++)
			for (int j = 2; j <= 8; j++)
				cnt += (nums[i][j] == 2);
		if (cnt == 7)
			return FanFu({ DUAN_YAO_JIU }, 1, 0, 0);
	}

	for (auto & x : partition) {
		type_of_tile = getTypeOfTile(x.part[0][0]);
		if (x.type != "KEZI")
			continue;
		if (pid == p.pos || x.part[0] != t)
			fu += 4;
		else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
			fu += 4;
		else // �����γ�������������ˣ�����Ϊ����
			fu += 2;
	}
	return FanFu({ DUAN_YAO_JIU }, 1, fu, 0);
}
	},
	{
		"һ����", YI_BEI_KOU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// Ҫ����ǰ��
	if (p.mingTiles.size() != 0 || p.anGangTiles.size() > 2 || partition.empty())
		return FanFu({}, 0, 0, 0);
	int type_of_tile, fu = 0;
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	vector<vector<int> > cntNum(3, vector<int>());
	for (auto & x : partition) {
		type_of_tile = getTypeOfTile(x.part[0][0]);
		if (x.type == "SHUNZI")
			cntNum[type_of_tile].push_back(x.part[0][1] - '0');
		if (x.type == "KEZI") {
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
	}
	int cnt = 0;
	for (int i = 0; i < 3; i++) {
		set<int> st(cntNum[i].begin(), cntNum[i].end());
		for (auto & val : st)
			cnt += count(cntNum[i].begin(), cntNum[i].end(), val) >= 2;
	}
	if (cnt == 1) // �����������ֿ���
		return FanFu({ YI_BEI_KOU }, 1, fu, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"���ư�", YI_PAI_BAI, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
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
			type_of_tile = getTypeOfTile(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
	}
	return FanFu({ YI_PAI_BAI }, 1, fu, 0);
}
	},
	{
		"���Ʒ�", YI_PAI_FA, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
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
			type_of_tile = getTypeOfTile(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
	}
	return FanFu({ YI_PAI_FA }, 1, fu, 0);
}
	},
	{
		"������", YI_PAI_ZHONG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
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
			type_of_tile = getTypeOfTile(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
	}
	return FanFu({ YI_PAI_ZHONG }, 1, fu, 0);
}
	},
	{
		"���Ͽ���", LING_SHANG_KAI_HUA, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ���Ͽ����������Լ��ܵ���
	// t�ǿ����Ժ�õ���
	if (!(lastBUGANG || lastANGANG || currGANG) || pid != p.pos || partition.empty()) // ���Ͽ���һ�������߶���
		return FanFu({}, 0, 0, 0);
	int fu = 0;
	for (auto & x : partition)
		if (x.type == "KEZI")
			fu += 4 + 4 * isYaoJiu(x.part[0]);
	return FanFu({ LING_SHANG_KAI_HUA }, 1, fu, 0);
}
	},
	{
		"����", QIANG_GANG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	// ��Ҫ�����ж��Ƿ���ƣ�t�Ǳ��˼Ӹܣ����Ǵ����ܺͰ����¡�pid��Ҫ��p.pos��һ��
	if (p.pos == pid || !lastBUGANG || partition.empty())
		return FanFu({}, 0, 0, 0);

	vector<string> hc = p.ownTiles;
	if (find(hc.begin(), hc.end(), t) != hc.end())// ���ܿ϶��������γɿ���
		return FanFu({}, 0, 0, 0);

	int fu = 0;
	for (auto & x : partition)
		if (x.type == "KEZI")
			fu += 4 + 4 * isYaoJiu(x.part[0]);
	return FanFu({ QIANG_GANG }, 1, fu, 0);
}
	},
	{
		"��������", HAI_DI_LAO_YUE, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.pos != pid)	// t���Լ��������һ����
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	if (p.mingTiles.size() == 0 && p.anGangTiles.size() == 0) { // �߶���
		int cnt = 0;
		for (int i = 0; i <= 3; i++)
			for (int j = 1; j <= 9; j++)
				cnt += (nums[i][j] == 2);
		if (cnt == 7)
			return FanFu({ HAI_DI_LAO_YUE }, 1, 0, 0);
	}

	int fu = 0;
	for (auto & x : partition)
		if (x.type == "KEZI")
			fu += 4 + 4 * isYaoJiu(x.part[0]);
	if (numRestTiles == 0)
		return FanFu({ HAI_DI_LAO_YUE }, 1, fu, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"�ӵ�����", HE_DI_MO_YU, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
	if (p.pos == pid) // t�Ǳ����������һ����
		return FanFu({}, 0, 0, 0);
	vector<string> hc = p.ownTiles;
	hc.push_back(t);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);

	if (p.mingTiles.size() == 0 && p.anGangTiles.size() == 0) { // �߶���
		int cnt = 0;
		for (int i = 0; i <= 3; i++)
			for (int j = 1; j <= 9; j++)
				cnt += (nums[i][j] == 2);
		if (cnt == 7)
			return FanFu({ HAI_DI_LAO_YUE }, 1, 0, 0);
	}

	int type_of_tile, fu = 0;
	for (auto & x : partition) {
		if (x.type == "KEZI") {
			type_of_tile = getTypeOfTile(x.part[0][0]);
			if (x.part[0] != t)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
	}
	if (numRestTiles == 0)
		return FanFu({ HE_DI_MO_YU }, 1, fu, 0);
	return FanFu({}, 0, 0, 0);
}
	},
	{
		"���Ƴ���", YI_PAI_CHANG_FENG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
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
			type_of_tile = getTypeOfTile(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
	}
	return FanFu({ YI_PAI_CHANG_FENG }, 1, fu, 0);
}
	},
	{
		"�����ŷ�", YI_PAI_MENG_FENG, [](const vector<Tiles>& partition, const Player& p, string t, int pid) -> FanFu {
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
			type_of_tile = getTypeOfTile(x.part[0][0]);
			if (pid == p.pos || x.part[0] != t)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else if (type_of_tile < 3 && nums[type_of_tile][t[1] - '0'] == 4)
				fu += 4 + 4 * isYaoJiu(x.part[0]);
			else // �����γ�������������ˣ�����Ϊ����
				fu += 2 + 2 * isYaoJiu(x.part[0]);
		}
	}
	return FanFu({ YI_PAI_MENG_FENG }, 1, fu, 0);
}
	},
};
