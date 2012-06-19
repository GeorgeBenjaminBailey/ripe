#include "stdafx.h"

#include "webutility.h"

#include <Windows.h>
#include <Wininet.h>

#include <wx/protocol/http.h>
#include <wx/sstream.h>
#include <wx/stopwatch.h>

const wxString QueryWebPage( const wxString &url )
{
    wxString result;
    HINTERNET hInternet, hFile;
    hInternet = InternetOpen( L"Browser", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, NULL );

    if( hInternet )
    {
        hFile = InternetOpenUrl(
            hInternet,
            url.wc_str(),
            NULL,
            0,
            INTERNET_FLAG_RELOAD,
            0
            );

        if( hFile )
        {
            char buffer[4096];
            DWORD dwRead;
            while( InternetReadFile( hFile, buffer, sizeof(buffer)-1, &dwRead ) )
            {
                if( dwRead == 0 )
                    break;
                buffer[dwRead] = 0;
                result += buffer;
            }
        }
        InternetCloseHandle( hFile );
    }
    InternetCloseHandle( hInternet );

    return result;
}

const wxString PostWebPage( const wxString &host, const wxString &path,
    const wxString &data )
{
    wxString result = "";
    wxSocketClient *socket = new wxSocketClient();

    //Set up header
    wxString header = "";

    //POST
    header += "POST ";
    header += path;
    header += " HTTP/1.0\n";

    //Write host website name
    header += "Host: ";
    header += host;
    header += "\n";

    //Write user agent
    header += "User-Agent: HTTPTool/1.0\n";

    //Write POST content length
    header += "Content-Length: ";
    header += wxString::Format("%d", data.Len());
    header += "\n";

    //Write content type
    header += "Content-Type: application/x-www-form-urlencoded\n";
    header += "\n";

    //Connect to host
    wxIPV4address * address = new wxIPV4address();
    address->Hostname(host);
    address->Service(80);

    if (!socket->Connect(*address))
        return wxT("Error connecting to server.");

    //Write header
    socket->Write(header.c_str(),header.Len());

    //Write data
    socket->Write(data.c_str(),data.Len());

    //Get Response
    if( socket->IsOk() )
    {
        do 
        {
            char buf[1024];
            socket->Read(buf, 1024);
            result.Append(buf, socket->LastCount());
        } while (socket->LastCount() > 0);
    }
    
    int loc = result.Find("\n\n");
    if( loc != wxNOT_FOUND )
    {
        result = result.Mid(loc+1);
    }

    return result;
}

// Encodes a string for safely passing it as a GET / POST parameter
const wxString URLEncode( const wxString &str )
{
    wxString result = "";
    for( size_t i=0; i < str.length(); ++i )
    {
        result += wxString::Format("%%%02X", str[i]);
    }

    return result;
}
