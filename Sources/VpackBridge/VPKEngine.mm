//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

#import "VPKEngine.h"
#import "VPKEngineCoreConvertible.h"
#import "computer.h"

template <typename T>
void unused(T t) { }

extern "C" {

void vpk_layout(VPKBridgeLayoutContext *context, CGRect rect, id<VPKLayoutable> layoutable) {
    assert([layoutable conformsToProtocol:@protocol(VPKEngineCoreConvertible)]);
    auto ptr = [((id<VPKEngineCoreConvertible>) layoutable) coreLayoutablePointer];
    auto computer = vpk::core::LayoutComputer<VPKEngineCoreIdentifier, VPKEngineCoreValue>(ptr);
    auto result = computer.compute(vpk::core::Rect<VPKEngineCoreValue>(rect.origin.x, rect.origin.y, rect.size.width, rect.size.height));
    
    auto layoutActions = context.nodeLayoutActions;
    for (const auto& [key, value] : result.map) {
        void (^body)(CGRect) = [layoutActions objectForKey:@(key)];
        if (body) {
            auto frame = value.frame;
            auto rect = CGRectMake(frame.x, frame.y, frame.width, frame.height);
            body(rect);
        }
    }
    unused(result);
}

}
