//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
// 

#import "VPKBridgeLayoutContext.h"

@implementation VPKBridgeLayoutContext

- (instancetype)init {
    self = [super init];
    if (self) {
        _nodeLayoutActions = [NSMutableDictionary dictionary];
    }
    return self;
}

- (void)reset {
    [_nodeLayoutActions removeAllObjects];
}

@end
