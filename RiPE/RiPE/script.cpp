#include "stdafx.h"

#include <utility>

#include "script.h"

#include <wx/regex.h>
#include <wx/intl.h>

#include "../processutility.h"
#include "RiPEApp.h"
#include "../RiPE.h" // needed for callback functions / interoperability with RiPE

// This namespace contains structures used for parsing
namespace Script
{
    extern OperatorList oList;
    extern HookInterface *hookIntf;
    extern bool mutIsExecuting; // Keeps track of if the ScriptManager is executing a script.
    // This prevents some common infinite loops

    HookInterface *hookIntf = NULL;
    bool mutIsExecuting = false;

    // Map of memory address to pair<scriptName, funcName>
    std::map<DWORD, std::pair<wxString, wxString> > hookedFunctions;

    // Populate a map of variables with registers
    void PopulateRegisters( EXCEPTION_POINTERS *exceptionInfo, std::map<wxString, Variable> &registers )
    {
        PCONTEXT pc = exceptionInfo->ContextRecord;

        // General-purpose
        registers["eax"] = pc->Eax;
        registers["ecx"] = pc->Ecx;
        registers["edx"] = pc->Edx;
        registers["ebx"] = pc->Ebx;
        registers["esp"] = pc->Esp;
        registers["ebp"] = pc->Ebp;
        registers["esi"] = pc->Esi;
        registers["edi"] = pc->Edi;

        registers["dr0"] = pc->Dr0;
        registers["dr1"] = pc->Dr1;
        registers["dr2"] = pc->Dr2;
        registers["dr3"] = pc->Dr3;
        registers["dr6"] = pc->Dr6;
        registers["dr7"] = pc->Dr7;

        registers["efl"] = pc->EFlags;
        registers["eip"] = pc->Eip;
    }

    // Modify the contents of the registers to match the new variables
    void UpdateRegisters( EXCEPTION_POINTERS *exceptionInfo, std::map<wxString, Variable> &registers )
    {
        PCONTEXT pc = exceptionInfo->ContextRecord;

        // General-purpose
        pc->Eax = registers["eax"].AsValue();
        pc->Ecx = registers["ecx"].AsValue();
        pc->Edx = registers["edx"].AsValue();
        pc->Ebx = registers["ebx"].AsValue();
        pc->Esp = registers["esp"].AsValue();
        pc->Ebp = registers["ebp"].AsValue();
        pc->Esi = registers["esi"].AsValue();
        pc->Edi = registers["edi"].AsValue();

        pc->Dr0 = registers["dr0"].AsValue();
        pc->Dr1 = registers["dr1"].AsValue();
        pc->Dr2 = registers["dr2"].AsValue();
        pc->Dr3 = registers["dr3"].AsValue();
        pc->Dr6 = registers["dr6"].AsValue();
        pc->Dr7 = registers["dr7"].AsValue();

        pc->EFlags = registers["efl"].AsValue();
        pc->Eip = registers["eip"].AsValue();
    }

    void LogRegistersHelper( EXCEPTION_POINTERS *exceptionInfo ) {
        std::map<wxString, Variable> registers;
        std::map<wxString, Variable *> myLocals;
        wxString scriptName;
        wxString functionName;

        // No script exists for this!  Uh oh!
        if( hookedFunctions.count(exceptionInfo->ContextRecord->Eip) == 0 )
        {
            return;
        }

        // Get script and function name
        scriptName   = hookedFunctions[exceptionInfo->ContextRecord->Eip].first;
        functionName = hookedFunctions[exceptionInfo->ContextRecord->Eip].second;

        // Populate registers
        PopulateRegisters( exceptionInfo, registers );

        // Copy registers to myLocals
        std::map<wxString, Variable>::iterator iter = registers.begin();
        for( ; iter != registers.end(); ++iter )
        {
            myLocals[iter->first] = &iter->second;
        }

        // Process the function
        RiPEApp::GetRiPE()->scriptManager.ProcessLogEvent( scriptName, functionName, myLocals );

        // Make changes to registers as needed
        UpdateRegisters( exceptionInfo, registers );
    }

    void LogRegisters( EXCEPTION_POINTERS *exceptionInfo )
    {
        __try {
            LogRegistersHelper( exceptionInfo );
        } __except (EXCEPTION_EXECUTE_HANDLER) {
        }
    }
    
    void InfixToPostfix( std::list<Token> &tokens )
    {
        std::stack<Token> intermediateStack;
        std::list<Token>  postfixList;
        bool              hasError = false;

        // for all elements in the infix expression
        std::list<Token>::iterator iter = tokens.begin();
        for( ; iter != tokens.end(); ++iter )
        {
            if( iter->type == Token::VARIABLE )
                postfixList.push_back( *iter );
            else if( iter->type == Token::DECIMAL )
                postfixList.push_back( *iter );
            else if( iter->name == "(" )
            {
                intermediateStack.push( *iter );
            }
            else if( iter->name == "BEGIN" )
            {
                intermediateStack.push( *iter );
                postfixList.push_back( *iter ); // push the BEGIN
            }
            else if( iter->name == "," )
            {
                while( (!intermediateStack.empty() ) &&
                    (intermediateStack.top().precedence <= 
                     iter->precedence ))
                {
                    postfixList.push_back( intermediateStack.top() );
                    intermediateStack.pop();
                }
                postfixList.push_back( *iter );
            }
            else if( iter->name == ")" )
            {
                while( !intermediateStack.empty() && (intermediateStack.top().name != "(" && intermediateStack.top().name != "BEGIN") )
                {
                    postfixList.push_back( intermediateStack.top() );
                    intermediateStack.pop();
                }
                intermediateStack.pop();
            }
            else if( iter->name == "[" )
            {
                intermediateStack.push( *iter );
            }
            else if( iter->name == "]" )
            {
                while( !intermediateStack.empty() && (intermediateStack.top().name != "[") )
                {
                    postfixList.push_back( intermediateStack.top() );
                    intermediateStack.pop();
                }
                if( intermediateStack.top().name == "[" )
                    intermediateStack.pop();
                postfixList.push_back( *iter ); // push the ]
            }
            else if( iter->name == "END" )
            {
                while( !intermediateStack.empty() && (intermediateStack.top().name != "(" && intermediateStack.top().name != "BEGIN") )
                {
                    postfixList.push_back( intermediateStack.top() );
                    intermediateStack.pop();
                }
                intermediateStack.pop(); // pop the BEGIN (it was already added)
                postfixList.push_back( *iter ); // push the END
            }
            else if( iter->type == Token::OPERATOR )
            {
                while( (!intermediateStack.empty() ) &&
                    (intermediateStack.top().precedence <= 
                     iter->precedence ))
                {
                    postfixList.push_back( intermediateStack.top() );
                    intermediateStack.pop();
                }
                intermediateStack.push( *iter );
            }
            else // Some kind of logical expression that should be at the front
                postfixList.push_back( *iter );
        }
        while( !intermediateStack.empty() )
        {
            postfixList.push_back( intermediateStack.top() );
            intermediateStack.pop();
        }

        // error-checking
//         if( postfixList.size() > 1 ) // Check for assignment to an immediate
//             if( postfixList)
        // check for hanging braces
        tokens = postfixList; // change from infix to postfix
    }

