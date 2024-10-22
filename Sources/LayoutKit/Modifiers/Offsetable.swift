//
//  Created by ktiays on 2024/10/21.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation

public protocol Offsetable {

    var offset: CGPoint { get set }
}

extension Offsetable {

    public func offset(x: CGFloat, y: CGFloat) -> Self {
        var newSelf = self
        newSelf.offset.x += x
        newSelf.offset.y += y
        return newSelf
    }

    public func offset(_ offset: CGSize) -> Self {
        var newSelf = self
        newSelf.offset.x += offset.width
        newSelf.offset.y += offset.height
        return newSelf
    }
}
