//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

public struct Empty: Layoutable, EngineLayoutableConvertible {

    public var sizeProperty: SizeProperty = .init(minWidth: 0, maxWidth: 0, minHeight: 0, maxHeight: 0)
    public var edgeInsets: EdgeInsets = .init()
    public var offset: CGPoint = .zero

    public let elements: [any Layoutable]? = nil

    @LayoutContext private var context

    public init() {}

    func asEngineLayoutable() -> any EngineLayoutable {
        VPKLayoutNode(
            identifier: context.nodeIdentifier(),
            layoutParams: .init(
                sizeProperty: sizeProperty,
                padding: edgeInsets,
                offset: offset,
                priority: 0
            )
        ) { _ in .zero }
    }
}
