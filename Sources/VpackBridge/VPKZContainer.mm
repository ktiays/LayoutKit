//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

#import <memory>
#import <vector>

#import "VPKEngineCoreConvertible.h"
#import "VPKLayoutParams.h"
#import "VPKZContainer.h"

#import "stack_container.h"

@interface VPKZContainer () <VPKEngineCoreConvertible>

@end

@implementation VPKZContainer {
    NSArray<VPKLayoutable> *_elements;
    VPKLayoutParams _layoutParams;
    VPKHorizontalAlignment _horizontalAlignment;
    VPKVerticalAlignment _verticalAlignment;
}

- (instancetype)initWithElements:(NSArray<VPKLayoutable> *)elements
                    layoutParams:(VPKLayoutParams)params
             horizontalAlignment:(VPKHorizontalAlignment)horizontalAlignment
               verticalAlignment:(VPKVerticalAlignment)verticalAlignment {
    self = [super init];
    _elements = elements;
    _layoutParams = params;
    _horizontalAlignment = horizontalAlignment;
    _verticalAlignment = verticalAlignment;
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
    return std::make_shared<vpk::core::StackContainer<VPKEngineCoreIdentifier, VPKEngineCoreValue>>(
        elements,
        vpk::core::LayoutParams(CoreSizeProperty(_layoutParams.sizeProperty), CoreEdgeInsets(_layoutParams.padding), CorePoint(_layoutParams.offset)),
        CoreAlignment(_horizontalAlignment, _verticalAlignment));
}

@end
