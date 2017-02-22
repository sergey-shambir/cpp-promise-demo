#pragma once
#include <boost/variant.hpp>
#include <boost/mpl/find.hpp>
#include <boost/mpl/vector.hpp>
#include <boost/mpl/copy.hpp>


namespace isprom
{
namespace detail
{
template <class VariantType, class VariantCase>
using WhichIndex = typename boost::mpl::find<
        typename boost::mpl::copy<
            typename VariantType::types,
            boost::mpl::back_inserter<boost::mpl::vector<>>
        >::type,
        VariantCase
    >::type::pos;

template <class VariantType, class VariantCase>
constexpr int VariantIndex = WhichIndex<VariantType, VariantCase>::value;
}
}
