//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

extension SizeProperty {
    /// A quiet NaN.
    static let nan: Self = .init(minWidth: .nan, maxWidth: .nan, minHeight: .nan, maxHeight: .nan)
}

public protocol FrameConstrainable {

    var sizeProperty: SizeProperty { get set }
}

extension FrameConstrainable {

    public func frame(width: CGFloat? = nil, height: CGFloat? = nil) -> Self {
        var newSelf = self
        if let width {
            newSelf.sizeProperty.minWidth = width
            newSelf.sizeProperty.maxWidth = width
        }
        if let height {
            newSelf.sizeProperty.minHeight = height
            newSelf.sizeProperty.maxHeight = height
        }
        return newSelf
    }

    public func frame(
        minWidth: CGFloat? = nil,
        maxWidth: CGFloat? = nil,
        minHeight: CGFloat? = nil,
        maxHeight: CGFloat? = nil
    ) -> Self {
        var newSelf = self
        if let minWidth {
            newSelf.sizeProperty.minWidth = minWidth
        }
        if let maxWidth {
            newSelf.sizeProperty.maxWidth = maxWidth
        }
        if let minHeight {
            newSelf.sizeProperty.minHeight = minHeight
        }
        if let maxHeight {
            newSelf.sizeProperty.maxHeight = maxHeight
        }
        return newSelf
    }
}
