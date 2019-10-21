/* mbed Command Interpreter Library
 * Copyright (c) 2011 wvd_vegt
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef MBED_CMDB_H
#define MBED_CMDB_H

#include "mbed.h"

#include <vector>
#include <limits>

//------------------------------------------------------------------------------

/** Max size of an Ansi escape code.
 */
#define MAX_ESC_LEN   5

/** Max (strlen) of a Param.
 */
#define MAX_PARM_LEN 32

/** Max eight parms.
 */
#define MAX_ARGS      8

/** Max 132 characters commandline.
 */
#define MAX_CMD_LEN 132

/** 'Show' hidden subsystems and commands.
 */
#define SHOWHIDDEN

/** Enable macro commands.
 */
#define ENABLEMACROS

/** Enable statemachine.
 *
 * Used to implement a series of commands running at power-up.
 *
 * @note Not Implemented!
 */
#undef STATEMACHINE

/** Enable subsystem prompts.
 *
 * When defined, prompts will reflect the SubSystem.
 */
#define SUBSYSTEMPROMPTS

//------------------------------------------------------------------------------

/** 8 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MIN_BYTE        std::numeric_limits<unsigned char>::min()

/** 8 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MAX_BYTE        std::numeric_limits<unsigned char>::max()

/** 8 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MIN_CHAR        std::numeric_limits<signed char>::min()

/** 8 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MAX_CHAR        std::numeric_limits<signed char>::max()

/** 16 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MIN_SHORT     std::numeric_limits<short int>::min()

/** 16 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MAX_SHORT     std::numeric_limits<short int>::max()

/** 16 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MIN_USHORT     std::numeric_limits<unsigned short int>::min()

/** 16 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MAX_USHORT     std::numeric_limits<unsigned short int>::max()

/** 32 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MIN_INT       std::numeric_limits<int>::min()
#define MAX_INT       std::numeric_limits<int>::max()

/** 32 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MIN_UINT       std::numeric_limits<unsigned int>::min()
#define MAX_UINT       std::numeric_limits<unsigned int>::max()

/** 32 bit limits.
 *
 * @see http://www.daniweb.com/forums/thread18963.html
 */
#define MIN_LONG      std::numeric_limits<long>::min()
#define MAX_LONG      std::numeric_limits<long>::max()

//------------------------------------------------------------------------------

/** Description of a command.
 */
struct cmd {
public:
    const char *cmdstr;
    int  subs;
    int  cid;
    const char *parms;
    const char *cmddescr;
    const char *parmdescr;
};

//------------------------------------------------------------------------------

/** Cr.
 */
static const char cr           = '\r';

/** Lf.
 */
static const char lf           = '\n';

/** Bell.
 */
static const char bell         = '\7';

/** Escape.
 */
static const char esc          = '\033';

/** Space.
 */
static const char sp           = ' ';

/** CrLf.
 */
static const char crlf[]       = "\r\n";

/** Backspace that 'tries' to wipe the last character.
 */
static const char bs[]         = "\b \b";

/** VT100 Bold Command.
 */
static const char boldon[]     = "\033[1m";

/** VT100 Normal Command.
 */
static const char boldoff[]    = "\033[0m";

/** VT100 Cls Command.
 */
static const char cls[]        = "\033[2J";

/** VT100 Home Command.
 */
static const char home[]       = "\033[H";

/** The default command prompt.
 */
static const char PROMPT[]     = "CMD>";

//------------------------------------------------------------------------------

/** Subsystem Id for a Subsystem.
 */
#define SUBSYSTEM         -1

/** Subsystem Id for a Global Command (always available).
 */
#define GLOBALCMD         -2

/** Subsystem Id for a Hidden Subsystem (ommitted from help).
 */
#define HIDDENSUB         -3

/** Predefined Dump Command.
 */
#define CID_COMMANDS 9989

/** Predefined Boot Command.
 */
#define CID_BOOT    9990