    void AddToken( std::list<Token> &tokens, const wxString &tokenName, std::stack<bool> &isFunction,
        Token &lastToken, bool &isValid, bool &firstToken )
    {
        Token t;

        t.name = tokenName;
        if( oList.opMap.count(tokenName) && 
            tokenName != "(" && tokenName != ")" &&
            tokenName != "[" && tokenName != "]" ) 
        {
            t.type = Token::OPERATOR;
        }
        else if( tokenName == "(" || tokenName == ")" ||
            tokenName == "[" || tokenName == "]")
        {
            t.type = Token::BRACE;
        }
        else if( tokenName.Lower().StartsWith("0x") )
        {
            t.type = Token::DECIMAL;
            t.v    = HexToDecimal<unsigned int>(tokenName);
        }
        else if( tokenName[0] >= '0' && tokenName[0] <= '9' )
        {
            t.type = Token::DECIMAL;
            t.v    = wxAtoi(tokenName);
        }
        else if( tokenName[0] == '"' )
        {
            wxString var = tokenName;
            var.Replace("\"", "", true); // strip quotes
            t.type = Token::DECIMAL;
            t.v    = Variable(var, true); // interpolated
        }
        else if( tokenName[0] == '\'' )
        {
            wxString var = tokenName;
            var.Replace("'", "", true); // strip quotes
            t.type = Token::DECIMAL;
            t.v   = Variable(var, false); // non-interpolated
        }
        else
        {
            t.type = Token::VARIABLE;
        }

        if( oList.opMap.count(t.name) > 0 )
            t.precedence = oList.opMap[t.name].precedence;

        // Check for invalid input and special characters
        if( lastToken.type == t.type && 
            (lastToken.type & (Token::DECIMAL|Token::VARIABLE)) > 0 )
        {
            isValid = false;
        }
        else if( lastToken.type == Token::OPERATOR && t.name == "-" )
        {
            t.name = "NEG";
        }
        else if( (lastToken.type == Token::VARIABLE || lastToken.name == "]" || lastToken.name == ")") && t.name == "++" )
        {
            t.name = "INC";
        }
        else if( (lastToken.type == Token::VARIABLE || lastToken.name == "]" || lastToken.name == ")") && t.name == "--" )
        {
            t.name = "DEC";
        }
        else if( (lastToken.type & (Token::OPERATOR|Token::BRACE)) > 0 && t.name == "&" )
        {
            t.name = "ADDRESS_OF";
        }
        else if( lastToken.type == Token::OPERATOR && t.name == "=" )
        {
            tokens.pop_back();
            std::list<Token> oldList = tokens;
            tokens.push_back(t); // push =
            std::list<Token>::const_iterator iter = oldList.begin();
            for( ; iter != oldList.end(); ++iter )
            {
                tokens.push_back(*iter);
            }
            t = lastToken; // we'll have this pushed on later
        }
        else if( (lastToken.type == Token::OPERATOR ||
            lastToken.type == Token::NONE ||
            lastToken.type == Token::BRACE) && 
            t.name == "*" && lastToken.name != ")" && lastToken.name != "]" && lastToken.name != "END" )
        {
            t.name = "DEREF";
        }

        // Special checks for function calls
        if( (!firstToken && t.name == "(") &&
            (lastToken.name == "INC" ||
            lastToken.name == "DEC" ||
            (lastToken.type != Token::OPERATOR && lastToken.name != "(" && lastToken.name != "BEGIN" && lastToken.name != "[")) )
        {
            t.name = "BEGIN";
            isFunction.push(true);
        }
        else if( t.name == "(" )
        {
            isFunction.push(false);
        }

        if( isFunction.size() > 0 && isFunction.top() && t.name == ")" )
        {
            // Add comma for function param if needed
            if( lastToken.name != "BEGIN" )
                tokens.push_back(Token(Token::OPERATOR, 18, ","));
            t.name = "END";
            isFunction.pop();
        }
        else if( t.name == ")" )
            isFunction.pop();

        // done checking invalid input / special characters
        if( oList.opMap.count(t.name) > 0 )
            t.precedence = oList.opMap[t.name].precedence;
        tokens.push_back(t);
        lastToken = t; // update the last token
        firstToken = false; // No longer on the first token
    }

    // performs the adding of a token to the list.  Splits up combined tokens as needed
    void AddTokens( std::list<Token> &tokens, const wxString &line )
    {
        wxString expLine = line;
        Token lastToken;
        bool isValid = true;
        bool firstToken = true;
        std::stack<bool> isFunction; // a stack used in determining if () is function or not

        // This RegEx expression parses all comments, symbols, strings, and so on
        wxRegEx expression( _("([/][/].*)|([\"][^\"]*[\"])|(['][^']*['])|([_a-zA-Z][_a-zA-Z0-9]*)|([0][x][0-9a-fA-F]+)|([0-9]+)|") +
            _("([=]{2})|([/]{2})|([.]{2})|([|]{2})|([&]{2})|([\\-]{2})|([+]{2})|") +
			_("([!<>][=])|") +
            _("([!%^&*()/=+|\\-.,><\"'\\[\\]])"), wxRE_ADVANCED ); // leave out the : because it isn't actually used
        while( expression.Matches(expLine) )
        {
            size_t start=0, len=0;
            if( expression.GetMatch(&start, &len) )
            {
                wxString tokenName = expLine.Mid(start, len);

                if( !tokenName.StartsWith("//") )
                {
                    // Special case for return statements
                    if( tokenName == "return" )
                    {
                        AddToken( tokens, "return", isFunction, lastToken, isValid, firstToken );
                        AddToken( tokens, "$_", isFunction, lastToken, isValid, firstToken );
                        AddToken( tokens, "=", isFunction, lastToken, isValid, firstToken );
                    }
                    else
                        AddToken( tokens, tokenName, isFunction, lastToken, isValid, firstToken );
                }
                expLine = expLine.Mid(start+len);
            }
            else
                break;
        }

        // Change from infix to postfix
        // Don't change function definition order
        if( tokens.size() > 0 && tokens.front().name != "def" )
            InfixToPostfix(tokens);
    }

    unsigned int GetAddressFromAoB( const std::vector<unsigned char> &arr, 
        unsigned int index, unsigned int min, unsigned int max )
    {
        Scanner s;
        wxString tempBuffer;
        std::string mask;
        char *aob;
        unsigned int result = 0;

        for( unsigned int i=0; i < arr.size(); ++i )
            tempBuffer += (char)arr[i];

        // remove white space
        tempBuffer.Replace(" ", "", true);

        // allocate space
        aob = new char[tempBuffer.size()/2];

        // For all characters, format the aob and mask properly
        for( unsigned int i=0; i < tempBuffer.size()/2; ++i )
        {
            bool isValid = true;
            char nibbles[3] = {0, 0, 0};

            nibbles[0] = tempBuffer[2*i];
            nibbles[1] = tempBuffer[2*i+1];

            for( int j=0; j < 2; ++j )
            {
                if( !((nibbles[j] >= '0' && nibbles[j] <= '9') ||
                    (nibbles[j] >= 'A' && nibbles[j] <= 'F') ||
                    (nibbles[j] >= 'a' && nibbles[j] <= 'f')) )
                    isValid = false;
            }

            if( isValid )
            {
                aob[i] = HexToDecimal<char>(nibbles);
                mask += 'x';
            }
            else // must be a mask
            {
                aob[i] = 0;
                mask += '?';
            }
        }

        result = s.GetAddressFromAoB( aob, mask, index, min, max );

        delete [] aob;

        return result;
    }

    void Worker::Work()
    {
        while( working )
        {
            Sleep(5);
            SendPackets();
        }
    }

    void Worker::AddPacket( const PacketProcessStruct &pkt )
    {
        if( m_mutex == NULL )
            return;

        boost::mutex::scoped_lock l(*m_mutex);
        packets.push_back(pkt);
    }

    void Worker::SendPackets()
    {
        if( m_mutex == NULL )
            return;

        boost::mutex::scoped_lock l(*m_mutex);
        if( working )
        {
            mutIsExecuting = true;
            std::list<PacketProcessStruct>::const_iterator citer = packets.begin();
            for( ; citer != packets.end(); ++citer )
                SendPacketToProcess( citer->pkt, citer->hIntf, citer->pktTyp, citer->s );
            packets.clear(); // flush the list
            mutIsExecuting = false;
        }
    }

    Script::Script()
    {
        enabled            = false;
        timeEventExecuting = false;
        drPtr              = NULL;
        scriptName         = "";
        lastTime           = GetMilliCount();
    }

    Script::Script(const wxString &scriptNam, Block b, const wxString &txt, Breakpoint *drP)
    {
        scriptName = scriptNam;
        block    = b;
        enabled  = false;
        text     = txt;
        drPtr    = drP;
        timeEventExecuting = false;
        lastTime = GetMilliCount();
        Init();
    }

    Script::Script(const Script &other)
    {
        block     = other.block;
        globals   = other.globals;
        functions = other.functions;
        events    = other.events;
        enabled   = other.enabled;
        text      = other.text;
        scriptName = other.scriptName;
        drPtr     = other.drPtr;
        timeEventExecuting = other.timeEventExecuting;
    }

    void Script::Init()
    {
        std::list<wxString> params;
        // Set default functions
        params.push_back("$0");
        params.push_back("$1");
        params.push_back("$2");
        params.push_back("$3");
        params.push_back("$4");
        params.push_back("$5");
        defaultFunctions["addressof"].params   = params;
        defaultFunctions["block"].params       = params;
        defaultFunctions["cleardr"].params     = params;
        defaultFunctions["clearint3"].params   = params;
        defaultFunctions["free"].params        = params;
        //defaultFunctions["getmodulehandle"].params = params;
        defaultFunctions["getprocaddress"].params = params;
        defaultFunctions["len"].params         = params;
        defaultFunctions["logpacketrecv"].params = params;
        defaultFunctions["logpacketsend"].params = params;
        defaultFunctions["logsend"].params     = params;
        defaultFunctions["malloc"].params      = params;
        defaultFunctions["messagebox"].params  = params;
        defaultFunctions["messageboxa"].params = params;
        defaultFunctions["read"].params        = params;
        defaultFunctions["recv"].params        = params;
        defaultFunctions["scan"].params        = params;
        defaultFunctions["send"].params        = params;
        defaultFunctions["setdr"].params       = params;
        defaultFunctions["sethookrecv"].params = params;
        defaultFunctions["sethooksend"].params = params;
        defaultFunctions["setpacketrecv"].params = params;
        defaultFunctions["setpacketsend"].params = params;
        defaultFunctions["setint3"].params     = params;
        defaultFunctions["vtoa"].params        = params;
        defaultFunctions["write"].params       = params;
        functions = defaultFunctions;

        // Set default variables
        globals["cdecl"]           = CC_CDECL;
        globals["msfastcall"]      = CC_MSFASTCALL;
        globals["borlandfastcall"] = CC_BORLANDFASTCALL;
        globals["syscall"]         = CC_SYSCALL;
        globals["gccthiscall"]     = CC_GCCTHISCALL;
        globals["msthiscall"]      = CC_MSTHISCALL;
        globals["stdcall"]         = CC_STDCALL;
        globals["calling_convention"] = CC_STDCALL;

        ExtractFunctions();
        ExtractEvents();

        std::map<wxString, Variable *> locals;
        Block b = block; // make a copy so it can be used by ExecuteBlock for recursion
        ExecuteBlock(b, locals);
    }

