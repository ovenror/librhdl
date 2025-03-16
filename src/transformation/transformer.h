#ifndef TRANSFORMER_H
#define TRANSFORMER_H

#include "transformation/transformationtypeid.h"
#include "representation/representations.h"

#include "c_api/namespace.h"

#include <map>
#include <memory>
#include <vector>

namespace rhdl {

class Transformation;
class Representation;

class Transformer : public Namespace
{
public:
	using Path = std::vector<RepresentationTypeID>;
	using PathResult = std::pair<const TransformationTypeID, Path>;

	Transformer();

	void addTransformation(std::unique_ptr<const Transformation> &&transformation);

	std::unique_ptr<Representation> transform(const Representation &source, TransformationTypeID ttype);

	const PathResult *getTransformationPath(RepresentationTypeID dstID, Representations::TypeSet existingRepresentations);
	const PathResult *getTransformationPath(RepresentationTypeID dstType, RepresentationTypeID srcType);

private:
	using PathMap = std::map<TransformationTypeID, Path>;

	const Transformation *select(TransformationTypeID t) const;

	void registerTransformations();
	void calculateTransformationPaths();

	std::map<TransformationTypeID, std::vector<const Transformation *>> transformations_;
	PathMap transformationPaths_;
};

extern Transformer *transformer;

}

#endif // TRANSFORMER_H
