#include "transformer.h"
#include "transformation.h"
#include "transformations.h"

#include "representation/representation.h"
#include "representation/representations.h"

#include "init/init.h"

#include <cassert>
#include <queue>

namespace rhdl {

Transformer::Transformer() : Namespace("transformations")
{
	registerTransformations();
	calculateTransformationPaths();
	init::transformer_ready();
}

void Transformer::addTransformation(std::unique_ptr<const Transformation> &&transformation)
{
	auto ptr = transformation.get();
	auto &obj = add(std::move(transformation));
	transformations_[ptr -> typeID()].push_back(ptr);
}

std::unique_ptr<Representation> Transformer::transform(const Representation &source, TransformationTypeID ttype)
{
	assert (source.typeID() == ttype.first);

	const Transformation *t = select(ttype);
	assert(t);

	return t -> execute(source);
}

const Transformer::PathResult *Transformer::getTransformationPath(RepresentationTypeID dstType, RepresentationTypeID srcType)
{
	TransformationTypeID ttype{srcType, dstType};
	auto iresult = transformationPaths_.find(ttype);

	if (iresult == transformationPaths_.end())
		return nullptr;

	return &*iresult;
}

const Transformer::PathResult *Transformer::getTransformationPath(Representation::TypeID dstType, Representations::TypeSet existingRepresentations)
{
	const PathResult *finalResult = nullptr;

	for (RepresentationTypeID srcType : Representations::validTypeIDs) {
		if (!existingRepresentations.test(srcType))
			continue;

		const PathResult *currentResult = getTransformationPath(dstType, srcType);

		if (currentResult == nullptr)
			continue;

		if (finalResult == nullptr || currentResult -> second.size() < finalResult -> second.size())
			finalResult = currentResult;
	}

	return finalResult;
}

void Transformer::calculateTransformationPaths()
{
	using WorkQueue = std::queue<TransformationTypeID>;

	std::unique_ptr<WorkQueue> currentWorkQueue = std::make_unique<WorkQueue>();

	for (Representation::TypeID srcType : Representations::validTypeIDs) {
		for (Representation::TypeID dstType : Representations::validTypeIDs) {
			if (srcType == dstType)
				continue;

			TransformationTypeID currentTransformation{srcType, dstType};

			if (select(currentTransformation)) {
				transformationPaths_[currentTransformation] = {dstType};
				continue;
			}

			currentWorkQueue -> push(currentTransformation);
		}
	}

	for (unsigned int minLength = 2; minLength < Representations::validTypeIDs.size(); ++minLength) {
		std::unique_ptr<WorkQueue> nextWorkQueue = std::make_unique<WorkQueue>();

		while (!currentWorkQueue -> empty()) {
			TransformationTypeID currentTransformation = currentWorkQueue -> front();
			currentWorkQueue -> pop();
			auto icurrentPath = transformationPaths_.find(currentTransformation);

			assert (icurrentPath == transformationPaths_.end() ||
					icurrentPath -> second.size() >= minLength);

			for (RepresentationTypeID intermediateType : Representations::validTypeIDs) {
				if (intermediateType == currentTransformation.first ||
					intermediateType == currentTransformation.second)
					continue;

				TransformationTypeID first{currentTransformation.first, intermediateType};
				TransformationTypeID second{intermediateType, currentTransformation.second};

				if (transformationPaths_.find(first) != transformationPaths_.end() &&
					transformationPaths_.find(second) != transformationPaths_.end())
				{
					Path &pfirst = transformationPaths_.at(first);
					Path &psecond = transformationPaths_.at(second);

					if (icurrentPath != transformationPaths_.end())
						if (icurrentPath -> second.size() <= pfirst.size() + psecond.size())
							continue;

					Path &currentPath = transformationPaths_[currentTransformation];

					currentPath = transformationPaths_[first];
					std::copy(psecond.begin(), psecond.end(), std::back_inserter(currentPath));

					if (currentPath.size() == minLength)
						break;
				}
			}

			if (transformationPaths_.find(currentTransformation) == transformationPaths_.end()) {
				nextWorkQueue -> push(currentTransformation);
				continue;
			}

			if (transformationPaths_.at(currentTransformation).size() > minLength)
				nextWorkQueue -> push(currentTransformation);
		}

		currentWorkQueue = std::move(nextWorkQueue);
	}
}

const Transformation *Transformer::select(TransformationTypeID t) const
{
	auto ikv = transformations_.find(t);

	if (ikv == transformations_.end())
		return nullptr;

	auto &list = transformations_.at(t);

	assert (!list.empty());

	if (list.empty())
		return nullptr;
	else
		return list.back();
}

void Transformer::registerTransformations()
{
	Transformations transformations;

	while (auto pt = transformations.next()) {
		addTransformation(std::move(pt));
	}
}

Transformer *transformer;

}

