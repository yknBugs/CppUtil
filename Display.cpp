#include "Display.h"

std::mutex Display::mtx;
COORD Display::cursorPosition = { 0, 0 };
std::string Display::cursorColor = "r";
size_t Display::specialCharCursor = 0;

char Display::c()
{
    std::lock_guard<std::mutex> lock(mtx);
#ifdef _WIN32
    return (char)_getch();
#else
    std::cout.flush();
    char buf = 0;
    termios old = {};
    if (tcgetattr(STDIN_FILENO, &old) < 0)
    {
        perror("tcsetattr()");
    }
    termios newt = old;
    newt.c_lflag &= ~ICANON;  // Turn off canonical mode
    newt.c_lflag &= ~ECHO;    // Turn off echo
    if (tcsetattr(STDIN_FILENO, TCSANOW, &newt) < 0)
    {
        perror("tcsetattr ICANON");
    }
    if (read(STDIN_FILENO, &buf, 1) < 0)
    {
        perror("read()");
    }
    if (tcsetattr(STDIN_FILENO, TCSANOW, &old) < 0)
    {
        perror("tcsetattr ~ICANON");
    }
    return buf;
#endif
}

void Display::changeCursor(char c)
{
    // Note: This functions is platform-independent, but it may not be accurate
    // Warning: Unknown Characters may exist
    std::lock_guard<std::mutex> lock(mtx);
    if (c == '\n')
    {
        specialCharCursor = 0;
        cursorPosition.X = 0;
        cursorPosition.Y++;
    }
    else if (c == '\r')
    {
        specialCharCursor = 0;
        cursorPosition.X = 0;
    }
    else if (c == '\b')
    {
        specialCharCursor = 0;
        cursorPosition.X--;
    }
    else if (c == '\t')
    {
        specialCharCursor = 0;
        cursorPosition.X++;
        while (cursorPosition.X % 8 != 0)
        {
            cursorPosition.X++;
        }
    }
    else if (c == '\0')
    {
        specialCharCursor = 0;
        return;
    }
    else if (c < 0)
    {
        specialCharCursor++;
#ifdef _WIN32
        cursorPosition.X++;
#else
        if (specialCharCursor % 3 != 0)
        {
            cursorPosition.X++;
        }
#endif
    }
    else if (c >= 32 && c <= 126)
    {
        specialCharCursor = 0;
        cursorPosition.X++;
    }
    else
    {
        specialCharCursor = 0;
    }
}

void Display::changeCursor(const std::string& text)
{
    // Warning: This is just a guess, not the actual cursor position
    for (char c : text)
    {
        changeCursor(c);
    }
}

bool Display::updateCursorPosition()
{
    // Note: GetCursorPosition requires platform-specific code
    // This function is only implemented for windows, for other platforms, it will return false for compatibility
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        std::lock_guard<std::mutex> lock(mtx);
        cursorPosition.X = csbi.dwCursorPosition.X;
        cursorPosition.Y = csbi.dwCursorPosition.Y;
        return true;
    }
#endif
    return false;
}

bool Display::inputIsEnter(const std::vector<char>& input)
{
    // Windows: '\r', Linux: '\n'
    if (input.size() == 1 && (input[0] == '\r' || input[0] == '\n'))
    {
        return true;
    }
    return false;
}

bool Display::inputIsBackspace(const std::vector<char>& input)
{
    // Windows: '\b', Linux: 127
    if (input.size() == 1 && (input[0] == '\b' || input[0] == 127))
    {
        return true;
    }
    return false;
}

bool Display::inputIsLeftArrow(const std::vector<char>& input)
{
    // Windows: (-32, 75), Linux: (27, 91, 68)
#ifdef _WIN32
    if (input.size() == 2 && input[0] == -32 && input[1] == 75)
    {
        return true;
    }
#else
    if (input.size() == 3 && input[0] == 27 && input[1] == 91 && input[2] == 68)
    {
        return true;
    }
#endif
    return false;
}

bool Display::inputIsRightArrow(const std::vector<char>& input)
{
    // Windows: (-32, 77), Linux: (27, 91, 67)
#ifdef _WIN32
    if (input.size() == 2 && input[0] == -32 && input[1] == 77)
    {
        return true;
    }
#else
    if (input.size() == 3 && input[0] == 27 && input[1] == 91 && input[2] == 67)
    {
        return true;
    }
#endif
    return false;
}

bool Display::inputIsUpArrow(const std::vector<char>& input)
{
    // Windows: (-32, 72), Linux: (27, 91, 65)
#ifdef _WIN32
    if (input.size() == 2 && input[0] == -32 && input[1] == 72)
    {
        return true;
    }
#else
    if (input.size() == 3 && input[0] == 27 && input[1] == 91 && input[2] == 65)
    {
        return true;
    }
#endif
    return false;
}

