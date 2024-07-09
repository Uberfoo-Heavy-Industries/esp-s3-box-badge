
/*

MIT No Attribution

Copyright (c) 2023 James Bryant

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

-cut-

SPDX-License-Identifier: MIT-0

*/

#ifndef _FIRE_H_
#define _FIRE_H_

#include <stdint.h>
#include "bsp/esp-bsp.h"


class Fire {
    private:
    
    lv_obj_t *canvas;
    uint8_t *fire;
    lv_color_t palette[256];

    public: 

    Fire(lv_obj_t *canvas);

    void init();
    void playFire();
};

typedef struct fire_thrd_params {
    int start;
    uint8_t *fire;
} fire_thrd_params;

#endif // _FIRE_H_