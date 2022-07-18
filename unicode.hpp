#pragma once
//周平
namespace unicode
{
    typedef unsigned char   UTF8;
    typedef unsigned int    UNIC;

    constexpr auto MAX_LEGAL_UNIC = (UNIC)0x0010FFFF;

    static const unsigned char BytesForUTF8[256] =
    {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
        3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 4, 4, 4, 4, 4, 4, 4, 4, 5, 5, 5, 5, 6, 6, 6, 6
    };

    inline unsigned int get_utf8_bytes(UTF8 first)
    {
        return BytesForUTF8[first];
    }

    inline int utf8_to_unicode_one(UTF8 *utf8, int utf8_size, UNIC *unic)
    {
        if (!utf8) return 0;
        *unic = 0;
        int bytes = get_utf8_bytes(*utf8);
        if (utf8_size < bytes) return 0;

        switch (bytes)
        {
        case 1:  //0xxx xxxx
        {
            *unic = *utf8;
            return bytes;
        }
        break;
        case 2: //110x xxxx 10xx xxxx
        {
            unsigned char &b1 = *utf8;
            unsigned char &b2 = *(utf8 + 1);
            if ((b2 & 0xC0) != 0x80) return 0;

            *unic = ((b1 & 0x1F) << 6) + (b2 & 0x3F); //110x xxxx 10xx xxxx
            return bytes;
        }
        break;
        case 3:  //1110 xxxx 10xx xxxx 10xx xxxx
        {
            unsigned char &b1 = *utf8;
            unsigned char &b2 = *(utf8 + 1);
            unsigned char &b3 = *(utf8 + 2);
            if ((b2 & 0xC0) != 0x80) return 0;
            if ((b3 & 0xC0) != 0x80) return 0;

            *unic = ((b1 & 0x0F) << 12) + ((b2 & 0x3F) << 6) + (b3 & 0x3F);
            return bytes;
        }
        break;
        case 4:  //1111 0xxx  10xx xxxx  10xx xxxx  10xx xxxx
        {
            unsigned char &b1 = *utf8;
            unsigned char &b2 = *(utf8 + 1);
            unsigned char &b3 = *(utf8 + 2);
            unsigned char &b4 = *(utf8 + 3);
            if ((b2 & 0xC0) != 0x80) return 0;
            if ((b3 & 0xC0) != 0x80) return 0;
            if ((b4 & 0xC0) != 0x80) return 0;

            *unic = ((b1 & 0x08) << 18) + ((b2 & 0x3F) << 12) + ((b3 & 0x3F) << 6) + (b4 & 0x3F);
            return bytes;
        }
        break;
        case 5:  //1111 10xx  10xx xxxx  10xx xxxx  10xx xxxx 10xx xxxx
        case 6:  //1111 110x  10xx xxxx  10xx xxxx  10xx xxxx 10xx xxxx 10xx xxxx
            break;
        default:
            break;
        }

        return 0;
    }

    inline int utf8_to_unicode(const UTF8 *utf8_str, int utf8_len, UNIC *unic_buf, int unic_buf_size)
    {
        if (!utf8_str || utf8_len <= 0 || !unic_buf || unic_buf_size <= 0) return 0;
        int  len = 0;
        int  index = 0;
        while (index < unic_buf_size && len < utf8_len)
        {
            int l = utf8_to_unicode_one((UTF8 *)utf8_str + len, utf8_len - len, &unic_buf[index]);
            if (l <= 0) return index;
            len += l;
            index++;
        }
        if (index < unic_buf_size) unic_buf[index] = 0;
        return index;
    }

    inline int unicode_to_utf8_one(UNIC unic, UTF8 *utf8_buf, int utf8_buf_size)
    {
        if (unic <= 0x7F)  // * U-00 - U-7F:  0xxxxxxx
        {
            if (!utf8_buf || utf8_buf_size < 1) return 0;
            *utf8_buf = (unic & 0x7F);
            return 1;
        }
        else if (unic <= 0x07FF)  // * U-80 - U-0x07FF:  110xxxxx 10xxxxxx
        {
            if (!utf8_buf || utf8_buf_size < 2) return 0;

            *(utf8_buf + 1) = (unic & 0x3F) | 0x80;  //10xx xxxx
            *utf8_buf = ((unic >> 6) & 0x1F) | 0xC0; //110x xxxx

            return 2;
        }
        else if (unic <= 0xFFFF)   // * U-0800 - U-FFFF:  1110xxxx 10xxxxxx 10xxxxxx
        {
            if (!utf8_buf || utf8_buf_size < 3) return 0;
            *(utf8_buf + 2) = (unic & 0x3F) | 0x80;        //10xx xxxx
            *(utf8_buf + 1) = ((unic >> 6) & 0x3F) | 0x80; //10xx xxxx
            *utf8_buf = ((unic >> 12) & 0x0F) | 0xE0;      //1110 xxxx

            return 3;
        }
        else if (unic <= MAX_LEGAL_UNIC)   // * U-10000 - U-10FFFF:  11110xxx  10xxxxxx 10xxxxxx 10xxxxxx
        {
            if (!utf8_buf || utf8_buf_size < 4) return 0;
            *(utf8_buf + 3) = (unic & 0x3F) | 0x80;         //10xx xxxx
            *(utf8_buf + 2) = ((unic >> 6) & 0x3F) | 0x80;  //10xx xxxx
            *(utf8_buf + 1) = ((unic >> 12) & 0x3F) | 0x80; //10xx xxxx
            *utf8_buf = ((unic >> 18) & 0x08) | 0xF0;       //1111 0xxx

            return 3;
        }

        return 0;
    }


    inline int unicode_to_utf8(const UNIC *unic_str, int unic_len, UTF8 *utf8_buf, int utf8_buf_size)
    {
        if (!unic_str || unic_len <= 0 || !utf8_buf || utf8_buf_size <= 0) return 0;
        int  len = 0;
        int  index = 0;
        while (index < unic_len && len < utf8_buf_size)
        {
            int l = unicode_to_utf8_one(unic_str[index], (UTF8 *)utf8_buf + len, utf8_buf_size - len);
            if (l <= 0) return len;
            len += l;
            index++;
        }

        if (len < utf8_buf_size) utf8_buf[len] = 0;

        return len;
    }
};