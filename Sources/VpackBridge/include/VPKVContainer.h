//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "VPKAlignment.h"
#import "VPKLayoutParams.h"
#import "VPKLayoutable.h"

NS_ASSUME_NONNULL_BEGIN

@interface VPKVContainer : NSObject <VPKLayoutable>

- (instancetype)initWithElements:(NSArray<VPKLayoutable> *)elements layoutParams:(VPKLayoutParams)params alignment:(VPKHorizontalAlignment)alignment;

@end

NS_ASSUME_NONNULL_END
