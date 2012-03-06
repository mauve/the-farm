/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef UTIL_FUNCTIONAL_CALLBACK_MAP_HPP_
#define UTIL_FUNCTIONAL_CALLBACK_MAP_HPP_

#include <map>

#include <boost/any.hpp>
#include <boost/noncopyable.hpp>
#include <boost/function.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

namespace util {

namespace functional {

namespace _detail {
class connection_impl_interface;

template <typename Key, typename Signature>
class connection_impl;

template <typename Key, typename Signature>
class callback_map_data;
}

class no_such_callback :
	public std::runtime_error
{
public:
	no_such_callback ()
		: std::runtime_error("no such callback")
	{}
};

class callback_connection;

template <typename Key,
		  typename Signature>
class callback_map :
	public boost::noncopyable
{
public:
	typedef typename _detail::callback_map_data<Key, Signature> data_t;

	typedef Key key_type;
	typedef Signature signature_type;
	typedef typename data_t::callback_type callback_type;

	class connection;

	callback_map ();
	~callback_map ();

	callback_connection connect (const key_type& key, const callback_type& cb);

	bool has_callback (const key_type& key) const;
	callback_type& get_callback(const key_type& key);

private:
	boost::shared_ptr<data_t> _data;
};

class callback_connection
{
public:
	void disconnect ();

	callback_connection ()
	{}

	callback_connection (boost::shared_ptr<_detail::connection_impl_interface>& handle)
		: _handle(handle)
	{}

private:
	boost::shared_ptr<_detail::connection_impl_interface> _handle;
};

namespace _detail {

class connection_impl_interface
{
public:
	virtual ~connection_impl_interface () = 0;
	virtual void disconnect () = 0;
};

template <typename Key, typename Signature>
class connection_impl :
	public connection_impl_interface
{
public:
	typedef typename boost::shared_ptr<callback_map_data<Key, Signature> > data_pointer;

	connection_impl (const Key& key, data_pointer& data);
	~connection_impl ();

	void disconnect ();

private:
	boost::any _key;
	boost::weak_ptr<callback_map_data<Key, Signature> > _data;
};

template <typename Key, typename Signature>
class callback_map_data
{
public:
	typedef typename boost::function<Signature> callback_type;
	typedef typename std::map<Key, std::pair<_detail::connection_impl_interface*, callback_type> > map_t;
	boost::mutex _mutex;
	map_t _callbacks;

	void disconnect (const boost::any& key, _detail::connection_impl_interface* cimpl);
};

}  // namespace _detail

/*
 * impls
 */

/*
 * callback_map
 */

template <typename Key, typename Signature>
callback_map<Key, Signature>::callback_map ()
	: _data(new data_t)
{}

template <typename Key, typename Signature>
callback_map<Key, Signature>::~callback_map ()
{}

template <typename Key, typename Signature>
callback_connection callback_map<Key, Signature>::connect (
		const key_type& key,
		const callback_type& cb)
{
	boost::unique_lock<boost::mutex> lock(_data->_mutex);
	boost::shared_ptr<_detail::connection_impl_interface> cimpl
		(new _detail::connection_impl<Key, Signature>(key, _data));

	_data->_callbacks.insert(std::make_pair(key, std::make_pair(cimpl.get(), cb)));
	return cimpl;
}

template <typename Key, typename Signature>
bool callback_map<Key, Signature>::has_callback (const key_type& key) const
{
	boost::unique_lock<boost::mutex> lock(_data->_mutex);
	typename data_t::map_t::const_iterator iter = _data->_callbacks.find(key);
	if (iter == _data->_callbacks.end())
		return false;
	return true;
}

template <typename Key, typename Signature>
typename callback_map<Key, Signature>::callback_type& callback_map<Key, Signature>::get_callback(const key_type& key)
{
	boost::unique_lock<boost::mutex> lock(_data->_mutex);
	typename data_t::map_t::iterator iter = _data->_callbacks.find(key);
	if (iter == _data->_callbacks.end())
		throw no_such_callback();

	return iter->second.second;
}

/*
 * callback_connection
 */
inline void callback_connection::disconnect()
{
	if (_handle) {
		_handle->disconnect();
	}
}


/*
 * _detail::connection_impl
 */

namespace _detail {

inline connection_impl_interface::~connection_impl_interface()
{}

template <typename Key, typename Signature>
connection_impl<Key, Signature>::connection_impl (const Key& key, data_pointer& data)
	: _key(key), _data(data)
{}

template <typename Key, typename Signature>
connection_impl<Key, Signature>::~connection_impl ()
{}

template <typename Key, typename Signature>
void connection_impl<Key, Signature>::disconnect ()
{
	data_pointer data_ptr = _data.lock();
	if (data_ptr) {
		data_ptr->disconnect(_key, this);
	}
}

/*
 * _detail::callback_map_data
 */
template <typename Key, typename Signature>
void callback_map_data<Key, Signature>::disconnect (const boost::any& key, _detail::connection_impl_interface* cimpl)
{
	const Key* extracted_key = boost::any_cast<Key>(&key);
	if (!extracted_key) {
		return;
	}

	boost::unique_lock<boost::mutex> lock(_mutex);
	typename map_t::iterator iter = _callbacks.find(*extracted_key);
	if (iter == _callbacks.end()) {
		return;
	}

	if (iter->second.first != cimpl) {
		return;
	}

	_callbacks.erase(iter);
}

}  // namespace _detail

}  // namespace functional

}  // namespace util

#endif /* UTIL_FUNCTIONAL_CALLBACK_MAP_HPP_ */
