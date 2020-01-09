#!/usr/bin/env python3

import sys
import json
import fileinput

from enum import Enum
from collections import namedtuple
from typing import List

Tile = namedtuple("Tile", ["type", "num", "chibao"])

def is_same_tile(t1: Tile, t2: Tile) -> bool:
    return t1.type == t2.type and t1.num == t2.num

def parse_tile(tile_str: str):
    if isinstance(tile_str, Tile):
        return tile_str

    t, n = tile_str
    if n == '0':
        return Tile(t, 5, True)
    else:
        return Tile(t, int(n), False)

class HandTiles:
    def __init__(self, tiles: List[str]):
        self._tiles = list(map(parse_tile, tiles))

    def play_tile(self, tile:Tile):
        self._tiles.remove(tile)

    def draw_tile(self, tile:Tile):
        self._tiles.append(tile)

    def __repr__(self):
        return ",".join([ "{}{}".format(t.type, t.num) for t in self._tiles ])
    def sort(self):
        self._tiles.sort()
# ActionType = Enum("PLAY", "CHI", "GANG", "PENG")

class Action:
    def __init__(self, t: Tile):
        self._tile = t
        self._mianzi = None
        # self._mianzi = tiles
    # def parse_req(self, info: List[str]):
        # raise NotImplementedError("parse need to be implemented")
    def played(self):
        return self._tile

    def to_show(self):
        raise NotImplementedError("parse need to be implemented")

class PlayAction(Action):
    def __init__(self, t: List[str]):
        super().__init__(parse_tile(t[0]))

#
class DrawAction(Action):
    def __init__(self, t: List[str]):
        # 这个摸牌看不到
        if t[0] is None:
            super().__init__(t[0])
        else:
            super().__init__(parse_tile(t[0]))


class ChiAction(Action):
    def __init__(self, t: List[str]):
        super().__init__(parse_tile(t[-1]))
        self._mianzi = parse_mianzi(t[:-1])

class PengAction(Action):
    def __init__(self, t: List[str]):
        super().__init__(parse_tile(t[-1]))
        self._mianzi = parse_mianzi(t[:-1])

class GangAction(Action): # 暂时先不要管暗杠之类的了
    def __init__(self, t: List[str]):
        super().__init__(parse_tile(t[-1]))
        self._mianzi = parse_mianzi(t[:-1])

class RiichiAction(Action):
    def __init__(self ):
        super.__init__(None)

def parse_action(raw_action : List[str]) -> Action:
    name, tiles_str = raw_action[0], raw_action[1:]
    if name == "DRAW":
        return DrawAction([None])
    elif name == "PLAY":
        return PlayAction(tiles_str)
    elif name == "CHI":
        return ChiAction(tiles_str)
    elif name == "PENG":
        return PengAction(tiles_str)
    elif name == "GANG":
        return GangAction(tiles_str)
    elif name == "RIICHI":
        return RiichiAction()

class Mianzi:
    def __init__(self, tiles):
        self._tiles = tiles

    def get_tiles(self):
        return self._tiles


class Shunzi(Mianzi):
    def __init__(self, tiles: List[Tile]):
        super(Shunzi ,self).__init__(tiles)
        tmin = tiles[0]
        if (tmin.type == "Z"):
            raise ValueError("字牌 {} 不能组成顺子".format(tmin))

        # self._tmin = tmin
    # def get_tiles(self):
        # t,n,_ = self._tmin
        # return [Tile(t, n+i, False) for i in range(3)]

class Kezi(Mianzi):
    def __init__(self, tiles: List[Tile]):
        # self._tile = t
        super().__init__(tiles)

    # def get_tiles(self):
        # return [self._tile] * 3

class Gangzi(Mianzi):
    def __init__(self, tiles: List[Tile]):
        super().__init__(tiles)

    # def get_tiles(self):
        # return [self._tile] * 4

def parse_mianzi(tiles: List[str]) -> Mianzi:
    """Be sure these tiles form a mianzi before
    """
    tiles = sorted([parse_tile(t) for t in tiles])
    tiles_count = len(tiles)
    t1, t2 = tiles[:2]
    if is_same_tile(t1, t2):
        if tiles_count == 3:
            return Kezi(tiles)
        else:
            assert(tiles_count == 4)
            return Gangzi(tiles)
    else:
        return Shunzi(tiles)