bool Display::inputIsDownArrow(const std::vector<char>& input)
{
    // Windows: (-32, 80), Linux: (27, 91, 66)
#ifdef _WIN32
    if (input.size() == 2 && input[0] == -32 && input[1] == 80)
    {
        return true;
    }
#else
    if (input.size() == 3 && input[0] == 27 && input[1] == 91 && input[2] == 66)
    {
        return true;
    }
#endif
    return false;
}

bool Display::inputIsFnLeftArrow(const std::vector<char>& input)
{
    // Windows: (-32, 71), Linux: (27, 91, 72)
#ifdef _WIN32
    if (input.size() == 2 && input[0] == -32 && input[1] == 71)
    {
        return true;
    }
#else
    if (input.size() == 3 && input[0] == 27 && input[1] == 91 && input[2] == 72)
    {
        return true;
    }
#endif
    return false;
}

bool Display::inputIsFnRightArrow(const std::vector<char>& input)
{
    // Windows: (-32, 79), Linux: (27, 91, 70)
#ifdef _WIN32
    if (input.size() == 2 && input[0] == -32 && input[1] == 79)
    {
        return true;
    }
#else
    if (input.size() == 3 && input[0] == 27 && input[1] == 91 && input[2] == 70)
    {
        return true;
    }
#endif
    return false;
}

bool Display::inputIsControlChar(const std::vector<char>& input)
{
    // WARNING: Unknown Control Characters may exist
    if (input[0] >= 0 && input[0] <= 31)
    {
        return true;
    }
#ifdef _WIN32
    if (input.size() == 2 && input[0] == -32)
    {
        return true;
    }
#endif
    return false;
}

size_t Display::previewGetInputString(const std::string oClr, const std::string& iStr, size_t cIdx, size_t* lAcuIdx, size_t lVisLen, bool allowClr)
{
    // Note: "&+ColorCode" will only be displayed when the cursor is near them
    std::cout << std::string(*lAcuIdx, '\b') << std::string(lVisLen, ' ') << std::string(lVisLen, '\b');
    // Show Text
    setTextColor(oClr);
    size_t i = 0;
    size_t x = 0;
    size_t visibleLength = 0;
    bool isColorChar = false;
    while (iStr[i] != '\0')
    {
        if (iStr[i] == '&' && isColorChar == false && allowClr)
        {
            isColorChar = true;
            i++;
            continue;
        }

        if (isColorChar)
        {
            bool setColorResult = this->setTextColor(iStr[i]);
            if (cIdx >= i - 1 && cIdx <= i + 1)
            {
                // Current Cursor near the color character, force show the color character
                std::cout << "&" << iStr[i];
                visibleLength += 2;
                if (cIdx == i)
                {
                    x++;
                }
                else if (cIdx > i)
                {
                    x += 2;
                }
            }
            else if (setColorResult == false)
            {
                // Failed to match any color, show the original character
                std::cout << "&" << iStr[i];
                visibleLength += 2;
                if (cIdx == i)
                {
                    x++;
                }
                else if (cIdx > i)
                {
                    x += 2;
                }
            }

            isColorChar = false;
            i++;
            continue;
        }

        std::cout << iStr[i];
        visibleLength++;
        if (i < cIdx)
        {
            x++;
        }
        i++;
    }

    // String ending with character '&'
    if (isColorChar)
    {
        std::cout << "&";
        visibleLength++;
        if (cIdx == i)
        {
            x++;
        }
    }

#ifndef _WIN32
    // In Linux, Unicode characters have 3 values but take only 2 character spaces
    i = 0;
    while (iStr[i] != '\0')
    {
        if (iStr[i] < 0)
        {
            if (i < cIdx)
            {
                x--;
            }
            visibleLength--;
            i += 2;
        }
        i++;
    }
#endif

    // Set cursor position to originalCursorPosition.X + x
    if (x < visibleLength)
    {
        std::cout << std::string(visibleLength - x, '\b');
    }
    *lAcuIdx = x;
    return visibleLength;
}

