//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
// 

#if canImport(AppKit)
import AppKit

extension NSView {
    
    public var layoutable: some Layoutable {
        Node { [weak self] rect in
            self?.frame = rect
        }
    }
}

#endif
