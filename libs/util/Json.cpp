/* Json.cpp */
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

#include "Json.h"

Json::Json ( unsigned int maxTokens )
        : maxTokenCount ( maxTokens ), source ( NULL ), sourceLength ( 0 )
{
    tokens = new jsmntok_t [ maxTokenCount ];
}

Json::Json ( const char * jsonString, size_t length, unsigned int maxTokens )
        : maxTokenCount ( maxTokens ), source ( (char*)jsonString ), sourceLength ( length )
{
    tokens = new jsmntok_t [ maxTokenCount ];

    if ((jsonString != NULL) && (length > 0))
    {
        jsmn_parser parser;
		jsmn_init ( &parser );
		tokenCount = jsmn_parse ( &parser, jsonString, length, tokens, maxTokenCount );
    }
}

int Json::parse ( const char * jsonString, size_t length )
{
	source = (char*)jsonString;
	sourceLength = length;

    if ((jsonString != NULL) && (length > 0))
    {
        jsmn_parser parser;
		jsmn_init ( &parser );
		tokenCount = jsmn_parse ( &parser, jsonString, length, tokens, maxTokenCount );

		return tokenCount;
    }
    else
    	return 0;
}

Json::Json ( const Json & )
        : maxTokenCount ( 0 ), source ( NULL ), sourceLength ( 0 )
{
    tokenCount = 0;
    tokens = NULL;
}

Json::~Json ()
{
    delete [] tokens;
}

int Json::findKeyIndex ( const char * key, const int &startingAt ) const
{
    int retVal = -1;

    int i = startingAt + 1;
    if ( i < 0 ) {
        i = 0;
    }
    
    for ( ; i < tokenCount; i++ )
    {
        jsmntok_t t = tokens [ i ];

        if ( t.type == JSMN_KEY )
        {
            size_t keyLength = (size_t) ( t.end - t.start );
            if ( ( strlen ( key ) == keyLength ) && ( strncmp ( source + t.start, key, keyLength ) == 0 ) )
            {
                retVal = i;
                break;
            }
        }
    }

    return retVal;
}

int Json::findKeyIndexIn ( const char * key, const int &parentIndex ) const
{
    int retVal = -1;

    if ( isValidToken ( parentIndex ) )
    {
        for ( int i = parentIndex + 1; i < tokenCount; i++ )
        {
            jsmntok_t t = tokens [ i ];

            if ( t.end >= tokens [ parentIndex ].end )
            {
                break;
            }

            if ( ( t.type == JSMN_KEY ) && ( t.parent == parentIndex ) )
            {
                size_t keyLength = (size_t) ( t.end - t.start );
                if ( ( strlen ( key ) == keyLength ) && ( strncmp ( source + t.start, key, keyLength ) == 0 ) )
                {
                    retVal = i;
                    break;
                }
            }
        }
    }

    return retVal;
}

int Json::findChildIndexOf ( const int &parentIndex, const int &startingAt ) const
{
    int retVal = -1;

    if ( isValidToken ( parentIndex ) )
    {

        jsmntype_t type = tokens [ parentIndex ].type;
        if ( ( type == JSMN_KEY ) || ( type == JSMN_OBJECT ) || ( type == JSMN_ARRAY ) )
        {
            int i = startingAt + 1;
            if ( startingAt < 0 )
            {
                i = 0;
            }

            for ( i += parentIndex; i < tokenCount; i++ )
            {
                if ( tokens [ i ].parent == parentIndex )
                {
                    retVal = i;
                    break;
                }
            }
        }
    }

    return retVal;
}

bool Json::matches ( const int & tokenIndex, const char * value ) const
{
    bool retVal = false;
    
    if ( isValidToken ( tokenIndex ) )
    {
        jsmntok_t token = tokens [ tokenIndex ];
        retVal = ( strncmp ( source + token.start, value, ( token.end - token.start ) ) == 0 );
    }
    
    return retVal;
}

int Json::tokenIntegerValue ( const int tokenIndex, int &returnValue ) const
{
    int retVal = -1;
    
    if ( type ( tokenIndex ) == JSMN_PRIMITIVE )
    {
        int len = tokenLength ( tokenIndex );
        char * tok = new char [ len + 1 ];
        strncpy ( tok, tokenAddress ( tokenIndex ), len );
        tok [ len ] = 0;
        returnValue = atoi ( tok );
        delete [] tok;
        retVal = 0;
    }
    return retVal;
}

int Json::tokenNumberValue ( const int tokenIndex, float &returnValue ) const
{
    int retVal = -1;
    
    if ( type ( tokenIndex ) == JSMN_PRIMITIVE )
    {
        int len = tokenLength ( tokenIndex );
        char * tok = new char [ len + 1 ];
        strncpy ( tok, tokenAddress ( tokenIndex ), len );
        tok [ len ] = 0;
        returnValue = atof ( tok );
        delete [] tok;
        retVal = 0;
    }
    
    return retVal;
}

int Json::tokenBooleanValue ( const int tokenIndex, bool &returnValue ) const
{
    int retVal = -1;
    
    if ( type ( tokenIndex ) == JSMN_PRIMITIVE )
    {
        returnValue = matches ( tokenIndex, "true" );
        retVal = 0;
    }

    return retVal;
}

char * Json::unescape ( char * jsonString )
{
    if ( jsonString != NULL )
    {
        int stringIndex = 0;
        int indentLevel = 0;
        int quoteCount = 0;
        for ( int i = 0; jsonString [ i ] != 0; i ++ )
        {
            switch ( jsonString [ i ] )
            {
                case '{':
                    indentLevel ++;
                    break;
                
                case '}':
                    indentLevel --;
                    if ( indentLevel == 0 ) {
                        // Just close and return the first valid JSON object.  No need to handle complex cases.
                        jsonString [ stringIndex ++ ] = '}';
                        jsonString [ stringIndex ] = 0;
                        return jsonString;
                    }
                    break;
                    
                case '\\':
                    i ++;
                    break;
                    
                case '"':
                    quoteCount ++;
                    break;
            }
            
            if ( indentLevel > 0 )
            {
                if ( quoteCount == 0 ) {
                    return jsonString; //No need to unescape.  JsonString needs to be already escaped
                }
                jsonString [ stringIndex ++ ] = jsonString [ i ];
            }
        }
        jsonString [ stringIndex ] = 0;
    }
    
    return jsonString;
}