    // Extract functions from the block variable.  Should only be called once.
    void Script::ExtractFunctions()
    {
        std::list<Container>::iterator iter = block.containers.begin();
        for( ; iter != block.containers.end(); ++iter )
        {
            if( iter->type == Container::TYPE_LINE &&
                iter->tokens.front().name == "def" )
            {
                std::list<Container>::iterator tmpIter = iter--;
                std::list<Container>::iterator tmpIterCopy = tmpIter; // used in case tmpIter is incremented
                Line *l = (Line *)&*tmpIter;
                std::list<Token>::iterator tokIter = l->tokens.begin();
                wxString functionName;

                // If tokens contain a function name, create the function
                if( l->tokens.size() > 1 )
                {
                    functionName = (++tokIter)->name;
                    ++tmpIter; // go to the function's block

                    // Make the function
                    if( tmpIter != block.containers.end() && 
                        tmpIter->type == Container::TYPE_BLOCK )
                    {
                        functions[functionName] = Function(*(Block *)&*tmpIter);

                        // Add function's parameters
                        for( ; tokIter != l->tokens.end(); ++tokIter )
                        {
                            if( tokIter->type == Token::VARIABLE && tokIter->name != functionName &&
                                tokIter->name != ":" )
                            {
                                functions[functionName].params.push_back(tokIter->name);
                            }
                        }

                        block.containers.erase(tmpIter); // erase the block
                    }
                }
                block.containers.erase(tmpIterCopy); // erase the line
            }
        }
    }

    // TODO:  Correct / finish this.  It's basically a copy of ExtractFunctions atm
    void Script::ExtractEvents()
    {
        std::set<wxString> eventNames;
        eventNames.insert("s");
        eventNames.insert("r");
        eventNames.insert("time");
        eventNames.insert("k");
        eventNames.insert("enable");
        eventNames.insert("disable");

        std::list<Container>::iterator iter = block.containers.begin();
        for( ; iter != block.containers.end(); ++iter )
        {
            // for all lines
            if( iter->type == Container::TYPE_LINE &&
                eventNames.count(iter->tokens.front().name) ) // if line is an event
            {
                std::list<Container>::iterator tmpIter = iter--; // iter-- sets iter before this block
                std::list<Container>::iterator tmpIterCopy = tmpIter; // used in case tmpIter is incremented
                Line *l = (Line *)&*tmpIter; // get a pointer to this line
                std::list<Token>::iterator tokIter = l->tokens.begin(); // Get the tokens in this line
                wxString expression = "";

                // If tokens contain an expression, set the expression
                // TODO:  Allow dynamic events by adding a mask to the Variable and adding
                //        an operator to compare the expression
                if( l->tokens.size() > 1 )
                {
                    expression = (++tokIter)->name;
                }

                expression.MakeLower(); // expression should be lowercase
                expression.Replace( " ", "", true ); // replace all white space
                expression.Replace( "\"", "", true ); // replace quotes

                ++tmpIter; // go to the event's block

                // Make the event
                if( tmpIter != block.containers.end() && 
                    tmpIter->type == Container::TYPE_BLOCK )
                {
                    events.push_back(Event( 
                        tmpIterCopy->tokens.front().name, *(Block *)&*tmpIter, expression) );
                    block.containers.erase(tmpIter); // erase the block
                }

                block.containers.erase(tmpIterCopy); // erase the line
            }
        }
    }

