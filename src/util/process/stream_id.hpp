
#ifndef UTIL_PROCESS_STREAM_ID_HPP_
#define UTIL_PROCESS_STREAM_ID_HPP_

namespace util {

namespace process {

class stream_id
{
public:
	typedef int native_stream_handle;

	explicit stream_id (native_stream_handle);
	stream_id (const stream_id&);
	~stream_id ();

	stream_id& operator= (const stream_id&);

	native_stream_handle native () const;

	const static stream_id standard_out;
	const static stream_id standard_err;
	const static stream_id standard_in;

private:
	native_stream_handle _handle;
};

bool operator== (const stream_id&, const stream_id&);
bool operator!= (const stream_id&, const stream_id&);
bool operator<  (const stream_id&, const stream_id&);

}  // namespace process

}  // namespace util


#endif /* UTIL_PROCESS_STREAM_ID_HPP_ */
