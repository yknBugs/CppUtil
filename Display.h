#pragma once

#include <iostream>
#include <cstring>
#include <sstream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <iomanip>
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
class Display
{
private:
#ifndef _WIN32
    // None windows platform must clear the console the first time this class is initialized because we cannot get the cursor position
    static size_t initCount;
#endif
    static COORD cursorPosition;
    static std::string cursorColor;
    std::chrono::system_clock::time_point startTime;

    static char c();

    // Guess the cursor position for non-Windows platform
    static void changeCursor(char c);
    static void changeCursor(const std::string& text);
    static bool updateCursorPosition();

    // Evaluate the input from different platforms
    static bool inputIsEnter(const std::vector<char>& input);
    static bool inputIsBackspace(const std::vector<char>& input); 
    static bool inputIsLeftArrow(const std::vector<char>& input);
    static bool inputIsRightArrow(const std::vector<char>& input);
    static bool inputIsUpArrow(const std::vector<char>& input);
    static bool inputIsDownArrow(const std::vector<char>& input);
    static bool inputIsFnLeftArrow(const std::vector<char>& input);
    static bool inputIsFnRightArrow(const std::vector<char>& input);
    static bool inputIsControlChar(const std::vector<char>& input);

    // Common algorithm
    int previewGetInputString(COORD originalPosition, std::string originalColor, std::string& inputString, int cursorIndex, int lastVisibleLength);

public:
    Display();
    ~Display();

    // Console related functions
    bool setTextColor(char colorCode);
    bool setTextColor(int red, int green, int blue);
    bool setTextColor(const std::vector<int>& color);
    bool setTextColor(std::string color);
    std::vector<char> getInput();

    void clear();
    void clear(size_t length, COORD position, char c = ' ', bool freezeCursor = true);
    void clear(size_t length, short x, short y, char c = ' ', bool freezeCursor = true);
    void print(const std::string& text);
    void print(const std::string& text, int red, int green, int blue, bool resetColor = true);
    void print(const std::string& text, const std::vector<int>& color, bool resetColor = true);
    void print(const std::string& text, std::string color, bool resetColor = true);

    // Show text to the terminal, use "&+Number" to change the color, use "#" to be a placeholder for parameters
    void showText(char c, char colorCode = 'r');
    void showText(const std::string& text);
    void showText(const std::string& text, const std::vector<std::string>& parameters);
    void showText(const std::string& text, const std::vector<int>& parameters);
    void showText(const std::string& text, const std::vector<double>& parameters, unsigned int accuracy = 4);
    void showText(const std::string& text1, const std::string& parameter, const std::string& text2);
    void showText(const std::string& text1, int parameter, const std::string& text2);
    void showText(const std::string& text1, double parameter, const std::string& text2, unsigned int accuracy = 4);

    // User Input
    int getInputInt(int max = 2147483647, bool canBelowZero = true);
    double getInputDouble(bool canBelowZero = true, bool acceptNaN = false, size_t maxLength = 18446744073709551615ULL);
    std::string getInputString(size_t minLength = 0, size_t maxLength = 18446744073709551615ULL);

    // Update vector data type
    static std::vector<std::string> vInt2vString(const std::vector<int>& vInt);
    static std::vector<std::string> vDouble2vString(const std::vector<double>& vDouble, unsigned int accuracy = 4);
    static std::string doubleToString(double number, unsigned int accuracy = 4);
    static std::vector<int> hexToColor(std::string hex);
    static std::string colorToHex(int red, int green, int blue);
    static std::string colorToHex(const std::vector<int>& color);

    static void setCursorPosition(short x, short y);
    static void setCursorPosition(COORD position);
    static COORD getCursorPosition();
    static int getCursorPositionX();
    static int getCursorPositionY();
    static std::string getCursorColor();

    // Timing function
    void timeStart();
    double getDurationSeconds();
};