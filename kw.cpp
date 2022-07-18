
#include "kw.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <list>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <algorithm>
#include <ctype.h>
#include "unicode.hpp"
using namespace unicode;
using namespace std;
#pragma warning(disable:4996)

constexpr auto MAX_KW_U8_LEN = 128;
constexpr auto MAX_KW_UNIC_LEN = 32;
constexpr auto MAX_MSG_LEN = 1024;

struct KeyWordsItem
{
    UNIC unic_words[MAX_KW_UNIC_LEN] = {};
    UTF8 u8_words[MAX_KW_U8_LEN] = {};
    int unic_len = {};
    int u8_len = {};
    bool operator<(const KeyWordsItem &other) const
    {
        int n = min(unic_len, other.unic_len);
        for (int i = 0; i < n; i++)
        {
            if (unic_words[i] != other.unic_words[i])
            {
                return unic_words[i] < other.unic_words[i];
            }
        }
        return unic_len > other.unic_len;
    }
};

vector<KeyWordsItem> g_KeyWords;
set<KeyWordsItem> s_KeyWords;

struct IndexItem
{
    int start = -1;
    int end = -1;
};

map<UNIC, IndexItem> g_UnicIndex;

void make_lower(UNIC *s, int len)
{
    static const UNIC tag[3] = { 65, 65313, 65345 };
    for (int i = 0; i < len; i++)
    {
        for (int j = 0; j < 3; j++)
        {
            if (s[i] >= tag[j] && s[i] <= (tag[j] + 25))
            {
                s[i] = 97 + (s[i] - tag[j]);
                break;
            }
        }
    }
}

int remove_space(char *s, int len)
{
    int index = 0;
    for (int i = 0; i < len; i++)
    {
        if (s[i] != ' ')
        {
            s[index] = s[i];
            index++;
        }
    }
    s[index] = 0;
    return index;
}

void add_key_word(const UTF8 *s, int len)
{
    UNIC unic[MAX_KW_UNIC_LEN] = { 0 };
    UTF8 utf8[MAX_KW_U8_LEN] = { 0 };
    if (len > MAX_KW_U8_LEN)
        return;
    memcpy(utf8, s, len * sizeof(UTF8));

    int u8_len = remove_space((char *)&utf8, len);

    int unic_len = utf8_to_unicode(utf8, u8_len, unic, MAX_KW_UNIC_LEN);
    if (unic_len <= 0)
    {
        return;
    }
    make_lower(unic, unic_len);

    KeyWordsItem item;
    item.u8_len = u8_len;
    item.unic_len = unic_len;

    memcpy(item.u8_words, utf8, u8_len * sizeof(UTF8));
    memcpy(item.unic_words, unic, unic_len * sizeof(UNIC));

    s_KeyWords.insert(item);
}

void sort_key_words_index()
{
    for (auto it = s_KeyWords.begin(); it != s_KeyWords.end(); it++)
    {
        g_KeyWords.push_back(*it);
    }

    int n = g_KeyWords.size();
    if (n == 0)
    {
        return;
    }

    UNIC t = g_KeyWords[0].unic_words[0];
    UNIC c = t;
    g_UnicIndex[c].start = 0;
    int group = 1;
    for (int i = 1; i < n; i++)
    {
        t = g_KeyWords[i].unic_words[0];
        if (c != t)
        {
            group++;
            g_UnicIndex[c].end = i;
            g_UnicIndex[t].start = i;
            c = t;
        }
    }
    g_UnicIndex[c].end = n;

    printf("group = %d\n", group);
}

bool isSkipChar(UNIC unic)
{
    if (unic > 0x7F)
    {
        return false;
    }

    if (isalpha(unic))
    {
        return false;
    }
    return true;
}

int kw::init(const char *kw_file)
{
    FILE *file = fopen(kw_file, "r");
    if (!file)
        return -1;
    char buf[MAX_KW_U8_LEN];
    g_KeyWords.clear();
    while (fgets(buf, MAX_KW_U8_LEN, file))
    {
        int len = strlen(buf);
        while (len > 0 && (buf[len - 1] == 13 || buf[len - 1] == 10 || buf[len - 1] == 32))
        {
            len--;
        }
        if (len > 0)
        {
            buf[len] = 0;
            add_key_word((UTF8 *)&buf, len);
        }
    }
    fclose(file);

    sort_key_words_index();
    return 0;
}