bool Display::printCharToProperPosition(char c, COORD topleft, COORD bottomright, bool wideCharPredict)
{
    COORD position = getCursorPosition();
    // Handle Control Characters
    if (c == '\0')
    {
        return false;
    }
    if (c == '\r')
    {
        setCursorPosition(topleft.X, position.Y);
        return true;
    }
    if (c == '\n')
    {
        setCursorPosition(topleft.X, position.Y + 1);
        return true;
    }
    if (c == '\b')
    {
        std::cout << '\b';
        changeCursor(c);
        updateCursorPosition();
        return true;
    }

    // Go to the next line if current position is out of the boundary
#ifdef _WIN32
    if ((c < 0 && specialCharCursor % 2 == 0) || (c >= 32 && c <= 126) || c == '\t')
#else
    if ((c < 0 && specialCharCursor % 3 == 0) || (c >= 32 && c <= 126) || c == '\t')
#endif
    {
        if (position.X > bottomright.X)
        {
            if (position.Y >= bottomright.Y)
            {
                return false;
            }
            setCursorPosition(topleft.X, position.Y + 1);
            position = getCursorPosition();
        }
    }

    if (c == '\t')
    {
        std::cout << ' ';
        changeCursor(' ');
        position = getCursorPosition();
        while (position.X - topleft.X % 8 != 0)
        {
            if (position.X > bottomright.X)
            {
                if (position.Y >= bottomright.Y)
                {
                    return false;
                }
                break;
            }
            std::cout << ' ';
            changeCursor(' ');
            position = getCursorPosition();
        }
        return true;
    }

    // Handle Unicode Characters
    if (c < 0)
    {
#ifdef _WIN32
        if (specialCharCursor % 2 == 0)
#else
        if (specialCharCursor % 3 == 0)
#endif
        {
            // If the input is estimated to be out of boundary, go to the next line, the input may never be printed if the width is too narrow
            while (wideCharPredict && position.X >= bottomright.X)
            {
                if (position.Y >= bottomright.Y)
                {
                    return false;
                }
                setCursorPosition(topleft.X, position.Y + 1);
                position = getCursorPosition();
            }
        }
        std::cout << c;
        changeCursor(c);
#ifdef _WIN32
        if (specialCharCursor % 2 == 0)
#else
        if (specialCharCursor % 3 == 0)
#endif
        {
            updateCursorPosition();
        }
        return true;
    }

    if (c >= 32 && c <= 126)
    {
        std::cout << c;
        changeCursor(c);
        updateCursorPosition();
        return true;
    }

    return true;
}

Display::Display()
{
    this->startTime = std::chrono::system_clock::now();
    this->updateCursorPosition();
}

Display::~Display()
{
    // Reset the color
    setTextColor('r');
}

bool Display::setTextColor(char colorCode)
{
    // Note: Based on the windows platform behavior
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (colorCode >= '0' && colorCode <= '9')
    {
        SetConsoleTextAttribute(hConsole, (int)(colorCode - '0'));
    }
    else if (colorCode >= 'A' && colorCode <= 'F')
    {
        SetConsoleTextAttribute(hConsole, (int)(colorCode - 'A' + 10));
    }
    else if (colorCode >= 'a' && colorCode <= 'f')
    {
        SetConsoleTextAttribute(hConsole, (int)(colorCode - 'a' + 10));
    }
    else if (colorCode == 'r' || colorCode == 'R')
    {
        SetConsoleTextAttribute(hConsole, 7);
    }
    else
    {
        return false;
    }
#else
    int color = 0;
    if (colorCode >= '0' && colorCode <= '9')
    {
        color = (int)(colorCode - '0');
    }
    else if (colorCode >= 'A' && colorCode <= 'F')
    {
        color = (int)(colorCode - 'A' + 10);
    }
    else if (colorCode >= 'a' && colorCode <= 'f')
    {
        color = (int)(colorCode - 'a' + 10);
    }
    else if (colorCode == 'r' || colorCode == 'R')
    {
        std::cout << "\033[0m";
        std::lock_guard<std::mutex> lock(mtx);
        cursorColor = std::string(1, colorCode);
        return true;
    }
    else
    {
        return false;
    }
    std::string colorCodeColor[16] = { "12;12;12", "0;55;218", "19;161;14", "58;150;221",
                                    "197;15;31", "136;23;152", "193;156;0", "204;204;204",
                                    "118;118;118", "54;120;255", "22;198;12", "97;214;214",
                                    "231;72;86", "180;0;158", "249;241;165", "242;242;242" };
    std::cout << "\033[38;2;" << colorCodeColor[color] << "m";
#endif
    std::lock_guard<std::mutex> lock(mtx);
    cursorColor = std::string(1, colorCode);
    return true;
}

bool Display::setTextColor(int red, int green, int blue)
{
    // Note: Some console may not support RGB color
    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0 || blue > 255)
    {
        return false;
    }
    // \033[38;2;<r>;<g>;<b>m<text>\033[0m
    std::cout << "\033[38;2;" << red << ";" << green << ";" << blue << "m";
    std::lock_guard<std::mutex> lock(mtx);
    cursorColor = colorToHex(red, green, blue);
    return true;
}

bool Display::setTextColor(const std::vector<int>& color)
{
    if (color.size() != 3)
    {
        return false;
    }
    return setTextColor(color[0], color[1], color[2]);
}

bool Display::setTextColor(std::string color)
{
    if (color.size() == 1)
    {
        return setTextColor(color[0]);
    }
    std::vector<int> clr = hexToColor(color);
    if (clr[0] == -1 || clr[1] == -1 || clr[2] == -1)
    {
        return false;
    }
    return setTextColor(clr);
}

