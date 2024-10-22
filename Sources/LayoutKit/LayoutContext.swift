//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

@propertyWrapper
struct LayoutContext {

    fileprivate static let context: Context = .init()

    var wrappedValue: Context {
        Self.context
    }

    class Context {
        
        struct LayoutDirection: OptionSet {
            
            let rawValue: Int8
            
            static let horizontal: Self = LayoutDirection(rawValue: 1 << 0)
            static let vertical: Self = LayoutDirection(rawValue: 1 << 1)
            static let both: Self = [.horizontal, .vertical]
        }

        fileprivate(set) var isValid: Bool = false

        private var identifierCounter: VPKEngineCoreIdentifier = 0
        let bridgeContext: BridgeLayoutContext = .init()
        
        var layoutDirectionStack: [LayoutDirection] = []

        func nodeIdentifier() -> VPKEngineCoreIdentifier {
            defer {
                identifierCounter &+= 1
            }
            return identifierCounter
        }

        fileprivate func reset() {
            isValid = true
            identifierCounter = 0
            bridgeContext.reset()
        }
    }
}

enum BeginLayoutContextError: Error {
    case alreadyInLayoutContext
}

func beginLayoutContext() throws {
    @LayoutContext var context
    if context.isValid {
        throw BeginLayoutContextError.alreadyInLayoutContext
    }
    context.reset()
}

func endLayoutContext() {
    LayoutContext.context.isValid = false
}
