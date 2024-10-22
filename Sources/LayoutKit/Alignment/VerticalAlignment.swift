//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

/// An alignment position along the vertical axis.
public enum VerticalAlignment: Equatable {

    /// A guide that marks the top edge of the view.
    case top

    /// A guide that marks the vertical center of the view.
    case center

    /// A guide that marks the bottom edge of the view.
    case bottom
}

extension VerticalAlignment {

    var engineRepresention: EngineVerticalAlignment {
        switch self {
        case .top:
            return .top
        case .center:
            return .center
        case .bottom:
            return .bottom
        }
    }
}
