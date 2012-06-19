#include "stdafx.h"

#include "../wx_pch.h"
#include "setCustomAddrDlg.h"

#ifndef WX_PRECOMP
	//(*InternalHeadersPCH(setCustomAddrDlg)
	#include <wx/intl.h>
	#include <wx/string.h>
	//*)
#endif
//(*InternalHeaders(setCustomAddrDlg)
//*)

//(*IdInit(setCustomAddrDlg)
const long setCustomAddrDlg::ID_TEXTCTRL1 = wxNewId();
//*)

BEGIN_EVENT_TABLE(setCustomAddrDlg,wxDialog)
	//(*EventTable(setCustomAddrDlg)
	//*)
END_EVENT_TABLE()

setCustomAddrDlg::setCustomAddrDlg(wxWindow* parent,wxWindowID id,const wxPoint& pos,const wxSize& size)
{
	//(*Initialize(setCustomAddrDlg)
	wxFlexGridSizer* FlexGridSizer1;

	Create(parent, id, _("Custom Address"), wxDefaultPosition, wxDefaultSize, wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER, _T("id"));
	SetClientSize(wxDefaultSize);
	Move(wxDefaultPosition);
	FlexGridSizer1 = new wxFlexGridSizer(0, 3, 0, 0);
	FlexGridSizer1->AddGrowableCol(0);
	FlexGridSizer1->AddGrowableRow(0);
	TextCtrl1 = new wxTextCtrl(this, ID_TEXTCTRL1, _("a=[esp+4]"), wxDefaultPosition, wxSize(150,200), wxTE_MULTILINE, wxDefaultValidator, _T("ID_TEXTCTRL1"));
	TextCtrl1->SetToolTip(_("Only enter one instruction per line.  The letter a will represent the address.\nInstructions are executed from right to left, top to bottom.\nWARNING:  As of now, order of operations is ignored. a=7*2+4 evaluates to 42,\nbut a=4+7*2 evaluates to 18.  Keep this in mind when writing your instructions.\n\nExamples of valid instructions:\na=[esp+4]\neax=[0x00401000]\na +=20\na *= 2\nebx += eax+2\nefl |= 0x00000001\na += [eax+ecx]\neax = &esp\n[eax] = [ecx]"));
	FlexGridSizer1->Add(TextCtrl1, 1, wxALL|wxEXPAND|wxALIGN_CENTER_HORIZONTAL|wxALIGN_CENTER_VERTICAL, 0);
	SetSizer(FlexGridSizer1);
	FlexGridSizer1->Fit(this);
	FlexGridSizer1->SetSizeHints(this);

	Connect(wxID_ANY,wxEVT_CLOSE_WINDOW,(wxObjectEventFunction)&setCustomAddrDlg::OnClose);
	//*)
}

setCustomAddrDlg::~setCustomAddrDlg()
{
	//(*Destroy(setCustomAddrDlg)
	//*)
}


void setCustomAddrDlg::OnClose(wxCloseEvent& event)
{
    if( SetSavedScript( TextCtrl1->GetValue() ) )
    {
        wxMessageBox( _("Script has been saved successfully."), _("SUCCESS") );
        event.Skip();
    }
    else if( wxMessageBox( _("There was an error in your script.\nYour script has not been saved.\nContinue without saving?"),
            _("WARNING"), 
            wxICON_QUESTION | wxOK | wxCANCEL | wxCANCEL_DEFAULT) == wxOK )
        event.Skip();
    return;
}

bool setCustomAddrDlg::SetSavedScript( const wxString inputStr )
{
    bool retVal = true;
    wxString infixString = inputStr;
    std::vector<Expression> infixVector;
    std::vector< std::vector<Expression> > newScript;

    while( !infixString.IsEmpty() )
    {
        wxString instructionLine = infixString.BeforeFirst( '\n' );

        instructionLine.Replace( _(" "), _(""), true );   //Remove whitespace
        instructionLine.Replace( _("\t"), _(""), true  ); //Remove tabs
        instructionLine = instructionLine.MakeUpper();        //Convert to upper to simplify comparisons
        // Swap dst and src to allow for proper dereferencing of the dst
        if( instructionLine.size() > 0 )
            instructionLine = instructionLine.AfterFirst('=') + '=' + instructionLine.BeforeFirst('=');

        if( !instructionLine.IsEmpty() ) //Prevent empty lines from being evaluated
        {
            //TODO: Check if invalid input for destination of assignment operator
            infixVector = StringToExpression( instructionLine );
            if( infixVector.empty() )
                retVal = false;
            else
            {
                infixVector = InfixToPostfix( infixVector );
                if( infixVector.empty() )
                    retVal = false;
            }
            if( retVal )
                newScript.push_back( infixVector );
            else
                break;
        }
        //Remove the instruction line from infixString
        infixString = infixString.AfterFirst( '\n' );
    }
    //If we succeeded, then we want to save the script
    if( retVal )
    {
        savedScript = newScript;
        /*
        for(unsigned int i=0; i < savedScript.size(); ++i)
        {
        	for(unsigned int j=0; j < savedScript[i].size(); ++j)
        		wxMessageBox( wxString::Format(_("%i"), (int)savedScript[i][j].type), _("HUR DUR") );
        }*/
    }
    return retVal;
}

