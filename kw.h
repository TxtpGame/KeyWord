#pragma once
//周平
namespace  kw
{
    // 加载初始化词库 成功返回0  kw_file文件编码utf8
    int init(const char *kw_file);

    // 检测是否存在关键字
    // msg hint 编码utf8 \0结尾
    // 返回值：0表示没有关键字 -1表示有关键字 hint表示触发的关键字
    int check(const char *msg, char *hint);

    // 和谐字符串 每一个关键字替换为一个星号
    // msg hexie_msg 编码utf8 \0结尾
    // 返回值：0表示没有关键字 -1表示存在关键字冲突 msg为替换后的字符串
    int hexie(char *msg);
};
