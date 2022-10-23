/*
 * rtc_macro.h
 *
 * Created by Ruibin.Chow on 2022/10/23.
 * Copyright (c) 2022年 Ruibin.Chow All rights reserved.
 */

#ifndef RTC_MACRO_H
#define RTC_MACRO_H

#include <unordered_map>
#include <string>

#define RTC_INLINE inline

namespace core {

using RTCString = std::string;
using StringHashMap = std::unordered_map<RTCString, RTCString>;

/* Points. */
struct RTCPoint {
    float x;
    float y;
};
typedef struct RTCPoint RTCPoint;

/* Sizes. */
struct RTCSize {
    float width;
    float height;
};
typedef struct RTCSize RTCSize;

/* Vectors. */
struct RTCVector {
    float dx;
    float dy;
};
typedef struct RTCVector RTCVector;

/* Rectangles. */
struct RTCRect {
    RTCPoint origin;
    RTCSize size;
};
typedef struct RTCRect RTCRect;

RTC_INLINE RTCPoint
RTCPointMake(float x, float y) {
    RTCPoint p; p.x = x; p.y = y; return p;
}

RTC_INLINE RTCSize
RTCSizeMake(float width, float height) {
    RTCSize size; size.width = width; size.height = height; return size;
}

RTC_INLINE RTCVector
RTCVectorMake(float dx, float dy) {
    RTCVector vector; vector.dx = dx; vector.dy = dy; return vector;
}

RTC_INLINE RTCRect
RTCRectMake(float x, float y, float width, float height) {
    RTCRect rect;
    rect.origin.x = x; rect.origin.y = y;
    rect.size.width = width; rect.size.height = height;
    return rect;
}

RTC_INLINE bool
RTCPointEqualToPoint(RTCPoint point1, RTCPoint point2) {
    return point1.x == point2.x && point1.y == point2.y;
}

RTC_INLINE bool
RTCSizeEqualToSize(RTCSize size1, RTCSize size2) {
    return size1.width == size2.width && size1.height == size2.height;
}

}


#endif /* !RTC_MACRO_H */
