//
// Created by 咔咔 on 2021/7/13.
//

#ifndef K_LOGGER_H
#define K_LOGGER_H

#include "Text.h"
#include <ctime>
#include <utility>

class Logger {
private:
    static void internalLog(TextColor* color, std::string name, Text* text) {
        time_t rawtime;
        struct tm* timeinfo;

        char buffer[80];
        time(&rawtime);
        timeinfo = localtime(&rawtime);

        strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", timeinfo);
        std::string str(buffer);

        auto format = TranslateText::of("%s %s %s")
                ->addWith(LiteralText::of("[" + str + "]")->setColor(TextColor::Gray))
                ->addWith(LiteralText::of("[" + name + "]")->setColor(color))
                ->addWith(text);
        std::cout << format->toAscii() << std::endl;
    }
public:
    static void log(std::string name, Text* text) {
        internalLog(TextColor::Gray, std::move(name), text);
    }

    static void info(std::string name, Text* text) {
        internalLog(TextColor::Green, std::move(name), text);
    }

    static void warn(std::string name, Text* text) {
        internalLog(TextColor::Gold, std::move(name), text);
    }

    static void error(std::string name, Text* text) {
        internalLog(TextColor::Red, std::move(name), text);
    }
};

#endif // K_LOGGER_H
