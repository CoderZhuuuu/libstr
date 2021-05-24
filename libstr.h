#pragma once
#pragma warning(disable:4996)

#include <set> /* For std::set */
#include <regex> /* For std::regex, std::regex_search, std::regex_match */
#include <tuple> /* For std::tuple, std::get */
#include <vector> /* For std::vector */
#include <string> /* For std::string */
#include <random> /* For random objects */
#include <numeric> /* For std::numeric_limits */
#include <functional> /* For std::function */

/* Helper functions */
namespace libstr {

	/* Used to get tuple in runtime */
	template <typename Arg>
	inline static Arg GetTupleArgByIndex(std::tuple<Arg>& tp, size_t index) {
		return std::get<0>(tp);
	}

	template <typename Arg, typename... Args>
	inline static Arg GetTupleArgByIndex(std::tuple<Arg, Args...>& tp, size_t index) {
		if (index > 0) {
			return GetTupleArgByIndex((std::tuple<Args...>&) tp, index - 1);
		}
		return std::get<0>(tp);
	}

}

namespace libstr {

	/* Is numeric, if empty, return false */
	inline bool isNumeric(const std::string& s) noexcept {
		return !s.empty() && std::any_of(
			('-' == s[0] || '+' == s[0]) ? std::next(std::begin(s)) : std::begin(s),
			std::end(s),
			::isdigit
		);
	}

	/* Split */
	inline std::vector<std::string> split(const std::string& s, const std::string& target, bool keepEmpty = false) noexcept {
		std::vector<std::string> strVec;
		if (!keepEmpty) {
			std::string::size_type lastPos = s.find_first_not_of(target);
			std::string::size_type pos = s.find_first_of(target, lastPos);
			while (std::string::npos != lastPos || std::string::npos != pos) {
				strVec.push_back(s.substr(lastPos, pos - lastPos));
				lastPos = s.find_first_not_of(target, pos);
				pos = s.find_first_of(target, lastPos);
			}
		}
		else {
			std::string::size_type lastPos = 0;
			std::string::size_type pos = s.find_first_of(target, lastPos);
			for (;;) {
				strVec.push_back(s.substr(lastPos, pos - lastPos));
				lastPos = pos + 1;
				if (0 == lastPos) {
					break;
				}
				pos = s.find_first_of(target, lastPos);
			}
		}
		return strVec;
	}

	/* regx is regex string */
	inline std::vector<std::string> regexSplit(const std::string& s, const std::string& pattern, bool keepEmpty = false) noexcept {
		std::vector<std::string> strVec;
		std::regex engine(pattern);
		/* Get result */
		std::sregex_token_iterator begin{ std::begin(s), std::end(s), engine, -1 };
		std::sregex_token_iterator end;
		if (!keepEmpty) {
			/* Remove empty */
			std::for_each(begin, end, [&](const std::string& str) {
				if (!str.empty()) {
					strVec.push_back(str);
				}
			});
		}
		else {
			strVec = { begin, {} };
			for (std::sregex_token_iterator tmp = std::next(begin); end != tmp; tmp = std::next(begin = tmp));
			std::smatch sm;
			if (std::regex_search(begin->second, s.cend(), sm, engine)) {
				strVec.push_back({});
			}
		}
		return strVec;
	}

	/* Trim */
	inline void ltrimIf(std::string& s, const std::function<bool(int)>& fn) noexcept {
		s.erase(std::begin(s), std::find_if(std::begin(s), std::end(s), std::not1(fn)));
	}

	inline void rtrimIf(std::string& s, const std::function<bool(int)>& fn) noexcept {
		s.erase(std::find_if(std::rbegin(s), std::rend(s), std::not1(fn)).base(), std::end(s));
	}

	inline void trimIf(std::string& s, const std::function<bool(int)>& lfn, const std::function<bool(int)>& rfn) noexcept {
		ltrimIf(s, lfn); rtrimIf(s, rfn);
	}