/** Predefined Macro Command.
 *
 * This command will take a string with spaces replace by _ and cr replace by | for later replay with run.
 */
#define CID_MACRO   9991

/** Predefined Macro Command.
 *
 * This command replay a macro.
 */
#define CID_RUN     9992

/** Predefined Macro Command.
 *
 * This command print the current macro.
 */
#define CID_MACROS  9993

/** Predefined Echo Command.
 *
 * This command turn echo on or off.
 */
#define CID_ECHO    9994

/** Predefined VT100 Bold Command.
 *
 * This command turn VT100 bold usage on or off.
 */
#define CID_BOLD    9995

/** Predefined VT100 Cls Command.
 *
 * This command will clear the screen.
 */
#define CID_CLS     9996

/** Predefined Idle Command.
 *
 * This command will return to the global command level, leaving the active subsystem.
 */
#define CID_IDLE    9997

/** Predefined Help Command.
 *
 * This command will either print all active command (without parameters) or a more detailed
 * help for a command passed as parameter.
 */
#define CID_HELP    9998

/** Predefided Semi Command.
 *
 * CID_LAST only functions as a special Commend Id to signal unknown commands.
 */
#define CID_LAST    9999

//------------------------------------------------------------------------------

/** The Boot Command.
 *
 * @note: this command can be used to list all commands in Windows ini file format for host processing.
 *
 * Optional.
 */
static const cmd COMMANDS = {"Commands",GLOBALCMD,CID_COMMANDS,"","Dump Commands"};

/** The Boot Command.
 *
 * Optional.
 */
static const cmd BOOT = {"Boot",GLOBALCMD,CID_BOOT,"","Boot mBed"};

/** The Macro Command.
 *
 * Optional.
 */
static const cmd MACRO = {"Macro",GLOBALCMD,CID_MACRO,"%s","Define macro (sp->_, cr->|)","command(s)"};

/** The Run Command.
 *
 * Optional.
 */
static const cmd RUN = {"Run",GLOBALCMD,CID_RUN,"","Run a macro"};

/** The Macros Command.
 *
 * Optional.
 */
static const cmd MACROS = {"Macros",GLOBALCMD,CID_MACROS,"","List macro(s)"};

/** The Echo Command.
 *
 * Optional.
 */
static const cmd ECHO = {"Echo",GLOBALCMD,CID_ECHO,"%bu","Echo On|Off (1|0)","state"};

/** The Bold Command.
 *
 * Optional.
 */
static const cmd BOLD = {"Bold",GLOBALCMD,CID_BOLD,"%bu","Bold On|Off (1|0)","state"};

/** The Cls Command.
 *
 * Optional.
 */
static const cmd CLS = {"Cls",GLOBALCMD,CID_CLS,"","Clears the terminal screen"};

/** The Idle Command.
 *
 * Mandatory if you use subsystems.
 */
static const cmd IDLE = {"Idle",GLOBALCMD,CID_IDLE,"","Deselect Subsystems"};

/** The Help Command.
 *
 * Mandatory.
 */
static const cmd HELP = {"Help",GLOBALCMD,CID_HELP,"%s","Help"};

//------------------------------------------------------------------------------

/** We'll only define the 4 cursor codes at the moment.
 */
#define ESC_TBL_LEN  4

/** Escape code definition struct.
 */
struct esc {
    char     *escstr;
    int     id;
};

/** The Escape Code Id's.
 */
enum {
    EID_CURSOR_UP,
    EID_CURSOR_DOWN,
    EID_CURSOR_RIGHT,
    EID_CURSOR_LEFT,
    EID_LAST
};

/** The Escape Codes Table.
 */
static const struct esc esc_tbl [ESC_TBL_LEN] = {
    { "\033[A",    EID_CURSOR_UP    },
    { "\033[B",    EID_CURSOR_DOWN  },
    { "\033[C",    EID_CURSOR_RIGHT },
    { "\033[D",    EID_CURSOR_LEFT  },
};