std::vector<char> Display::getInput()
{
    /*
    // This solution is disabled because Ctrl + V can lead to a false positive
    std::vector<char> result;
    char str = c();
    result.push_back(str);
    timeval tv = {0, 5000};  // Waiting for 5ms, charactors input within 5ms will be regarded as a single input
    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(STDIN_FILENO, &fds);
    while (select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv) > 0)
    {
        str = c();
        result.push_back(str);
        tv = {0, 5000};
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
    }
    return result;
    */

    // Note: In different platform, Display::c() may return different result for the same input
    // Some input may have 2 ascii or more, this function will connect those ascii that should be regarded as 1 single input
    // WARNING: This function just handles the most common cases, for unknown cases, it will not connect the ascii
    std::vector<char> result;
    char str = c();
    result.push_back(str);

    // For arrow keys, windows: (-32, 75) up, linux: (27, 91, 65) up.
#ifdef _WIN32
    if (str == -32)
    {
        str = c();
        result.push_back(str);
        return result;
    }
#else
    if (str == 27)
    {
        // WARNING: In linux, ESC will return a single 27 value, we can't distinguish it from the arrow keys
        str = c();
        result.push_back(str);
        if (str == 91)
        {
            str = c();
            result.push_back(str);
        }
        return result;
        // UNIMPLEMENTED: Fn + UP = (27, 91, 53, 126), has 4 values
    }
#endif

    // For Unicode characters, windows has 2 values, linux has 3 values (Needs more infomation)
    if (str < 0)
    {
        str = c();
        result.push_back(str);
#ifndef _WIN32
        str = c();
        result.push_back(str);
#endif
        return result;
    }

    // For other cases, return the result directly
    return result;
}

void Display::clear()
{
    setTextColor('r');
    std::lock_guard<std::mutex> lock(mtx);
    cursorPosition.X = 0;
    cursorPosition.Y = 0;
#ifdef _WIN32
    system("cls");
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
#else
    std::cout << "\033[2J\033[1;1H";
#endif
}

void Display::clear(size_t length, COORD position, char c, bool freezeCursor)
{
    // Note: require accurate cursor position if freezeCursor is true
    COORD originalPosition = getCursorPosition();
    setCursorPosition(position);
    for (size_t i = 0; i < length; i++)
    {
        std::cout << c;
        changeCursor(c);
    }
    if (freezeCursor)
    {
        setCursorPosition(originalPosition);
    }
    updateCursorPosition();
}

void Display::clear(size_t length, short x, short y, char c, bool freezeCursor)
{
    COORD position = { x, y };
    clear(length, position, c, freezeCursor);
}

void Display::print(const std::string& text)
{
    std::cout << text;
    this->changeCursor(text);
    this->updateCursorPosition();
}

void Display::print(const std::string& text, int red, int green, int blue, bool resetColor)
{
    std::string clr = getCursorColor();
    bool isColorValid = this->setTextColor(red, green, blue);
    if (isColorValid == false)
    {
        // Not a valid color, using default color
        this->setTextColor('r');
    }
    print(text);
    if (resetColor)
    {
        this->setTextColor(clr);
    }
}

void Display::print(const std::string& text, const std::vector<int>& color, bool resetColor)
{
    std::string clr = getCursorColor();
    bool isColorValid = this->setTextColor(color);
    if (isColorValid == false)
    {
        // Not a valid color, using default color
        this->setTextColor('r');
    }
    print(text);
    if (resetColor)
    {
        this->setTextColor(clr);
    }
}

void Display::print(const std::string& text, std::string color, bool resetColor)
{
    std::string clr = getCursorColor();
    bool isColorValid = this->setTextColor(color);
    if (isColorValid == false)
    {
        // Not a valid color, using default color
        this->setTextColor('r');
    }
    print(text);
    if (resetColor)
    {
        this->setTextColor(clr);
    }
}

void Display::showText(char c, char colorCode)
{
    this->setTextColor(colorCode);
    std::cout << c;
    this->changeCursor(c);
    this->updateCursorPosition();
}

void Display::showText(const std::string& text)
{
    size_t i = 0;
    bool isColorChar = false;

    while (text[i] != '\0')
    {
        if (text[i] == '&' && isColorChar == false)
        {
            isColorChar = true;
            i++;
            continue;
        }

        if (isColorChar)
        {
            bool setColorResult = this->setTextColor(text[i]);
            if (setColorResult == false)
            {
                // Failed to match any color, show the original character
                std::cout << "&" << text[i];
                this->changeCursor('&');
                this->changeCursor(text[i]);
            }

            isColorChar = false;
            i++;
            continue;
        }

        std::cout << text[i];
        this->changeCursor(text[i]);
        i++;
    }

    // String ending with character '&'
    if (isColorChar)
    {
        std::cout << "&";
        this->changeCursor('&');
    }

    this->updateCursorPosition();
}

