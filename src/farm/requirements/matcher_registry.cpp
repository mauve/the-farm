
#include <farm/requirements/matcher.hpp>
#include <farm/requirements/matcher_registry.hpp>
#include <farm/requirements/requirement.hpp>

namespace farm {

namespace requirements {

MatcherRegistry::MatcherRegistry ()
{}

MatcherRegistry::~MatcherRegistry ()
{
	for (std::vector<Matcher*>::const_iterator iter = _matchers.begin();
			iter != _matchers.end(); ++iter)
	{
		delete *iter;
	}
}

void MatcherRegistry::register_matcher (Matcher* m)
{
	if (!m)
		return;
	_matchers.push_back(m);
}

bool MatcherRegistry::has_matcher (const std::string& op) const
{
	return find_matcher(op) != _matchers.end();
}

bool MatcherRegistry::matches (const std::string& left,
								const std::string& op,
								const std::string& right) const
{
	std::vector<Matcher*>::const_iterator iter = find_matcher(op);
	if (iter == _matchers.end())
		return false;

	return (*iter)->matches(left, op, right);
}

std::vector<Requirement> MatcherRegistry::matches (const std::vector<Requirement>& reqs,
													const std::map<std::string, std::string>& set) const
{
	std::vector<Requirement> unfulfilled;
	std::string empty;

	for (std::vector<Requirement>::const_iterator iter = reqs.begin();
			iter != reqs.end(); ++iter)
	{
		const std::string* value;
		std::map<std::string, std::string>::const_iterator value_iter = set.find(iter->get_name());
		if (value_iter == set.end()) {
			value = &empty;
		} else {
			value = &(value_iter->second);
		}

		if (!matches (*value, iter->get_op(), iter->get_match()))
		{
			unfulfilled.push_back (*iter);
		}
	}

	return unfulfilled;
}

std::vector<Matcher*>::const_iterator MatcherRegistry::find_matcher(const std::string& op) const
{
	for (std::vector<Matcher*>::const_iterator iter = _matchers.begin();
			iter != _matchers.end(); ++iter)
	{
		if ((*iter)->supports(op))
			return iter;
	}
	return _matchers.end();
}

}  // namespace requirements

}  // namespace farm
