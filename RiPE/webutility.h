#ifndef __WEB_UTILITY_H
#define __WEB_UTILITY_H

#include <wx/string.h>

const wxString QueryWebPage( const wxString &url );

const wxString PostWebPage( const wxString &host, const wxString &path,
        const wxString &data );

// Encodes a string for safely passing it as a GET / POST parameter
const wxString URLEncode( const wxString &str );

#endif // __WEB_UTILITY_H