void Display::showText(const std::string& text, const std::vector<std::string>& parameters)
{
    std::string buffer;
    size_t inputTextIndex = 0;
    size_t parameterIndex = 0;
    while (text[inputTextIndex] != '\0')
    {
        if (text[inputTextIndex] == '#')
        {
            // show text before '#'
            this->showText(buffer);
            buffer.clear();
            // show parameters
            if (parameterIndex < parameters.size())
            {
                this->print(parameters[parameterIndex]);
            }
            else
            {
                this->print("#");
            }
            parameterIndex++;
        }
        else
        {
            buffer += text[inputTextIndex];
        }
        inputTextIndex++;
    }

    // show text after the last '#'
    this->showText(buffer);
}

void Display::showText(const std::string& text, const std::vector<int>& parameters)
{
    std::vector<std::string> vString = vInt2vString(parameters);
    this->showText(text, vString);
}

void Display::showText(const std::string& text, const std::vector<double>& parameters, unsigned int accuracy)
{
    std::vector<std::string> vString = vDouble2vString(parameters, accuracy);
    this->showText(text, vString);
}

void Display::showText(const std::string& text1, const std::string& parameter, const std::string& text2)
{
    this->showText(text1);
    this->print(parameter);
    this->showText(text2);
}

void Display::showText(const std::string& text1, int parameter, const std::string& text2)
{
    this->showText(text1);
    this->print(std::to_string(parameter));
    this->showText(text2);
}

void Display::showText(const std::string& text1, double parameter, const std::string& text2, unsigned int accuracy)
{
    this->showText(text1);
    this->print(doubleToString(parameter, accuracy));
    this->showText(text2);
}

void Display::createText(char c, COORD topleft, COORD bottomright, bool freezeCursor)
{
    if (topleft.X > bottomright.X || topleft.Y > bottomright.Y)
    {
        return;
    }
    if (c == '\t' || c == '\n' || c == '\r' || c == '\b')
    {
        c = ' ';
    }
    COORD originalPosition = getCursorPosition();
    setCursorPosition(topleft);
    for (short y = 0; y <= bottomright.Y - topleft.Y; y++)
    {
        std::cout << std::string(bottomright.X - topleft.X + 1, c);
        setCursorPosition(topleft.X, topleft.Y + y + 1);
    }
    if (freezeCursor)
    {
        setCursorPosition(originalPosition);
    }
}

void Display::createText(const std::string& text, COORD topleft, COORD bottomright, bool wideCharPredict, bool freezeCursor)
{
    if (topleft.X > bottomright.X || topleft.Y > bottomright.Y)
    {
        return;
    }

    COORD originalPosition = getCursorPosition();
    setCursorPosition(topleft);
    size_t i = 0;
    bool isColorChar = false;
    bool isFinished = false;

    while (text[i] != '\0')
    {
        if (text[i] == '&' && isColorChar == false)
        {
            isColorChar = true;
            i++;
            continue;
        }

        if (isColorChar)
        {
            bool setColorResult = this->setTextColor(text[i]);
            if (setColorResult == false)
            {
                // Failed to match any color, show the original character
                if (this->printCharToProperPosition('&', topleft, bottomright, wideCharPredict) == false)
                {
                    isFinished = true;
                    break;
                }
                if (this->printCharToProperPosition(text[i], topleft, bottomright, wideCharPredict) == false)
                {
                    isFinished = true;
                    break;
                }
            }

            isColorChar = false;
            i++;
            continue;
        }

        if (this->printCharToProperPosition(text[i], topleft, bottomright, wideCharPredict) == false)
        {
            isFinished = true;
            break;
        }
        i++;
    }

    // String ending with character '&'
    if (isColorChar && isFinished == false)
    {
        this->printCharToProperPosition('&', topleft, bottomright, wideCharPredict);
    }
    if (freezeCursor)
    {
        setCursorPosition(originalPosition);
    }
}

int Display::getInputInt(int max, bool canBelowZero)
{
    bool isBelowZero = false;
    int result = 0;
    int length = 0;
    int maxLength = 0;
    int temp = 0;
    std::vector<char> cstr;

    // Get the maximum length of the input
    temp = max;
    while (temp != 0)
    {
        maxLength++;
        temp /= 10;
    }

    // Get the input
    cstr = getInput();
    // break the loop if the input is Enter
    while (inputIsEnter(cstr) == false || length == 0)
    {
        if (inputIsBackspace(cstr))
        {
            // Handle backspace
            if (length > 0)
            {
                // Delete Digit
                length--;
                result = result / 10;
                std::cout << '\b' << ' ' << '\b';
            }
            else if (length == 0 && isBelowZero == true)
            {
                // Delete minus sign
                isBelowZero = false;
                std::cout << '\b' << ' ' << '\b';
            }
        }
        else if (cstr.size() == 1 && cstr[0] == '-' && isBelowZero == false && length == 0 && canBelowZero == true)
        {
            // Handle minus sign
            std::cout << '-';
            isBelowZero = true;
        }
        else if (cstr.size() == 1 && cstr[0] >= '0' && cstr[0] <= '9')
        {
            // Handle number
            if (result > max / 10 || (result == max / 10 && cstr[0] - '0' > max % 10))
            {
                // Input is too large, replace the input with the maximum value
                size_t visibleLength = length + (isBelowZero ? 1 : 0);
                std::cout << std::string(visibleLength, '\b') << std::string(visibleLength, ' ') << std::string(visibleLength, '\b');
                if (isBelowZero == true)
                {
                    std::cout << -max;
                }
                else
                {
                    std::cout << max;
                }
                result = max;
                length = maxLength;
            }
            else if (length == 1 && result == 0)
            {
                // Leading 0 is not allowed, replace the input with the new input
                std::cout << '\b' << cstr[0];
                result = result * 10 - '0' + cstr[0];
            }
            else
            {
                // Normal input
                std::cout << cstr[0];
                result = result * 10 - '0' + cstr[0];
                length++;
            }
        }
        cstr = getInput();
    }

    this->print("\n");
    updateCursorPosition();
    if (isBelowZero == true)
    {
        return -result;
    }
    return result;
}

