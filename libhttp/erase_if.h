#pragma once

namespace http
{
template<typename ContainerT, typename PredicateT>
void erase_if(ContainerT& items, PredicateT&& predicate)
{
	for (auto it = std::begin(items); it != std::end(items);)
	{
		if (predicate(*it))
		{
			it = items.erase(it);
		}
		else
		{
			++it;
		}
	}
};
}
