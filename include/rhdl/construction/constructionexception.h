#ifndef CONSTRUCTIONEXCEPTION_H
#define CONSTRUCTIONEXCEPTION_H

#include <rhdl/construction/error.h>
#include <string>

namespace rhdl {

class ConstructionException : public std::exception
{
public:
	ConstructionException(Errorcode code, const std::string &info = "");
	virtual ~ConstructionException();

	const char *whatcode() const noexcept;
	const char *what() const noexcept override;
	Errorcode errorcode() const {return code_;}

protected:
	std::string info_;

private:
	Errorcode code_;
};


}

#endif // CONSTRUCTIONEXCEPTION_H