double Display::getInputDouble(bool canBelowZero, bool acceptNaN, size_t maxLength)
{
    bool isBelowZero = false;
    bool hasDot = false;
    bool isNaN = false;
    size_t length = 0;
    std::vector<char> cstr;
    double result = 0.0;
    double decResult = 1.0;
    std::vector<char> input;

    // Get the input
    cstr = getInput();
    while (inputIsEnter(cstr) == false || (length == 0 && isNaN == false))
    {
        if (inputIsBackspace(cstr) && input.size() > 0)
        {
            // Handle backspace
            if (input[input.size() - 1] == '-')
            {
                // Delete minus sign
                isBelowZero = false;
                input.pop_back();
                std::cout << '\b' << ' ' << '\b';
            }
            else if (input[input.size() - 1] == '.')
            {
                // Delete dot
                hasDot = false;
                input.pop_back();
                std::cout << '\b' << ' ' << '\b';
            }
            else if (input[input.size() - 1] == 'n')
            {
                // Delete NaN (3 characters)
                isNaN = false;
                input.pop_back();
                std::cout << "\b\b\b" << "   " << "\b\b\b";
            }
            else if (input[input.size() - 1] == 'i')
            {
                // Delete Infinity (8 characters)
                isNaN = false;
                input.pop_back();
                std::cout << "\b\b\b\b\b\b\b\b" << "        " << "\b\b\b\b\b\b\b\b";
            }
            else if (length > 0)
            {
                // Delete Digit
                length--;
                result = result / 10;
                input.pop_back();
                std::cout << '\b' << ' ' << '\b';
            }
        }
        else if (cstr.size() == 1 && cstr[0] == '-' && isBelowZero == false && isNaN == false && length == 0 && canBelowZero == true)
        {
            // Handle minus sign
            std::cout << '-';
            input.push_back('-');
            isBelowZero = true;
        }
        else if (cstr.size() == 1 && cstr[0] == '.' && hasDot == false && isNaN == false && length < maxLength)
        {
            // Handle dot
            if (length == 0)
            {
                // Dot is not allowed at the beginning, fill with 0.
                std::cout << '0';
                input.push_back('0');
                length++;
            }
            std::cout << '.';
            input.push_back('.');
            hasDot = true;
        }
        else if (cstr.size() == 1 && (cstr[0] == 'n' || cstr[0] == 'N') && isNaN == false && acceptNaN && length == 0 && isBelowZero == false && hasDot == false)
        {
            // Handle NaN
            std::cout << "NaN";
            input.push_back('n');
            isNaN = true;
        }
        else if (cstr.size() == 1 && (cstr[0] == 'i' || cstr[0] == 'I') && isNaN == false && acceptNaN && length == 0 && hasDot == false)
        {
            // Handle Infinity
            std::cout << "Infinity";
            input.push_back('i');
            isNaN = true;
        }
        else if (cstr.size() == 1 && cstr[0] >= '0' && cstr[0] <= '9' && isNaN == false)
        {
            // Handle number
            if (length == 1 && input[input.size() - 1] == '0' && hasDot == false)
            {
                // Leading 0 is not allowed, replace the input with the new input
                std::cout << '\b' << cstr[0];
                input[input.size() - 1] = cstr[0];
            }
            else if (length < maxLength)
            {
                std::cout << cstr[0];
                input.push_back(cstr[0]);
                length++;
            }
        }
        cstr = getInput();
    }

    // Convert the input to double
    hasDot = false;
    isBelowZero = false;
    for (char c : input)
    {
        if (c == '-')
        {
            isBelowZero = true;
        }
        else if (c == '.')
        {
            hasDot = true;
        }
        else if (c >= '0' && c <= '9')
        {
            if (hasDot)
            {
                decResult /= 10;
                result = result + ((double)c - '0') * decResult;
            }
            else
            {
                result = result * 10 - '0' + c;
            }
        }
        else if (c == 'n')
        {
            result = std::nan("");
            break;
        }
        else if (c == 'i')
        {
            result = std::numeric_limits<double>::infinity();
            break;
        }
    }

    if (isBelowZero)
    {
        result = -result;
    }

    this->print("\n");
    updateCursorPosition();
    return result;
}

