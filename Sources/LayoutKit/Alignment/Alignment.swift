//
//  Created by ktiays on 2024/10/22.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation

/// An alignment in both axes.
public struct Alignment: Equatable {

    /// The alignment on the horizontal axis.
    public var horizontal: HorizontalAlignment

    /// The alignment on the vertical axis.
    public var vertical: VerticalAlignment

    init(
        horizontal: HorizontalAlignment,
        vertical: VerticalAlignment
    ) {
        self.horizontal = horizontal
        self.vertical = vertical
    }
}

extension Alignment {

    /// A guide that marks the top and leading edges of the view.
    public static let topLeading: Alignment = .init(horizontal: .leading, vertical: .top)

    /// A guide that marks the top edge of the view.
    public static let top: Alignment = .init(horizontal: .center, vertical: .top)

    /// A guide that marks the top and trailing edges of the view.
    public static let topTrailing: Alignment = .init(horizontal: .trailing, vertical: .top)

    /// A guide that marks the leading edge of the view.
    public static let leading: Alignment = .init(horizontal: .leading, vertical: .center)

    /// A guide that marks the center of the view.
    public static let center: Alignment = .init(horizontal: .center, vertical: .center)

    /// A guide that marks the trailing edge of the view.
    public static let trailing: Alignment = .init(horizontal: .trailing, vertical: .center)

    /// A guide that marks the leading and bottom edges of the view.
    public static let bottomLeading: Alignment = .init(horizontal: .leading, vertical: .bottom)

    /// A guide that marks the bottom edge of the view.
    public static let bottom: Alignment = .init(horizontal: .center, vertical: .bottom)

    /// A guide that marks the bottom and trailing edges of the view.
    public static let bottomTrailing: Alignment = .init(horizontal: .trailing, vertical: .bottom)

}
