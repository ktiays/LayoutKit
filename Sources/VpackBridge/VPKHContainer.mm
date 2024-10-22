//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

#import <memory>
#import <vector>

#import "VPKEngineCoreConvertible.h"
#import "VPKHContainer.h"
#import "horizontal_container.h"

@interface VPKHContainer () <VPKEngineCoreConvertible>

@end

@implementation VPKHContainer {
    NSArray<VPKLayoutable> *_elements;
    VPKLayoutParams _layoutParams;
    VPKVerticalAlignment _alignment;
}

- (instancetype)initWithElements:(NSArray<VPKLayoutable> *)elements layoutParams:(VPKLayoutParams)params alignment:(VPKVerticalAlignment)alignment {
    self = [super init];
    _elements = elements;
    _layoutParams = params;
    _alignment = alignment;
    return self;
}

- (VPKEngineCoreLayoutablePtr)coreLayoutablePointer {
    std::vector<VPKEngineCoreLayoutablePtr> elements;
    for (id<VPKLayoutable> layoutable in _elements) {
        NSAssert([layoutable conformsToProtocol:@protocol(VPKEngineCoreConvertible)], @"Container elements must conform to `VPKEngineCoreConvertible`");
        auto convertible = ((id<VPKEngineCoreConvertible>) layoutable);
        auto ptr = [convertible coreLayoutablePointer];
        assert(ptr != nullptr);
        elements.push_back(std::move(ptr));
    }
    return std::make_shared<vpk::core::HorizontalContainer<VPKEngineCoreIdentifier, VPKEngineCoreValue>>(
        elements,
        vpk::core::LayoutParams(
            CoreSizeProperty(_layoutParams.sizeProperty),
            CoreEdgeInsets(_layoutParams.padding),
            CorePoint(_layoutParams.offset),
            _layoutParams.priority
        ),
        CoreVerticalAlignment(_alignment)
    );
}

@end