	inline void trimIf(std::string& s, const std::function<bool(int)>& fn) noexcept {
		trimIf(s, fn, fn);
	}

	inline void ltrim(std::string& s) noexcept {
		ltrimIf(s, ::isspace);
	}

	inline void rtrim(std::string& s) noexcept {
		rtrimIf(s, ::isspace);
	}

	inline void trim(std::string& s) noexcept {
		ltrim(s); rtrim(s);
	}

	inline std::string ltrimCopyIf(const std::string& s, const std::function<bool(int)>& fn) noexcept {
		return std::string(std::find_if(std::begin(s), std::end(s), std::not1(fn)), std::end(s));
	}

	inline std::string rtrimCopyIf(const std::string& s, const std::function<bool(int)>& fn) noexcept {
		return std::string(std::begin(s), std::find_if(std::rbegin(s), std::rend(s), std::not1(fn)).base());
	}

	inline std::string trimCopyIf(const std::string& s, const std::function<bool(int)>& lfn, const std::function<bool(int)>& rfn) noexcept {
		return ltrimCopyIf(rtrimCopyIf(s, rfn), lfn);
	}

	inline std::string trimCopyIf(const std::string& s, const std::function<bool(int)>& fn) noexcept {
		return trimCopyIf(s, fn, fn);
	}

	inline std::string ltrimCopy(const std::string& s) noexcept {
		return ltrimCopyIf(s, ::isspace);
	}

	inline std::string rtrimCopy(const std::string& s) noexcept {
		return rtrimCopyIf(s, ::isspace);
	}

	inline std::string trimCopy(const std::string& s) noexcept {
		return ltrimCopy(rtrimCopy(s));
	}

	/* Start with */
	inline bool startWith(const std::string& s, const std::string& prefix) noexcept {
		return prefix.length() <= s.length() && std::equal(std::begin(prefix), std::end(prefix), std::begin(s));
	}

	/* End with */
	inline bool endWith(const std::string& s, const std::string& suffix) noexcept {
		return suffix.length() <= s.length() && std::equal(std::rbegin(suffix), std::rend(suffix), std::rbegin(s));
	}

	/* To upper case and to lower case */
	inline void toUpper(std::string& s) noexcept {
		std::transform(std::begin(s), std::end(s), std::begin(s), ::toupper);
	}

	inline void toLower(std::string& s) noexcept {
		std::transform(std::begin(s), std::end(s), std::begin(s), ::tolower);
	}

	/* Upper case to lower case and lower case to upper case */
	inline void toUpperLower(std::string& s) noexcept {
		std::transform(std::begin(s), std::end(s), std::begin(s), [](int ch) {
			return (isupper(ch)) ? (tolower(ch)) : ((islower(ch)) ? (toupper(ch)) : (ch));
		});
	}

	inline std::string toUpperCopy(const std::string& s) noexcept {
		std::string upper(s.length(), '\0');
		std::transform(std::cbegin(s), std::cend(s), std::begin(upper), ::toupper);
		return upper;
	}

	inline std::string toLowerCopy(const std::string& s) noexcept {
		std::string lower(s.length(), '\0');
		std::transform(std::cbegin(s), std::cend(s), std::begin(lower), ::tolower);
		return lower;
	}

	/* Upper case to lower case and lower case to upper case */
	inline std::string toUpperLowerCopy(const std::string& s) noexcept {
		std::string result(s.length(), '\0');
		std::transform(std::begin(s), std::end(s), std::begin(result), [](int ch) {
			return (isupper(ch)) ? (tolower(ch)) : ((islower(ch)) ? (toupper(ch)) : (ch));
		});
		return result;
	}

	/* Join */
	inline std::string join(const std::vector<std::string>& strVec, const std::string& connector) noexcept {
		std::string result;
		auto iterBegin = std::begin(strVec), iterEnd = std::end(strVec);
		if (iterBegin != iterEnd) {
			result.append(*iterBegin++);
		}
		while (iterBegin != iterEnd) {
			result.append(connector);
			result.append(*iterBegin++);
		}
		return result;
	}