//------------------------------------------------------------------------------

/** The Command Interpreter Version.
 */
#define CMDB_VERSION     0.81

//------------------------------------------------------------------------------

/** Command Interpreter class.
 *
 * Steps to take:
 *
 * 1) Create a std::vector<cmd> and fill it with at least
 *    the mandatory commands IDLE and HELP.
 *
 * 2) Create an Cmdb class instance and pass it the vector,
 *    a Serial port object like Serial serial(USBTX, USBRX);
 *    and finally a command dispatcher function.
 *
 * 3) Feed the interpreter with characters received from your serial port.
 *    Note: Cmdb self does not retrieve input it must be handed to it.
 *    It implements basic members for checking/reading the serial port.
 *
 * 4) Handle commands added by the application by the Cid and parameters passed.
 *
 * Note: Predefined commands and all subsystems transitions are handled by the internal dispatcher.
 * So the passed dispatcher only has to handle user/application defined commands'.
 *
 * @see main.cpp for a demo.
 */
class Cmdb {
public:
    /** Create a Command Interpreter.
     *
     * @see http://www.newty.de/fpt/fpt.html#chapter2 for function pointers.
     * @see http://stackoverflow.com/questions/9410/how-do-you-pass-a-function-as-a-parameter-in-c
     * @see http://www.daniweb.com/forums/thread293338.html
     *
     * @param serial a Serial port used for communication.
     * @param cmds a vector with the command table.
     */
    Cmdb(Serial &_serial, std::vector<cmd>& _cmds, void (*_callback)(Cmdb&,int) );

    /** The version of the Command Interpreter.
     *
     * returns the version.
     */
    static float version() {
        return CMDB_VERSION;
    }

    /** NULL is used as No Comment Value.
      */
    static const char* NoComment;

    /** Column 72 is used as Default Comment Starting Position.
      */
    static int DefComPos;

    /** Checks if the macro buffer has any characters left.
     *
     * @returns true if any characters left.
     */
    bool macro_hasnext();

    /** Gets the next character from the macro buffer and
     *  advances the macro buffer pointer.
     *
     * @note Do not call if no more characters are left!
     *
     * @returns the next character.
     */
    char macro_next();

    /** Gets the next character from the macro buffer
     *  but does not advance the macro buffer pointer.
     *
     * @note Do not call if no more characters are left!
     *
     * @returns the next character.
     */
    char macro_peek();

    /** Resets the macro buffer and macro buffer pointer.
     *
     */
    void macro_reset();

    /** Checks if the serial port has any characters
     * left to read by calling serial.readable().
     *
     * @returns true if any characters available.
     */
    bool hasnext();

    /** Gets the next character from the serial port by
     *  calling serial.getc().
     *
     *  Do not call if no characters are left!
     *
     * @returns the next character.
     */
    char next();

    /** Add a character to the command being processed.
     * If a cr is added, the command is parsed and executed if possible
     * If supported special keys are encountered (like backspace, delete and cursor up) they are processed.
     *
     * @param c the character to add.
     *
     * @returns true if a command was recognized and executed.
     */
    bool scan(const char c);

    /** printf substitute using the serial parameter passed to the constructor.
     *
     * @see http://www.cplusplus.com/reference/clibrary/cstdio/printf/
     *
     * @parm format the printf format string.
     * @parm ... optional paramaters to be merged into the format string.
     *
     * @returns the printf return value.
     */
    int printf(const char *format, ...);

    /** print is simply printf without parameters using the serial parameter passed to the constructor.
     *
     * @parm msg the string to print.
     *
     * @returns the printf return value.
     */
    int print(const char *msg);

    /** println is simply printf without parameters using the serial parameter passed to the constructor.
     *
     * @parm msg the string to print followed by a crlf.
     *
     * @returns the printf return value.
     */
    int println(const char *msg);
    
    /** printch is simply putc subsitute using the serial parameter passed to the constructor.
     *
     * @parm msg the string to print.
     *
     * @returns the printf return value.
     */
    char printch(const char ch);

