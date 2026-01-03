

#include "m_bbox.h"

void M_ClearBox(fixed_t *box)
{
    box[BOXTOP] = FIXED_MIN;
    box[BOXRIGHT] = FIXED_MIN;
    box[BOXBOTTOM] = FIXED_MAX;
    box[BOXLEFT] = FIXED_MAX;
}

void M_AddToBox(fixed_t *box, fixed_t x, fixed_t y)
{
    if (x < box[BOXLEFT])
        box[BOXLEFT] = x;
    else if (x > box[BOXRIGHT])
        box[BOXRIGHT] = x;

    if (y < box[BOXBOTTOM])
        box[BOXBOTTOM] = y;
    else if (y > box[BOXTOP])
        box[BOXTOP] = y;
}
