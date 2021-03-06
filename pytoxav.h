/**
 * pytoxcore
 *
 * Copyright (C) 2015 Anton Batenev <antonbatenev@yandex.ru>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */
//----------------------------------------------------------------------------------------------
#ifndef _pytoxav_h_
#define _pytoxav_h_
//----------------------------------------------------------------------------------------------
#include "pytoxcore.h"
//----------------------------------------------------------------------------------------------
typedef enum {
    TOXAV_VIDEO_FRAME_FORMAT_BGR,
    TOXAV_VIDEO_FRAME_FORMAT_RGB,
    TOXAV_VIDEO_FRAME_FORMAT_YUV420
} TOXAV_VIDEO_FRAME_FORMAT;
//----------------------------------------------------------------------------------------------
typedef struct {
    PyObject_HEAD
    ToxAV*                   av;
    ToxCore*                 core;
    vpx_image_t*             frame;
    pthread_mutex_t*         frame_mutex;
    TOXAV_VIDEO_FRAME_FORMAT format;
    uint8_t*                 rgb;
    pthread_mutex_t*         rgb_mutex;
    size_t                   rgb_size;
} ToxCoreAV;
//----------------------------------------------------------------------------------------------
extern PyTypeObject ToxAVType;
//----------------------------------------------------------------------------------------------
extern PyObject* ToxAVException;
//----------------------------------------------------------------------------------------------
void ToxAV_install_dict(void);
//----------------------------------------------------------------------------------------------
#endif   // _pytoxav_h_
//----------------------------------------------------------------------------------------------
