//
// Created by 咔咔 on 2021/7/13.
//

#include <utility>
#include "Text.h"
#include "Logger.h"

TextColor::TextColor(char code, int color, bool isBright) {
    this->colorCode = code;
    this->color = color;
    this->isBright = isBright;

    byCode.insert(std::pair<char, TextColor*>(code, this));
}

std::string TextColor::toAscii() {
    std::string brightPrefix = this->isBright ? ";1" : "";
    std::ostringstream stream;
    stream << "\u001b[" << color << brightPrefix << "m";
    return stream.str();
}

TextColor *TextColor::of(char c) {
    auto iter = byCode.find(c);
    if(iter == byCode.end()) {
        return nullptr;
    }
    return iter->second;
}

std::map<char, TextColor*> TextColor::byCode;

TextColor* TextColor::Black      = new TextColor('0', 30);
TextColor* TextColor::DarkBlue   = new TextColor('1', 30, true);
TextColor* TextColor::DarkGreen  = new TextColor('2', 30, true);
TextColor* TextColor::DarkAqua   = new TextColor('3', 30, true);
TextColor* TextColor::DarkRed    = new TextColor('4', 30, true);
TextColor* TextColor::DarkPurple = new TextColor('5', 30, true);
TextColor* TextColor::Gold       = new TextColor('6', 33);
TextColor* TextColor::Gray       = new TextColor('7', 37);
TextColor* TextColor::DarkGray   = new TextColor('8', 30, true);
TextColor* TextColor::Blue       = new TextColor('9', 30, true);
TextColor* TextColor::Green      = new TextColor('a', 32, true);
TextColor* TextColor::Aqua       = new TextColor('b', 34, true);
TextColor* TextColor::Red        = new TextColor('c', 31, true);
TextColor* TextColor::Purple     = new TextColor('d', 31, true);
TextColor* TextColor::Yellow     = new TextColor('e', 33, true);
TextColor* TextColor::White      = new TextColor('f', 37, true);
TextColor* TextColor::Reset      = new TextColor('r', 0);

LiteralText::LiteralText() = default;

LiteralText *LiteralText::of(std::string text) {
    auto result = new LiteralText();
    result->text = std::move(text);
    return result;
}

