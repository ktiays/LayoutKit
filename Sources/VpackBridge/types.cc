//
// Created by ktiays on 2022/8/13.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#include "types.h"

namespace vpk::core {

Alignment Alignment::top_leading = { VerticalAlignment::top, HorizontalAlignment::leading };
Alignment Alignment::top_center = { VerticalAlignment::top, HorizontalAlignment::center };
Alignment Alignment::top_trailing = { VerticalAlignment::top, HorizontalAlignment::trailing };

Alignment Alignment::leading = { VerticalAlignment::center, HorizontalAlignment::leading };
Alignment Alignment::center = { VerticalAlignment::center, HorizontalAlignment::center };
Alignment Alignment::trailing = { VerticalAlignment::center, HorizontalAlignment::trailing };

Alignment Alignment::bottom_leading = { VerticalAlignment::bottom, HorizontalAlignment::leading };
Alignment Alignment::bottom_center = { VerticalAlignment::bottom, HorizontalAlignment::center };
Alignment Alignment::bottom_trailing = { VerticalAlignment::bottom, HorizontalAlignment::trailing };

}