    /** printsection prints an inifile Section Header
     *  like:
     *
     *  [Section]\r\n
     *
     *  Usage: cmdb.printsection("GP");
     *
     *  @parm section the section to print.
     *
     *  @returns the printf return value.
     */
    int printsection(const char *section);

    /** printmsg prints an inifile Msg Key=Value pair.
     *  like:
     *
     *  Msg={msg}\r\n
     *
     *  Usage: cmdb.printmsg("Validation successfull");
     *
     *  @parm msg the msg to print.
     *
     *  @returns the printf return value.
     */
    int printmsg(const char *msg);

    /** printerror prints an inifile Error Section Header and Error Msg Key=Value pair.
     *  like:
     *
     *  [Error]\r\nmsg={errormsg}\r\n
     *
     *  Usage: cmdb.printerror("Data Size Incorrect");
     *
     * @parm errormsg the error msg to print.
     *
     * @returns the printf return value.
     */
    int printerror(const char *errormsg);

    /** printerror prints an inifile Error Section Header and Error Msg Key=Value pair.
     *  like:
     *
     *  [Error]\r\nmsg={errormsg}\r\n
     *
     *  Usage: cmdb.printerrorf("Data Size Incorrect %d", 15);
     *
     * @parm format the error msg to print.
     * @parm parameter to print.
     *
     * @returns the printf return value.
     */
    int printerrorf(const char *format, ...);
    
    /** printvalue prints an inifile Key/Value Pair
     *  like:
     *
     *  Key=Value   ;comment\r\n
     *
     *  Note: the Comment is (if present) located at position 72.
     *
     *  Usage: cmdb.printvaluef("Value", Cmdb::DefComPos, "Hex", "0x%8.8X", LPC_RTC->GPREG0);
     *
     * @parm key the key to print.
     * @parm comment the comment to print.
     * @parm width the location of the comment to print.
     * @parm format the value to print.
     * @parm parameter to print.
     *
     * @returns the printf return value.
     */
    int printvaluef(const char *key, const int width, const char *comment, const char *format, ...);

    /** printvalue prints an inifile Key/Value Pair
     *  like:
     *
     *  Key=Value\r\n
     *
     *  Usage: cmdb.printvaluef("Value", "0x%8.8X", LPC_RTC->GPREG0);
     *
     * @parm key the key to print.
     * @parm format the value to print.
     * @parm parameter to print.
     *
     * @returns the printf return value.
     */
    int printvaluef(const char *key, const char *format, ...);

    /** printvalue prints an inifile Key/Value Pair
     *  like:
     *
     *  Key=Value   ;comment\r\n
     *
     *  Note the Comment is (if present) located at position 72.
     *
     * @parm key the key to print.
     * @parm value the value to print.
     * @parm comment the comment to print.
     * @parm width the location of the comment to print.
     *
     * @returns the printf return value.
     */
    int printvalue(const char *key, const char *value, const char *comment = NoComment, const int width = DefComPos);

    int printcomment(const char *comment, const int width = DefComPos);

//------------------------------------------------------------------------------

    /** Initializes the parser (called by the constructor).
     *
     * @parm full if true the macro buffer is also cleared else only the command interpreter is reset.
     */
    void init(const char full);

//------------------------------------------------------------------------------
//----These helper functions retieve parameters in the correct format.
//------------------------------------------------------------------------------

    /** Typecasts parameter ndx to a bool.
     *
     * mask: %bu
     *
     * @parm the parameter index
     *
     * @return a bool
     */
    bool BOOLPARM(int ndx) {
        return parms[ndx].val.uc!=0;
    }

    /** Typecasts parameter ndx to a byte/unsigned char.
     *
     * mask: %bu
     *
     * @parm the parameter index
     *
     * @return a byte/unsigned char
     */
    unsigned char BYTEPARM(int ndx) {
        return parms[ndx].val.uc;
    }

