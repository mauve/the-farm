/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_THREAD_UNLOCK_GUARD_HPP_
#define UTIL_THREAD_UNLOCK_GUARD_HPP_

namespace util {

namespace thread {

template <class Lockable>
class unlock_guard
{
public:
	unlock_guard (Lockable& lockable)
		: _lockable(lockable)
	{
		_lockable.unlock();
	}

	~unlock_guard ()
	{
		_lockable.lock();
	}

private:
	Lockable& _lockable;
};

}  // namespace thread

}  // namespace util


#endif /* UTIL_THREAD_UNLOCK_GUARD_HPP_ */