    void Script::ExecuteFunction( const wxString &name, std::map<wxString, TokenEval> &parentLocals )
    {
        // Prevent crashes from unallocated variables
        Variable trash0=0, trash1=0, trash2=0, trash3=0, trash4=0, trash5=0;
        
        if( parentLocals.count("$0") == 0 )
        {
            parentLocals["$0"].vPtr = &trash0;
            parentLocals["$0"].v    = trash0;
        }
        if( parentLocals.count("$1") == 0 )
        {
            parentLocals["$1"].vPtr = &trash1;
            parentLocals["$1"].v    = trash1;
        }
        if( parentLocals.count("$2") == 0 )
        {
            parentLocals["$2"].vPtr = &trash2;
            parentLocals["$2"].v    = trash2;
        }
        if( parentLocals.count("$3") == 0 )
        {
            parentLocals["$3"].vPtr = &trash3;
            parentLocals["$3"].v    = trash3;
        }
        if( parentLocals.count("$4") == 0 )
        {
            parentLocals["$4"].vPtr = &trash4;
            parentLocals["$4"].v    = trash4;
        }
        if( parentLocals.count("$5") == 0 )
        {
            parentLocals["$5"].vPtr = &trash5;
            parentLocals["$5"].v    = trash5;
        }

        if( name == "addressof" ) // Get the address of a variable's data
        {
            *(parentLocals["$_"].vPtr) = (unsigned int)parentLocals["$0"].v.GetAddress();
        }
        else if( name == "block" )
        {
            // Not yet implemented
        }
        else if( name == "cleardr" ) // Clear a DR register.  Params:  (drToUse)
        {
            // Note that we don't know which hooked function we need to clear,
            // so we will leave the old location as-is for now.
            DWORD drToUse = parentLocals["$0"].v.AsValue();

            drPtr->ClearHardwareBreakpoint(drToUse);
        }
        else if( name == "clearint3" ) // Clear a software breakpoint.  Params:  (hookLocation)
        {
            DWORD hookLocation = parentLocals["$0"].v.AsValue();

            drPtr->ClearSoftwareBreakpoint( hookLocation );

            if( hookedFunctions.count(hookLocation) > 0 )
                hookedFunctions.erase(hookLocation);
        }
        else if( name == "free" )
        {
            free((void *)parentLocals["$0"].v.AsValue());
        }
        // getprocaddress(dll, functionName)
        else if( name == "getprocaddress" )
        {
            wxString dll = parentLocals["$0"].v.AsASCIIString();
            wxString functionName = parentLocals["$1"].v.AsASCIIString();

            HMODULE hModule = GetModuleHandleA(dll.mb_str());
            if (hModule == NULL) {
                hModule = LoadLibraryA(dll.mb_str());
            }

            if (hModule != NULL) {
                *(parentLocals["$_"].vPtr) = 
                    (DWORD)GetProcAddress( hModule, functionName.mb_str() );
            } else {
                *(parentLocals["$_"].vPtr) = 0;
            }
        }
        else if( name == "len" )
        {
            *(parentLocals["$_"].vPtr) = parentLocals["$0"].v.GetSize();
        }
        // logpacketrecv(headerPointer, dataPointer, lengthPointer, lengthCorrection, returnAddress)
        else if( name == "logpacketrecv" )
        {
            unsigned int displayStr = parentLocals["$5"].v.AsValue();
            wxString str = parentLocals["$5"].v.AsASCIIString();
            wxCharBuffer buffer = str.ToUTF8();
            *(parentLocals["$_"].vPtr) = RiPEApp::GetRiPE()->hookIntf->LogPacket( 
                (short *)parentLocals["$0"].v.AsValue(),
                (char *)parentLocals["$1"].v.AsValue(),
                (DWORD *)parentLocals["$2"].v.AsValue(),
                PACKET_STREAM_RECV,
                parentLocals["$4"].v.AsValue(),
                parentLocals["$3"].v.AsValue(),
                (displayStr ? buffer.data() : NULL) );
        }
        // logpacketsend(headerPointer, dataPointer, lengthPointer, lengthCorrection, returnAddress)
        else if( name == "logpacketsend" )
        {
            unsigned int displayStr = parentLocals["$5"].v.AsValue();
            wxString str = _("");
            Variable arr = parentLocals["$5"].v.AsArray();
            for( unsigned int i=0; i < arr.GetSize(); ++i )
                str += (char)arr[i];
            wxCharBuffer buffer = str.ToUTF8();
            *(parentLocals["$_"].vPtr) = RiPEApp::GetRiPE()->hookIntf->LogPacket( 
                (short *)parentLocals["$0"].v.AsValue(),
                (char *)parentLocals["$1"].v.AsValue(),
                (DWORD *)parentLocals["$2"].v.AsValue(),
                PACKET_STREAM_SEND,
                parentLocals["$4"].v.AsValue(),
                parentLocals["$3"].v.AsValue(),
                (displayStr ? buffer.data() : NULL) );
        }
        //logsend(address, size, packet)
        else if( name == "logsend" )
        {
            wxListCtrl *list = RiPEApp::GetRiPE()->ListCtrl1;
            long index = list->GetItemCount();
            wxString address = _("");
            Variable arr = parentLocals["$0"].v.AsArray();
            for( unsigned int i=0; i < arr.GetSize(); ++i )
                address += (char)arr[i];
            wxString size = _("");
            arr = parentLocals["$1"].v.AsArray();
            for( unsigned int i=0; i < arr.GetSize(); ++i )
                size += (char)arr[i];
            wxString packet = _("");
            arr = parentLocals["$2"].v.AsArray();
            for( unsigned int i=0; i < arr.GetSize(); ++i )
                packet += (char)arr[i];

            index      = list->InsertItem(index, address);
            list->SetItem(index, 1, size);
            list->SetItem(index, 2, packet);
            list->SetColumnWidth(2, wxLIST_AUTOSIZE);
        }
        else if( name == "malloc" )
        {
            unsigned int numBytes = parentLocals["$0"].v.AsValue();
            DWORD oldProtection = 0;
            void *addr = malloc(numBytes);
            *(parentLocals["$_"].vPtr) = (unsigned int)addr;
            VirtualProtect(addr, numBytes, PAGE_EXECUTE_READWRITE, &oldProtection );
        }
        else if( name == "messagebox" )
        {
            wxMessageBox(parentLocals["$0"].v.AsString());
        }
        else if( name == "messageboxa" ) // Message box ASCII
        {
            wxString message = _("");
            Variable arr = parentLocals["$0"].v.AsArray();
            for( unsigned int i=0; i < arr.GetSize(); ++i )
                message += (char)arr[i];
            wxMessageBox(message);
        }
        else if( name == "read" )
        {
            if( parentLocals.count("$_") )
                *(parentLocals["$_"].vPtr) = ReadAddress((LPVOID)parentLocals["$0"].v.AsValue());
        }
        else if( name == "recv" )
        {
            m_worker.AddPacket(PacketProcessStruct(std::string(parentLocals["$0"].v.AsString().mb_str()), hookIntf, __RECV_PACKET, 0));
        }
        else if( name == "scan" )
        {
            *(parentLocals["$_"].vPtr) = GetAddressFromAoB( parentLocals["$0"].v.AsArray(), parentLocals["$1"].v.AsValue(), parentLocals["$2"].v.AsValue(), parentLocals["$3"].v.AsValue() );
        }
        else if( name == "send" )
        {
            m_worker.AddPacket(PacketProcessStruct(std::string(parentLocals["$0"].v.AsString().mb_str()), hookIntf, __SEND_PACKET, 0));
        }
        else if( name == "setdr" ) // Set a DR register.  Params:  (hookLocation, function, drToUse)
        {
            wxString funcName = *(wxString *)(parentLocals["$1"].v.AsValue());

            hookedFunctions[parentLocals["$0"].v.AsValue()] = 
                std::make_pair<wxString, wxString>(scriptName, funcName);
            drPtr->SetHardwareBreakpoint(
                parentLocals["$0"].v.AsValue(),
                NULL,
                BREAK_ON_BYTE,
                BREAK_ON_EXECUTION,
                parentLocals["$2"].v.AsValue(),
                false );
        }
        else if( name == "sethookrecv" )
        {
            hookRecvFunction = *(wxString *)(parentLocals["$0"].v.AsValue());
            RiPEApp::GetRiPE()->hookIntf->SetHookRecvScript( new wxString(scriptName) );
        }
        else if( name == "sethooksend" )
        {
            hookSendFunction = *(wxString *)(parentLocals["$0"].v.AsValue());
            RiPEApp::GetRiPE()->hookIntf->SetHookSendScript( new wxString(scriptName) );
        }
        else if( name == "setint3" ) // Sets a software breakpoint.  Params:  (hookLocation, function)
        {
            wxString funcName = *(wxString *)(parentLocals["$1"].v.AsValue());

            hookedFunctions[parentLocals["$0"].v.AsValue()] = 
                std::make_pair<wxString, wxString>(scriptName, funcName);
            drPtr->SetSoftwareBreakpoint(
                parentLocals["$0"].v.AsValue(),
                NULL );
        }
        else if( name == "setpacketrecv" )
        {
            packetRecvFunction = *(wxString *)(parentLocals["$0"].v.AsValue());
            RiPEApp::GetRiPE()->hookIntf->SetPacketRecvScript( new wxString(scriptName) );
        }
        else if( name == "setpacketsend" )
        {
            packetSendFunction = *(wxString *)(parentLocals["$0"].v.AsValue());
            RiPEApp::GetRiPE()->hookIntf->SetPacketSendScript( new wxString(scriptName) );
        }
        else if( name == "vtoa" )
        {
            std::vector<unsigned char> arr = parentLocals["$0"].v.AsArray();
            wxString delimiter = parentLocals["$1"].v.AsValue() == 0 ? " " : 
                    parentLocals["$1"].v.AsASCIIString();
            wxString result = "";
            std::vector<unsigned char>::iterator iter = arr.begin();
            for( ; iter != arr.end(); )
            {
                result += wxString::Format("%02X", *iter);
                if (++iter != arr.end()) {
                    result += delimiter;
                }
            }
            *(parentLocals["$_"].vPtr) = Variable(result, false);
        }
        else if( name == "write" )
        {
            std::vector<unsigned char> arr = parentLocals["$1"].v.AsArray();
            WriteAddress( (LPVOID)parentLocals["$0"].v.AsValue(), arr.data(), 
                    arr.size()/*parentLocals["$2"].v.AsValue()*/ );
        }
    }

    // Executes an arbitrary function call.
    void Script::ExecuteNearCall( unsigned long addr, 
        std::map<wxString, TokenEval> &parentLocals, std::vector<TokenEval> &callStack )
    {
        unsigned int callingConvention = parentLocals["calling_convention"].v.AsValue();
        size_t paramCount = callStack.size(); // the number of parameters
        unsigned int arg  = paramCount*4; // size of arguments passed
        unsigned int *args = new unsigned int[paramCount]; // keep track of arguments to pass in
        unsigned int *argsIter = args;
        unsigned int result = 0;

        // temp values used in C++ code
        unsigned int eaxArg = 0;
        unsigned int edxArg = 0;
        unsigned int ecxArg = 0;

        // Populate args (they're backwards)
        for( unsigned int i=0; i < callStack.size(); ++i )
            args[i] = callStack[callStack.size()-i-1].v.AsValue();

        switch( callingConvention )
        {
        case CC_CDECL:
            argsIter = args+paramCount-1; // set to end
            __asm
            {
                pushad
                mov  esi,argsIter
                mov  ecx,paramCount
                jecxz DoneLooping    // No params to push
PushValues:
                push dword ptr [esi] // push the variable
                sub  esi,4           // move to next value
                loop PushValues
DoneLooping:
                call addr            // call the function
                mov  result,eax      // assign result to return value
                add  esp,arg         // fix the stack
                popad
            }
            break;

        case CC_MSFASTCALL:
            argsIter = args+paramCount-1; // set to end

            if( paramCount >= 1 )
                ecxArg = args[0];
            if( paramCount >= 2 )
                edxArg = args[1];

            __asm
            {
                pushad
                mov  esi,argsIter
                mov  ecx,paramCount
                sub  ecx,2
                cmp  ecx,0
                jle  DoneLooping2
PushValues2:
                push dword ptr [esi]
                sub  esi,4
                loop PushValues2
DoneLooping2:
                mov  ecx,ecxArg
                mov  edx,edxArg
                call addr
                mov  result,eax      // assign result to return value
                popad
            }

            break;

        case CC_BORLANDFASTCALL:
            argsIter = args+3; // set to beginning

            if( paramCount >= 1 )
                eaxArg = args[0];
            if( paramCount >= 2 )
                edxArg = args[1];
            if( paramCount >= 3 )
                ecxArg = args[2];

            __asm
            {
                pushad
                mov  esi,argsIter
                mov  ecx,paramCount
                sub  ecx,3
                cmp  ecx,0
                jle  DoneLooping3
PushValues3:
                push dword ptr [esi]
                add  esi,4
                loop PushValues3
DoneLooping3:
                mov  eax,eaxArg
                mov  edx,edxArg
                mov  ecx,ecxArg
                call addr
                mov  result,eax      // assign result to return value
                popad
            }
            break;

        case CC_SYSCALL:
            argsIter = args+paramCount-1; // set to end
            __asm
            {
                pushad
                mov  esi,argsIter
                mov  ecx,paramCount
                jecxz DoneLooping4    // No params to push
PushValues4:
                push dword ptr [esi] // push the variable
                sub  esi,4           // move to next value
                loop PushValues4
DoneLooping4:
                mov  eax,paramCount  // set the number of parameters in al
                call addr            // call the function
                add  esp,arg         // fix the stack  TODO:  Is this correct?!
                mov  result,eax      // assign result to return value
                popad
            }
            break;

        case CC_GCCTHISCALL: // identical to cdecl :O
            argsIter = args+paramCount-1; // set to end
            __asm
            {
                pushad
                mov  esi,argsIter
                mov  ecx,paramCount
                jecxz DoneLooping5    // No params to push
PushValues5:
                push dword ptr [esi] // push the variable
                sub  esi,4           // move to next value
                loop PushValues5
DoneLooping5:
                call addr            // call the function
                add  esp,arg         // fix the stack
                mov  result,eax      // assign result to return value
                popad
            }
            break;

        case CC_MSTHISCALL:
            argsIter = args+paramCount-1; // set to end

            if( paramCount >= 1 )
                ecxArg = args[0];

            __asm
            {
                pushad
                mov  esi,argsIter
                mov  ecx,paramCount
                dec  ecx
                cmp  ecx,0
                jle  DoneLooping6
PushValues6:
                push dword ptr [esi]
                sub  esi,4
                loop PushValues6
DoneLooping6:
                mov  ecx,ecxArg
                call addr
                mov  result,eax      // assign result to return value
                popad
            }
            break;

        case CC_STDCALL: // fall through (default)
        default: // stdcall
            argsIter = args+paramCount-1; // set to end

            __asm
            {
                pushad
                mov   esi,argsIter
                mov   ecx,paramCount
                jecxz DoneLooping7
PushValues7:
                push  dword ptr [esi]
                sub   esi,4
                loop  PushValues7
DoneLooping7:
                call  addr
                mov  result,eax      // assign result to return value
                popad
            }
            break;
        }

        // Assign the result
        *(parentLocals["$_"].vPtr) = result;

        delete [] args;
    }

