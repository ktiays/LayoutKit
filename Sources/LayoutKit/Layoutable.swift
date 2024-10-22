//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

public protocol Layoutable: Paddingable, Offsetable, FrameConstrainable {

    var elements: [any Layoutable]? { get }
}

public func withLayoutable<Content>(in rect: CGRect, @_implicitSelfCapture @LayoutableBuilder _ content: () -> Content) where Content: Layoutable {
    if !Thread.isMainThread {
        assertionFailure("You must call `withLayoutable` on the main thread.")
        return
    }
    
    do {
        try beginLayoutContext()
        
        @LayoutContext var context
        
        let content = Stack {
            content()
        }
        let engineLayoutable = content.asEngineLayoutable()
        vpk_layout(context.bridgeContext, rect, engineLayoutable)
        endLayoutContext()
    } catch {
        switch error {
        case BeginLayoutContextError.alreadyInLayoutContext:
            logger.error("You can only call `withLayoutable` once in the same scope.")
        default:
            logger.error("Begin layout context error: \(error)")
        }
    }
}
