//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

#ifndef VPKEngineCoreConvertible_Header_h
#define VPKEngineCoreConvertible_Header_h

#import <cmath>
#import <memory>

#import <Foundation/Foundation.h>

#import "optional.h"
#import "VPKAlignment.h"
#import "VPKLayoutParams.h"
#import "layoutable.h"
#import "VPKLayoutable.h"

typedef vpk::core::LayoutablePointer<VPKEngineCoreIdentifier, VPKEngineCoreValue> VPKEngineCoreLayoutablePtr;
typedef vpk::core::Size<VPKEngineCoreValue> VPKEngineSize;

vpk::core::HorizontalAlignment CoreHorizontalAlignment(const VPKHorizontalAlignment alignment);
vpk::core::VerticalAlignment CoreVerticalAlignment(const VPKVerticalAlignment alignment);

inline vpk::core::Alignment CoreAlignment(const VPKHorizontalAlignment horizontal_alignment, const VPKVerticalAlignment vertical_alignment) {
    return vpk::core::Alignment(CoreVerticalAlignment(vertical_alignment), CoreHorizontalAlignment(horizontal_alignment));
}

inline static vpk::optional<VPKEngineCoreValue> CoreOptionalValue(const CGFloat value) {
    return std::isnan(value) ? vpk::nullopt : vpk::optional<VPKEngineCoreValue>(value);
}

vpk::core::SizeProperty<VPKEngineCoreValue> CoreSizeProperty(const VPKSizeProperty size_property);

inline vpk::core::EdgeInsets<VPKEngineCoreValue> CoreEdgeInsets(const VPKEdgeInsets edge_insets) {
    return vpk::core::EdgeInsets<VPKEngineCoreValue>(edge_insets.left, edge_insets.top, edge_insets.right, edge_insets.bottom);
}

inline vpk::core::Point<VPKEngineCoreValue> CorePoint(const CGPoint point) {
    return vpk::core::Point<VPKEngineCoreValue>(point.x, point.y);
}

@protocol VPKEngineCoreConvertible <NSObject>

- (VPKEngineCoreLayoutablePtr)coreLayoutablePointer;

@end

#endif /* VPKEngineCoreConvertible_Header_h */