	/* Is upper or lower */
	inline bool isUpper(const std::string& s) noexcept {
		return std::all_of(std::begin(s), std::end(s), ::isupper);
	}

	inline bool isLower(const std::string& s) noexcept {
		return std::all_of(std::begin(s), std::end(s), ::islower);
	}

	/* Multiply */
	inline std::string mul(const char& ch, int count) noexcept {
		return std::string(std::max(count, 0), ch);
	}

	inline std::string mul(const std::string& s, int count) noexcept {
		std::string result;
		while (count-- > 0) { result += s; }
		return result;
	}

	/* Count */
	inline int count(const std::string& s, const char& target) noexcept {
		int cnt = 0;
		std::for_each(std::begin(s), std::end(s), [&](const char& ch) {
			if (ch == target) {
				++cnt; 
			} 
		});
		return cnt;
	}

	inline int count(const std::string& s, const std::string& target) noexcept {
		int cnt = 0;
		if (!target.empty()) {
			for (std::string::size_type pos = 0, targetLength = target.length();
				std::string::npos != (pos = s.find(target, pos));
				pos += targetLength) {
				++cnt;
			}
		}
		return cnt;
	}

	/* Make transform */
	inline std::string maketrans(const std::string& s, const std::string& from, const std::string& to) noexcept {
		int fromLength = from.length();
		int toLength = to.length();
		/* Length must be equal and no more than 256 */
		if (fromLength != toLength || fromLength > 256 || toLength > 256) {
			return s;
		}
		/* Has same charactors in from or to */
		if (std::set<char>(std::begin(from), std::end(from)).size() != fromLength ||
			std::set<char>(std::begin(to), std::end(to)).size() != toLength) {
			return s;
		}
		char table[256] = { 0 };
		for (int i = 0; i < fromLength; ++i) {
			table[from[i]] = to[i];
		}
		std::string result = s;
		std::for_each(std::begin(result), std::end(result), [&](char& ch) {
			if (table[ch]) ch = table[ch];
		});
		return result;
	}

	/* Get random string*/
	/* Get random number string in [minn, maxn] */
	inline std::string getRandomNumberString(const uint32_t& minn = (std::numeric_limits<uint32_t>::min)(),
		const uint32_t& maxn = (std::numeric_limits<uint32_t>::max)()) noexcept {
		static std::mt19937 mt(std::random_device{}());
		static std::uniform_int_distribution<uint32_t> distr(minn, maxn);
		return std::to_string(distr(mt));
	}

	/* Get random number string in [minn, maxn] */
	inline std::string getRandomNumberString_64(const uint64_t& minn = (std::numeric_limits<uint64_t>::min)(),
		const uint64_t& maxn = (std::numeric_limits<uint64_t>::max)()) noexcept {
		static std::mt19937 mt64(std::random_device{}());
		static std::uniform_int_distribution<uint64_t> distr(minn, maxn);
		return std::to_string(distr(mt64));
	}

	inline std::string getRandomAlphabetString(const size_t& length, const std::function<bool(int)>& validCharFn = ::isalnum) noexcept {
		static std::vector<int> vec = []() { std::vector<int> vec(256); std::iota(std::begin(vec), std::end(vec), 0); return vec; }();
		std::string result(length, '\0');
		/* make sure at least one character can return true to avoid endless loop */
		if (std::any_of(vec.begin(), vec.end(), validCharFn)) {
			static std::default_random_engine e(std::random_device{}());
			static std::uniform_int_distribution<short> distr(0, 256);
			std::for_each(std::begin(result), std::end(result), [&](char& val) { while (!validCharFn((uint8_t)(val = distr(e)))); });
		}
		return result;
	}

	/* Replace */
	inline std::string replace(const std::string& s, const std::string& target, const std::string& replacement) noexcept {
		return join(split(s, target, true), replacement);
	}