std::string Display::getInputText(size_t minLength, size_t maxLength, bool allowColor)
{
    std::string result = "";
    std::string asciiLabel = "";
    size_t cursorIndex = 0;
    std::vector<char> cstr;
    size_t length = 0;
    size_t lastVisibleLength = 0;
    size_t accurateCursorIndex = 0;
    std::string color = getCursorColor();

    cstr = getInput();
    while (inputIsEnter(cstr) == false || length < minLength)
    {
        // Handle backspace
        if (inputIsBackspace(cstr))
        {
            if (length > 0 && cursorIndex > 0)
            {
                char asciiType = asciiLabel[cursorIndex - 1];
                for (char i = '0'; i < asciiType; i++)
                {
                    if (cursorIndex > 0 && asciiLabel[cursorIndex - 1] == asciiType)
                    {
                        length--;
                        cursorIndex--;
                        result.erase(cursorIndex, 1);
                        asciiLabel.erase(cursorIndex, 1);
                    }
                    else
                    {
                        break;
                    }
                }
                lastVisibleLength = previewGetInputString(color, result, cursorIndex, &accurateCursorIndex, lastVisibleLength, allowColor);
            }
        }
        else if (cstr.size() == 1 && cstr[0] == '\t')
        {
            // Handle tab, since backspace cannot delete tab, replace tab with 4 spaces
            if (length <= maxLength - 4)
            {
                result.insert(cursorIndex, 4, ' ');
                asciiLabel.insert(cursorIndex, 4, '1');
                length += 4;
                cursorIndex += 4;
                lastVisibleLength = previewGetInputString(color, result, cursorIndex, &accurateCursorIndex, lastVisibleLength, allowColor);
            }
        }
        else if (inputIsLeftArrow(cstr))
        {
            // left
            if (cursorIndex > 0)
            {
                char asciiType = asciiLabel[cursorIndex - 1];
                for (char i = '0'; i < asciiType; i++)
                {
                    if (cursorIndex > 0 && asciiLabel[cursorIndex - 1] == asciiType)
                    {
                        cursorIndex--;
                    }
                    else
                    {
                        break;
                    }
                }
                lastVisibleLength = previewGetInputString(color, result, cursorIndex, &accurateCursorIndex, lastVisibleLength, allowColor);
            }
        }
        else if (inputIsRightArrow(cstr))
        {
            // right
            if (cursorIndex < length)
            {
                char asciiType = asciiLabel[cursorIndex];
                for (char i = '0'; i < asciiType; i++)
                {
                    if (cursorIndex < length && asciiLabel[cursorIndex] == asciiType)
                    {
                        cursorIndex++;
                    }
                    else
                    {
                        break;
                    }
                }
                lastVisibleLength = previewGetInputString(color, result, cursorIndex, &accurateCursorIndex, lastVisibleLength, allowColor);
            }
        }
        else if (inputIsUpArrow(cstr) || inputIsFnLeftArrow(cstr))
        {
            // begin
            cursorIndex = 0;
            lastVisibleLength = previewGetInputString(color, result, cursorIndex, &accurateCursorIndex, lastVisibleLength, allowColor);
        }
        else if (inputIsDownArrow(cstr) || inputIsFnRightArrow(cstr))
        {
            // end
            cursorIndex = length;
            lastVisibleLength = previewGetInputString(color, result, cursorIndex, &accurateCursorIndex, lastVisibleLength, allowColor);
        }
        else if (length + cstr.size() <= maxLength && inputIsControlChar(cstr) == false)
        {
            // Handle printable characters
            for (size_t i = 0; i < cstr.size(); i++)
            {
                result.insert(cursorIndex, 1, cstr[i]);
                asciiLabel.insert(cursorIndex, 1, '0' + (char)cstr.size());
                length++;
                cursorIndex++;
            }
            lastVisibleLength = previewGetInputString(color, result, cursorIndex, &accurateCursorIndex, lastVisibleLength, allowColor);
        }
        cstr = getInput();
    }

    // Make sure no "&+ColorCode" is displayed
    previewGetInputString(color, result, -10, &accurateCursorIndex, lastVisibleLength, allowColor);
    print("\n");
    setTextColor(color);
    updateCursorPosition();
    return result;
}

std::string Display::getInputString(std::string whitelistChar, std::string defaultValue, size_t minLength, size_t maxLength)
{
    std::string result = defaultValue;
    std::vector<char> cstr;
    size_t length = result.size();

    std::cout << result;
    cstr = getInput();
    while (inputIsEnter(cstr) == false || length < minLength)
    {
        if (inputIsBackspace(cstr))
        {
            if (length > 0)
            {
                length--;
                result.pop_back();
                std::cout << '\b' << ' ' << '\b';
            }
        }
        else if (inputIsControlChar(cstr) == false && cstr.size() == 1 && length < maxLength)
        {
            if (whitelistChar.size() == 0 || whitelistChar.find(cstr[0]) != std::string::npos)
            {
                result += cstr[0];
                length++;
                std::cout << cstr[0];
            }
        }
        cstr = getInput();
    }

    this->print("\n");
    updateCursorPosition();
    return result;
}