    // Executes an arbitrary function call.
    void Script::ExecuteSpoofReturnCall( unsigned long addr, 
        std::map<wxString, TokenEval> &parentLocals, std::vector<TokenEval> &callStack )
    {
        // Nothing to do
        if( callStack.size() < 2 )
            return;

        unsigned int callingConvention = parentLocals["calling_convention"].v.AsValue();
        size_t paramCount = callStack.size()-2; // the number of parameters
        unsigned int arg  = paramCount*4; // size of arguments passed
        unsigned int *args = new unsigned int[paramCount]; // keep track of arguments to pass in
        unsigned int *argsIter = args;
        unsigned int result = 0;
        addr = callStack[callStack.size()-1].v.AsValue();
        unsigned int fakeReturn = callStack[callStack.size()-2].v.AsValue();

        // temp values used in C++ code
        unsigned int eaxArg = 0;
        unsigned int edxArg = 0;
        unsigned int ecxArg = 0;

        // Populate args (they're backwards)
        for( unsigned int i=0; i < callStack.size()-2; ++i )
            args[i] = callStack[callStack.size()-i-1-2].v.AsValue();

        switch( callingConvention )
        {
        case CC_CDECL:
            argsIter = args+paramCount-1; // set to end
            __asm
            {
                pushad
                mov  esi,argsIter

                push ReturnLocation

                mov  ecx,paramCount
                jecxz DoneLooping    // No params to push
PushValues:
                push dword ptr [esi] // push the variable
                sub  esi,4           // move to next value
                loop PushValues
DoneLooping:
                push fakeReturn     // perform a fake call
                jmp addr            // call the function
ReturnLocation:
                mov  result,eax      // assign result to return value
                add  esp,arg         // fix the stack
                popad
            }
            break;

        case CC_MSFASTCALL:
            argsIter = args+paramCount-1; // set to end

            if( paramCount >= 1 )
                ecxArg = args[0];
            if( paramCount >= 2 )
                edxArg = args[1];

            __asm
            {
                pushad
                mov  esi,argsIter

                push ReturnLocation2

                mov  ecx,paramCount
                sub  ecx,2
                cmp  ecx,0
                jle  DoneLooping2
PushValues2:
                push dword ptr [esi]
                sub  esi,4
                loop PushValues2
DoneLooping2:
                mov  ecx,ecxArg
                mov  edx,edxArg
                push fakeReturn
                jmp  addr
ReturnLocation2:
                mov  result,eax      // assign result to return value
                popad
            }

            break;

        case CC_BORLANDFASTCALL:
            argsIter = args+3; // set to beginning

            if( paramCount >= 1 )
                eaxArg = args[0];
            if( paramCount >= 2 )
                edxArg = args[1];
            if( paramCount >= 3 )
                ecxArg = args[2];

            __asm
            {
                pushad
                mov  esi,argsIter

                push ReturnLocation3

                mov  ecx,paramCount
                sub  ecx,3
                cmp  ecx,0
                jle  DoneLooping3
PushValues3:
                push dword ptr [esi]
                add  esi,4
                loop PushValues3
DoneLooping3:
                mov  eax,eaxArg
                mov  edx,edxArg
                mov  ecx,ecxArg
                push fakeReturn
                jmp addr
ReturnLocation3:
                mov  result,eax      // assign result to return value
                popad
            }
            break;

        case CC_SYSCALL:
            argsIter = args+paramCount-1; // set to end
            __asm
            {
                pushad
                mov  esi,argsIter

                push ReturnLocation4

                mov  ecx,paramCount
                jecxz DoneLooping4    // No params to push
PushValues4:
                push dword ptr [esi] // push the variable
                sub  esi,4           // move to next value
                loop PushValues4
DoneLooping4:
                mov  eax,paramCount  // set the number of parameters in al
                push fakeReturn
                jmp addr            // call the function
ReturnLocation4:
                add  esp,arg         // fix the stack  TODO:  Is this correct?!
                mov  result,eax      // assign result to return value
                popad
            }
            break;

        case CC_GCCTHISCALL: // identical to cdecl :O
            argsIter = args+paramCount-1; // set to end
            __asm
            {
                pushad
                mov  esi,argsIter

                push ReturnLocation5

                mov  ecx,paramCount
                jecxz DoneLooping5    // No params to push
PushValues5:
                push dword ptr [esi] // push the variable
                sub  esi,4           // move to next value
                loop PushValues5
DoneLooping5:
                push fakeReturn
                jmp addr            // call the function
ReturnLocation5:
                add  esp,arg         // fix the stack
                mov  result,eax      // assign result to return value
                popad
            }
            break;

        case CC_MSTHISCALL:
            argsIter = args+paramCount-1; // set to end

            if( paramCount >= 1 )
                ecxArg = args[0];

            __asm
            {
                pushad
                mov  esi,argsIter

                push ReturnLocation6

                mov  ecx,paramCount
                dec  ecx
                cmp  ecx,0
                jle  DoneLooping6
PushValues6:
                push dword ptr [esi]
                sub  esi,4
                loop PushValues6
DoneLooping6:
                mov  ecx,ecxArg
                push fakeReturn
                jmp addr
ReturnLocation6:
                mov  result,eax      // assign result to return value
                popad
            }
            break;

        case CC_STDCALL: // fall through (default)
        default: // stdcall
            argsIter = args+paramCount-1; // set to end

            __asm
            {
                pushad
                mov   esi,argsIter

                push ReturnLocation7

                mov   ecx,paramCount
                jecxz DoneLooping7
PushValues7:
                push  dword ptr [esi]
                sub   esi,4
                loop  PushValues7
DoneLooping7:
                push fakeReturn
                jmp  addr
ReturnLocation7:
                mov  result,eax      // assign result to return value
                popad
            }
            break;
        }

        // Assign the result
        *(parentLocals["$_"].vPtr) = result;

        delete [] args;
    }

