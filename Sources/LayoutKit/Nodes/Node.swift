//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

public struct Node: Layoutable, EngineLayoutableConvertible {

    public var sizeProperty: SizeProperty = .nan
    public var edgeInsets: EdgeInsets = .init()
    public var offset: CGPoint = .zero

    public let elements: [any Layoutable]? = nil
    private let layoutBody: (@convention(block) (CGRect) -> Void)
    private var measureBody: ((@convention(block) (CGSize) -> CGSize))?

    @LayoutContext private var context

    public init(_ layout: @escaping (CGRect) -> Void) {
        self.layoutBody = layout
    }

    func asEngineLayoutable() -> any EngineLayoutable {
        let id = context.nodeIdentifier()
        let bridgeContext = context.bridgeContext
        assert(bridgeContext.nodeLayoutActions.object(forKey: id) == nil)
        bridgeContext.nodeLayoutActions[id] = layoutBody

        return VPKLayoutNode(
            identifier: id,
            layoutParams: .init(
                sizeProperty: sizeProperty,
                padding: edgeInsets,
                offset: offset,
                priority: 0
            )
        ) { size in
            let minWidth = sizeProperty.minWidth
            let minHeight = sizeProperty.minHeight
            let maxWidth = sizeProperty.maxWidth
            let maxHeight = sizeProperty.maxHeight

            let defaultWidth: CGFloat =
                if !minWidth.isNaN && !maxWidth.isNaN {
                    minWidth == maxWidth ? minWidth : 0
                } else {
                    0
                }
            let defaultHeight: CGFloat =
                if !minHeight.isNaN && !maxHeight.isNaN {
                    minHeight == maxHeight ? minHeight : 0
                } else {
                    0
                }
            guard let measureBody else {
                return CGSize(width: defaultWidth, height: defaultHeight)
            }

            let measured = measureBody(size)
            return CGSize(
                width: defaultWidth > 0 ? defaultWidth : measured.width,
                height: defaultHeight > 0 ? defaultHeight : measured.height
            )
        }
    }

    public func measure(_ body: @escaping (CGSize) -> CGSize) -> Self {
        var newSelf = self
        newSelf.measureBody = body
        return newSelf
    }
}
