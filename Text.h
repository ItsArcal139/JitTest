//
// Created by 咔咔 on 2021/7/13.
//

#ifndef K_TEXT_H
#define K_TEXT_H

#include <iostream>
#include <map>
#include <utility>
#include <vector>
#include <sstream>

class TextColor {
private:
    static std::map<char, TextColor*> byCode;
public:
    static TextColor* Black;
    static TextColor* DarkBlue;
    static TextColor* DarkGreen;
    static TextColor* DarkAqua;
    static TextColor* DarkRed;
    static TextColor* DarkPurple;
    static TextColor* Gold;
    static TextColor* Gray;
    static TextColor* DarkGray;
    static TextColor* Blue;
    static TextColor* Green;
    static TextColor* Aqua;
    static TextColor* Red;
    static TextColor* Purple;
    static TextColor* Yellow;
    static TextColor* White;
    static TextColor* Reset;

    char colorCode;
    int color;
    bool isBright;

    TextColor(char code, int color, bool isBright = false);
    virtual std::string toAscii();

    static TextColor* of(char c);
};

class LiteralText;

class Text {
protected:
    std::vector<Text*> extras;
public:
    TextColor* color;
    Text* parent;

    Text() {
        color = nullptr;
        parent = nullptr;
    }

    virtual std::string toAscii() {
        std::ostringstream stream;
        for(auto iter = extras.begin(); iter != extras.end(); iter++) {
            stream << (*extras.begin())->toAscii();
            if(color != nullptr) {
                stream << color->toAscii();
            } else {
                stream << getParentColor()->toAscii();
            }
        }

        return stream.str();
    }

    virtual TextColor* getParentColor() {
        if(parent == nullptr) return TextColor::Reset;
        if(parent->color == nullptr) {
            return parent->getParentColor();
        }
        return parent->color;
    }
};

template<class T> class TextGeneric : public Text {
public:
    T* addExtra(Text* text) {
        extras.push_back(text);
        text->parent = this;
        return resolveThis();
    }

    T* setColor(TextColor* color) {
        this->color = color;
        return resolveThis();
    }

    virtual T* resolveThis() {
        FILE* f = stderr;
        char err[] = "The method resolveThis() is not implemented by base class.";
        fwrite(err, sizeof(err), 1, f);
        return nullptr;
    }
};

class LiteralText : public TextGeneric<LiteralText> {
private:
    LiteralText();
public:
    std::string text;

    LiteralText* resolveThis() override {
        return this;
    }

    static LiteralText* of(std::string text);
    std::string toAscii() override {
        std::string base = this->Text::toAscii();
        std::string c;
        if(color != nullptr) {
            c = color->toAscii();
        } else {
            c = getParentColor()->toAscii();
        }

        std::ostringstream stream;
        stream << c << text << base;
        return stream.str();
    }
};

class TranslateText : public TextGeneric<TranslateText> {
public:
    std::string translate;
    std::vector<Text*> withs;

    TranslateText* addWith(Text* text) {
        withs.push_back(text);
        text->parent = this;
        return this;
    }

    std::string toAscii() override {
        std::string extra = this->Text::toAscii();
        std::string color;
        if(this->color != nullptr) {
            color = this->color->toAscii();
        } else {
            color = getParentColor()->toAscii();
        }

        std::string output;
        const char* fmt = translate.c_str();
        char* p = const_cast<char *>(fmt);

        int i = 0;
        while(*p) {
            if(*p == '%') {
                p++;
                if(*p != 's' || i >= withs.size()) {
                    // Treat it as not formatted
                    output += "%";
                    output += *p;
                } else {
                    Text* t = withs[i];
                    output += t->toAscii();
                    i++;
                }
                p++;
            } else {
                output += *p;
                p++;
            }
        }

        return color + output + extra;
    }

    static TranslateText* of(std::string fmt) {
        auto result = new TranslateText();
        result->translate = std::move(fmt);
        return result;
    }
};

#endif // K_TEXT_H
