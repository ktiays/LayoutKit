//
//  Created by ktiays on 2024/10/21.
//  Copyright (c) 2024 ktiays. All rights reserved.
//

import Foundation

/// An enumeration to indicate one edge of a rectangle.
public enum Edge: Int8, CaseIterable {

    case top = 0x1

    case leading = 0x2

    case bottom = 0x4

    case trailing = 0x8

    /// An efficient set of edges.
    public struct Set: OptionSet {

        /// The element type of the option set.
        ///
        /// To inherit all the default implementations from the `OptionSet` protocol,
        /// the `Element` type must be `Self`, the default.
        public typealias Element = Edge.Set

        /// The corresponding value of the raw type.
        public let rawValue: Int8

        /// Creates a new option set from the given raw value.
        public init(rawValue: Int8) {
            self.rawValue = rawValue
        }

        public static let top: Edge.Set = .init(.top)

        public static let leading: Edge.Set = .init(.leading)

        public static let bottom: Edge.Set = .init(.bottom)

        public static let trailing: Edge.Set = .init(.trailing)

        public static let all: Edge.Set = .init(rawValue: Edge.top.rawValue | Edge.leading.rawValue | Edge.bottom.rawValue | Edge.trailing.rawValue)

        public static let horizontal: Edge.Set = .init(rawValue: Edge.leading.rawValue | Edge.trailing.rawValue)

        public static let vertical: Edge.Set = .init(rawValue: Edge.top.rawValue | Edge.bottom.rawValue)

        /// Creates set of edges containing only the specified edge.
        public init(_ e: Edge) {
            self.rawValue = e.rawValue
        }

        /// The raw type that can be used to represent all values of the conforming
        /// type.
        public typealias RawValue = Int8
    }

    /// The raw type that can be used to represent all values of the conforming
    /// type.
    ///
    /// Every distinct value of the conforming type has a corresponding unique
    /// value of the `RawValue` type, but there may be values of the `RawValue`
    /// type that don't have a corresponding value of the conforming type.
    public typealias RawValue = Int8
}
