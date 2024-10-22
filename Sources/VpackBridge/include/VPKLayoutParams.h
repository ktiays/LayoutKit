//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

NS_SWIFT_NAME(EdgeInsets)
typedef struct NS_SWIFT_SENDABLE {
    CGFloat top;
    CGFloat left;
    CGFloat bottom;
    CGFloat right;
} VPKEdgeInsets;

NS_SWIFT_NAME(SizeProperty)
typedef struct NS_SWIFT_SENDABLE {
    CGFloat minWidth;
    CGFloat maxWidth;
    CGFloat minHeight;
    CGFloat maxHeight;
} VPKSizeProperty;

NS_SWIFT_NAME(LayoutParams)
typedef struct NS_SWIFT_SENDABLE {
    VPKSizeProperty sizeProperty;
    VPKEdgeInsets padding;
    CGPoint offset;
    int priority;
} VPKLayoutParams;

NS_ASSUME_NONNULL_END
