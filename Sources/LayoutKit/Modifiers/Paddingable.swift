//
//  Created by ktiays on 2024/10/21.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

public protocol Paddingable {

    var edgeInsets: EdgeInsets { get set }
}

extension Paddingable {

    public func padding(_ edges: Edge.Set, _ length: CGFloat) -> Self {
        var newSelf = self
        if edges.contains(.top) {
            newSelf.edgeInsets.top += length
        }
        if edges.contains(.leading) {
            newSelf.edgeInsets.left += length
        }
        if edges.contains(.bottom) {
            newSelf.edgeInsets.bottom += length
        }
        if edges.contains(.trailing) {
            newSelf.edgeInsets.right += length
        }
        return newSelf
    }

    public func padding(_ edgeInsets: PlatformEdgeInsets) -> Self {
        var newSelf = self
        newSelf.edgeInsets.top += edgeInsets.top
        newSelf.edgeInsets.left += edgeInsets.left
        newSelf.edgeInsets.bottom += edgeInsets.bottom
        newSelf.edgeInsets.right += edgeInsets.right
        return newSelf
    }
}
