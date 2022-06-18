
#include "jin_string.h"

string str_rpbrk( const string str, const string chars ) {
    size_t i = strlen(str);
    
    for ( ; i > 0; i-- ) {
        if( memchr( chars, str[i - 1], sizeof(TRIMMED_CHARS) ) != NULL )
            return &str[i - 1];
    }
    return NULL;
}

string str_ltrim ( const string str ) {

    // handle the case where there is no trimmable character on the left,
    // in this scenario strspn would fail returning a false positive
    if ( memchr(TRIMMED_CHARS, str[0], sizeof(TRIMMED_CHARS) ) == NULL )
        return str;

    return (str + (strspn(str, TRIMMED_CHARS) ));
}

string str_rtrim( string str ) {

    size_t len = strlen(str);
        // handle the case where there is no trimmable character on the right,
        // in this scenario strspn would fail returning a false positive
    if ( memchr(TRIMMED_CHARS, str[len-1], sizeof(TRIMMED_CHARS) ) == NULL )
        return str;
    
    string last_char = str_rpbrk(str, TRIMMED_CHARS );
    if ( last_char != NULL )
        *last_char = 0;

    return str;
}

string str_trim( string str ) {
    return str_rtrim( str_ltrim(str) );
}

bool str_isalphanum(const string str, size_t len) {
    for ( unsigned int i = 0; i < len; i++ ){
        if ( !isalnum(str[i]) )
            return false;
    }
    return true;
}
