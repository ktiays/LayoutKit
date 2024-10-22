//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
// 

#import "VPKEngineCoreConvertible.h"

vpk::core::HorizontalAlignment CoreHorizontalAlignment(const VPKHorizontalAlignment alignment) {
    switch (alignment) {
    case VPKHorizontalAlignmentLeading:
        return vpk::core::HorizontalAlignment::leading;
    case VPKHorizontalAlignmentCenter:
        return vpk::core::HorizontalAlignment::center;
    case VPKHorizontalAlignmentTrailing:
        return vpk::core::HorizontalAlignment::trailing;
    }
}

vpk::core::VerticalAlignment CoreVerticalAlignment(const VPKVerticalAlignment alignment) {
    switch (alignment) {
    case VPKVerticalAlignmentTop:
        return vpk::core::VerticalAlignment::top;
    case VPKVerticalAlignmentCenter:
        return vpk::core::VerticalAlignment::center;
    case VPKVerticalAlignmentBottom:
        return vpk::core::VerticalAlignment::bottom;
    }
}

vpk::core::SizeProperty<VPKEngineCoreValue> CoreSizeProperty(const VPKSizeProperty size_property) {
    return vpk::core::SizeProperty<VPKEngineCoreValue>(
        CoreOptionalValue(size_property.minWidth),
        CoreOptionalValue(size_property.minHeight),
        CoreOptionalValue(size_property.maxWidth),
        CoreOptionalValue(size_property.maxHeight));
}
