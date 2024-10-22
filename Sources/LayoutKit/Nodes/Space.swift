//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

public struct Space: Layoutable, EngineLayoutableConvertible {

    public var sizeProperty: SizeProperty = .init()
    public var edgeInsets: EdgeInsets = .init()
    public var offset: CGPoint = .zero

    public let elements: [any Layoutable]? = nil
    private let direction: LayoutContext.Context.LayoutDirection

    @LayoutContext private var context

    public init() {
        @LayoutContext var context
        direction = context.layoutDirectionStack.last ?? .both
    }

    func asEngineLayoutable() -> any EngineLayoutable {
        let sizeProperty = SizeProperty(
            minWidth: 0,
            maxWidth: direction.contains(.horizontal) ? .infinity : 0,
            minHeight: 0,
            maxHeight: direction.contains(.vertical) ? .infinity : 0
        )
        return VPKLayoutNode(
            identifier: context.nodeIdentifier(),
            layoutParams: .init(sizeProperty: sizeProperty, padding: edgeInsets, offset: offset, priority: -1)
        ) {
            .init(
                width: direction.contains(.horizontal) ? $0.width : 0,
                height: direction.contains(.vertical) ? $0.height : 0
            )
        }
    }
}
