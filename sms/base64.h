#include <string>

std::string base64_encode(unsigned char const* , unsigned int len);
std::string base64_decode(std::string const& s);


// �ַ���base64ת��
inline void encode64(std::string & s )
{
	s = base64_encode((unsigned char *)s.c_str(), s.size());
}

inline void decode64(std::string & s )
{
	s = base64_decode(s);
}
