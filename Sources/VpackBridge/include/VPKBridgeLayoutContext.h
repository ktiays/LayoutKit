//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
// 

#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

NS_SWIFT_NAME(BridgeLayoutContext)
@interface VPKBridgeLayoutContext : NSObject

@property (nonatomic, strong) NSMutableDictionary<NSNumber *, id> *nodeLayoutActions;

- (void)reset;

@end

NS_ASSUME_NONNULL_END
