# Riichi Mahjong 裁判代码

## Dependence
  * Boost
  * [jsoncpp](https://github.com/open-source-parsers/jsoncpp)

`jsoncpp` 的 include 路径默认为 `json/json.h` 可以在 `mahjong.h` 中更改。

## Build

```shell
# 编译
g++ --std=c++11 Mahjong2.cpp Yizhong.cpp Player.cpp Tools.cpp -ljsoncpp -o mahjong

# 之后可以执行
./mahjong
```