	inline std::string replaceFirst(const std::string& s, const std::string& target, const std::string& replacement) noexcept {
		std::string result = s;
		std::string::size_type pos = s.find(target);
		if (std::string::npos != pos) {
			result.replace(pos, target.length(), replacement);
		}
		return result;
	}

	inline std::string replaceLast(const std::string& s, const std::string& target, const std::string& replacement) noexcept {
		std::string result = s;
		std::string::size_type pos = s.rfind(target);
		if (std::string::npos != pos) {
			result.replace(pos, target.length(), replacement);
		}
		return result;
	}

	inline std::string regexReplace(const std::string& s, const std::string& target, const std::string& replacement) noexcept {
		std::string result;
		std::regex engine(target);
		std::regex_replace(std::back_inserter(result), std::begin(s), std::end(s), engine, replacement);
		return result;
	}

	inline std::string regexReplaceFirst(const std::string& s, const std::string& target, const std::string& replacement) noexcept {
		std::string result;
		std::regex engine(target);
		std::regex_replace(std::back_inserter(result), std::begin(s), std::end(s), engine, replacement, std::regex_constants::format_first_only);
		return result;
	}

	inline std::string regexReplaceLast(const std::string& s, const std::string& target, const std::string& replacement) noexcept {
		std::string result = s;
		std::regex engine(target);
		auto begin = std::cbegin(s);
		auto end = std::cend(s);;
		std::smatch sm;
		std::string replaced = "";
		for (auto iter = std::prev(end); iter != begin; --iter) {
			if (std::regex_search(iter, end, sm, engine)) {
				result.replace(iter - begin, sm[0].length(), replacement);
				break;
			}
		}
		return result;
	}

	/* Formatter */
	/* printf type format */
	template<typename ...Args>
	inline std::string format(const char* fmt, Args&& ... args) noexcept {
		size_t size = std::snprintf(nullptr, 0, fmt, args ...) + 1;
		std::string bytes(size, '\0');
		std::snprintf(&bytes[0], size, fmt, args ...);
		return std::string(bytes);
	}

	/* Use {} to format, throws std::invalid_argument exception */
	template<typename ...Args>
	std::string sformat(const char* fmt, const Args&&... args) noexcept(false) {
		bool autoIncrease = true;
		int autoIncreaseIndex = 0;
		int argSize = sizeof...(args);
		auto argTuple = std::forward_as_tuple(args...);
		std::string result;
		while (*fmt) {
			if ('{' == *fmt) {
				/* Double `{` */
				if ('{' == (*(fmt + 1))) {
					result += '{';
					++++fmt;
				}
				else if ('}' == (*(fmt + 1))) {
					/* auto increase */
					if (!autoIncrease) {
						throw std::invalid_argument("cannot switch from automatic field numbering to manual field specification");
					}
					if (autoIncreaseIndex >= argSize) {
						throw std::invalid_argument("Replacement index out of range for positional args tuple");
					}
					result += std::to_string(GetTupleArgByIndex(argTuple, autoIncreaseIndex++));
					++++fmt;
				}
				else {
					if (autoIncreaseIndex) { /* If autoIncreaseIndex > 0 means auto increase brach run at least once */
						throw std::invalid_argument("cannot switch from automatic field numbering to manual field specification");
					}
					autoIncrease = false;
					std::string indexStr;
					while (*fmt++ != '}') {
						indexStr += *fmt;
					}
					if (isNumeric(indexStr)) {
						int index = stoi(indexStr);
						if (index < 0 || index >= argSize) {
							throw std::invalid_argument("Index out of range");
						}
						else {
							result += std::to_string(GetArgByIndex(argTuple, index));
						}
					}
					else {
						result += "{" + indexStr + "}";
					}
				}
			}
			else if ('}' == *fmt) {
				if ('}' == *(fmt + 1)) {
					result += '}';
					++++fmt;
				}
				else {
					throw std::invalid_argument("Single '}' encountered in format string");
				}
			}
			else {
				result += *fmt++;
			}
		}
		return result;
	}

}
