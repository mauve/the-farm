/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_TYPE_TRAITS_REMOVE_CV_REF_HPP_
#define UTIL_TYPE_TRAITS_REMOVE_CV_REF_HPP_

#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>

namespace util {

namespace type_traits {

template <typename T>
struct remove_cv_ref
{
	typedef typename boost::remove_reference <
				typename boost::remove_cv< T >::type
			>::type type;
};

}  // namespace type_traits

}  // namespace util

#endif /* UTIL_TYPE_TRAITS_REMOVE_CV_REF_HPP_ */
