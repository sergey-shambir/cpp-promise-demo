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
}
}

#define ISPROM_WHICH(VariantType, VariantCase) (::isprom::detail::WhichIndex<VariantType, VariantCase>::value)
