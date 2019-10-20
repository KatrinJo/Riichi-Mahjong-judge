#include "mahjong.h"

/*
代码中可能的麻将编码方式有：checkZhenTing
1. F1 表示东风
	B 123456789
	T 123456789
	W 123456789
	Z --- F 1234 J 567
	红宝牌用0特殊表示，如B0表示红五饼
2. 数字 34种牌+3种红宝牌（红宝要特殊标），给每种牌计数
3. 数字 136张牌，暂时不用吧
*/



struct Game
{
	Player players[4];
	int changFeng;
	bool isSiFengLianDa; // 无人鸣牌的状态下4人都在第一巡打出同一种风牌
	bool isSiGangSanLe; // 2人以上合计开杠4次
	bool isJiuZhongJiuPai; // 第一巡轮到自己之前无人鸣牌状态下拥有9种及以上的幺九牌
	bool isSiJiaLiZhi; // 4人都宣告立直成功
	string baoPaiZhiShiTiles[2][5]; // [0][0~5]:表示牌5张,[1][0~5]:里表示牌
	int numOfGang; // 表示开杠的次数，不能超过4次
	vector<string> mountainTiles; // 牌山 一开始有74张，包括4张岭上牌，岭上牌是从0~3
	vector<Event> history;

	Game() {
		isSiFengLianDa = false;
		isSiGangSanLe = false;
		isJiuZhongJiuPai = false;
		isSiJiaLiZhi = false;
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
			
		quan = 0;
		string tmp;
		char c;
		int type_of_tiles, idx;

		for (int j = 0; j < 2; j++) {
			for (int i = 0; i < 5; i++) {
				tmp = mountainTiles.back();
				baoPaiZhiShiTiles[j][i] = tmp;
				mountainTiles.pop_back();
				c = tmp[0];
				type_of_tiles = getTypeOfTile(c);
				idx = tmp[1] - '0';
			}
		}

		for (int i = 0; i < 4; i++) {
			vector<string> hc;
			int countT = 13;
			while (countT--) {
				tmp = mountainTiles.back();
				hc.push_back(tmp);
				mountainTiles.pop_back();
				c = tmp[0];
				type_of_tiles = getTypeOfTile(c);
				idx = tmp[1] - '0';
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
				if (players[pid].checkHuPrerequisite(tmp, pid)) {
					res.push_back(tmp);
				}
			}
		}
		return res;
	}
	



};

Game g;

string FanNames[50] = { "国士无双十三面","四暗刻单骑","纯正九莲宝灯","大四喜","国士无双","四暗刻","九莲宝灯","天和","地和","字一色","绿一色","清老头","大三元","四杠子","小四喜","清一色","二杯口","纯全带幺九","混一色","七对子","双立直","混全带幺九","三色同顺","对对和","三暗刻","三杠子","三色同刻","混老头","小三元","一气通贯","立直","一发","门前清自摸","平和","断幺九","一杯口","役牌白","役牌发","役牌中","岭上开花","抢杠","海底捞月","河底摸鱼","役牌场风","役牌门风","流局满贯","宝牌","红宝牌","里宝牌" };

Json::Value inputValue, outputValue;

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
//8-12:玩家杠牌，通知所有玩家
//13-16:玩家立直，通知所有玩家

int numRestTiles = 70;
bool isdebug = false;

void showDoras(Json::Value & v) {
	string doras = "";
	for (int i = 0; i <= g.numOfGang; i++) {
		doras += g.baoPaiZhiShiTiles[0][i];
		if (i == g.numOfGang)
			break;
		doras += " ";
	}
	v["doras"] = doras;
}

// playerError不用改可以直接用
void playerError(int player, const string code)
{
	outputValue["display"]["action"] = code;
	outputValue["display"]["player"] = player;
	outputValue["command"] = "finish";
	for (int i = 0; i < 4; i++) {
		if (i == player) {
			outputValue["display"]["score"][i] = -33000;
			outputValue["content"][to_string(i)] = -33000;
		}
		else {
			outputValue["display"]["score"][i] = 11000;
			outputValue["content"][to_string(i)] = 11000;
		}
	}
	cout << outputValue;
	exit(0);
}

