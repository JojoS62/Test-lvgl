/* Json.h */
/* Original Author: Faheem Inayat
 * Created by "Night Crue" Team @ TechShop San Jose, CA
 *
 * MIT License
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software
 * and associated documentation files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge, publish, distribute,
 * sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all copies or
 * substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
 * BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __JSON_LIB_CLASS_H_
#define __JSON_LIB_CLASS_H_

#include "jsmn.h"
#include <stdlib.h>
#include <string.h>

/**
 * C++ JSON wrapper over JSMN lib (https://github.com/zserge/jsmn).
 *
 * This C++ Class is a set of common tools/procedures as a C++ wrapper over JSMN
 * JSON parser library.  It is intended to provide the boiler-plate code, with
 * intentions to reduce code clutter, in more of C++ fashion.
 *
 * In contrast to original library, Json is intended to work strictly with valid
 * JSON structures.  Non-standard JSON structures should result in an error.
 *
 * This class works explicitly on the indices returned by underlying JSMN
 * library.  In the scope of this class, its function parameters, return types,
 * and documentation, the term 'index' will always mean the index of JSMN 
 * tokens, parsed by the Json constructor, unless and until explicitly mentioned
 * otherwise.
 */
 
 /*
    Example:
    
    Let's say we have to parse the samle JSON:
    
    {
        "team": "Night Crue",
        "company": "TechShop",
        "city": "San Jose",
        "state": "California",
        "country": "USA",
        "zip": 95113,
        "active": true,
        "members":
        [
            {
                "firstName": "John",
                "lastName": "Smith",
                "active": false,
                "hours": 18.5,
                "age": 21
            },
            {
                "firstName": "Foo",
                "lastName": "Bar",
                "active": true,
                "hours": 25,
                "age": 21
            },
            {
                "firstName": "Peter",
                "lastName": "Jones",
                "active": false
            }
        ]
    }
    
    which without the "white spaces" will look like: {"team":"Night Crue","company":"TechShop","city":"San Jose","state":"California","country":"USA","zip":95113,"active":true,"members":[{"firstName":"John","lastName":"Smith","active":false,"hours":18.5,"age":21},{"firstName":"Foo","lastName":"Bar","active":true,"hours":25,"age":21},{"firstName":"Peter","lastName":"Jones","active":false}]}
    
    Anyways, this class doesn't care about the formatting of JSON, however, it
    DOES care about the validity of JSON.  So here's a sample code to parse and
    extract values from this JSON structure.
    
    @code
    
    void main ()
    {
        // Note that the JSON object is 'escaped'.  One doesn't get escaped JSON
        // directly from the webservice, if the response type is APPLICATION/JSON
        // Just a little thing to keep in mind.
        const char * jsonSource = "{\"team\":\"Night Crue\",\"company\":\"TechShop\",\"city\":\"San Jose\",\"state\":\"California\",\"country\":\"USA\",\"zip\":95113,\"active\":true,\"members\":[{\"firstName\":\"John\",\"lastName\":\"Smith\",\"active\":false,\"hours\":18.5,\"age\":21},{\"firstName\":\"Foo\",\"lastName\":\"Bar\",\"active\":true,\"hours\":25,\"age\":21},{\"firstName\":\"Peter\",\"lastName\":\"Jones\",\"active\":false}]}";
        
        Json json ( jsonSource, strlen ( jsonSource ) );
        
        if ( !json.isValidJson () )
        {
            logError ( "Invalid JSON: %s", jsonSource );
            return;
        }

        if ( json.type (0) != JSMN_OBJECT )
        {
            logError ( "Invalid JSON.  ROOT element is not Object: %s", jsonSource );
            return;
        }
        
        // Let's get the value of key "city" in ROOT object, and copy into 
        // cityValue
        char cityValue [ 32 ];
        
        logInfo ( "Finding \"city\" Key ... " );
        // ROOT object should have '0' tokenIndex, and -1 parentIndex
        int cityKeyIndex = json.findKeyIndexIn ( "city", 0 );
        if ( cityKeyIndex == -1 )
        {
            // Error handling part ...
            logError ( "\"city\" does not exist ... do something!!" );
        }
        else
        {
            // Find the first child index of key-node "city"
            int cityValueIndex = json.findChildIndexOf ( cityKeyIndex, -1 );
            if ( cityValueIndex > 0 )
            {
                const char * valueStart  = json.tokenAddress ( cityValueIndex );
                int          valueLength = json.tokenLength ( cityValueIndex );
                strncpy ( cityValue, valueStart, valueLength );
                cityValue [ valueLength ] = 0; // NULL-terminate the string
                
                //let's print the value.  It should be "San Jose"
                logInfo ( "city: %s", cityValue );
            }
        }
        
        // More on this example to come, later.
    }
    
    @endcode
 */