std::vector<std::string> Display::vInt2vString(const std::vector<int>& vInt)
{
    std::vector<std::string> vString;
    for (const int& i : vInt)
    {
        vString.push_back(std::to_string(i));
    }
    return vString;
}

std::vector<std::string> Display::vDouble2vString(const std::vector<double>& vDouble, unsigned int accuracy)
{
    std::vector<std::string> vString;
    for (const double& d : vDouble)
    {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(accuracy) << d;
        vString.push_back(oss.str());
    }
    return vString;
}

std::string Display::doubleToString(double number, unsigned int accuracy)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(accuracy) << number;
    return oss.str();
}

std::string Display::vectorToString(const std::vector<std::string>& vString, bool hasQuoteMark)
{
    std::vector<std::string> format = { "{ ", "", "", ", ", " }" };
    if (hasQuoteMark)
    {
        format[1] = "\"";
        format[2] = "\"";
    }
    return vectorToString(vString, format);
}

std::string Display::vectorToString(const std::vector<std::string>& vString, const std::vector<std::string>& format)
{
    // format = { prefix, open bracket, close bracket, split char, suffix }
    // missing format will be regarded as empty string 
    std::string result = "";
    if (format.size() > 0)
    {
        result += format[0];
    }
    for (size_t i = 0; i < vString.size(); i++)
    {
        if (format.size() > 1)
        {
            result += format[1];
        }
        result += vString[i];
        if (format.size() > 2)
        {
            result += format[2];
        }
        if (i < vString.size() - 1)
        {
            if (format.size() > 3)
            {
                result += format[3];
            }
        }
    }
    if (format.size() > 4)
    {
        result += format[4];
    }
    return result;
}

std::vector<int> Display::hexToColor(std::string hex)
{
    if (hex[0] == '#')
    {
        hex = hex.substr(1);
    }

    if (hex.size() == 6)
    {
        std::vector<int> color(3, 0);
        try
        {
            for (int i = 0; i < 3; i++)
            {
                color[i] = std::stoi(hex.substr(i * 2, 2), nullptr, 16);
            }
        }
        catch (...)
        {
            return { -1, -1, -1 };
        }
        return color;
    }

    if (hex.size() == 3)
    {
        std::vector<int> color(3, 0);
        try
        {
            for (int i = 0; i < 3; i++)
            {
                color[i] = std::stoi(hex.substr(i, 1), nullptr, 16) * 17;
            }
        }
        catch (...)
        {
            return { -1, -1, -1 };
        }
        return color;
    }

    return { -1, -1, -1 };
}

std::string Display::colorToHex(int red, int green, int blue)
{
    if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0 || blue > 255)
    {
        return "";
    }

    std::ostringstream oss;
    oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << red;
    oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << green;
    oss << std::uppercase << std::hex << std::setw(2) << std::setfill('0') << blue;
    return oss.str();
}

std::string Display::colorToHex(const std::vector<int>& color)
{
    if (color.size() != 3)
    {
        return "";
    }

    return colorToHex(color[0], color[1], color[2]);
}

void Display::setCursorPosition(short x, short y)
{
    std::lock_guard<std::mutex> lock(mtx);
    cursorPosition.X = x;
    cursorPosition.Y = y;
#ifdef _WIN32
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), cursorPosition);
#else
    std::cout << "\033[" << y + 1 << ";" << x + 1 << "H";
#endif
}

void Display::setCursorPosition(COORD position)
{
    setCursorPosition(position.X, position.Y);
}

COORD Display::getCursorPosition()
{
#ifdef _WIN32
    // Try to update the Cursor Position Using Windows API First
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
    {
        std::lock_guard<std::mutex> lock(mtx);
        cursorPosition.X = csbi.dwCursorPosition.X;
        cursorPosition.Y = csbi.dwCursorPosition.Y;
    }
#endif
    return cursorPosition;
}

short Display::getCursorPositionX()
{
    return getCursorPosition().X;
}

short Display::getCursorPositionY()
{
    return getCursorPosition().Y;
}

std::string Display::getCursorColor()
{
    std::lock_guard<std::mutex> lock(mtx);
    return cursorColor;
}

void Display::timeStart()
{
    this->startTime = std::chrono::system_clock::now();
}

double Display::getDurationSeconds()
{
    std::chrono::system_clock::time_point endTime = std::chrono::system_clock::now();
    std::chrono::nanoseconds spendTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - this->startTime);  // 1e-9 seconds
    double result = (double)spendTime.count() * std::chrono::nanoseconds::period::num / std::chrono::nanoseconds::period::den;
    return result;
}