//infixStr is guaranteed to be uppercase.
//Returns a vector of size 0 for invalid strings
std::vector<Expression> setCustomAddrDlg::StringToExpression( __in wxString infixStr )
{
    //Vector to store each instruction and variable 
    std::vector<Expression> infixVector;
    Expression              currentExpression;
    ExpressionFlag          lastExpressionFlag = IS_OPERATOR;
    for( unsigned int i=0; i < infixStr.Len(); ++i )
    {
        currentExpression.mask = IS_INVALID; //Assign to invalid to allow for
                                             //shortcuts to work properly
		currentExpression.type = UNKNOWN;
        switch( (char)infixStr[i] )
        {
          case '+':
            currentExpression.mask = IS_OPERATOR;
            currentExpression.type = ADD;
            break;
          case '-':
            currentExpression.mask = IS_OPERATOR;
            currentExpression.type = SUB;
            break;
          case '*':
            currentExpression.mask = IS_OPERATOR;
            currentExpression.type = MUL;
            break;
          case '/':
            currentExpression.mask = IS_OPERATOR;
            currentExpression.type = DIV;
            break;
          case '|':
            currentExpression.mask = IS_OPERATOR;
            currentExpression.type = OR;
            break;
          case '^':
            currentExpression.mask = IS_OPERATOR;
            currentExpression.type = XOR;
            break;
          case '&':
            currentExpression.mask = IS_OPERATOR;
            currentExpression.type = AND;
            break;
          case '%':
            currentExpression.mask = IS_OPERATOR;
            currentExpression.type = MOD;
            break;
          case '=':
              currentExpression.mask = IS_OPERATOR;
              currentExpression.type = ASSIGN;
              break;
          case '<<':
              currentExpression.mask = IS_OPERATOR;
              currentExpression.type = SHIFT_LEFT;
              break;
          case '>>':
              currentExpression.mask = IS_OPERATOR;
              currentExpression.type = SHIFT_RIGHT;
              break;
          case '(':
            currentExpression.mask = IS_BRACE;
            currentExpression.type = LEFT_PARENTHESIS;
            break;
          case ')':
            currentExpression.mask = IS_BRACE;
            currentExpression.type = RIGHT_PARENTHESIS;
            break;
          case '[':
            currentExpression.mask = IS_BRACE;
            currentExpression.type = LEFT_BRACKET;
            break;
          case ']':
            currentExpression.mask = IS_BRACE;
            currentExpression.type = RIGHT_BRACKET;
            break;
        };
        // Since there are so many registers, we're taking a shortcut here
        if( currentExpression.mask == IS_INVALID )
            currentExpression.mask = IS_REGISTER;
        // The following if statements check for registers and variables, and
        // increment i accordingly.  Special care must be taken to treat cases
        // like 'A' after cases for strings starting with 'A' (like "AX")

        //Part of EAX register
        if( infixStr.Mid( i ).StartsWith( _("EAX") ) )
        {
            currentExpression.type = EAX;
            i += 2;
        }
        else if( infixStr.Mid( i ).StartsWith( _("AX") ) )
        {
            currentExpression.type = AX;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("AH") ) )
        {
            currentExpression.type = AH;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("AL") ) )
        {
            currentExpression.type = AL;
            ++i;
        }
        //Part of ECX register
        else if( infixStr.Mid( i ).StartsWith( _("ECX") ) )
        {
            currentExpression.type = ECX;
            i += 2;
        }
        else if( infixStr.Mid( i ).StartsWith( _("CX") ) )
        {
            currentExpression.type = CX;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("CH") ) )
        {
            currentExpression.type = CH;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("CL") ) )
        {
            currentExpression.type = CL;
            ++i;
        }
        //Part of EDX register
        else if( infixStr.Mid( i ).StartsWith( _("EDX") ) )
        {
            currentExpression.type = EDX;
            i += 2;
        }
        else if( infixStr.Mid( i ).StartsWith( _("DX") ) )
        {
            currentExpression.type = DX;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("DH") ) )
        {
            currentExpression.type = DH;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("DL") ) )
        {
            currentExpression.type = DL;
            ++i;
        }
        //Part of EBX register
        else if( infixStr.Mid( i ).StartsWith( _("EBX") ) )
        {
            currentExpression.type = EBX;
            i += 2;
        }
        else if( infixStr.Mid( i ).StartsWith( _("BX") ) )
        {
            currentExpression.type = BX;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("BH") ) )
        {
            currentExpression.type = BH;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("BL") ) )
        {
            currentExpression.type = BL;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("ESP") ) )
        {
            currentExpression.type = ESP;
            i += 2;
        }
        else if( infixStr.Mid( i ).StartsWith( _("EBP") ) )
        {
            currentExpression.type = EBP;
            i += 2;
        }
        else if( infixStr.Mid( i ).StartsWith( _("ESI") ) )
        {
            currentExpression.type = ESI;
            i += 2;
        }
        else if( infixStr.Mid( i ).StartsWith( _("EDI") ) )
        {
            currentExpression.type = EDI;
            i += 2;
        }
        else if( infixStr.Mid( i ).StartsWith( _("EIP") ) )
        {
            currentExpression.type = EIP;
            i += 2;
        }
        else if( infixStr.Mid( i ).StartsWith( _("EFL") ) )
        {
            currentExpression.type = EFL;
            i += 2;
        }
        else if( infixStr.Mid( i ).StartsWith( _("CS") ) )
        {
            currentExpression.type = SEG_CS;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("DS") ) )
        {
            currentExpression.type = SEG_DS;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("ES") ) )
        {
            currentExpression.type = SEG_ES;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("SS") ) )
        {
            currentExpression.type = SEG_SS;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("FS") ) )
        {
            currentExpression.type = SEG_FS;
            ++i;
        }
        else if( infixStr.Mid( i ).StartsWith( _("GS") ) )
        {
            currentExpression.type = SEG_GS;
            ++i;
        }
        //Floating point registers (TODO: Implement floating point registers)

        //Address
        else if( infixStr.Mid( i ).StartsWith( _("A") ) )
        {
            currentExpression.mask = IS_ADDRESS;
			currentExpression.type = ADDRESS_A;
        }
        //Address R
        else if( infixStr.Mid( i ).StartsWith( _("R") ) )
        {
            currentExpression.mask = IS_ADDRESS;
            currentExpression.type = ADDRESS_R;
        }
        //Hexadecimal number, so count number of numbers and
        //convert to hexadecimal
        else if( infixStr.Mid( i ).StartsWith( _("0X") ) )
        {
            wxString bufferString  = _("");
            currentExpression.mask = IS_IMMEDIATE;
			currentExpression.type = NUMBER_VALUE;
            for( int j=i+2; isxdigit( infixStr[j] ) ; ++j )
                bufferString.Append( infixStr[j] );
            if( bufferString.Len() <= 8 )
                currentExpression.immLong = HexToDecimal<int>( bufferString.mb_str() );
            else
                currentExpression.mask = IS_INVALID; //Too long, so invalid
			i += bufferString.Len()+1;
        }
        //Normal number, so count the number of numbers and convert
        //to decimal
        else if( (infixStr[i] >= '0') && (infixStr[i] <= '9') )
        {
            wxString bufferString  = infixStr[i];
            currentExpression.mask = IS_IMMEDIATE;
			currentExpression.type = NUMBER_VALUE;
            for( int j=i+1; isdigit( infixStr[j] ); ++j )
                bufferString.Append( infixStr[j] );
            if( !bufferString.ToLong( &currentExpression.immLong ) )
                currentExpression.mask = IS_INVALID; //We had an error while converting
			else
				i += bufferString.Len()-1;
        }
		else if( !(currentExpression.mask == IS_OPERATOR || currentExpression.mask == IS_BRACE) )
			currentExpression.mask = IS_INVALID;

        //Check for invalid input and special operators
        if( lastExpressionFlag == currentExpression.mask )
        {
            if( (lastExpressionFlag & (IS_IMMEDIATE|IS_ADDRESS|IS_REGISTER)) > 0 )
                currentExpression.mask = IS_INVALID;
            //A check to see if a subtraction sign is really a negation operator
            else if( lastExpressionFlag == IS_OPERATOR && currentExpression.type == SUB )
                currentExpression.type = NEGATION;
            else if( lastExpressionFlag == IS_OPERATOR && currentExpression.type == AND )
                currentExpression.type = ADDRESS_OF;
            else if( lastExpressionFlag == IS_OPERATOR && currentExpression.type == ASSIGN )
            {
                unsigned int previousSize = infixVector.size()-1; //Don't copy the assignment operator
                std::swap( infixVector[infixVector.size()-1], currentExpression );
                for( unsigned int j=0; j < previousSize; ++j )
                    infixVector.push_back( infixVector[j] );
            }
        }
        //If invalid input, then return an error by returning vector of 0 size
        if( currentExpression.mask == IS_INVALID )
        {
            infixVector.clear();
            break; //Exit for loop
        }
        infixVector.push_back( currentExpression );
        lastExpressionFlag = currentExpression.mask;
    }
    return infixVector;
}