class Json
{
    private:
        const unsigned int maxTokenCount;
        char * source;
        size_t sourceLength;
        jsmntok_t * tokens;
        int tokenCount;

        // Copy COntructor is intentionally kept private to enforce the caller
        // to use pointers/reference, and never pass-by-value
        Json ( const Json & );

    public:

        Json ( unsigned int maxTokens = 32 );

        /** The only constructor allowed.
         As JSON object will create/allocate memory for its working, in favor of
         small memory footprints, it is not allowed to be passed-by-value.  So
         there is no copy- or default-constructor

         @param jsonString char string containing JSON data
         @param length length of the jsonString
         @param maxTokens optional maximum count of Tokens. Default is 32.
         */
        Json ( const char * jsonString, size_t length, unsigned int maxTokens = 32 );


        /** Although there is no virtual function to this class, destructor is
         still made virtual, for just-in-case use.  Destructor will delete the
         'tokens' array, created in constructor.
         */
        virtual ~Json ();

        int parse ( const char * jsonString, size_t length );

        /** findKeyIndex will find and return the token index representing the
         'Key' in underlying JSON object.  It is a strictly a linear key search
         and will return the first occurrence, without the JSON node structure
         semantics.  For search in a specific node, refer to #findKeyIndex

         @param key a char string to find as a 'Key' in JSON structure.
         @param startingAt the starting token-index for 'key' search.  The
                search will NOT include this index, but instead will use the
                next one as the starting point.  In case, a negative value is
                passed, search will start from '0'.  It's caller's
                responsibility to make sure what values they're passing.
                Default value is set to '0', as the zero-th token index in any
                valid JSON object should always be starting object brace '{'.
                So default behavior is to always find the very first occurrence
                of key as represented by 'key' parameter.

         @return a non-zero positive integer, if a key is found in the source
                 JSON.  If no key is found, -1 will be returned. There should be
                 no '0' value returned in any valid case.
         */
        int findKeyIndex ( const char * key, const int &startingAt = 0 ) const;


        /** findKeyIndexIn will find and return the token index representing the
         'Key' in underlying JSON object node.  It is strictly a single-level
         key search function, and will NOT look for the key in any child JSON
         nodes (JSON Object/Array).

         @param key a char string to find as a 'Key' in JSON structure.
         @param parentIndex the starting token-index for 'key' search.  The
                search will look for the key, only under the JSON node
                represented by this parentIndex.  Default value is '0', making
                the default behavior to look for only root-level keys.  The
                valid value range is 0 to [parsedTokenCount()-1] both inclusive.

         @return a non-zero positive integer, if a key is found in the source
                 JSON.  If no key is found, -1 will be returned. There should be
                 no '0' value returned in any valid case.
         */
        int findKeyIndexIn ( const char * key, const int &parentIndex = 0 ) const;


        /** findChildIndexOf will find and return the token index representing
         first child a JSON node represented by parentIndex (that is either a
         Key, an Object, or an Array), and exists after the startingAt value.
         This function is particularly handy in iterating over Array Objects, or
         getting the index for JSON 'Value' of a JSON 'Key'.

         @param parentIndex token index representing the parent node in JSON
                source.  The valid value range is 0 to [parsedTokenCount()-1]
                both inclusive.
         @param startingAt describes the starting index of the nodes to search.
                In other words, if caller wants to skip some nodes, they can
                provide this value.  Default value is 0, which means search for
                all nodes in the parent.

         @return a non-zero positive integer, if the child node is found in 
                 source JSON.  If no child is found, -1 will be returned. There
                 should be no '0' value returned in any valid case.
         */
        int findChildIndexOf ( const int &parentIndex, const int &startingAt = 0 ) const;


