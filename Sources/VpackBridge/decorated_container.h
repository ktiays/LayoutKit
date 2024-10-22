//
// Created by ktiays on 2022/8/24.
// Copyright (c) 2022 ktiays. All rights reserved.
//

#ifndef VPACKCORE_DECORATED_CONTAINER_H
#define VPACKCORE_DECORATED_CONTAINER_H

#include "stack_container.h"

namespace vpk::core {

enum class DecoratedStyle {
    background,
    overlay,
};

#define DEAL_CONTENT_ELEMENT_WITH(property, direction) \
__DEAL_MIN_WIDTH_ELSE(property) content->property() + padding.direction()

#define DEAL_DECORATED_SIZE_PROPERTY \
const SizeProperty<ValueType> size_property = params.size_property; \
const auto content = content_element();                             \
const EdgeInsets<ValueType> padding = content_element()->padding(); \
DEAL_CONTENT_ELEMENT_WITH(min_width, horizontal);                   \
DEAL_CONTENT_ELEMENT_WITH(max_width, horizontal);                   \
DEAL_CONTENT_ELEMENT_WITH(min_height, vertical);                    \
DEAL_CONTENT_ELEMENT_WITH(max_height, vertical)

/// A container for handling decorated views.
///
/// The container contains only 2 elements, one for the content element and the other for the decorated view.
/// The size of the decorated view is determined by the size of the content view.
template<typename Identifier, typename ValueType>
class DecoratedContainer : public StackContainer<Identifier, ValueType> {
public:
    DecoratedContainer(const std::vector<LayoutablePointer<Identifier, ValueType>>& children,
                       const LayoutParams<ValueType>& params,
                       const DecoratedStyle& decorated_style)
        : StackContainer<Identifier, ValueType>(children, params, Alignment::center),
          decorated_style_(decorated_style) {
        assert(children.size() == 2);

        DEAL_DECORATED_SIZE_PROPERTY;
    }

    Size<ValueType> measure(const Size<ValueType>& size) override;

private:
    /// An enumeration value specifying which of the two elements is the decorated view.
    DecoratedStyle decorated_style_;

    using Element = LayoutablePointer<Identifier, ValueType>;

    // The index of the content element.
    // i.e. the index of the element that affects the container size.
    auto content_index() const {
        return decorated_style_ == DecoratedStyle::background ? 1 : 0;
    }

    const Element& content_element() const {
        return this->children.at(content_index());
    }

    const Element& decorated_element() const {
        return this->children.at(content_index() ^ 1);
    }
};

#undef DEAL_DECORATED_SIZE_PROPERTY
#undef DEAL_CONTENT_ELEMENT_WITH

template<typename Identifier, typename ValueType>
Size<ValueType> DecoratedContainer<Identifier, ValueType>::measure(const Size<ValueType>& size) {
    using usize = typename decltype(this->children)::size_type;
    const usize content_index = this->content_index();

    const auto& content = content_element();
    const EdgeInsets<ValueType> content_padding = content->padding();
    // The size of the container used to calculate the content view.
    const Size<ValueType> content_container_size = {
        std::max(content->min_width(), std::min(size.width - content_padding.horizontal(), content->max_width())),
        std::max(content->min_height(), std::min(size.height - content_padding.vertical(), content->max_height())),
    };
    Size<ValueType> content_size = content->measure(content_container_size);
    content_size = {
        std::min(std::max(content->min_width(), content_size.width), content_container_size.width),
        std::min(std::max(content->min_height(), content_size.height), content_container_size.height)
    };
    // The size of the wrapped content view, i.e. the size of the element after adding padding.
    //
    // When calculating the size of a container, it is based on the total size occupied by the element,
    // not the actual size rendered by the element.
    const Size<ValueType> wrapped_content_size = {
        content_size.width + content_padding.horizontal(),
        content_size.height + content_padding.vertical()
    };
    this->size_list.at(content_index) = content_size;
    this->cached_measured_size = wrapped_content_size;

    const auto& decorated = this->decorated_element();
    const EdgeInsets<ValueType> decorated_padding = decorated->padding();
    // Use the size of the content element as the container size of the decorated view.
    Size<ValueType> decorated_size = decorated->measure({
                                                            wrapped_content_size.width - decorated_padding.horizontal(),
                                                            wrapped_content_size.height - decorated_padding.vertical()
                                                        });
    this->size_list.at(content_index ^ 1) = {
        std::min(std::max(decorated->min_width(), decorated_size.width), decorated->max_width()),
        std::min(std::max(decorated->min_height(), decorated_size.height), decorated->max_height()),
    };

    return wrapped_content_size;
}

}

#endif //VPACKCORE_DECORATED_CONTAINER_H
