//
//  Created by ktiays on 2024/10/21.
//  Copyright (c) 2024 ktiays. All rights reserved.
// 

import Foundation
import VpackBridge

public struct ArrayLayoutable: Layoutable {
    
    public var sizeProperty: SizeProperty = .nan
    public var edgeInsets: EdgeInsets = .init()
    public var offset: CGPoint = .zero
    
    public let elements: [any Layoutable]?
    
    init(_ elements: [any Layoutable]) {
        self.elements = elements
    }
    
    func asEngineLayoutables() -> [any EngineLayoutable] {
        engineElements(from: elements)
    }
}

func engineElements(from elements: [any Layoutable]?) -> [any EngineLayoutable] {
    guard let elements else { return [] }
    var result: [any EngineLayoutable] = []
    for element in elements {
        if let convertible = element as? EngineLayoutableConvertible {
            let layoutable = convertible.asEngineLayoutable()
            result.append(layoutable)
        } else if let array = element as? ArrayLayoutable {
            let layoutables = array.asEngineLayoutables()
            result.append(contentsOf: layoutables)
        } else {
            assertionFailure()
        }
    }
    return result
}
