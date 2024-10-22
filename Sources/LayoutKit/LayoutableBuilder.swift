//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation

@resultBuilder
public struct LayoutableBuilder {
    
    public static func buildBlock() -> Empty {
        .init()
    }
    
    public static func buildBlock<each Content>(_ content: repeat each Content) -> ArrayLayoutable where repeat each Content: Layoutable {
        var elements = [any Layoutable]()
        repeat elements.append(each content)
        return .init(elements)
    }
}
