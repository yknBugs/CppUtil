#pragma once

#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <mutex>
#ifdef _WIN32
#include <conio.h>
#include <Windows.h>
#else
#include <termios.h>
#include <unistd.h>
struct COORD
{
    short X;
    short Y;
};
#endif

// This class is platform-independent, but for best performance, it is recommended to use it on Windows
// Even we tried to make it thread-safe, it is recommended to lock the instance in a multi-thread environment
// To avoid unexpected behavior, if you use this class to print text to the console, please don't use std::cout at the same time
// This is because we guess the current cursor position based on the text, if you use std::cout, we cannot update the cursor position in time
// In Windows, we will try to update the cursor position using the Windows API to make it accurate
// But in Linux, we can only guess the cursor position based on your input, so please clear the console before using this class
// You can use the clear() function in this class to clear the console
class Display
{
private:
    // try to lock static variables to make it thread-safe
    static std::mutex mtx;
    // current cursor position, may not be accurate
    static COORD cursorPosition;
    // current cursor color
    static std::string cursorColor;
    // handle unicode characters
    static size_t specialCharCursor;
    // record the start time to support timing function
    std::chrono::system_clock::time_point startTime;

    // Get a single input byte from the console
    static char c();

    // Guess the cursor position (mainly for non-Windows platform)
    static void changeCursor(char c);
    // Guess the new cursor position
    static void changeCursor(const std::string& text);
    // Use platform-specific API to get and update the actual cursor position, return false if failed
    static bool updateCursorPosition();

    // Evaluate the input from different platforms
    // ENTER: '\r' in Windows, '\n' in Linux
    static bool inputIsEnter(const std::vector<char>& input);
    // BACKSPACE: '\b' in Windows, 127 in Linux
    static bool inputIsBackspace(const std::vector<char>& input);
    // LEFT ARROW: (-32, 75) in Windows, (27, 91, 68) in Linux
    static bool inputIsLeftArrow(const std::vector<char>& input);
    // RIGHT ARROW: (-32, 77) in Windows, (27, 91, 67) in Linux
    static bool inputIsRightArrow(const std::vector<char>& input);
    // UP ARROW: (-32, 72) in Windows, (27, 91, 65) in Linux
    static bool inputIsUpArrow(const std::vector<char>& input);
    // DOWN ARROW: (-32, 80) in Windows, (27, 91, 66) in Linux
    static bool inputIsDownArrow(const std::vector<char>& input);
    // Fn + LEFT ARROW: (-32, 71) in Windows, (27, 91, 72) in Linux
    static bool inputIsFnLeftArrow(const std::vector<char>& input);
    // Fn + RIGHT ARROW: (-32, 79) in Windows, (27, 91, 70) in Linux
    static bool inputIsFnRightArrow(const std::vector<char>& input);
    // Input is not a printable character, such as BACKSPACE, ENTER, ARROW KEYS, etc.
    static bool inputIsControlChar(const std::vector<char>& input);

    // Common algorithm
    // Called by getInputText to preview the input string with color support and update cursor position, return the visible length of the string
    size_t previewGetInputString(const std::string oClr, const std::string& iStr, size_t cIdx, size_t* lAcuIdx, size_t lVisLen, bool allowClr);
    // Called by createText to print a single character to the proper position, return false if there is not enough space
    bool printCharToProperPosition(char c, COORD topleft, COORD bottomright, bool wideCharPredict);

public:
    // You need to create an instance if you want to use this class to print text to the console
    Display();
    // Destruct the instance will reset the color to default
    ~Display();

    // Console related functions
    // Set the text color with color code, support '0' to '9', 'a' to 'f', 'r' for reset, not case-sensitive, return false if the color code is invalid
    bool setTextColor(char colorCode);
    // Set the text color with RGB color, return false if rgb value is < 0 or > 255
    bool setTextColor(int red, int green, int blue);
    // Set the text color with RGB color, you must provide a vector with exact 3 elements representing red, green, and blue, otherwise return false
    bool setTextColor(const std::vector<int>& color);
    // Set the text color, it can be a color code or an RGB color in hex format, not case-sensitive and leading '#' is allowed, return false if the color is invalid
    bool setTextColor(std::string color);
    // Get a single input character from the console, return the input, usually a unicode character will have 2 or more bytes
    std::vector<char> getInput();

    // Clear all text in the console and set the color to default
    void clear();
    // Fill a specific area in the console (with space by default). The cursor will jump to the original position if freezeCursor is true
    void clear(size_t length, COORD position, char c = ' ', bool freezeCursor = true);
    // Fill a specific area in the console (with space by default). The cursor will jump to the original position if freezeCursor is true
    void clear(size_t length, short x, short y, char c = ' ', bool freezeCursor = true);
    // Similar to std::cout
    void print(const std::string& text);
    // Print text with specific color, it will use the default color if the color is invalid, set resetColor to false if you want to keep the color after printing
    void print(const std::string& text, int red, int green, int blue, bool resetColor = true);
    // Print text with specific color, you must provide a vector with exact 3 elements representing red, green, and blue, otherwise it will use the default color
    void print(const std::string& text, const std::vector<int>& color, bool resetColor = true);
    // Print text with specific color, color should be a color code or an RGB color in hex format, not case-sensitive and leading '#' is allowed
    void print(const std::string& text, std::string color, bool resetColor = true);