// checkHu 需要给出最后的结果——多少分数、有哪些番、番数多少、符数多少
// checkHu 里不判断是否振听，只在调用之后考虑。checkHu只在满足能胡条件下，是否有役
vector<FanFu> checkHu(int player, const string c, int offer, bool finish) { // 能进到这个函数的，一定是满足了能胡牌的条件的
	auto & p = g.players[player];

	vector<Tiles> tmp;
	vector<FanFu> vecFanFu, maxScoreFanFu;
	bool isGuoShi = false, isQiDuiZi = false, isYaKuMan = false;

	vector<int> namesFan;
	vector<string> hc = p.handTiles;
	hc.push_back(c);
	vector<vector<int> > nums = string_to_34_tiles(hc, true);
	int numRedFive = 0, numBaoPai = 0, numLiBaoPai = 0; // 计算各种宝牌的数量
	int score = 0, fan = 0, fu = 0, maxScore = 0, maxScoreFan = 0, maxScoreFu = 0;
	auto ownTiles = p.retOwnTiles(true);

	for (int _id = 0; _id < 3; _id++) {
		numRedFive += nums[_id][0];
		nums[_id][5] += nums[_id][0];
		nums[_id][0] = 0;
	}
	
	vector<vector<Tiles>> allCombinations = getAllCombinations(nums);

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

	// TODO: 是国士无双，不需要后续计算符数
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
		maxScore = getScore(maxScoreFan, 0, true);
	}
	else { // 对于每一种拆牌都check一下常见役种
		string baopai[2] = { "234567891",  "2341675" };
		for (int i = 0; i <= g.numOfGang; i++) {
			string t = g.baoPaiZhiShiTiles[0][i];
			t[1] = baopai[t[0] == 'Z'][t[1] - '1'];
			for (auto & x : ownTiles)
				if (isTileSame(t, x))
					numBaoPai++;
			if (g.players[player].isLiZhi) {
				string t = g.baoPaiZhiShiTiles[1][i];
				t[1] = baopai[t[0] == 'Z'][t[1] - '1'];
				for (auto & x : ownTiles)
					if (isTileSame(t, x))
						numLiBaoPai++;
			}
		}
		// 计算符数
		int baseFu = 20;
		
		int tingType = checkTingType(hc, c, 1);
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
			if (isYaoJiu(x[0])) baseFu += 32;
			else baseFu += 16;
		}
		// 计算明刻明杠
		for (auto & x : g.players[player].mingTiles) {
			if (x.size() == 4) {
				if (isYaoJiu(x[0])) baseFu += 16;
				else baseFu += 8;
			}
			else if (isTileSame(x[0], x[1])) {
				if (isYaoJiu(x[0])) baseFu += 4;
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
			score = getScore(fan, fu, false);
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
						if (find(v.begin(), v.end(), ER_BEI_KOU) != v.end())
							tmpFanFu.erase(it);
					}
				}
			}

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
				
			score = getScore(fan, fu, false);

			if (score > maxScore) {
				maxScoreFanFu = tmpFanFu;
				maxScore = score;
				maxScoreFan = fan;
				maxScoreFu = fu;
			}
		}
		// allScores.push_back(1);

		if (maxScoreFan > 13)
			maxScoreFan = 13;
	}

	// TODO: 处理一下一炮多响的情况
	if (finish) {
		if (maxScoreFanFu.empty()) {
			playerError(player, "WA");
			return {};
		}
		outputValue["display"][to_string(player)]["isYaKuMan"] = isYaKuMan;
		outputValue["display"][to_string(player)]["action"] = "HU";
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
void checkInputPASS(Json::Value &playerOutput, int player, bool isPLAY)
{
	if (playerOutput["verdict"] != "OK") {
		if (!isdebug) {
			playerOutput["verdict"] = "OK";
			playerOutput["response"] = "PASS";
		}
		else playerError(player, playerOutput["verdict"].asString());
	}
	if (playerOutput["response"] != "PASS") {
		if (!isdebug) playerOutput["response"] = "PASS";
		else (player, "WA");
	}
	if (isPLAY && lastTile.size() == 2 && roundStage >= 4) {
		// 需要多判断一下振听情况
		auto & p = g.players[player];
		if (p.checkHuPrerequisite(lastTile, roundStage % 4)) {
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
	if (playerOutput["verdict"] != "OK") { // 在摸牌时若有不对，就摸到什么牌打出什么牌
		if (!isdebug) playerOutput["response"] = "PLAY " + lastTile;
		else playerError(player, playerOutput["verdict"].asString());
	}
	vector<string> outputList;
	string outputString = playerOutput["response"].asString();
	auto & p = g.players[player];
	p.isZhenTing[1] = false; // 同巡振听解除
	boost::split(outputList, outputString, boost::is_any_of(" "));
	string oldLastTile = lastTile;
	bool hasFault = false;
	
	for (int tryTime = 0; tryTime < 1; tryTime++) {
		if (outputList.size() == 1) {
			if (outputList[0] == "HU") {
				if (!p.checkHuPrerequisite(lastTile, player)) {
					if (!isdebug) {
						hasFault = true; break;
					}
					else playerError(player, "WA");
				}

				// 自己摸牌的时候不会点别人
				// 也不用考虑振听情况，只需要有役即可
				auto vecRes = checkHu(player, lastTile, player, false);
				if (vecRes.empty()) {
					if (!isdebug) {
						hasFault = true; break;
					}
					else playerError(player, "WA");
				}
			}
		}
		else if (outputList.size() == 2) { // oldLastTile
			auto ting_bef = g.get_ting_tiles(player); 
			p.handTiles.push_back(lastTile);
			string befTile = lastTile; // befTile 是之前抽到的牌
			lastTile = outputList[1]; // lastTile 是这次打出去的牌，要么是暗杠的牌，要么是选择进行补杠的牌
			if (outputList[0] == "ANGANG") {
				if (g.numOfGang == 4) {
					if (!isdebug) {
						auto it = find(p.handTiles.begin(), p.handTiles.end(), befTile);
						p.handTiles.erase(it);
						hasFault = true; break;
					}
					else playerError(player, "WA");
				}
				// 做暗杠需要告知是什么牌，因为有可能来一张能暗杠的牌，但是先留着之后再暗杠。
				auto it = p.handTiles.begin();
				int n = 0;
				vector<string> tiles;
				for (; it != p.handTiles.end();) {
					if (isTileSame(*it, lastTile)) {
						tiles.push_back(*it);
						it = p.handTiles.erase(it);
						n++;
					}
					else
						++it;
				}
				if (n != 4) {
					if (!isdebug) {
						for (auto & x : tiles)
							p.handTiles.push_back(x);
						it = find(p.handTiles.begin(), p.handTiles.end(), befTile);
						p.handTiles.erase(it);
						hasFault = true; break;
					}
					else playerError(player, "WA");
				}
				if (p.isLiZhi) { // 立直后，只有不影响听牌的杠才能成立
                    // 立直后，只能对摸牌进行暗杠
                    if (!isTileSame(befTile, lastTile)) {
                        if (!isdebug) {
                            for (auto & x : tiles)
                                p.handTiles.push_back(x);
                            it = find(p.handTiles.begin(), p.handTiles.end(), befTile);
                            p.handTiles.erase(it);
                            hasFault = true; break;
                        }
                        else playerError(player, "WA");
                    }
					auto ting_aft = g.get_ting_tiles(player);
					bool flag = false;
					if (ting_aft.size() != ting_bef.size())
						flag = true;
					else {
						for (int i = 0; i < ting_bef.size(); i++) {
							if (!isTileSame(ting_bef[i], ting_aft[i]))
								flag = true;
						}
					}
					if (flag) {  // 杠会影响结果，所以不能杠，要变ANGANG为PLAY
						for (auto & x : tiles)
							p.handTiles.push_back(x);
						it = find(p.handTiles.begin(), p.handTiles.end(), befTile);
						p.handTiles.erase(it);
						hasFault = true;
					}
				}
			}
			else if (outputList[0] == "BUGANG") { // 确认要补杠什么
				if (g.numOfGang == 4) {
					if (!isdebug) {
						auto it = find(p.handTiles.begin(), p.handTiles.end(), befTile);
						p.handTiles.erase(it);
						hasFault = true; break;
					}
					else playerError(player, "WA");
				}
				auto it = p.handTiles.begin();
				for (; it != p.handTiles.end(); it++) {
					if (isTileSame(*it, lastTile))
						break;
				}
				if (it == p.handTiles.end()) {
					if (!isdebug) {
						it = find(p.handTiles.begin(), p.handTiles.end(), befTile);
						p.handTiles.erase(it);
						hasFault = true; break;
					}
					else playerError(player, "WA");
				}
				bool flag = true;
				for (unsigned int i = 0; i < p.mingTiles.size(); i++) {
					auto &x = p.mingTiles[i];
					if (x.size() == 3 && x[0] == x[1] && x[1] == lastTile)
						flag = false;
				}
				if (flag) {
					if (!isdebug) {
						it = find(p.handTiles.begin(), p.handTiles.end(), befTile);
						p.handTiles.erase(it);
						hasFault = true; break;
					}
					else playerError(player, "WA");
				}
			}
			else if (outputList[0] == "LIZHI") {
				if (p.mingTiles.size()) {
					if (!isdebug) {
						auto it = find(p.handTiles.begin(), p.handTiles.end(), befTile);
						p.handTiles.erase(it);
						hasFault = true; break;
					}
					else playerError(player, "WA");
				}

				vector<string>::iterator curr = find(
					p.handTiles.begin(),
					p.handTiles.end(),
					lastTile);
				if (curr != p.handTiles.end()) {
					p.handTiles.erase(curr);
					auto tingTiles = g.get_ting_tiles(player); // 判断是否能立直，同时要计算出听什么
					if (tingTiles.empty()) {
						p.handTiles.push_back(lastTile);
						auto it = find(p.handTiles.begin(), p.handTiles.end(), befTile);
						p.handTiles.erase(it);
						if (!isdebug) {
							hasFault = true; break;
						}
						else playerError(player, "WA");
					}
				}
			}
		}

	}

	if (hasFault) {
		lastTile = oldLastTile;
		playerOutput["response"] = "PLAY " + lastTile;
		outputString = "PLAY " + lastTile;
		boost::split(outputList, outputString, boost::is_any_of(" "));
	}

	if (outputList.size() == 1) {
		if (outputList[0] == "HU") {
			if (!p.checkHuPrerequisite(lastTile, player))
				playerError(player, "WA");
			// 自己摸牌的时候不会点别人
			// 也不用考虑振听情况，只需要有役即可
			auto vecRes = checkHu(player, lastTile, player, true);
			if (vecRes.empty()) playerError(player, "WA");
			int maxScore = outputValue["display"][to_string(player)]["ScoreCnt"].asInt();
			// if (p.pos == p.changFeng) maxScore = ceil(1.5 * maxScore);
			maxScore = ceil(1.0 * maxScore / 300) * 300;
			outputValue["display"][to_string(player)]["ScoreCnt"] = maxScore;
			for (int i = 0; i < 4; i++) {
				if (i == player) {
					outputValue["display"]["score"][i] = 25000 + maxScore;
					outputValue["content"][to_string(i)] = 25000 + maxScore;
				}
				else {
					outputValue["display"]["score"][i] = 25000 - maxScore / 3;
					outputValue["content"][to_string(i)] = 25000 - maxScore /3;
				}
			}
			cout << outputValue;
			exit(0);
		}
	}
	else if (outputList.size() == 2) {
		p.handTiles.push_back(lastTile);
		string befTile = lastTile; // befTile 是之前抽到的牌
		lastTile = outputList[1]; // lastTile 是这次打出去的牌，要么是暗杠的牌，要么是选择进行补杠的牌
		if (outputList[0] == "PLAY") {
			if (currBUGANG || currGANG)
				g.numOfGang += currBUGANG + currGANG;
			if (p.isLiZhi) { // 立直了就不管看他到底是想出什么牌，随便。但是摸到就打
				lastTile = befTile;
				p.isYiFa = false;
			}
			vector<string>::iterator curr = find(p.handTiles.begin(), p.handTiles.end(), lastTile);
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
			auto ting_bef = g.get_ting_tiles(player);
			vector<string> tiles;
			for (; it != p.handTiles.end();) {
				if (isTileSame(*it, lastTile)) {
					tiles.push_back(*it);
					it = p.handTiles.erase(it);
					n++;
				}
				else
					++it;
			}
			if (n != 4)
				playerError(player, "WA");
			if (p.isLiZhi) { // 立直后，只有不影响听牌的杠才能成立
				auto ting_aft = g.get_ting_tiles(player);
				bool flag = false;
				if (ting_aft.size() != ting_bef.size())
					flag = true;
				else {
					for (int i = 0; i < ting_bef.size(); i++) {
						if (!isTileSame(ting_bef[i], ting_aft[i]))
							flag = true;
					}
				}
				if (flag) { // 杠会影响结果，所以不能杠，要变ANGANG为PLAY
					playerOutput["response"] = "PLAY " + lastTile;
					lastOp = "PLAY";
					roundStage = player + 4;
					p.outTiles.push_back(lastTile);
					p.isYiFa = false;
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
		else if (outputList[0] == "BUGANG") { // 确认要补杠什么
			currBUGANG = true;
			// g.numOfGang++;
			tileGANG = lastTile;

			auto it = p.handTiles.begin();
			for (; it != p.handTiles.end(); it++) {
				if (isTileSame(*it, lastTile))
					break;
			}
			if (it == p.handTiles.end())
				playerError(player, "WA");
			for (unsigned int i = 0; i < p.mingTiles.size(); i++) {
				auto &x = p.mingTiles[i];
				if (x.size() == 3 && x[0] == x[1] && x[1] == lastTile) {
					roundStage = player + 8;
					p.mingTiles[i].push_back(lastTile);
					p.handTiles.erase(it);
					return;
				}
			}
			playerError(player, "WA");
		}
		// TODO: 加一个立直
		else if (outputList[0] == "LIZHI") {
			if (p.mingTiles.size()) {
				playerError(player, "WA");
			}
			
			vector<string>::iterator curr = find(
				p.handTiles.begin(),
				p.handTiles.end(),
				lastTile);
			if (curr != p.handTiles.end()) {
				p.handTiles.erase(curr);
				p.isLiZhi = p.isYiFa = true;
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
						if (isTileSame(x, y)) {
							p.isZhenTing[0] = p.isZhenTing[2] = true;
							flag = false;
							break;
						}
				if (flag)
					p.isZhenTing[0] = false;
				p.numLiZhi = p.outTiles.size(); // TODO: check
				return;
			}
		}
	}
	return;
}


//检查其他玩家打出牌后，玩家的输出。所以没有暗杠、补杠请求
//1检查和牌操作
void checkInputPLAY1(Json::Value &playerOutput, int player)
{
	if (playerOutput["verdict"] != "OK") {
		if (!isdebug) {
			playerOutput["verdict"] = "OK";
			playerOutput["response"] = "PASS";
		}
		else playerError(player, playerOutput["verdict"].asString());
	}
	string outputString = playerOutput["response"].asString();
	auto & p = g.players[player];

	bool hasFault = false;

	for (int tryTime = 0; tryTime < 1; tryTime++) {
		if (outputString == "HU") {
			// TODO：这里需要处理的是checkHu里面要不要有处理振听情况。要理清楚HU与振听的关系
			auto vecRes = checkHu(player, lastTile, roundStage % 4, false);
			if (vecRes.empty()) {
				if (!isdebug) {
					hasFault = true; break;
				}
				else
					playerError(player, "WA");
			}
		}
	}

	if (hasFault) {
		outputValue["command"] = "request";
		playerOutput["response"] = "PASS";
	}

	outputString = playerOutput["response"].asString();
	if (outputString == "HU") {
		// TODO：这里需要处理的是checkHu里面要不要有处理振听情况。要理清楚HU与振听的关系
		auto vecRes = checkHu(player, lastTile, roundStage % 4, true);
		if (vecRes.empty()) playerError(player, "WA");
		// 在所有checkHu的地方都要check是否有振听的情况
		bool isSheZhangZhenTing = false; // 先更新一下舍张振听的情况
		for (auto & x : p.outTiles)
			if (p.checkHuPrerequisite(x, player))
				isSheZhangZhenTing = true;
		if (isSheZhangZhenTing) // 舍张振听只有自摸且有役才能解
			p.isZhenTing[0] = true;
		else
			p.isZhenTing[0] = false;
		if (p.isZhenTing[0] || p.isZhenTing[1] || p.isZhenTing[2]) {
			outputValue["command"] = "request";
			playerOutput["response"] = "PASS";
		}
	}
	// 处理振听
	if (outputString == "PASS") {
		if (p.checkHuPrerequisite(lastTile, roundStage % 4)) {
			if (p.isLiZhi)
				p.isZhenTing[2] = true; // 立直振听
			else
				p.isZhenTing[1] = true; // 同巡振听
		}
	}
}
//2检查碰牌、杠牌
bool checkInputPLAY2(Json::Value &playerOutput, int player)
{
	vector<string> outputList;
	string outputString = playerOutput["response"].asString();
	auto & p = g.players[player];
	boost::split(outputList, outputString, boost::is_any_of(" "));

	bool hasFault = false;
	for (int tryTime = 0; tryTime < 1; tryTime++) {
		if (outputList.size() == 1) {
			if (outputList[0] == "PASS") {
				return false;
			}
			else if (outputList[0] == "GANG" && g.numOfGang < 4) {
				auto it = p.handTiles.begin();
				int n = 0;
				for (; it != p.handTiles.end(); it++)
					if (isTileSame(*it, lastTile))
						n++;
				if (n == 3)
					break;
			}
			if (!isdebug) {
				hasFault = true;
			}
			else
				playerError(player, "WA");
		}
		else if (outputList.size() == 4) {
			if (outputList[0] == "PENG") {
				// outputList里，第二个和第三个是要碰掉的牌，第四个string是打出的牌
				if (!isTileSame(lastTile, outputList[1]) || !isTileSame(lastTile, outputList[2])) {
					if (!isdebug) {
						hasFault = true; break;
					}
					else
						playerError(player, "WA");
				}
				auto it = find(p.handTiles.begin(), p.handTiles.end(), outputList[1]);
				if (it == p.handTiles.end()) {
					if (!isdebug) {
						hasFault = true; break;
					}
					else
						playerError(player, "WA");
				}
				p.handTiles.erase(it);
				it = find(p.handTiles.begin(), p.handTiles.end(), outputList[2]);
				if (it == p.handTiles.end()) {
					p.handTiles.push_back(outputList[1]);
					if (it == p.handTiles.end()) {
						if (!isdebug) {
							hasFault = true; break;
						}
						else
							playerError(player, "WA");
					}
				}
				p.handTiles.erase(it);

				// 不能碰掉一样的牌，然后又打出去一张。打出去的牌也必须是自己有的
				if (isTileSame(lastTile, outputList[3]) || 
					find(p.handTiles.begin(), p.handTiles.end(), outputList[3]) == p.handTiles.end()) { 
					p.handTiles.push_back(outputList[1]);
					p.handTiles.push_back(outputList[2]);
					if (it == p.handTiles.end()) {
						if (!isdebug) {
							hasFault = true; break;
						}
						else
							playerError(player, "WA");
					}
				}
			}
			if (!isdebug) {
				hasFault = true; break;
			}
			else
				playerError(player, "WA");
		}
	}

	if (hasFault) {
		playerOutput["response"] = "PASS";
	}
	outputString = playerOutput["response"].asString();
	boost::split(outputList, outputString, boost::is_any_of(" "));

	if (outputList.size() == 1) {
		if (outputList[0] == "PASS") {
			return false;
		}
		else if (outputList[0] == "GANG") {
			isMingPai = true;
			auto it = p.handTiles.begin();
			int n = 0;
			vector<string> tiles{ lastTile };
			for (; it != p.handTiles.end();) {
				if (isTileSame(*it, lastTile)) {
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
	else if (outputList.size() == 4) {
		if (outputList[0] == "PENG") {
			isMingPai = true;
			for (int i = 0; i < 4; i++)
				if (g.players[i].isLiZhi)
					g.players[i].isYiFa = false;
			// outputList里，第二个和第三个是要碰掉的牌，第四个string是打出的牌
			vector<string> tiles{ lastTile };
			if (!isTileSame(lastTile, outputList[1]) || !isTileSame(lastTile, outputList[2]))
				playerError(player, "WA");
			auto it = find(p.handTiles.begin(), p.handTiles.end(), outputList[1]);
			if (it == p.handTiles.end())
				playerError(player, "WA");
			p.handTiles.erase(it);
			tiles.push_back(outputList[1]);
			it = find(p.handTiles.begin(), p.handTiles.end(), outputList[2]);
			if (it == p.handTiles.end())
				playerError(player, "WA");
			p.handTiles.erase(it);
			tiles.push_back(outputList[2]);
			
			p.mingTiles.push_back(tiles);
			p.mingTilesOffer.push_back(make_pair(roundStage % 4, lastTile));
			lastOp = "PENG";
			roundStage = player + 8;

			if (isTileSame(lastTile, outputList[3])) // 不能碰掉一样的牌，然后又打出去一张
				playerError(player, "WA");
			tilePENG = lastTile + " " + outputList[1] + " " + outputList[2];

			lastTile = outputList[3];
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
						if (isTileSame(x, y)) {
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
		playerError(player, "WA");
	}
	return false;
}
//3检查吃牌
bool checkInputPLAY3(Json::Value &playerOutput, int player)
{
	vector<string> outputList;
	string outputString = playerOutput["response"].asString();
	auto & p = g.players[player];
	boost::split(outputList, outputString, boost::is_any_of(" "));

	bool hasFault = false;
	for (int tryTime = 0; tryTime < 1; tryTime++) {
		if (outputList.size() == 5) {
			// 前3个分别为吃牌组成，其中第一张牌是从上家那里吃到的（其实就是lastTile），第4个为打出的牌
			if (outputList[0] != "CHI" || (roundStage - player) % 4 != 3 || 
				(lastTile[0] != 'W' && lastTile[0] != 'B' && lastTile[0] != 'T') ||
				(outputList[2][0] != lastTile[0]) || (outputList[3][0] != lastTile[0]) || 
				(outputList[3] == lastTile) || (outputList[2] == lastTile) || (outputList[3] == outputList[2])) {
				if (!isdebug) {
					hasFault = true; break;
				}
				else
					playerError(player, "WA");
			}

			vector<string> tiles{ lastTile, outputList[2], outputList[3] };
			for (auto & x : tiles)
				if (x[1] == '0')
					x[1] = '5';
			sort(tiles.begin(), tiles.end());

			auto it1 = find(p.handTiles.begin(), p.handTiles.end(), outputList[2]),
				it2 = find(p.handTiles.begin(), p.handTiles.end(), outputList[3]);
			if (it1 == p.handTiles.end() || it2 == p.handTiles.end() || 
				tiles[2][1] - tiles[1][1] != 1 || tiles[1][1] - tiles[0][1] != 1) {
				if (!isdebug) {
					hasFault = true; break;
				}
				else
					playerError(player, "WA");
			}
			p.handTiles.erase(it1); p.handTiles.erase(it2);
			
			vector<string> cannotOut{ lastTile };
			string left = tiles[0], right = tiles[2];
			if (isTileSame(left, lastTile) && right[1] < '9') {
				right[1]++;
				cannotOut.push_back(right);
				right[1]--;
			}
			if (isTileSame(right, lastTile) && left[1] > '1') {
				left[1]--;
				cannotOut.push_back(left);
				left[1]++;
			}
			for (auto & x : cannotOut)
				if (isTileSame(x, outputList[4])) {  // 不能吃掉一张，然后再打出相同的一张。或者可替代的。
					if (!isdebug) {
						p.handTiles.push_back(outputList[2]); p.handTiles.push_back(outputList[3]);
						hasFault = true; break;
					}
					else
						playerError(player, "WA");
				}
			lastTile = outputList[4];
			auto it = find(p.handTiles.begin(), p.handTiles.end(), lastTile);
			if (it == p.handTiles.end()) {
				if (!isdebug) {
					p.handTiles.push_back(outputList[2]); p.handTiles.push_back(outputList[3]);
					hasFault = true; break;
				}
				else
					playerError(player, "WA");
			}
		}
	}
	if (hasFault) {
		playerOutput["response"] = "PASS";
		boost::split(outputList, outputString, boost::is_any_of(" "));
	}
	if (outputList.size() == 5) {
		// 前3个分别为吃牌组成，其中第一张牌是从上家那里吃到的（其实就是lastTile），第4个为打出的牌
		if (outputList[0] != "CHI" || (roundStage - player) % 4 != 3) {
			playerError(player, "WA");
		}
		isMingPai = true;
		for (int i = 0; i < 4; i++)
			if (g.players[i].isLiZhi)
				g.players[i].isYiFa = false;
		if ((lastTile[0] != 'W' && lastTile[0] != 'B' && lastTile[0] != 'T') || 
			(outputList[2][0] != lastTile[0]) || (outputList[3][0] != lastTile[0]))
			playerError(player, "WA");
		vector<string> tiles{ lastTile };
		string card1 = outputList[2], card2 = outputList[3];
		auto it = find(p.handTiles.begin(), p.handTiles.end(), card1);
		if (it == p.handTiles.end())
			playerError(player, "WA");
		p.handTiles.erase(it);
		tiles.push_back(card1);
		it = find(p.handTiles.begin(), p.handTiles.end(), card2);
		if (it == p.handTiles.end())
			playerError(player, "WA");
		p.handTiles.erase(it);
		tiles.push_back(card2);
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
		if (isTileSame(left, lastTile) && right[1] < '9') {
			right[1]++;
			cannotOut.push_back(right);
			right[1]--;
		}
		if (isTileSame(right, lastTile) && left[1] > '1') {
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
			if (isTileSame(x, lastTile))
				continue;
			tileCHI += " " + x;
		}
		for (auto & x : cannotOut)
			if (isTileSame(x, outputList[4]))
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
					if (isTileSame(x, y)) {
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
void checkInputGANG(Json::Value &playerOutput, int player)
{
	if (playerOutput["verdict"] != "OK") {
		if (!isdebug) {
			playerOutput["verdict"] = "OK";
			playerOutput["response"] = "PASS";
		}
		else playerError(player, playerOutput["verdict"].asString());
	}

	bool hasFault = false;

	for (int tryTime = 0; tryTime < 1; tryTime++) {
		if (lastBUGANG && roundStage % 4 != player && playerOutput["response"] == "HU") {
			if (!g.players[player].checkHuPrerequisite(lastTile, player)) {
				if (!isdebug) {
					hasFault = true; break;
				}
				else
					playerError(player, "WA");
			}
			auto vecRes = checkHu(player, lastTile, roundStage % 4, false);
			if (vecRes.empty()) {
				if (!isdebug) {
					hasFault = true; break;
				}
				else
					playerError(player, "WA");
			}
		}
		// 特殊判断国士无双可以抢暗杠
		if (lastANGANG && roundStage % 4 != player && playerOutput["response"] == "HU") {
			if (isYaoJiu(lastTile)) {
				int res = g.players[player].checkHuPrerequisite(lastTile, player) - 100;
				if (res == GUO_SHI_WU_SHUANG || res == GUO_SHI_WU_SHUANG_SHI_SAN_MIAN) {
					auto vecRes = checkHu(player, lastTile, roundStage % 4, false);
					if (vecRes.empty()) {
						if (!isdebug) {
							hasFault = true; break;
						}
						else
							playerError(player, "WA");
					}
				}
			}
		}
	}
	if (hasFault) {
		playerOutput["response"] = "PASS";
	}
	if (playerOutput["response"] == "PASS") {
		checkInputPASS(playerOutput, player, true); // TODO：check一下是否有判断振听的情况
		return;
	}
	if (lastBUGANG && roundStage % 4 != player && playerOutput["response"] == "HU") {
		if (!g.players[player].checkHuPrerequisite(lastTile, player)) {
			playerError(player, "WA");
		}
			
		auto vecRes = checkHu(player, lastTile, roundStage % 4, true);
		if (vecRes.empty()) playerError(player, "WA");
	}
	// 特殊判断国士无双可以抢暗杠
	if (lastANGANG && roundStage % 4 != player && playerOutput["response"] == "HU") {
		if (isYaoJiu(lastTile)) {
			int res = g.players[player].checkHuPrerequisite(lastTile, player) - 100;
			if (res == GUO_SHI_WU_SHUANG || res == GUO_SHI_WU_SHUANG_SHI_SAN_MIAN) {
				auto vecRes = checkHu(player, lastTile, roundStage % 4, true);
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
			showDoras(info);
			outputValue["content"][to_string(i)].append(info);
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
					if (!isYaoJiu(x)) {
						liuman[i] = false;
						break;
					}
				}
			}

			for (int i = 0; i < 4; i++)
				cntLiuman += liuman[i];
			if (cntLiuman) {
				int s[4] = { 0 };
				if (cntLiuman == 1) {
					for (int i = 0; i < 4; i++) {
						if (liuman[i]) s[i] = 8100;
						else s[i] = -2700;
					}
				}
				else if (cntLiuman == 2) {
					for (int i = 0; i < 4; i++) {
						if (liuman[i]) s[i] = 8000;
						else s[i] = -8000;
					}
				}
				else if (cntLiuman == 3) {
					for (int i = 0; i < 4; i++) {
						if (liuman[i]) s[i] = 8000;
						else s[i] = -24000;
					}
				}

				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][i] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
				}
			}
			else {
				bool isTing[4] = { false };
				int s[4] = { 0 }, cntTing = 0;
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
							s[i] = 3000 / cntTing;
						else
							s[i] = -3000 / cntTing;
					}
				}
				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][i] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
				}
			}
			
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
		for (int i = 0; i < 4; i++) {
			Json::Value info;
			showDoras(info);
			if (roundStage % 4 == i) {
				info["action"] = "2 " + lastTile;
				// outputValue["content"][to_string(i)] = "2 " + lastTile;
			}
			else {
				info["action"] = "3 " + to_string(roundStage % 4) + " DRAW";
				// outputValue["content"][to_string(i)] = "3 " + to_string(roundStage % 4) + " DRAW";
			}
			outputValue["content"][to_string(i)].append(info);
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
		else if (lastOp == "PENG") { // TODO:
			outputValue["display"]["tilePENG"] = tilePENG;
		}
		else if (lastOp == "GANG" || lastOp == "ANGANG" || lastOp == "BUGANG") { }

		for (int i = 0; i < 4; i++) {
			Json::Value info;
			showDoras(info);
			if (lastOp == "CHI") { // CHI 后面会跟四张牌，前三张是组成顺子的，其中第一张是吃的牌。第四张是自己打出的牌。
				info["action"] = "3 " + to_string(roundStage % 4) + " CHI " + tileCHI + " " + lastTile;
				// outputValue["content"][to_string(i)] = "3 " + to_string(roundStage % 4) + " CHI " + tileCHI + " " + lastTile;
			}
			else if (lastOp == "PENG") { // PENG 后面会跟三张牌，前两张和lastTile组成刻子，第三张是自己打出的牌。
				info["action"] = "3 " + to_string(roundStage % 4) + " PENG " + tilePENG + " " + lastTile;
			}
			else {
				info["action"] = "3 " + to_string(roundStage % 4) + " " + lastOp + " " + lastTile;
				// outputValue["content"][to_string(i)] = "3 " + to_string(roundStage % 4) + " " + lastOp + " " + lastTile;
			}
			outputValue["content"][to_string(i)].append(info);

			if (g.players[i].checkHuPrerequisite(lastTile, roundStage % 4)) {
				auto vecRes = checkHu(i, lastTile, roundStage % 4, false);
				if (vecRes.empty())
					outputValue["display"]["canHu"][i] = 0;
				else
					outputValue["display"]["canHu"][i] = vecRes[0].score;
			}
		}
	}
	else if (roundStage >= 8 && roundStage < 12) {
		string cOp = "GANG " + lastTile;
		if (lastANGANG) {
			cOp = "ANGANG " + lastTile;
			if (isYaoJiu(lastTile)) {
				for (int i = 0; i < 4; i++) {
					if (roundStage % 4 != i) {
						if (g.players[i].checkHuPrerequisite(lastTile, roundStage % 4)) {
							auto vecRes = checkHu(i, lastTile, roundStage % 4, false);
							if (vecRes.empty()) outputValue["display"]["canHu"][i] = 0;
							else outputValue["display"]["canHu"][i] = vecRes[0].score;
						}
					}
				}
			}
			outputValue["display"]["action"] = "ANGANG";
		}
		else {
			if (lastBUGANG) {
				cOp = "BUGANG " + lastTile;
				for (int i = 0; i < 4; i++) {
					if (roundStage % 4 != i) {
						if (g.players[i].checkHuPrerequisite(lastTile, i)) {
							auto vecRes = checkHu(i, lastTile, roundStage % 4, false);
							if (vecRes.empty()) outputValue["display"]["canHu"][i] = 0;
							else {
								outputValue["display"]["canHu"][i] = vecRes[0].score;
							}
						}
					}
				}
				outputValue["display"]["action"] = "BUGANG";
			}
			else
				outputValue["display"]["action"] = "GANG";
		}
		// outputValue["display"]["action"] = lastBUGANG ? "BUGANG" : "GANG";
		outputValue["display"]["player"] = roundStage % 4;
		outputValue["display"]["tile"] = lastTile;
		for (int i = 0; i < 4; i++) {
			outputValue["content"][to_string(i)] = "3 " + to_string(roundStage % 4) + " " + cOp;
		}
	}
	else { 
		// 立直情况
		outputValue["display"]["action"] = lastOp;
		outputValue["display"]["player"] = roundStage % 4;
		outputValue["display"]["tile"] = lastTile;
		for (int i = 0; i < 4; i++) {
			outputValue["content"][to_string(i)] = "3 " + to_string(roundStage % 4) + " " + lastOp + " " + lastTile;
			if (roundStage % 4 != i) {
				if (g.players[i].checkHuPrerequisite(lastTile, roundStage % 4)) {
					auto vecRes = checkHu(i, lastTile, roundStage % 4, false);
					if (vecRes.empty()) outputValue["display"]["canHu"][i] = 0;
					else {
						outputValue["display"]["canHu"][i] = vecRes[0].score;
					}
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

		if (currGANG || currBUGANG || currANGANG) {
			cout << inputValue << endl;
			cout << roundStage % 4 << endl;
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
				outputValue["display"]["action"] = "四杠散了";
				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][i] = 25000;
					outputValue["content"][to_string(i)] = 25000;
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
		// TODO: 需要确认一下
		if (outputValue["command"].isString()) {
			string temp = outputValue["command"].asString();
			if (temp == "finish") {
				int s[4] = { 0 };
				for (int i = 0; i < 4; i++)
					if (outputValue["display"][to_string(i)].isObject())
						s[i] = outputValue["display"][to_string(i)]["ScoreCnt"].asInt();
				if (s[roundStage % 4]) exit(1); // 不知道会不会出现这种奇怪的情况 roundStage % 4 放炮的
				s[roundStage % 4] = -s[0] - s[1] - s[2] - s[3];
				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][i] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
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
		if (g.numOfGang == 4)
			playerError(roundStage % 4, "WA");

		for (int i = 0; i < 4; i++) {
			checkInputGANG(inputValue[to_string(i)], i);
		}
		// TODO: 需要确认一下
		if (outputValue["command"].isString()) {
			string temp = outputValue["command"].asCString();
			if (temp == "finish") {
				int s[4] = { 0 };
				for (int i = 0; i < 4; i++)
					if (outputValue["display"][to_string(i)].isObject())
						s[i] = outputValue["display"][to_string(i)]["ScoreCnt"].asInt();
				if (s[roundStage % 4]) exit(1); // 不知道会不会出现这种奇怪的情况 roundStage % 4 放炮的
				s[roundStage % 4] = -s[0] - s[1] - s[2] - s[3];
				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][i] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
				}
				cout << outputValue;
				exit(0);
			}
		}
		roundStage -= 8;
	}
	else if (roundStage >= 12 && roundStage < 16) {
		// 立直情况类似直接出牌，但是需要另外做标记
		g.players[roundStage % 4].isLiZhi = true;
		g.players[roundStage % 4].numLiZhi = g.players[roundStage % 4].outTiles.size();
		
		for (int i = 0; i < 4; i++) {
			if (roundStage == i + 12) {
				checkInputPASS(inputValue[to_string(i)], i, true); // 是自己打出的牌，看看是不是ok
			}
			else {
				checkInputPLAY1(inputValue[to_string(i)], i); // 自己打的牌有没有点炮
			}
		}
		// TODO: 需要确认一下
		if (outputValue["command"].isString()) {
			string temp = outputValue["command"].asCString();
			if (temp == "finish") {
				int s[4] = { 0 };
				for (int i = 0; i < 4; i++)
					if (outputValue["display"][to_string(i)].isObject())
						s[i] = outputValue["display"][to_string(i)]["ScoreCnt"].asInt();
				if (s[roundStage % 4]) exit(1); // 不知道会不会出现这种奇怪的情况 roundStage % 4 放炮的
				s[roundStage % 4] = -s[0] - s[1] - s[2] - s[3];
				for (int i = 0; i < 4; i++) {
					outputValue["display"]["score"][roundStage % 4] = 25000 + s[i];
					outputValue["content"][to_string(i)] = 25000 + s[i];
				}
				cout << outputValue;
				exit(0);
			}
		}
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
		playerError(roundStage % 4, "roundStage is too big");
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

	freopen("in.json", "r", stdin);
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
			}
			str.pop_back();
		}
		
		if (inputValue["initdata"]["quan"].isUInt())
			quan = inputValue["initdata"]["quan"].asUInt();
		else
			quan = 0; // 默认为速东局
		
		if (inputValue["initdata"]["rules"]["killIfError"].isBool()) {
			isdebug = inputValue["initdata"]["rules"]["killIfError"].asBool();
		}
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
			outputValue["content"][to_string(i)] = "0 " + to_string(i) + " " + to_string(quan);
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
	cout << inputValue["log"][1] << endl;
	for (unsigned int i = 2; i < inputValue["log"].size(); i += 2) {
		Json::Value tmp = outputValue;
		outputValue["display"].clear();
		for (int i = 0; i < 4; i++) {
			outputValue["display"]["canHu"][i] = -4;
		}
		roundOutput(tmp);

		if ((inputValue["log"].size() - i) < 4) {
			for (int id = 0; id < 4; id++) {
				auto & p = g.players[id];
				cout << id << " : ";
				for (auto & x : p.handTiles)
					cout << x << " ";
				cout << ".    Own :";
				for (auto & x : p.ownTiles)
					cout << " " << x;
				cout << endl;
			}
			cout << endl;
		}
		if (i >= inputValue["log"].size() - 2) {
			cout << "here!" << endl;
			cout << inputValue["log"][i + 1] << endl;
		}
			
		roundInput(inputValue["log"][i + 1]);
	}
	outputValue["display"].clear();
	for (int i = 0; i < 4; i++) {
		outputValue["display"]["canHu"][i] = -4;
	}
	
	roundOutput(outputValue);
	outputValue["display"]["tileCnt"] = g.mountainTiles.size() - 4;
	cout << randSeed; // 先确认一下随机种子
	cout << outputValue;

	for (int id = 0; id < 4; id++) {
		auto & p = g.players[id];
		cout << id << " : ";
		for (auto & x : p.handTiles)
			cout << x << " ";
		cout << ".    Own :";
		for (auto & x : p.ownTiles)
			cout << " " << x;
		cout << endl;
	}
	cout << endl;
	return 0;
}
