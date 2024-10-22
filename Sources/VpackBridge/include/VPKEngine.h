//
//  Created by ktiays on 2024/10/21.
//  Copyright (c) 2024 ktiays. All rights reserved.
// 

#ifndef VPKEngine_Header_h
#define VPKEngine_Header_h

#import <Foundation/Foundation.h>

#import "VPKLayoutable.h"
#import "VPKBridgeLayoutContext.h"

NS_ASSUME_NONNULL_BEGIN

FOUNDATION_EXTERN void vpk_layout(VPKBridgeLayoutContext *context, CGRect rect, id<VPKLayoutable> layoutable);

NS_ASSUME_NONNULL_END

#endif /* VPKEngine_Header_h */
