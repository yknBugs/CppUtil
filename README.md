# Cpp Util  C++ 工具

This repository contains some useful functions.

本仓库包含一系列工具函数。

## Display

Class `Display` is mainly used for the console input and output. With it, you can print text with different colors easily, and you can also get the user input and make sure the input is valid.

`Display`类主要用于控制台的输入输出。通过此类，可以方便的在控制台上显示彩色文字，还可以方便的读取用户输入并保证其合法性。

Example Usage:

使用示例:

```cpp
Display display;
// Use &+Number to change text colors
display.print("Hello, World!\n", 158, 210, 13);
display.showText("&6Insert a Integer: &a");
int inputInt = display.getInputInt();
display.showText("&6Insert a Double: &a");
double inputDouble = display.getInputDouble(true, true, 6);
display.showText("&6inputInt * inputDouble = &a", inputInt * inputDouble, "\n");
```