    // Show text to the terminal, use "&+Number" to change the color, use "#" to be a placeholder for parameters
    // Print a single char with specific color, color will not be reset after printing
    void showText(char c, char colorCode = 'r');
    // Print text to the terminal, use "&+ColorCode" to change the text color after them
    void showText(const std::string& text);
    // Print text. Use "&+ColorCode" to change color, use "#" to be a placeholder for parameters, "&+ColorCode" will not working in parameters
    void showText(const std::string& text, const std::vector<std::string>& parameters);
    // Print text. Use "&+ColorCode" to change color, "#" will be replaced to parameters, parameter followed by "&" cannot change the color
    void showText(const std::string& text, const std::vector<int>& parameters);
    // Print text. Use "&+ColorCode" to change color, "#" will be replaced to parameters, more "#" than parameters will be printed as "#"
    void showText(const std::string& text, const std::vector<double>& parameters, unsigned int accuracy = 4);
    // Print text as "text1 + parameter + text2", "&+ColorCode" in text (not in parameter) will change the text color after them
    void showText(const std::string& text1, const std::string& parameter, const std::string& text2);
    // Print text as "text1 + parameter + text2", "&+ColorCode" will change the text color, but even if text1 end with "&", parameter will not be regarded as a ColorCode
    void showText(const std::string& text1, int parameter, const std::string& text2);
    // Print text as "text1 + parameter + text2", Use "&+ColorCode" to change the text color
    void showText(const std::string& text1, double parameter, const std::string& text2, unsigned int accuracy = 4);

    // Draw text to a certain area in the console, use "&+Number" to change the color
    // Fill a specific area with a signle character. The cursor will jump to the original position if freezeCursor is true
    void createText(char c, COORD topleft, COORD bottomright, bool freezeCursor = true);
    // Print a text to a specific area in the console, set wideCharPredict to true to make sure 2-character-wide unicode characters not exceed the boundary
    void createText(const std::string& text, COORD topleft, COORD bottomright, bool wideCharPredict = true, bool freezeCursor = true);

    // User Input
    // Get an integer input from the console, set canBelowZero to true to accept negative numbers
    int getInputInt(int max = INT_MAX, bool canBelowZero = true);
    // Get a double input from the console, set canBelowZero to false to reject negative numbers, set acceptNaN to true to accept NaN and Infinity
    double getInputDouble(bool canBelowZero = true, bool acceptNaN = false, size_t maxLength = SIZE_MAX);
    // Get a string text input from the console, support "&+ColorCode" to change the text color, support unicode characters
    std::string getInputText(size_t minLength = 0, size_t maxLength = SIZE_MAX, bool allowColor = true);
    // Get a string input from the console, doesn't support unicode characters and color code, leave whitelistChar empty to accept all ascii characters
    std::string getInputString(std::string whitelistChar = "", std::string defaultValue = "", size_t minLength = 0, size_t maxLength = SIZE_MAX);

    // Update vector data type
    // Convert a vector of int to a vector of string
    static std::vector<std::string> vInt2vString(const std::vector<int>& vInt);
    // Convert a vector of double to a vector of string with specific accuracy
    static std::vector<std::string> vDouble2vString(const std::vector<double>& vDouble, unsigned int accuracy = 4);
    // Convert a double to a string with specific accuracy, for int, just call std::to_string
    static std::string doubleToString(double number, unsigned int accuracy = 4);
    // Convert a vector to a string in the format { "element1", "element2", ... }
    static std::string vectorToString(const std::vector<std::string>& vString, bool hasQuoteMark = false);
    // Convert a vector to a string, format should be { prefix, open bracket, close bracket, split char, suffix }, missing format will be regarded as empty string
    static std::string vectorToString(const std::vector<std::string>& vString, const std::vector<std::string>& format);
    // Convert a hex representation of a color to RGB, not case-sensitive and leading '#' is allowed, return {-1, -1, -1} if the color is invalid
    static std::vector<int> hexToColor(std::string hex);
    // Convert RGB to hex representation of a color, return is uppercase without leading '#', return empty string if the color is invalid
    static std::string colorToHex(int red, int green, int blue);
    // Convert RGB to hex representation of a color, the color must have exact 3 elements representing red, green, and blue, otherwise return empty string
    static std::string colorToHex(const std::vector<int>& color);

    // Console related functions
    // Set console cursor position
    static void setCursorPosition(short x, short y);
    // Set console cursor position
    static void setCursorPosition(COORD position);
    // Get cursor position, may not be accurate (especially in non-Windows platform)
    static COORD getCursorPosition();
    // Get cursor position X (Column)
    static short getCursorPositionX();
    // Get cursor position Y (Line)
    static short getCursorPositionY();
    // Get current cursor color, can be a color code or an RGB color
    static std::string getCursorColor();

    // Timing function
    // Set the start time to the current system time
    void timeStart();
    // Get the duration from the start time to the current system time in seconds
    double getDurationSeconds();
};