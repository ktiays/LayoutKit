//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

#import <memory>
#import <vector>

#import "VPKEngineCoreConvertible.h"
#import "VPKLayoutParams.h"
#import "VPKVContainer.h"

#import "vertical_container.h"

@interface VPKVContainer () <VPKEngineCoreConvertible>

@end

@implementation VPKVContainer {
    NSArray<VPKLayoutable> *_elements;
    VPKLayoutParams _layoutParams;
    VPKHorizontalAlignment _alignment;
}

- (instancetype)initWithElements:(NSArray<VPKLayoutable> *)elements layoutParams:(VPKLayoutParams)params alignment:(VPKHorizontalAlignment)alignment {
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
    return std::make_shared<vpk::core::VerticalContainer<VPKEngineCoreIdentifier, VPKEngineCoreValue>>(
        elements,
        vpk::core::LayoutParams(CoreSizeProperty(_layoutParams.sizeProperty), CoreEdgeInsets(_layoutParams.padding), CorePoint(_layoutParams.offset), _layoutParams.priority),
        CoreHorizontalAlignment(_alignment)
    );
}

@end
