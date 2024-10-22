//
//  Created by ktiays on 2024/10/20.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation
import VpackBridge

/// An alignment position along the horizontal axis.
public enum HorizontalAlignment: Equatable {

    /// A guide that marks the leading edge of the view.
    case leading

    /// A guide that marks the horizontal center of the view.
    case center

    /// A guide that marks the trailing edge of the view.
    case trailing
}

extension HorizontalAlignment {

    var engineRepresention: EngineHorizontalAlignment {
        switch self {
        case .leading:
            return .leading
        case .center:
            return .center
        case .trailing:
            return .trailing
        }
    }
}