std::vector<Expression> setCustomAddrDlg::InfixToPostfix( __in std::vector<Expression> infixVector )
{
    std::stack<Expression>  intermediateStack;
    std::vector<Expression> postfixVector;

    //For all elements in the infix expression
    for( unsigned int i=0; i < infixVector.size(); ++i )
    {
        if( (infixVector[i].mask & (IS_ADDRESS|IS_IMMEDIATE|IS_REGISTER)) > 0 )
        {
            postfixVector.push_back( infixVector[i] );
        }
        else if( infixVector[i].type == LEFT_PARENTHESIS )
            intermediateStack.push( infixVector[i] );

        else if( infixVector[i].type == RIGHT_PARENTHESIS )
        {
            while( !intermediateStack.empty() && (intermediateStack.top().type != LEFT_PARENTHESIS) )
            {
                postfixVector.push_back( intermediateStack.top() );
                intermediateStack.pop();
            }
            intermediateStack.pop();
        }

        else if( infixVector[i].type == LEFT_BRACKET )
            intermediateStack.push( infixVector[i] );
        else if( infixVector[i].type == RIGHT_BRACKET )
        {
            while( !intermediateStack.empty() && (intermediateStack.top().type != LEFT_BRACKET) )
            {
                postfixVector.push_back( intermediateStack.top() );
                intermediateStack.pop();
            }
            if( intermediateStack.top().type == LEFT_BRACKET )
                intermediateStack.pop();
            Expression newExpressionElement;
            newExpressionElement.mask = IS_OPERATOR;
            newExpressionElement.type = DEREFERENCE;
            intermediateStack.push( newExpressionElement );
        }

        else if( infixVector[i].mask == IS_OPERATOR )
        {
            while( ( !intermediateStack.empty() ) &&
                ( GetOperatorPrecedence( intermediateStack.top().type ) <
                GetOperatorPrecedence( infixVector[i].type ) ) )
            {
                postfixVector.push_back( intermediateStack.top() );
                intermediateStack.pop();
            }
            intermediateStack.push( infixVector[i] );
        }
    }
    while( !intermediateStack.empty() )
    {
        postfixVector.push_back( intermediateStack.top() );
        intermediateStack.pop();
    }

    //Error-checking
    if( postfixVector.size() > 1 ) //Check for assignment to an immediate
        if( postfixVector[ postfixVector.size()-2 ].type == NUMBER_VALUE )
            postfixVector.clear();
    for( unsigned int i=0; i < postfixVector.size(); ++i ) //Check for hanging parentheses/braces
    {
        if( postfixVector[i].type == LEFT_PARENTHESIS ||
            postfixVector[i].type == RIGHT_PARENTHESIS ||
            postfixVector[i].type == LEFT_BRACKET ||
            postfixVector[i].type == RIGHT_BRACKET )
        {
            postfixVector.clear();
            break;
        }
    }

    //for( unsigned int j=0; j < postfixVector.size(); ++j )
    //    wxMessageBox( wxString::Format(_("%i"), (int)postfixVector[j].type), _("HUR DUR") );

    return postfixVector;
}

//Returns the precedence of the operation.  A lower return value signifies
//higher precedence.  -1 is returned if precedence is unknown.
int setCustomAddrDlg::GetOperatorPrecedence( __in const ExpressionType op )
{
    int retVal = -1;

    if( op == DEREFERENCE )
        retVal = 2;
    else if( op == NEGATION || op == ADDRESS_OF )
        retVal = 3;
    else if( op == MUL || op == DIV || op == MOD )
        retVal = 5;
    else if( op == ADD || op == SUB )
        retVal = 6;
    else if( op == SHIFT_LEFT || op == SHIFT_RIGHT )
        retVal = 7;
    else if( op == AND )
        retVal = 10;
    else if( op == XOR )
        retVal = 11;
    else if( op == OR )
        retVal = 12;
    else if( op == ASSIGN  )
        retVal = 13;
    else if( op == LEFT_PARENTHESIS || op == LEFT_BRACKET )
        retVal = 0x7FFFFFFF;
    return retVal;
}