    TokenEval Script::ExecuteExpression( Line *line, std::map<wxString, Variable *> &parentLocals, 
        std::map<wxString, Variable> &myLocals, int depth )
    {
        std::stack<TokenEval> varStack;
        std::vector<TokenEval> callStack;
        std::stack<FunctionCall> functionCallStack;
        Variable result = 0; // a variable used in function calls.  Should persist throughout the line

        // for all tokens
        std::list<Token>::iterator iter = line->tokens.begin();
        for( ; iter != line->tokens.end(); ++iter )
        {
            std::list<Token>::iterator nextIter = iter;
            ++nextIter;

            // If we're pushing a function as an argument
            // Otherwise, the function will end up getting pushed
            // as a normal varStack argument
            if( (nextIter == line->tokens.end() || nextIter->name != "BEGIN") &&
                functions.count(iter->name) > 0 )
            {
                // Functions can be passed as arguments.  The way to determine
                // whether this is a function call or an argument is to see if
                // the next iterator is a "BEGIN" brace.  If it is, then it's
                // a function call.
                varStack.push( TokenEval(
                    Variable((DWORD)&(functions.find(iter->name)->first)), // pointer to the name
                    NULL, // A pointer doesn't make sense in this context
                    TokenEval::VARIABLE) );
            }
            // If operator is return, then end the function
            else if( iter->name == "return" )
            {
                iter = line->tokens.end();
                --iter;
            }
            // If operator is function call
            else if( iter->name == "END" && functionCallStack.size() > 0 )
            {
                // Add the locals, from broad to specific
                wxString functionName = functionCallStack.top().name;
                std::map<wxString, TokenEval> functionTokenLocals;
                std::map<wxString, Variable *> functionLocals = parentLocals;

                std::map<wxString, Variable>::iterator globalIter = globals.begin();
                for( ; globalIter != globals.end(); ++globalIter )
                {
                    functionTokenLocals[globalIter->first] = TokenEval(globalIter->second, &globalIter->second);
                }

                std::map<wxString, Variable *>::iterator iter5 = parentLocals.begin();
                for( ; iter5 != parentLocals.end(); ++iter5 )
                {
                    functionTokenLocals[iter5->first] = TokenEval(*(iter5->second), iter5->second);
                }

                std::map<wxString, Variable>::iterator iter2 = myLocals.begin();
                for( ; iter2 != myLocals.end(); ++iter2 )
                {
                    functionLocals[iter2->first] = &iter2->second;
                    functionTokenLocals[iter2->first] = TokenEval(iter2->second, &iter2->second);
                }

                // Set the params
                std::vector<TokenEval>::iterator iter3 = callStack.begin();

                if( functions.count(functionName) > 0 )
                {
                    std::list<wxString>::iterator iter4 = functions[functionName].params.begin();
                    for( ; iter3 != callStack.end() &&
                        iter4 != functions[functionName].params.end(); ++iter3, ++iter4 )
                    {
                        functionLocals[*iter4]      = iter3->vPtr == NULL ? &iter3->v : iter3->vPtr;
                        functionTokenLocals[*iter4] = iter3->vPtr == NULL ? TokenEval(iter3->v, &iter3->v) : TokenEval(iter3->v, iter3->vPtr);
                    }
                }
                else // not an actual function, so make stuff up
                {
                    for( int i=0; iter3 != callStack.end(); ++iter3, ++i )
                    {
                        functionLocals[wxString::Format("$%i", i)] = iter3->vPtr == NULL ? &iter3->v : iter3->vPtr;
                        functionTokenLocals[wxString::Format("$%i", i)] = iter3->vPtr == NULL ? TokenEval(iter3->v, &iter3->v) : TokenEval(iter3->v, iter3->vPtr);
                    }
                }

                // Set return address
                functionLocals["$_"] = &result;
                functionTokenLocals["$_"] = TokenEval(result, &result);

                Block functionBlock;
                if( functions.count(functionName) > 0 )
                    functionBlock = functions[functionName].block; // make a copy

                if( defaultFunctions.count(functionName) > 0 )
                {
                    ExecuteFunction(functionName, functionTokenLocals);
                }
                else if( functionName == "spoofreturn" ) // Special case for spoofing the return value
                {
                    ExecuteSpoofReturnCall(functionCallStack.top().location, functionTokenLocals, callStack);
                }
                else if(functions.count(functionName) == 0) // If near call
                {
                    ExecuteNearCall(functionCallStack.top().location, functionTokenLocals, callStack);
                }
                // Else call the function normally
                else
                    ExecuteBlock(functionBlock, functionLocals, depth+1);

                // Push the return value
                varStack.push(TokenEval(result, &result));

                // Reset callStack to what it was before the function call
                callStack = functionCallStack.top().callStack;
                functionCallStack.pop();
            }
            // Function call
            else if( iter->name == "BEGIN" )
            {
                nextIter = iter;
                if( nextIter-- != line->tokens.begin() )
                {
                    FunctionCall fc;
                    fc.name = nextIter->name;
                    fc.callStack = callStack;

                    if( !varStack.empty() )
                    {
                        fc.location = varStack.top().v.AsValue();
                        varStack.pop();
                    }
                    functionCallStack.push( fc );
                }
            }
            // if the token is an operator
            else if( oList.opMap.count(iter->name) > 0 )
            {
                oList.opMap[iter->name].expression(varStack, callStack);
            }
            else if( iter->type == Token::DECIMAL )
            {
                varStack.push( TokenEval(iter->v, NULL, TokenEval::VARIABLE) );
            }
            else if( iter->type == Token::VARIABLE ) // token is a variable
            {
                // Make sure this isn't a function first
                if( functions.count(iter->name) == 0 )
                {
                    if( myLocals.count(iter->name) > 0 ) // my local
                    {
                        varStack.push( TokenEval(myLocals[iter->name], &myLocals[iter->name], TokenEval::VARIABLE) );
                    }
                    else if( parentLocals.count(iter->name) > 0 ) // if a local
                    {
                        varStack.push( TokenEval(*parentLocals[iter->name], parentLocals[iter->name], TokenEval::VARIABLE) );
                    }
                    else if( globals.count(iter->name) > 0 ) // if a global
                    {
                        varStack.push( TokenEval(globals[iter->name], &globals[iter->name], TokenEval::VARIABLE) );
                    }
                    else // a new local
                    {
                        if( depth == 0 )
                            varStack.push( TokenEval(globals[iter->name], &globals[iter->name], TokenEval::VARIABLE) );
                        else
                            varStack.push( TokenEval(myLocals[iter->name], &myLocals[iter->name], TokenEval::VARIABLE) );
                    }
                }
                else // special case for functions
                {
                    varStack.push( TokenEval(0, NULL, TokenEval::VARIABLE) );
                }
            }
        }

        return varStack.size() > 0 ? varStack.top() : TokenEval(0, NULL);
    }

    bool Script::ExecuteLineSEH( Line *line, std::map<wxString, Variable *> &parentLocals,
            std::map<wxString, Variable> &myLocals )
    {
        bool canEnterBlock = false;

        // Check to see if logical (block) expression (if, for, s, r, time, etc.)
        if( line->tokens.size() > 0 && 
            (line->tokens.front().name == "if" ||
            line->tokens.front().name == "while" ||
            line->tokens.front().name == "for") )
        {
            line->tokens.pop_front(); // pop the "if"
            canEnterBlock = ExecuteExpression( line, parentLocals, myLocals ).v != 0;
        }
        else 
            ExecuteExpression( line, parentLocals, myLocals );

        return canEnterBlock;
    }

    bool Script::ExecuteLine(Block &b, std::map<wxString, Variable *> &parentLocals, 
        std::map<wxString, Variable> &myLocals, int depth )
    {
        Line *line = (Line *)&(b.containers.front());
        bool canEnterBlock = false;

        __try
        {
            canEnterBlock = ExecuteLineSEH(line, parentLocals, myLocals);
        }
        __except(EXCEPTION_EXECUTE_HANDLER)
        {
            // Do nothing.  This makes sure we don't crash on things like:  a = 0 / 0
        }

        b.containers.pop_front();

        return canEnterBlock;
    }

    void Script::ExecuteBlock(Block &b, std::map<wxString, Variable *> &parentLocals, int depth)
    {
        std::map<wxString, Variable> myLocals;
        bool canEnterBlock = false;
        bool isLoop = false; // keeps track of whether the last line was a loop (for, while)
        Line lastLine;  // keeps track of the last line to execute in case we need to execute it again

        // for all containers in b
        while( b.containers.size() > 0 )
        {
            if( b.containers.front().type == Container::TYPE_LINE )
            {
                std::map<wxString, Variable *> tempLocals = parentLocals;
                std::map<wxString, Variable>::iterator iter = myLocals.begin();
                for( ; iter != myLocals.end(); ++iter )
                    tempLocals[iter->first] = &iter->second;

                if( b.containers.front().tokens.size() > 0 &&
                    (b.containers.front().tokens.front().name == "while" ||
                    b.containers.front().tokens.front().name == "for") )
                {
                    isLoop = true;
                }
                else
                    isLoop = false;

                lastLine      = *(Line *)&b.containers.front();
                canEnterBlock = ExecuteLine( b, tempLocals, myLocals, depth );
            }
            else if( b.containers.front().type == Container::TYPE_BLOCK && canEnterBlock )
            {
                Block lastBlock = *(Block *)&b.containers.front();
                canEnterBlock = false;

                // Add the locals
                std::map<wxString, Variable *> newLocals = parentLocals;
                std::map<wxString, Variable>::iterator iter2 = myLocals.begin();
                for( ; iter2 != myLocals.end(); ++iter2 )
                    newLocals[iter2->first] = &iter2->second;
                ExecuteBlock( *(Block *)&b.containers.front(), newLocals, depth+1 );

                b.containers.pop_front(); // blocks don't pop themselves

                if( isLoop )
                {
                    b.containers.push_front(lastBlock); // ExecuteBlock messes up our block, so push again
                    b.containers.push_front(lastLine);
                }
            }
            else // an error has occurred.  Pop to prevent an infinite loop
            {
                b.containers.pop_front();
            }
            // popped from ExecuteLine
        }
    }

    void Script::ProcessDisableEvent()
    {
        std::map<wxString, Variable *> myLocals;

        // Add globals to our variables
        std::map<wxString, Variable>::iterator iter = globals.begin();
        for( ; iter != globals.end(); ++iter )
            myLocals[iter->first] = &iter->second;

        // Execute all events that meet the criteria
        std::list<Event>::const_iterator citer = events.begin();
        for( ; citer != events.end(); ++citer )
        {
            if( citer->type == "disable" ) // if event is an enable event
            {
                Block b = citer->block; // make a copy of the block
                ExecuteBlock( b, myLocals );
            }
        }
    }

