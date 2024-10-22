//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

public struct Stack<Content>: Layoutable, EngineLayoutableConvertible where Content: Layoutable {

    public let alignment: Alignment
    public var sizeProperty: SizeProperty = .nan
    public var edgeInsets: EdgeInsets = .init()
    public var offset: CGPoint = .zero

    public let elements: [any Layoutable]?

    public init(
        alignment: Alignment = .center,
        @LayoutableBuilder content: () -> Content
    ) {
        self.alignment = alignment

        @LayoutContext var context
        context.layoutDirectionStack.append(.both)
        let content = content()
        _ = context.layoutDirectionStack.popLast()
        self.elements = content.elements
    }

    func asEngineLayoutable() -> any EngineLayoutable {
        VPKZContainer(
            elements: engineElements(from: elements),
            layoutParams: .init(sizeProperty: sizeProperty, padding: edgeInsets, offset: offset, priority: 0),
            horizontalAlignment: alignment.horizontal.engineRepresention,
            verticalAlignment: alignment.vertical.engineRepresention
        )
    }
}
