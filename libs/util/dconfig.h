/**
* @file dconfig.h
*
* @author hillkim7@gmail.com
* @brief The key and value pairs config wrapper class for managing app setting.
* Most of programs need to hold application specific setting.
* The dconfig class can provide functionality for that.
*/

#pragma once

#include <string>
#include <map>
#include <stdlib.h>

/**  Example:
 * @code
 *
 * #include <stdio.h>
 * #include "dconfig.h"
 *
 * class MyConfig : public DConfig
 * {
 * public:
 *  virtual void reset_default(void)
 *  {
 *      (*this)["tz"] = "9";
 *      (*this)["name"] = "my_app";
 *  }
 * };
 *
 * int main(int argc, char* argv[])
 * {
 *  MyConfig mycfg;
 *  std::string serialized_data;
 *
 *  // setup initial key & value
 *  mycfg.reset_default();
 *
 *  printf("name=%s\r\n", mycfg.lookup_as_cstr("name", ""));
 *  printf("tz=%d\r\n", mycfg.lookup_as_int("tz", 0));
 *  return 0;
 * }

 */

typedef bool (*DConfigOutput_t) (void *user_data, char c);

class DConfig : public std::map<std::string, std::string>
{
public:
	DConfig(char kv_delimiter='=', char item_delimiter='\n')
		: kv_delimiter_(kv_delimiter)
		, item_delimiter_(item_delimiter)
	{
	}
	
    /** lookup value by key and return value as int if a key exists. */
    int lookup_as_int(const std::string &key_name, int default_value) const;

    /** lookup value by key and return value as C string if a key exists. */
    const char *lookup_as_cstr(const std::string &key_name, const char *default_value) const;

    /** lookup value by key */
    bool lookup(const std::string &key_name, std::string &value) const;

    /** lookup value by key and return value as C string if a key exists. */
    std::string lookup(const std::string &key_name) const;

    /** replace value of existing value matching key. */
    bool value_replace(const std::string &key_name, std::string value);

    /** replace value of existing value matching key. */
    bool value_replace(const std::string &key_name, int value);

    /** load key & value pairs from memory.
     * The delimiters are same as them used in the load_from().
     */
    bool load_from(const char *ptr, size_t ptr_len);

    /** save all key & value pairs by calling callback function.
    */
    bool save_to(DConfigOutput_t func, void *user_data) const;


    /** print all item into the stdout */
	void print_all(void) const;

    /** estimate number of bytes for saving */
    size_t estimate_save(void) const;

    /** check if a key exists. */
    bool has_key(const std::string &key_name) const
    {
        return this->find(key_name) != this->end();
    }

    /** virtual function for default setting. */
    virtual void reset_default(void)
    {
    }

protected:
	/// kv_delimiter delimiter char between key and value.
	char kv_delimiter_;
	/// item_delimiter delimiter char between items.
	char item_delimiter_;
};


