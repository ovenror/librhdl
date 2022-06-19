#ifndef CSINGLE_H
#define CSINGLE_H

#include "cbase.h"

namespace rhdl {

class ISingle;

struct CSingleOps : public CSametypeOpsBase<ISingle&, CSingleOps> {
	using Super::Super;

	bool compatible() const;
};

class CSingle : public CBase<CSingleOps>
{
public:
	using Super::Super;

	virtual ~CSingle();

	void eval_int() const override;
};

}

#endif // CSINGLE_H