    void Script::ProcessEnableEvent()
    {
        std::map<wxString, Variable *> myLocals;

        // Add globals to our variables
        std::map<wxString, Variable>::iterator iter = globals.begin();
        for( ; iter != globals.end(); ++iter )
            myLocals[iter->first] = &iter->second;

        // Execute all events that meet the criteria
        std::list<Event>::const_iterator citer = events.begin();
        for( ; citer != events.end(); ++citer )
        {
            if( citer->type == "enable" ) // if event is an enable event
            {
                Block b = citer->block; // make a copy of the block
                ExecuteBlock( b, myLocals );
            }
        }
    }

    void Script::ProcessKeyboardEvent( unsigned int vkCode )
    {
        Variable k = vkCode;
        std::map<wxString, Variable *> myLocals;

        // Add globals to our variables
        std::map<wxString, Variable>::iterator iter = globals.begin();
        for( ; iter != globals.end(); ++iter )
            myLocals[iter->first] = &iter->second;

        // add k to our variables
        myLocals["k"] = &k;

        // Execute all events that meet the criteria
        std::list<Event>::const_iterator citer = events.begin();
        for( ; citer != events.end(); ++citer )
        {
            if( citer->type == "k" ) // if event is a keyboard event
            {
                unsigned int thisK = 0;
                if( citer->expression.StartsWith("0x") )
                    thisK = HexToDecimal<unsigned int>(citer->expression);
                else
                    thisK = wxAtoi(citer->expression);

                // if this event should be triggered
                if( thisK == k.AsValue() || citer->expression.length() == 0 )
                {
                    Block b = citer->block; // make a copy of the block
                    ExecuteBlock( b, myLocals );
                }
            }
        }
    }

    void Script::FunctionCallHelper( const wxString &functionName, std::vector<TokenEval> &callStack, int depth )
    {
        Variable result;
        // Add the locals, from broad to specific
        std::map<wxString, TokenEval> functionTokenLocals;
        std::map<wxString, Variable *> functionLocals;

        std::map<wxString, Variable>::iterator globalIter = globals.begin();
        for( ; globalIter != globals.end(); ++globalIter )
        {
            functionTokenLocals[globalIter->first] = TokenEval(globalIter->second, &globalIter->second);
        }

        // Set the params
        if( functions.count(functionName) > 0 )
        {
            std::vector<TokenEval>::iterator iter3 = callStack.begin();
            std::list<wxString>::iterator iter4 = functions[functionName].params.begin();
            for( ; iter3 != callStack.end() &&
                iter4 != functions[functionName].params.end(); ++iter3, ++iter4 )
            {
                functionLocals[*iter4]      = iter3->vPtr == NULL ? &iter3->v : iter3->vPtr;
                functionTokenLocals[*iter4] = iter3->vPtr == NULL ? TokenEval(iter3->v, &iter3->v) : TokenEval(iter3->v, iter3->vPtr);
            }
        }
        else // not an actual function, so make stuff up
        {
            std::vector<TokenEval>::iterator iter3 = callStack.begin();
            for( int i=0; iter3 != callStack.end(); ++iter3, ++i )
            {
                functionLocals[wxString::Format("$%i", i)] = iter3->vPtr == NULL ? &iter3->v : iter3->vPtr;
                functionTokenLocals[wxString::Format("$%i", i)] = iter3->vPtr == NULL ? TokenEval(iter3->v, &iter3->v) : TokenEval(iter3->v, iter3->vPtr);
            }
        }

        // Set return address
        functionLocals["$_"] = &result;
        functionTokenLocals["$_"] = TokenEval(result, &result);

        Block functionBlock;
        if( functions.count(functionName) > 0 )
            functionBlock = functions[functionName].block; // make a copy

        if( defaultFunctions.count(functionName) > 0 )
        {
            ExecuteFunction(functionName, functionTokenLocals);
        }
        // Else call the function normally
        else
            ExecuteBlock(functionBlock, functionLocals, depth+1);
    }

    void Script::ProcessHookRecvEvent( const bool enable )
    {
        Variable en = enable;
        std::vector<TokenEval> callStack;

        callStack.push_back(TokenEval(en, &en));
        FunctionCallHelper( hookRecvFunction, callStack, 1 );
    }

    void Script::ProcessHookSendEvent( const bool enable )
    {
        Variable en = enable;
        std::vector<TokenEval> callStack;

        callStack.push_back(TokenEval(en, &en));
        FunctionCallHelper( hookSendFunction, callStack, 1 );
    }

    // Used for DR, Int3, and other hooks.  This is as low-level as RiPEST goes.
    // No processing is done to the registers before or after this function is called.
    // Any changes made here will reflect in the appropriate register.
    void Script::ProcessLogEvent( const wxString &functionName, std::map<wxString, Variable *> &registers )
    {
        // If the function name exists
        if( functions.count(functionName) > 0 )
        {
            Block b = functions[functionName].block;
            ExecuteBlock( b, registers );
        }
    }

    void Script::ProcessPacketRecvEvent( const char *cstrPacket, SOCKET s )
    {
        Variable packet = cstrPacket;
        Variable socket = s;
        std::vector<TokenEval> callStack;

        callStack.push_back(TokenEval(packet, &packet));
        callStack.push_back(TokenEval(socket, &socket));
        FunctionCallHelper( packetRecvFunction, callStack, 1 );
    }

    void Script::ProcessPacketSendEvent( const char *cstrPacket, SOCKET s )
    {
        Variable packet = cstrPacket;
        Variable socket = s;
        std::vector<TokenEval> callStack;

        callStack.push_back(TokenEval(packet, &packet));
        callStack.push_back(TokenEval(socket, &socket));
        FunctionCallHelper( packetSendFunction, callStack, 1 );
    }

    void Script::ProcessRecvEvent( std::vector<unsigned char> &packet )
    {
        Variable r = packet;
        std::map<wxString, Variable *> myLocals;

        // Add globals to our variables
        std::map<wxString, Variable>::iterator iter = globals.begin();
        for( ; iter != globals.end(); ++iter )
            myLocals[iter->first] = &iter->second;

        // add r to our variables
        myLocals["r"] = &r;

        // Execute all events that meet the criteria
        std::list<Event>::const_iterator citer = events.begin();
        for( ; citer != events.end(); ++citer )
        {
            if( citer->type == "r" ) // if event is a recv packet event
            {
                wxString expr = citer->expression;
                bool isMatch = true;

                // Compare expr with r for a match
                for( size_t i=0; i < expr.length() && i/2 < packet.size(); ++i )
                {
                    if( expr[i] != '?' )
                    {
                        unsigned char c = expr[i];
                        c = c > '9' ? c-'a'+10 : c-'0';
                        if( c != ((packet[i/2] >> (((i+1)%2)*4)) & 0x0F) )
                        {
                            isMatch = false;
                            break;
                        }
                    }
                }

                // If match, trigger event
                if( isMatch )
                {
                    Block b = citer->block; // make a copy of the block
                    ExecuteBlock( b, myLocals );
                }
            }
        }

        // Write back to packet
        packet = r.AsArray();
    }

    void Script::ProcessSendEvent( std::vector<unsigned char> &packet )
    {
        Variable s = packet;
        std::map<wxString, Variable *> myLocals;

        // Add globals to our variables
        std::map<wxString, Variable>::iterator iter = globals.begin();
        for( ; iter != globals.end(); ++iter )
            myLocals[iter->first] = &iter->second;

        // add s to our variables
        myLocals["s"] = &s;

        // Execute all events that meet the criteria
        std::list<Event>::const_iterator citer = events.begin();
        for( ; citer != events.end(); ++citer )
        {
            if( citer->type == "s" ) // if event is a send packet event
            {
                wxString expr = citer->expression;
                bool isMatch = true;

                // Compare expr with s for a match
                for( size_t i=0; i < expr.length() && i/2 < packet.size(); ++i )
                {
                    if( expr[i] != '?' )
                    {
                        unsigned char c = expr[i];
                        c = c > '9' ? c-'a'+10 : c-'0';
                        if( c != ((packet[i/2] >> (((i+1)%2)*4)) & 0x0F) )
                        {
                            isMatch = false;
                            break;
                        }
                    }
                }

                // If match, trigger event
                if( isMatch )
                {
                    Block b = citer->block; // make a copy of the block
                    ExecuteBlock( b, myLocals );
                }
            }
        }

        // Write back to packet
        packet = s.AsArray();
    }

    // Protip:  Don't screw around with the time variable while you're
    //          in an event... that would be stupid.
    void Script::ProcessTimeEvent( int t )
    {
        Variable time = t;
        std::map<wxString, Variable *> myLocals;

        // If nothing to do, return
        if( timeEventExecuting )
            return;
        timeEventExecuting = true;

        // Add globals to our variables
        std::map<wxString, Variable>::iterator iter = globals.begin();
        for( ; iter != globals.end(); ++iter )
            myLocals[iter->first] = &iter->second;

        // add time to our variables
        myLocals["time"] = &time;

        // Execute all events that meet the criteria
        std::list<Event>::iterator iter2 = events.begin();
        for( ; iter2 != events.end(); ++iter2 )
        {
            if( iter2->type == "time" ) // if event is a time event
            {
                int thisTime = 0;
                if( iter2->expression.StartsWith("0x") )
                    thisTime = HexToDecimal<unsigned int>(iter2->expression);
                else
                    thisTime = wxAtoi(iter2->expression);

                // Special check for dynamic expressions
                if( iter2->timer >= thisTime || iter2->expression.length() == 0 )
                {
                    iter2->timer = 0;
                    Block b = iter2->block; // make a copy of the block
                    ExecuteBlock( b, myLocals );
                }

                iter2->timer += GetMilliSpan(lastTime, t);

                // Catch up
                while( iter2->timer >= thisTime && iter2->expression.length() > 0 )
                {
                    iter2->timer -= thisTime;
                    Block b = iter2->block; // make a copy of the block
                    ExecuteBlock( b, myLocals );
                }
            }
        }
        // Update the last time
        // Note:  We're not using time, because it's "supposed" to be passed by
        //        value and *not* by reference.
        lastTime           = t;
        timeEventExecuting = false;
    }

