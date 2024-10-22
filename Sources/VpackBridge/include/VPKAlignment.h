//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

#ifndef VPKAlignment_Header_h
#define VPKAlignment_Header_h

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

typedef NS_ENUM(NSUInteger, VPKHorizontalAlignment) {
    VPKHorizontalAlignmentLeading = 0,
    VPKHorizontalAlignmentCenter,
    VPKHorizontalAlignmentTrailing,
} NS_SWIFT_NAME(EngineHorizontalAlignment);

typedef NS_ENUM(NSUInteger, VPKVerticalAlignment) {
    VPKVerticalAlignmentTop = 0,
    VPKVerticalAlignmentCenter,
    VPKVerticalAlignmentBottom,
} NS_SWIFT_NAME(EngineVerticalAlignment);

NS_ASSUME_NONNULL_END

#endif /* VPKAlignment_Header_h */
