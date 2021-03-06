#pragma once

#include "ExportableObject.h"	
#include "ExportableMesh.h"
#include "ExportableScene.h"
#include "Transform.h"

enum class TransformKind
{
	// A simple transform without segment scale compensation nor pivot point
	Simple,

	// A joint with segment scale compensation
	ComplexJoint,

	// A transform with a pivot point
	ComplexTransform
};

class ExportableNode : public ExportableObject
{
public:
	~ExportableNode();
	
	ExportableMesh* mesh() const { return m_mesh.get(); }

	const MDagPath dagPath;

	TransformKind transformKind = TransformKind::Simple;

	double scaleFactor = 1.0;

	MPoint pivotPoint;

	// nullptr for root nodes.
	ExportableNode* parentNode = nullptr;

	NodeTransformState initialTransformState;
	NodeTransformState currentTransformState;

	std::unique_ptr<NodeAnimation> createAnimation(const ExportableFrames& frameTimes, const double scaleFactor) override;

	// The primary node to represent the transform
	// See Transform.h for details
	GLTF::Node& glPrimaryNode() { return m_glNodes[0]; }
	const GLTF::Node& glPrimaryNode() const { return const_cast<ExportableNode*>(this)->glPrimaryNode(); }

	// The secondary node to represent the transform
	// Can be the same as the primary node for simple transforms
	// See Transform.h for details
	GLTF::Node& glSecondaryNode() { return m_glNodes[transformKind != TransformKind::Simple]; }
	const GLTF::Node& glSecondaryNode() const { return const_cast<ExportableNode*>(this)->glSecondaryNode(); }

	MDagPath parentDagPath() const { return parentNode ? parentNode->dagPath : MDagPath(); }

	// Update the node transforms using the values at the current frame
	void updateNodeTransforms(NodeTransformCache& transformCache);

	// If this node is a redundant shape node, move the mesh to the parent node, and return true. 
	bool tryMergeRedundantShapeNode();

private:
	friend class ExportableScene;

	ExportableNode(const MDagPath& dagPath);

	void load(ExportableScene& scene, NodeTransformCache& transformCache);

	std::array<GLTF::Node, 2> m_glNodes;
	std::unique_ptr<ExportableMesh> m_mesh;

	DISALLOW_COPY_MOVE_ASSIGN(ExportableNode);
};