    /** Typecasts parameter ndx to a char.
     *
     * mask: %c
     *
     * @parm the parameter index
     *
     * @return a char
     */
    char CHARPARM(int ndx) {
        return parms[ndx].val.c;
    }

    /** Typecasts parameter ndx to word/unsigned int.
     *
     * mask: %hu
     *
     * @parm the parameter index
     *
     * @return a word/unsigned int
     */
    unsigned int WORDPARM(int ndx) {
        return parms[ndx].val.ui;
    }

    /** Typecasts parameter ndx to a unsigned int.
     *
     * mask: %u
     *
     * @parm the parameter index
     *
     * @return a unsigned int
     */
    unsigned int UINTPARM(int ndx) {
        return parms[ndx].val.ui;
    }

    /** Typecasts parameter ndx to a int.
     *
     * mask: %i
     *
     * @parm the parameter index
     *
     * @return a int
     */
    int INTPARM(int ndx) {
        return parms[ndx].val.i;
    }

    /** Typecasts parameter ndx to a bool.
     *
     * mask: %lu
     *
     * @parm the parameter index
     *
     * @return a bool
     */
    unsigned long DWORDPARM(int ndx) {
        return parms[ndx].val.ul;
    }

    /** Typecasts parameter ndx to a long.
     *
     * mask: %li
     *
     * @parm the parameter index
     *
     * @return a long
     */
    long LONGPARM(int ndx) {
        return parms[ndx].val.l;
    }

    /** Typecasts parameter ndx to a float.
     *
     * mask: %f
     *
     * @parm the parameter index
     *
     * @return a float
     */
    float FLOATPARM(int ndx) {
        return parms[ndx].val.f;
    }

    /** Typecasts parameter ndx to a string.
     *
     * @note spaces are not allowed as it makes parsing so much harder.
     *
     * mask: %s
     *
     * @parm the parameter index
     *
     * @return a string
     */
    char* STRINGPARM(int ndx) {
        return parms[ndx].val.s;
    }

    bool present(char *cmdstr) {
        return cmdid_search(cmdstr)!=CID_LAST;
    }

    void replace(std::vector<cmd>& newcmds)  {
        cmds.assign(newcmds.begin(), newcmds.end());
    }


    int indexof(int cid) {
        return cmdid_index(cid);
    }

    //FAILS...
    /*
    void insert(int cid, cmd newcmd) {
        //Add Command (update our original and then assign/replace cmdb's copy)...
        vector<cmd>::iterator iter;

        std::vector<cmd> newcmds = std::vector<cmd>(cmds);

        iter = newcmds.begin();

        newcmds.insert(iter+indexof(cid)+1,newcmd);

        replace(newcmds);

        printf("Index: %d\r\n", ndx);

        print("check #1\r\n");
        print("check #2\r\n");

        vector<cmd>::iterator it;
        it=newcmds.begin();

        print("check #3\r\n");
        ndx++;
        newcmds.insert(it, newcmd);
        print("check #4\r\n");

        //cmds.push_back(c1);
        cmds.assign(newcmds.begin(), newcmds.end());
    }
    */

private:

    /** Internal Serial Port Storage.
    */
    Serial &serial;

    /** Internal Command Table Vector Storage.
     *
     * @see http://www.cplusplus.com/reference/stl/vector/
     */
    std::vector<cmd> cmds;

    /** C callback function
     *
     * @see See http://www.newty.de/fpt/fpt.html#chapter2 for function pointers.
     *
     * C++ member equivalent would be void (Cmdb::*callback)(Cmdb&,int);
     */
    void (*user_callback)(Cmdb&,int);

    /** Searches the escape code list for a match.
    *
    * @param char* escstr the escape code to lookup.
    *
    * @returns the index of the escape code or -1.
    */
    int escid_search(char *escstr);

    /** Checks if the command table for a match.
     *
     * @param char* cmdstr the command to lookup.
     *
     * @returns the id of the command or -1.
     */
    int cmdid_search(char *cmdstr);

