/*
 * Copyright (C) 2012, All rights reserved, Mikael Olenfalk <mikael@olenfalk.se>
 */

#ifndef __UTILS_FUNCTIONAL__MEMBER_FUNCTION_REGISTRY_HPP__
#define __UTILS_FUNCTIONAL__MEMBER_FUNCTION_REGISTRY_HPP__

#include <string>
#include <map>

#include <boost/any.hpp>
#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/utility/enable_if.hpp>

#include <boost/call_traits.hpp>
#include <boost/type_traits/remove_cv.hpp>
#include <boost/type_traits/remove_reference.hpp>
#include <boost/type_traits/is_member_function_pointer.hpp>
#include <boost/type_traits/is_same.hpp>

#include <boost/function_types/result_type.hpp>
#include <boost/function_types/parameter_types.hpp>

#include <boost/fusion/view/joint_view/joint_view.hpp>
#include <boost/fusion/algorithm/transformation/push_front.hpp>
#include <boost/fusion/algorithm/transformation/push_back.hpp>
#include <boost/fusion/container/list/cons.hpp>
#include <boost/fusion/functional/invocation/invoke.hpp>

namespace util {

namespace functional {

class any_return
{
public:
	typedef boost::any return_type;

	template <typename T>
	void handle (const T& v)
	{
		_value = v;
	}

	boost::any result () const
	{
		return _value;
	}

private:
	boost::any _value;
};

/*
 * member_function_registry is a class which automatically generates trampolines.
 *
 * It is templated on the types:
 *
 *    * Class, the class type of the member function pointers
 *    * ArgumentParser, a type which is responsible to convert from an
 *    		ArgumentPack to a native type
 *    * ArgumentPack, a type which is used to send arguments to bound functions
 *    *	ReturnParser, a type which handles the return value
 *    * Context, a context type which is sent to all functions
 *    * FunctionKey, a type which is used to identify a bound function
 *
 * For example if you have the following class:
 *
 *      class Class {
 *      public:
 *        void f(std::ostream& os, int a, int b) {
 *           os << a << " + " << b << " = " << (a + b) << "\n";
 *        }
 *      };
 *
 * And want to invoke it from a textual representation, you could do this:
 *
 *   	typedef std::vector<std::string> arg_pack;
 *
 * 		class arg_parser
 * 		{
 * 			public:
 * 				arg_parser (const arg_pack& args)
 * 					: _args(args), _iter(_args.begin())
 * 				{}
 *
 * 				template <typename Result>
 * 				Result pop ()
 * 				{
 * 					return boost::lexical_cast<Result> (*_iter++);
 * 				}
 *
 * 			private:
 *				const arg_pack& _args;
 *				arg_pack::const_iterator _iter;
 * 		}
 *
 * 		int main ()
 * 		{
 * 			member_function_registry<Class,
 *                     arg_parser, arg_pack, any_return,
 *                     std::ostream, std::string> reg;
 *
 * 			reg.register_function ("f1", &Class::f);
 *
 * 			arg_pack args;
 * 			args.push_back ("1");
 * 			args.push_back ("2");
 *
 *          Class instance;
 * 			reg.call (&instance, std::cout, "f1", args); // <CALL>
 *
 * 			return 0;
 * 		}
 *
 *  The above code will in the line marked with <CALL> invoke the function
 *  instance.f(int, int) with the parameters 1 and 2 respectively.
 */
template <
	class Class,
	class ArgumentParser,
	class ArgumentPack,
	class ReturnParser,
	class Context,
	class FunctionKey = std::string>
class member_function_registry
{
public:
	typedef typename ReturnParser::return_type return_type;

	member_function_registry ()
	{
		// do nothing
	}

	template <typename Function>
	typename boost::enable_if< boost::is_member_function_pointer<Function> >::type
	register_function (typename boost::call_traits<FunctionKey>::param_type name, Function f)
	{
		_invokers[name] =
				boost::bind (
						& invoker<
								Function,
								/* From */
								typename boost::mpl::next<
											typename boost::mpl::next<
													typename boost::mpl::begin< boost::function_types::parameter_types<Function> >::type
												>::type
											>::type,
								/* To */
								typename boost::mpl::end< boost::function_types::parameter_types<Function> >::type
							> ::
							template apply<boost::fusion::nil>,
						f, _1, _2, _3, _4,
						boost::fusion::nil());
	}

