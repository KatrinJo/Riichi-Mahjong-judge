#include "mahjong.h"

vector<string> Player::retOwnTiles(bool reCalculate) {
	// reCalculate=trueʱ����ownTiles������
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

void Player::initalize(int p, const vector<string> & hc) {
	pos = p;
	isLiZhi = false;
	numLiZhi = 25;
	isLiuJuManGuan = true;
	memset(isZhenTing, 0, sizeof(isZhenTing));
	handTiles = hc;
}

// person put out c
int Player::checkHuPrerequisite(const string c, int offer) {
	// �ֱ���B�� T�� W�� Z�֣�F�� J����
	sort(this->handTiles.begin(), this->handTiles.end());
	vector<string> hc;
	hc = this->handTiles;
	hc.push_back(c);
	vector<vector<int> > nums;
	nums = string_to_34_tiles(hc, false);

	// TODO: ��仰��д�������߼���ʱ����Ҫע��������Ƚ�Ӱ�����ܡ��ڳ����ܵ�ʱ����Ҫע�����ownTiles���߼�
	this->retOwnTiles(true);

	vector<Tiles> tmp;
	if (anGangTiles.size() == 0 && mingTiles.size() == 0) {
		// check GUO_SHI_WU_SHUANG and QI_DUI_ZI ---- return 2
		for (auto & x : specialYiZhong) {
			FanFu checkRes = x.checker(tmp, *this, c, offer);
			if (checkRes.fan > 0) {
				return x.id + 100; // ��ʾ���ƿ�������������ͨ�����Ƽ�⡣�����߶��ӻ���������Ҫ��֮������жϡ�
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

