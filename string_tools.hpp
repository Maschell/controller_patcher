#ifndef _STRING_TOOLS_H_
#define _STRING_TOOLS_H_

bool EndsWith(const std::string& a, const std::string& b);
std::vector<std::string> MyStringSplit(const std::string & inValue, const std::string & splitter);

/* Main */
#ifdef __cplusplus
extern "C" {
#endif

//! C wrapper for our C++ functions
const char *byte_to_binary(int x);

#ifdef __cplusplus
}
#endif

#endif /* _STRING_TOOLS_H_ */
