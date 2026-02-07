#pragma once

class ConsoleCursor
{
public:
    struct CursorPosition
    {
        int cursorX;
        int cursorY;

        inline void SetPosition(int newCursorX, int newCursorY)
        {
            cursorX = newCursorX;
            cursorY = newCursorY;
        }

        inline void AddPosition(int xAddition, int yAddition)
        {
            cursorX += xAddition;
            cursorY += yAddition;
        }
    };

public:
    static CursorPosition cursorPosition;
    static const char* currentCellTextInfo;
};