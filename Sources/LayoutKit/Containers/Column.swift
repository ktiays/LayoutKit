//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

public struct Column<Content>: Layoutable, EngineLayoutableConvertible where Content: Layoutable {

    public let alignment: HorizontalAlignment
    public let spacing: CGFloat
    public var sizeProperty: SizeProperty = .nan
    public var edgeInsets: EdgeInsets = .init()
    public var offset: CGPoint = .zero

    public let elements: [any Layoutable]?
    
    public init(
        alignment: HorizontalAlignment = .center,
        spacing: CGFloat = 0,
        @LayoutableBuilder content: () -> Content
    ) {
        self.alignment = alignment
        self.spacing = spacing
        
        @LayoutContext var context
        context.layoutDirectionStack.append(.vertical)
        let content = content()
        _ = context.layoutDirectionStack.popLast()
        self.elements = content.elements
    }

    func asEngineLayoutable() -> any EngineLayoutable {
        VPKVContainer(
            elements: engineElements(from: elements),
            layoutParams: .init(sizeProperty: sizeProperty, padding: edgeInsets, offset: offset, priority: 0),
            alignment: alignment.engineRepresention
        )
    }
}
