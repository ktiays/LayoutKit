//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "VPKLayoutParams.h"
#import "VPKLayoutable.h"

NS_ASSUME_NONNULL_BEGIN

@interface VPKLayoutNode : NSObject <VPKLayoutable>

- (instancetype)initWithIdentifier:(VPKEngineCoreIdentifier)identifier
                      layoutParams:(VPKLayoutParams)layoutParams
                           measure:(CGSize (^)(CGSize size))measure;

@end

NS_ASSUME_NONNULL_END