        /** matches will tell if the token data (either key or value) matches
         with the value provided.  This function is particularly handy in
         iterating over the keys, and finding a specific key, in an object.  The
         comparison is case-sensitive. i.e. 'Apple' will NOT match with 'apple'.

         @param tokenIndex representing the token to compare.  The valid value
                range is 0 to [parsedTokenCount()-1] both inclusive.
         @param value to compare the token data with.

         @return true if the token data matches with value.  false will be
                 returned either the value doesn't match OR the tokenIndex is
                 not valid.
         */
        bool matches ( const int & tokenIndex, const char * value ) const;


        /** parsedTokenCount will tell how many tokens have been parsed by JSMN
         parser.  It is a utility function, for token validity.

         @return non-negative integer number of tokens parsed by JSMN library.
                 Negative value may be returned in case of error, but this
                 behavior is not tested, yet.
         */
        inline int parsedTokenCount () const;


        /** isValidJson will tell the caller if the parsed JSON was valid,
         parsed, and accepted to further work on.

         @return true if the JSON is valid, false otherwise.
         */
        inline bool isValidJson () const;


        /** isValidToken will tell the caller if the tokenIndex is in valid
         range.  The valid value range is 0 to [parsedTokenCount()-1] both
         inclusive.

         @param tokenIndex representing the token in the JSON source

         @return true if the JSON is valid, false otherwise.
         */
        inline bool isValidToken ( const int tokenIndex ) const;


        /** type will return the JSMN type represented by the tokenIndex.

         @param tokenIndex representing the token in the JSON source.  The valid
                value range is 0 to [parsedTokenCount()-1] both inclusive.

         @return the type represented by tokenIndex.  In case of invalid
                 tokenIndex, JSMN_UNDEFINED is returned.
         */
        inline jsmntype_t type ( const int tokenIndex ) const;


        /** parent is a utility function to get the parent index of the
         tokenIndex passed.

         @param tokenIndex representing the token in the JSON source.  The valid
                value range is 0 to [parsedTokenCount()-1] both inclusive.

         @return the parentIndex if the node has a parent, and tokenIndex is a
                 valid index.  In case of no parent, or invalid tokenIndex, -1
                 is returned.
         */
        inline int parent ( const int tokenIndex ) const;


        /** childCount returns the number of children sharing the same parent.
         This utility function is handy for iterating over Arrays or Objects.

         @param tokenIndex representing the token in the JSON source.  The valid
                value range is 0 to [parsedTokenCount()-1] both inclusive.

         @return non-negative integer representing the number of children
                 tokenIndex node has.  0 is a valid number, in case the node has
                 no child nodes.  -1 will be returned if the tokenIndex is not
                 valid.
        */
        inline int childCount ( const int tokenIndex ) const;


        /** tokenLength returns the number of characters a node takes up in JSON
         source string.

         @param tokenIndex representing the token in the JSON source.  The valid
                value range is 0 to [parsedTokenCount()-1] both inclusive.

         @return positive integer value representing the length of the token
                 sub-string in the source JSON.  The 0 value is an invalid state
                 and should never occur.  -1 will be returned in case of invalid
                 tokenIndex.
         */
        inline int tokenLength ( const int tokenIndex ) const;


        /** tokenAddress returns the pointer that marks as the start of token
         in JSON source string.  This is a utility function for character/string
         manipulation by the caller.

         @param tokenIndex representing the token in the JSON source.  The valid
                value range is 0 to [parsedTokenCount()-1] both inclusive.

         @return a non-NULL pointer will be returned if tokenIndex is valid, -1
                 otherwise.
         */
        inline const char * tokenAddress ( const int tokenIndex ) const;


