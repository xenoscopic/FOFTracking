#pragma once

#include <algorithm>
#include <stdexcept>
#include <vector>
#include <utility>

namespace Aetherspark
{
	namespace Utility
	{
		template <class Compare, class RandomAccessIterator>
		struct dual_sort_compare_wrapper
		{
			dual_sort_compare_wrapper(Compare comparer) :
			_comparer(comparer)
			{

			}

			bool operator() (std::pair<size_t, RandomAccessIterator> x, 
							std::pair<size_t, RandomAccessIterator> y)
			{
				return _comparer(*x.second, *y.second);
			}

			Compare _comparer;
		};

		template <class T>
		struct dual_sort_basic_compare
		{
			bool operator() (T x, T y)
			{
				return x < y;
			}
		};

		template <class IndexIterator, class RandomAccessIterator, class OutputRandomAccessIterator>
		void index_shuffle_copy(IndexIterator begin, 
								IndexIterator end, 
								RandomAccessIterator toSort,
								OutputRandomAccessIterator output)
		{
			while(begin != end)
			{
				*output = *(toSort + begin->first);
				begin++;
				output++;
			}
		}

		//Unfortunately we have to specify content types because we have to make a copy buffer.  Moving
		//data in-place would be O(n log n) at minimum, if not more.
		template <class RandomAccessIteratorType1, class RandomAccessIteratorType2, class Compare>
		void dual_sort(RandomAccessIteratorType1 it1_begin, 
						RandomAccessIteratorType1 it1_end, 
						RandomAccessIteratorType2 it2_begin, 
						RandomAccessIteratorType2 it2_end,
						Compare comp)
		{
			//Count how many elements we have
			unsigned n1 = it1_end - it1_begin;
			unsigned n2 = it2_end - it2_begin;

			//Do checking
			if(n1 == 0)
			{
				return;
			}
			if(n1 < 0)
			{
				//We got some messed up values
				throw std::invalid_argument("First set of iterator arguments are invalid.");
			}
			if(n1 != n2)
			{
				//The arrays were different sizes
				throw std::invalid_argument("Containers have different sizes.");
			}

			//Create a tracker
			std::vector<std::pair<size_t, RandomAccessIteratorType1> > order(n1);
			size_t n = 0;
			for(RandomAccessIteratorType1 it = it1_begin; it != it1_end; it++, n++)
			{
				order[n] = std::make_pair(n, it);
			}

			//Do the sorting
			std::sort(order.begin(), 
						order.end(), 
						dual_sort_compare_wrapper<Compare, 
													RandomAccessIteratorType1 >(comp));

			//Order vectors appropriately
			std::vector<typename std::iterator_traits<RandomAccessIteratorType1>::value_type > copyBuffer1(n1);
			std::vector<typename std::iterator_traits<RandomAccessIteratorType2>::value_type > copyBuffer2(n2);
			
			index_shuffle_copy<typename std::vector<std::pair<size_t, RandomAccessIteratorType1> >::iterator, 
								RandomAccessIteratorType1, 
								typename std::vector<typename std::iterator_traits<RandomAccessIteratorType1>::value_type >::iterator>
								(order.begin(), order.end(), it1_begin, copyBuffer1.begin());
								
			index_shuffle_copy<typename std::vector<std::pair<size_t, RandomAccessIteratorType1> >::iterator, 
								RandomAccessIteratorType2, 
								typename std::vector<typename std::iterator_traits<RandomAccessIteratorType2>::value_type >::iterator>
								(order.begin(), order.end(), it2_begin, copyBuffer2.begin());
								
			std::copy(copyBuffer1.begin(), copyBuffer1.end(), it1_begin);
			std::copy(copyBuffer2.begin(), copyBuffer2.end(), it2_begin);
		}

		template <class RandomAccessIteratorType1, class RandomAccessIteratorType2, class Type1, class Type2>
		void dual_sort(RandomAccessIteratorType1 it1_begin, 
						RandomAccessIteratorType1 it1_end, 
						RandomAccessIteratorType2 it2_begin, 
						RandomAccessIteratorType2 it2_end)
		{
			dual_sort<RandomAccessIteratorType1, RandomAccessIteratorType2, Type1, Type2, dual_sort_basic_compare<RandomAccessIteratorType1> >
				(it1_begin, it1_end, it2_begin, it2_end, dual_sort_basic_compare<RandomAccessIteratorType1>());
		}
	}
}

