//
//  Created by ktiays on 2024/10/21.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import VpackBridge

#if canImport(UIKit)
import UIKit

public typealias PlatformEdgeInsets = UIEdgeInsets
#elseif canImport(AppKit)
import AppKit

public typealias PlatformEdgeInsets = NSEdgeInsets
#endif