        /** tokenInterValue will convert the value as int represented by the
         tokenIndex.  A typical use is that caller has found the Key-index, and
         then has retrieved the Value-index (by using findChildIndexOf function)
         , and now they want to read the value of Value-index, as integer value.

         @param tokenIndex representing the "value" in the JSON source.  The
                valid value range is 0 to [parsedTokenCount()-1] both inclusive.

         @param returnValue is a return-parameter passed by reference to hold up
                the integer value parsed by this function.  If the converted
                value would be out of the range of representable values by an
                int, it causes undefined behavior.  It is caller's
                responsibility to check for these cases.

         @return 0 if the operation is successful.  -1 if tokenIndex is invalid.
        */
        int tokenIntegerValue ( const int tokenIndex, int &returnValue ) const;


        /** tokenNumberValue will convert the value as float represented by the
         tokenIndex.  A typical use is that caller has found the Key-index, and
         then has retrieved the Value-index (by using findChildIndexOf function)
         , and now they want to read the value of Value-index, as floating-point
         value.

         @param tokenIndex representing the "value" in the JSON source.  The
                valid value range is 0 to [parsedTokenCount()-1] both inclusive.

         @param returnValue is a return-parameter passed by reference to hold up
                the floating-point value parsed by this function.  If the
                converted value would be out of the range of representable
                values by a float, it causes undefined behavior.  It is caller's
                responsibility to check for these cases.

         @return 0 if the operation is successful.  -1 if tokenIndex is invalid.
        */
        int tokenNumberValue ( const int tokenIndex, float &returnValue ) const;


        /** tokenBooleanValue will convert the value as bool represented by
         the tokenIndex.  A typical use is that caller has found the Key-index,
         and then has retrieved the Value-index (by using findChildIndexOf
         function), and now they want to read the value of Value-index, as
         boolean value.

         @param tokenIndex representing the "value" in the JSON source.  The
                valid value range is 0 to [parsedTokenCount()-1] both inclusive.

         @param returnValue is a return-parameter passed by reference to hold up
                the bool value parsed by this function.

         @return 0 if the operation is successful.  -1 if tokenIndex is invalid.
        */
        int tokenBooleanValue ( const int tokenIndex, bool &returnValue ) const;


        /** unescape is a utility function to unescape a JSON string.  This
         function does not change any state of Json object, and is a pure
         static utility function.  This function is in-pace unescaping, and WILL
         modify the source parameter.

         @param jsonString representing an escaped JSON string.  This parameter
                is also the return parameter as well.  All modifications will be
                reflected in this parameter.

         @return pointer to unescaped JSON string.  This is exactly the same
                 pointer as jsonString parameter.
         */
        static char * unescape ( char * jsonString );
};

inline int Json::parsedTokenCount () const
{
    return tokenCount;
}

inline bool Json::isValidJson () const
{
    return ( tokenCount >= 1 );
}

inline bool Json::isValidToken ( const int tokenIndex ) const
{
    return ( tokenIndex >= 0 && tokenIndex < tokenCount );
}

inline jsmntype_t Json::type ( const int tokenIndex ) const
{
    jsmntype_t retVal = JSMN_UNDEFINED;

    if ( isValidToken ( tokenIndex ) )
    {
        retVal = tokens [ tokenIndex ].type;
    }

    return retVal;
}

inline int Json::parent ( const int tokenIndex ) const
{
    int retVal = -1;

    if ( isValidToken ( tokenIndex ) )
    {
        retVal = tokens [ tokenIndex ].parent;
    }

    return retVal;
}

inline int Json::childCount ( const int tokenIndex ) const
{
    int retVal = -1;

    if ( isValidToken ( tokenIndex ) )
    {
        retVal = tokens [ tokenIndex ].childCount;
    }

    return retVal;
}

inline int Json::tokenLength ( const int tokenIndex ) const
{
    int retVal = -1;

    if ( isValidToken ( tokenIndex ) )
    {
        retVal = tokens [ tokenIndex ].end - tokens [ tokenIndex ].start;
    }

    return retVal;
}

inline const char * Json::tokenAddress ( const int tokenIndex ) const
{
    char * retVal = NULL;

    if ( isValidToken ( tokenIndex ) )
    {
        retVal = (char *) source + tokens [ tokenIndex ].start;
    }

    return retVal;
}

#endif