	return_type call (Class* instance, Context& ctx, typename boost::call_traits<FunctionKey>::param_type name, const ArgumentPack& pack = ArgumentPack())
	{
		typename function_map::iterator invoker = _invokers.find(name);
		if (invoker == _invokers.end())
			throw std::out_of_range("function with name '" + boost::lexical_cast<std::string>(name) + "' not found");

		ArgumentParser arg_parser(pack);
		ReturnParser ret_parser;
		(invoker->second) (instance, ctx, arg_parser, ret_parser);

		return ret_parser.result ();
	}

	void call (Class* instance, Context& ctx, typename boost::call_traits<FunctionKey>::param_type name, ReturnParser& ret_parser, const ArgumentPack& pack = ArgumentPack())
	{
		typename function_map::iterator invoker = _invokers.find(name);
		if (invoker == _invokers.end())
			throw std::out_of_range("function with name '" + boost::lexical_cast<std::string>(name) + "' not found");

		ArgumentParser arg_parser(pack);
		(invoker->second) (instance, ctx, arg_parser, ret_parser);
	}

private:
	template <
			typename Function,
			class From,
			class To
		>
	struct invoker
	{
		template <typename Args>
		static inline
		void apply (Function f, Class* instance, Context& ctx, ArgumentParser& arg_parser, ReturnParser& ret_parser, const Args& args)
		{
			invoker::apply (f, ctx, arg_parser, ret_parser, boost::fusion::push_back( args, instance ));
		}

		template <typename Args>
		static inline
		void apply (Function f, Context& ctx, ArgumentParser& arg_parser, ReturnParser& ret_parser, const Args& args)
		{
			invoker::apply (f, arg_parser, ret_parser, boost::fusion::push_back( args, boost::ref(ctx) ));
		}

		template <typename Args>
		static inline
		void apply (Function f, ArgumentParser& arg_parser, ReturnParser& ret_parser, const Args& args)
		{
			typedef typename boost::mpl::deref<From>::type arg_type;

			invoker<Function, typename boost::mpl::next<From>::type, To>
				::apply (f, arg_parser, ret_parser, boost::fusion::push_back( args, arg_parser.template pop<arg_type>() ));
		}
	};

	template <typename Function, class To>
	struct invoker<Function, To, To>
	{
		template <typename Args>
		static inline
		void apply (Function f, Class* instance, Context& ctx, ArgumentParser& arg_parser, ReturnParser& ret_parser, const Args& args)
		{
			invoker::apply (f, ctx, arg_parser, ret_parser, boost::fusion::push_back( args, instance ));
		}

		template <typename Args>
		static inline
		void apply (Function f, Context& ctx, ArgumentParser& arg_parser, ReturnParser& ret_parser, const Args& args)
		{
			invoker::apply (f, arg_parser, ret_parser, boost::fusion::push_back( args, boost::ref(ctx) ) );
		}

		template <typename Args>
		static inline
		void apply (Function f, ArgumentParser& arg_parser, ReturnParser& ret_parser, const Args& args)
		{
			real_invoker<
					Function,
					typename boost::function_types::result_type<Function>::type
				>::do_invoke
					(f, ret_parser, args);
		}
	};

	template <typename Function, typename FunctionReturnType>
	struct real_invoker
	{
		template <typename Args>
		static inline
		void do_invoke (Function f, ReturnParser& ret_parser, const Args& args)
		{
			ret_parser.handle (boost::fusion::invoke (f, args));
		}
	};

	template <typename Function>
	struct real_invoker<Function, void>
	{
		template <typename Args>
		static inline
		void do_invoke (Function f, ReturnParser& ret_parser, const Args& args)
		{
			boost::fusion::invoke (f, args);
		}
	};

	typedef typename boost::function<void (Class*, Context&, ArgumentParser&, ReturnParser&)> invoker_function_type;
	typedef typename std::map<FunctionKey, invoker_function_type> function_map;

	function_map _invokers;
};

}  // namespace functional

}  // namespace utils

#endif // __UTILS_FUNCTIONAL__MEMBER_FUNCTION_REGISTRY_HPP__