class PlayerPublic:
    """A public player contains the info that is shown to all
    the players in the game
    Including the played cards,
    """
    def __init__(self, pos: int):
        self._pos = int(pos)
        self._ming_mian = []  # 放明的面子
        self._played_tiles = []
        self._riichi = False # 是否立直

    def do_action(self, a: Action):
        if isinstance(a, DrawAction):
            # 别的玩家无法观察到摸牌的操作
            pass
        elif isinstance(a,RiichiAction):
            self._riichi = True
        elif isinstance(a, Action):
            # 先打出牌

            if isinstance(a, PlayAction):
                self._played_tiles.append(a.played())
            else:
                self._played_tiles.append(a.played())
                self.show_mianzi(a._mianzi)

    def play_tile(self, t: Tile):
        self._played_tiles.append(t)

    def show_mianzi(self, m: Mianzi):
        self._ming_mian.append(m)

# State MO ，打牌
class Player(PlayerPublic):

    def __init__(self, pos):
        super().__init__(pos)
        self._state = "MO"
        self._players = [None] * 4

        # 初始化玩家
        for i in range(4):
            if i != self._pos:
                self._players[i] = PlayerPublic(i)
            else:
                self._players[i] = self
        self._last_player = None
        self._last_action = None

    def set_tiles(self, tiles):
        self._hand_tiles = HandTiles(tiles)

    def draw_tile(self, tile):
        self._hand_tiles.draw_tile(tile)

    def play_tile(self, tile):
        self._hand_tiles.play_tile(tile)

    def hand_tile_num(self):
        return len(self._hand_tiles._tiles)

    def do_action(self, a: Action):
        self._hand_tiles.sort()
        if isinstance(a, DrawAction):
            self.draw_tile(a.played())
        else:
            self.play_tile(a.played())
            # 把要亮明的面子从手牌中拿出去
            if a._mianzi is not None:
                # 面子里的第一张需要从上一个玩家的牌堆中拿出来
                self._players[self._last_player]._played_tiles.remove(a._mianzi.get_tiles()[0])

                for _t in a._mianzi.get_tiles()[1:]: # 第一张牌不是手里的
                    self._hand_tiles.play_tile(_t)

            # super(Player, self).do_action(a)
            PlayerPublic.do_action(self, a)
        self._last_player = self._pos
        self._last_action =  Action

    def witness(self, player_idx: int, action: Action):
        p = self._players[player_idx]
        # import pdb; pdb.set_trace()
        # try:
        if isinstance(p, Player):
            print(p._hand_tiles._tiles)
        print(action)
        p.do_action(action)

        self._last_player = player_idx
        self._last_action = action


    def should_play(self):
        """上一个请求是摸牌，所以应该打出一张
        """
        self._last_player == self._pos and isinstance( self._last_action, DrawAction )


    def legal_actions(self, latest_request):
        """latest request is of form

        {
        "doraIndicators": "T8",
        "state": "2 W4",
        "validact": null
        },
        """
        import pdb; pdb.set_trace()
        validact = latest_request["validact"]
        # which move can the agent do
        if self.should_play():
            # 摸牌之后只能打牌
            legal_actions = [ PlayAction([t]) for t in self._hand_tiles._tiles ]
        elif validact is not None:
            validacts = latest_request["validact"].split(",")

            legal_actions = generate_fulu(self._hand_tiles, validacts) # TODO
        else:
            pass


player = None
def process_input(content):
    parsed_content = json.loads(content)
    reqs = parsed_content["requests"]
    resps = parsed_content["responses"]
    print(parsed_content["requests"])
    for i,(req,resp) in enumerate( zip(reqs, resps) ):
        if "state" in req:
            req_cont = req["state"].split()
        else:
            req_cont = req["handTiles"].split()

        if i == 0:
            player = Player(req_cont[1])
        elif i == 1:
            player.set_tiles(req_cont[1:])
        else:
            # first process request
            if req_cont[0] == "2":
                # 自己摸牌
                action = DrawAction([parse_tile(req_cont[1])])
                player.do_action(action)
            elif req_cont[0] == "3":
                # 看到的别人行动
                action_player = int(req_cont[1])
                player.witness(action_player, parse_action(req_cont[2:]))

            print(req_cont)
            print(resp)

    actions = player.legal_actions(reqs[-1])

import sys

def main():
    # while 1:
    # content = sys.stdin.read()
    if len(sys.argv) < 2:
        print("Should give a filename as argument")
        return;

    with open(sys.argv[1]) as fp:
        content = fp.read()
    process_input(content)

if __name__ == '__main__':
    main()