    const wxString & Script::GetText() const
    {
        return text;
    }

    //////////////////////////////////////////////////////////////////////////
    // BEGIN OF ScriptManager
    //////////////////////////////////////////////////////////////////////////

    Block ScriptManager::ParseScript( const wxString &src )
    {
        wxString code = src; // make a copy of the code that we're going to parse

        // Convert all tabs to spaces
        code.Replace("\t", "    ", true);

        return ParseScriptHelper( code, 0 );
    }

    Block ScriptManager::ParseScriptHelper( wxString &src, size_t depth )
    {
        Block block;

        // Remove annoying carriage returns
        src.Replace("\r", "", true);

        // Parse each line
        while( src.Length() > 0 )
        {
            wxString line = src.BeforeFirst('\n');
            size_t lineDepth = GetLineDepth( line );

            // If indented, this is a new block
            if( lineDepth > depth )
                block.containers.push_back(ParseScriptHelper(src, lineDepth));
            else if( lineDepth == depth )
            {
                Line sLine(line);
                if( sLine.tokens.size() > 0 ) // Don't add empty lines
                    block.containers.push_back(sLine);
                src = src.AfterFirst('\n');
            }
            else if( lineDepth == -1 ) // empty line
            {
                src = src.AfterFirst('\n');
            }
            else // leave the block
                break;
        }

        return block;
    }

    // returns the depth of the line
    size_t ScriptManager::GetLineDepth( const wxString &line )
    {
        wxString strippedLine;
        size_t pos = -1;
        size_t commentPos = -1;

        commentPos = line.Find("//");
        if( commentPos != wxNOT_FOUND )
        {
            strippedLine = line.Mid(0, commentPos);
            pos = strippedLine.find_first_not_of(' ');
            if( pos >= strippedLine.size() )
                pos = wxNOT_FOUND;
        }
        else
        {
            pos = line.find_first_not_of(' ');
            if( pos >= line.size() )
                pos = wxNOT_FOUND;
        }

        if( pos != wxNOT_FOUND )
            return pos/4;
        else
            return -1; // empty line
    }

    ScriptManager::ScriptManager()
    {
        mutIsExecuting = false;

        drPtr = new Breakpoint();
        drPtr->SetCallbackFunction( LogRegisters );
    }

    bool ScriptManager::AddScript( const wxString &name, const wxString &text )
    {
        return AddScript( name, text, ParseScript(text) );
    }

    bool ScriptManager::AddScript(const wxString &name, const wxString &text, Block block)
    {
        wxString noCarriages = text;
        noCarriages.Replace("\r", "");
        if( scripts.count(name) == 0 )
        {
            scripts[name] = Script(name, block, noCarriages, drPtr);
            return true;
        }
        return false;
    }

    void ScriptManager::RemoveScript(const wxString &name)
    {
        if( scripts.count(name) > 0 )
            scripts.erase(name);
        if( activeScripts.count(name) > 0 )
            activeScripts.erase(name);
    }

    void ScriptManager::ToggleScript(const wxString &name, const bool enable)
    {
        if( enable )
        {
            // activate the script if it exists
            if( scripts.count(name) > 0 )
            {
                activeScripts[name] = scripts[name];
                ProcessEnableEvent(name);
            }
        }
        else // disable
        {
            if( activeScripts.count(name) > 0 )
            {
                ProcessDisableEvent(name);
                activeScripts.erase(name);
            }
        }
    }

    void ScriptManager::SetHookInterface( HookInterface *hiPtr )
    {
        hookIntf = hiPtr;
    }

    void ScriptManager::ProcessDisableEvent( const wxString &script )
    {
        if( activeScripts.count(script) > 0 )
            activeScripts[script].ProcessDisableEvent();
    }

    void ScriptManager::ProcessEnableEvent( const wxString &script )
    {
        if( activeScripts.count(script) > 0 )
            activeScripts[script].ProcessEnableEvent();
    }

    void ScriptManager::ProcessHookRecvEvent( const wxString &script, const bool enable )
    {
        if( activeScripts.count(script) > 0 )
            activeScripts[script].ProcessHookRecvEvent(enable);
    }

    void ScriptManager::ProcessHookSendEvent( const wxString &script, const bool enable )
    {
        if( activeScripts.count(script) > 0 )
            activeScripts[script].ProcessHookSendEvent(enable);
    }

    void ScriptManager::ProcessKeyboardEvent( unsigned int vkCode )
    {
        if( !mutIsExecuting )
        {
            mutIsExecuting = true;

            // send the event to each Script for processing
            std::map<wxString, Script>::iterator iter = activeScripts.begin();
            for( ; iter != activeScripts.end(); ++iter )
            {
                iter->second.ProcessKeyboardEvent(vkCode);
            } 

            mutIsExecuting = false;
        }  
    }

    // Used for DR, Int3, and other hooks.  This is as low-level as RiPEST goes.
    // No processing is done to the registers before or after this function is called.
    // Any changes made here will reflect in the appropriate register.
    void ScriptManager::ProcessLogEvent( const wxString &scriptName, const wxString &functionName, 
        std::map<wxString, Variable *> &registers )
    {
        // If the script is active, process it
        if( activeScripts.count(scriptName) > 0 )
            activeScripts[scriptName].ProcessLogEvent( functionName, registers );
    }

    void ScriptManager::ProcessPacketRecvEvent( const wxString &script, const char *cstrPacket, const SOCKET s )
    {
        if( activeScripts.count(script) > 0 )
            activeScripts[script].ProcessPacketRecvEvent(cstrPacket, s);
    }

    void ScriptManager::ProcessPacketSendEvent( const wxString &script, const char *cstrPacket, const SOCKET s )
    {
        if( activeScripts.count(script) > 0 )
            activeScripts[script].ProcessPacketSendEvent(cstrPacket, s);
    }

    void ScriptManager::ProcessRecvEvent( std::vector<unsigned char> &packet )
    {
        if( !mutIsExecuting )
        {
            mutIsExecuting = true;

            // send the event to each Script for processing
            std::map<wxString, Script>::iterator iter = activeScripts.begin();
            for( ; iter != activeScripts.end(); ++iter )
            {
                iter->second.ProcessRecvEvent(packet);
            }  

            mutIsExecuting = false;
        }  
    }

    void ScriptManager::ProcessSendEvent( std::vector<unsigned char> &packet )
    {
        if( !mutIsExecuting )
        {
            mutIsExecuting = true;

            // send the event to each Script for processing
            std::map<wxString, Script>::iterator iter = activeScripts.begin();
            for( ; iter != activeScripts.end(); ++iter )
            {
                iter->second.ProcessSendEvent(packet);
            }  

            mutIsExecuting = false;
        }  
    }

    void ScriptManager::ProcessTimeEvent()
    {
        int t = GetMilliCount();
        if( !mutIsExecuting )
        {
            mutIsExecuting = true;

            // send the event to each Script for processing
            std::map<wxString, Script>::iterator iter = activeScripts.begin();
            for( ; iter != activeScripts.end(); ++iter )
            {
                iter->second.ProcessTimeEvent(t);
            }

            mutIsExecuting = false;
        }
    }

    // Saves the scripts in a /script folder
    void ScriptManager::SaveScripts()
    {
        wxString curDir = GetCurrentProcessDirectory();
        // Create folder if it doesn't exist
        if( !wxDirExists( curDir + wxT("scripts") ) )
        {
            wxMkdir( curDir + wxT("scripts") );
        }

        // For all scripts
        std::map<wxString, Script>::const_iterator iter = scripts.begin();
        for( ; iter != scripts.end(); ++iter )
        {
            wxFFile file;
            file.Open( curDir + wxT("scripts/") + iter->first, "w");
            file.Write(iter->second.GetText());
            file.Close();
        }
    }

    void ScriptManager::DeleteScript( const wxString &name )
    {
        wxString filename = GetCurrentProcessDirectory() + wxT("scripts/") + name;
        if (wxFileExists(filename)) {
            wxRemoveFile(filename);
        }
    }

    // Loads script from filename
    bool ScriptManager::LoadScript( const wxString &filename, const wxString &name )
    {
        wxFFile file(filename);
        wxString script;

        file.ReadAll(&script);
        file.Close();
        return AddScript(name, script);
    }
}
