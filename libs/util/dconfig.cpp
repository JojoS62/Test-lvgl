/**
* @file dconfig.h
*
* @author hillkim7@gmail.com
* @brief The key and value pairs config wrapper class for managing app setting.
*
*/

#include <string.h>
#include <stdio.h>
#include "dconfig.h"

int DConfig::lookup_as_int( const std::string &key_name, int default_value ) const
{
    std::string val;

    if (lookup(key_name, val))
        return atoi(val.c_str());
    else
        return default_value;
}

const char * DConfig::lookup_as_cstr( const std::string &key_name, const char *default_value ) const
{
    std::map<std::string, std::string>::const_iterator it = this->find(key_name);
    if (it != this->end())
        return it->second.c_str();

    return default_value;
}

bool DConfig::lookup( const std::string &key_name, std::string &value ) const
{
    std::map<std::string, std::string>::const_iterator it = this->find(key_name);
    if (it != this->end())
    {
        value = it->second;
        return true;
    }

    return false;
}

std::string DConfig::lookup( const std::string &key_name ) const
{
    std::string s;

    lookup(key_name, s);

    return s;
}

bool DConfig::load_from( const char *ptr, size_t ptr_len )
{
    const char *end = ptr + ptr_len;
    const char *begin;
    std::string str_key, str_value;

    while (ptr < end)
    {
        // load key part
        begin = ptr;
        while (*ptr != kv_delimiter_)    // go until it reaches delimiter
        {
            ptr++;
            if (ptr > end)
            {
                return false;
            }
        }

        std::string key(begin, ptr);
        ++ptr;  // skip delimiter
        if (ptr >= end)
        {
            return false;
        }

        // load value part
        begin = ptr;
        while (*ptr != item_delimiter_)
        {
            ptr++;
            if (ptr > end)
            {
                return false;
            }
        }

        std::string val(begin, ptr);
        ++ptr;
        if (ptr > end)
        {
            return false;
        }

        (*this)[key] = val;
    }

    return true;
}

static bool put_cstring(DConfigOutput_t func, void *user_data, const char *s, char delimiter)
{
    while (*s && func(user_data, *s))
        s++;

    return func(user_data, delimiter);
}

bool DConfig::save_to( DConfigOutput_t func, void *user_data ) const
{
    for (std::map<std::string, std::string>::const_iterator it = this->begin();
        it != this->end(); ++it)
    {
        if (!put_cstring(func, user_data, it->first.c_str(), kv_delimiter_))
            return false;

        if (!put_cstring(func, user_data, it->second.c_str(), item_delimiter_))
            return false;
    }

    return true;
}


static bool print_char(void *user_data, char c)
{
	if (c == '\n')
		putchar('\r');
	putchar(c);

	return true;
}

void DConfig::print_all( void ) const
{
	save_to(print_char, NULL);
}

static bool count_config(void *user_data, char c)
{
    size_t *cnt = (size_t*)user_data;

    *cnt += 1;

    return true;
}

size_t DConfig::estimate_save( void ) const
{
    size_t cnt = 0;

    save_to(count_config, &cnt);

    return cnt;
}

bool DConfig::value_replace( const std::string &key_name, std::string value )
{
    if (!has_key(key_name))
        return false;

    (*this)[key_name] = value;

    return true;
}

bool DConfig::value_replace( const std::string &key_name, int value )
{
    char tmp[16];

    sprintf(tmp, "%d", value);

    return value_replace(key_name, tmp);
}