int kw::check(const char *msg, char *hint)
{
    int msg_len = strlen(msg);
    if (msg_len >= MAX_MSG_LEN)
    {
        sprintf(hint, u8"参数超长");
        return -1;
    }
    UNIC unic[MAX_MSG_LEN] = { 0 };
    UTF8 utf8[MAX_MSG_LEN] = { 0 };
    memcpy(utf8, msg, msg_len * sizeof(UTF8));

    int u8_len = remove_space((char *)&utf8, msg_len);

    int unic_len = utf8_to_unicode(utf8, u8_len, unic, MAX_MSG_LEN);
    make_lower(unic, unic_len);
    int index = 0;
    while (index < unic_len)
    {
        UNIC c = unic[index];
        auto unicindex = g_UnicIndex.find(c);
        if (unicindex == g_UnicIndex.end())
        {
            index++;
            continue;
        }

        int start = unicindex->second.start;
        int end = unicindex->second.end;;
        for (int j = start; j < end; j++)
        {
            int key_len = g_KeyWords[j].unic_len;
            if ((unic_len - index) < key_len)
            {
                continue;
            }

            int macth_len = 0;
            int skip_len = 0;
            for (int k = 0; k < g_KeyWords[j].unic_len; k++)
            {
                int i = index + macth_len + skip_len;
                while (i < unic_len)
                {
                    if (isSkipChar(unic[i]))
                    {
                        i++;
                        continue;
                    }
                    break;
                }
                if (i == unic_len)
                {
                    break;
                }

                if (unic[i] != g_KeyWords[j].unic_words[k])
                {
                    break;
                }
                macth_len++;
            }
            if (macth_len == key_len)
            {
                sprintf(hint, "%s", g_KeyWords[j].u8_words);
                return -1;
            }
        }
        index++;
    }
    return 0;
}

// 中文规则
int kw::hexie(char *msg)
{
    int msg_len = strlen(msg);
    if (msg_len >= MAX_MSG_LEN)
    {
        return -1;
    }

    UNIC unic_old[MAX_MSG_LEN] = { 0 };
    UNIC unic[MAX_MSG_LEN] = { 0 };
    UTF8 utf8[MAX_MSG_LEN] = { 0 };
    UNIC unic_new[MAX_MSG_LEN] = { 0 };
    memcpy(utf8, msg, msg_len * sizeof(UTF8));


    int u8_len = msg_len;
    int unic_len = utf8_to_unicode(utf8, u8_len, unic, MAX_MSG_LEN);
    memcpy(unic_old, unic, MAX_MSG_LEN * sizeof(UNIC));
    make_lower(unic, unic_len);
    int index = 0;
    int tempIndex = 0;
    bool bb = false;
    while (index < unic_len)
    {
        UNIC c = unic[index];
        UNIC c_old = unic_old[index];// 记录下原始字符

        auto unicindex = g_UnicIndex.find(c);
        if (unicindex == g_UnicIndex.end())
        {
            unic_new[tempIndex] = c_old;
            index++;
            tempIndex++;
            continue;
        }

        int start = unicindex->second.start;
        int end = unicindex->second.end;
        int macth_len = 0;
        int skip_len = 0;
        bool find = false;
        for (int j = start; j < end; j++)
        {
            int key_len = g_KeyWords[j].unic_len;
            if ((unic_len - index) < key_len)
            {
                continue;
            }

            macth_len = 0;
            skip_len = 0;
            for (int k = 0; k < g_KeyWords[j].unic_len; k++)
            {
                int i = index + macth_len + skip_len;
                while (i < unic_len)
                {
                    if (isSkipChar(unic[i]))
                    {
                        i++;
                        skip_len++;
                        continue;
                    }
                    break;
                }
                if (i == unic_len || (unic[i] != g_KeyWords[j].unic_words[k]))
                {
                    break;
                }
                macth_len++;
            }
            if (macth_len == key_len)
            {
                for (int i = 0; i < macth_len + skip_len; i++)
                {
                    unic_new[tempIndex + i] = '*';
                }
                find = true;
                break;
            }
        }
        if (find)
        {
            bb = true;
            index += macth_len + skip_len;
            tempIndex += macth_len + skip_len;
        }
        else
        {
            unic_new[tempIndex] = c_old;
            index++;
            tempIndex++;
        }
    }
    if (bb)
    {
        unicode_to_utf8(unic_new, tempIndex, (UTF8 *)msg, msg_len);
        return -1;
    }
    return 0;
}
