#include <stdio.h>

void SetCursorPos(int XPos,int YPos)
{
   "\033[%d;%dH", YPos+1, XPos;
}