    /** Converts an command id to an index of the command table.
     *
     * @param cmdid the command id to lookup.
     *
     * @returns the index of the command or -1.
     */
    int  cmdid_index(int cmdid);

    /** Writes a prompt to the serial port.
     *
     */
    void prompt(void);

    /** Called by cmd_dispatch it parses the command against the command table.
     *
     * @param cmd the command and paramaters to parse.
     *
     * @returns the id of the parsed command.
     */
    int parse(char *cmd);

    /** Called by scan it processes the arguments and dispatches the command.
     *
     * Note: This member calls the callback callback function.
     *
     * @param cmd the command to dispatch.
     */
    void cmd_dispatcher(char *cmd);

    /** Generates Help from the command table and prints it.
     *
     * @param pre leading text
     * @param ndx the index of the command in the command table.
     * @param post trailing text.
     */
    void cmd_help(char *pre, int ndx, char *post);

    /** Dumps all commands in ini file format.
     */
    void cmd_dump();

    /** memset wrapper.
     *
     * @param p The string to be cleared.
     * @param siz The string size.
     */
    void zeromemory(char *p,unsigned int siz);

    /** Case insensitive compare.
     *
     * @see strcmp.
     *
     * @param s1
     * @param s2 the second string to compare.
     *
     * @returns 0 if s1=s2, -1 if s1<s2 or +1 if s1>s2.
     */
    int stricmp (char *s1, char *s2);

    /** Internal Echo Flag Storage.
    */
    bool echo;

    /** Internal VT100 Bold Flag Storage.
    */
    bool bold;

    /** Internal Command Table Length Storage.
    */
    //int CMD_TBL_LEN;

    //Macro's.
    /** Internal Macro Pointer.
    */
    int macro_ptr;

    /** Internal Macro Buffer.
    */
    char macro_buf[1 + MAX_CMD_LEN];

    /** Used for parsing parameters.
    */
    enum parmtype {
        PARM_UNUSED,            //0

        PARM_FLOAT,             //1     (f)

        PARM_LONG,              //2     (l/ul)
        PARM_INT,               //3     (i/ui)
        PARM_SHORT,             //4     (w/uw)

        PARM_CHAR,              //5     (c/uc)
        PARM_STRING             //6     (s)
    };

    /** Used for parsing parameters.
    */
    union value {
        float               f;

        unsigned long       ul;
        long                 l;

        int                  i;
        unsigned int        ui;

        short                w;
        unsigned short      uw;

        char                 c;
        unsigned char       uc;

        char                 s[MAX_PARM_LEN];
    };

    /** Used for parsing parameters.
    */
    struct parm {
        enum parmtype    type;
        union value     val;
    };

    //------------------------------------------------------------------------------
    //----Buffers & Storage.
    //------------------------------------------------------------------------------

    /** Command Buffer.
    */
    char    cmdbuf [1 + MAX_CMD_LEN];           // command buffer

    /** Command Buffer Pointer.
    */
    char    cmdndx;                             // command index

    /** Last Command Buffer (Used when pressing Cursor Up).
    */
    char    lstbuf [1 + MAX_CMD_LEN];           // last command buffer

    /** Escape Code Buffer.
    */
    char    escbuf [1 + MAX_ESC_LEN];

    /** Escape Code Buffer Pointer.
    */
    unsigned char   escndx;

    /** Storage for Parsed Parameters
    */
    struct  parm parms[MAX_ARGS];

    /** Parsed Parameters Pointer.
     */
    int     noparms;

    /** Current Selected Subsystem (-1 for Global).
    */
    int     subsystem;

    /** No of arguments found in command.
    */
    int     argcnt;

    /** No of arguments to find in parameter definition (Command Table).
    */
    int     argfnd;

    /** strtoXX() Error detection.
    */
    int     error;
};

extern "C" void mbed_reset();

#endif