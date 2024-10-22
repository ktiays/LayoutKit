//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

#import <memory>

#import "VPKEngineCoreConvertible.h"
#import "VPKLayoutNode.h"
#import "item.h"

@interface VPKLayoutNode () <VPKEngineCoreConvertible>

@end

@implementation VPKLayoutNode {
    VPKEngineCoreIdentifier _identifier;
    VPKLayoutParams _layoutParams;
    CGSize (^_measure)(CGSize);
}

- (instancetype)initWithIdentifier:(VPKEngineCoreIdentifier)identifier layoutParams:(VPKLayoutParams)layoutParams measure:(CGSize (^)(CGSize))measure {
    self = [super init];
    _identifier = identifier;
    _layoutParams = layoutParams;
    _measure = measure;
    return self;
}

- (VPKEngineCoreLayoutablePtr)coreLayoutablePointer {
    __weak auto weakSelf = self;
    auto measurable = std::make_shared<vpk::core::AnyMeasurable<VPKEngineCoreValue>>([weakSelf](VPKEngineSize size) {
        __strong auto strongSelf = weakSelf;
        if (!strongSelf) {
            return VPKEngineSize();
        }
        auto result = strongSelf->_measure(CGSizeMake(size.width, size.height));
        return VPKEngineSize(result.width, result.height);
    });
    return std::make_shared<vpk::core::Item<VPKEngineCoreIdentifier, VPKEngineCoreValue>>(
        _identifier,
        vpk::core::LayoutParams(
            CoreSizeProperty(_layoutParams.sizeProperty),
            CoreEdgeInsets(_layoutParams.padding),
            CorePoint(_layoutParams.offset),
            _layoutParams.priority
        ),
        measurable
    );
}

@end
