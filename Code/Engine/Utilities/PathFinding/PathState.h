#pragma once

#include <Foundation/Containers/HashTable.h>
#include <Foundation/Math/Math.h>
#include <Utilities/UtilitiesDLL.h>

/// \brief Base class for all path finding state objects.
struct plPathState
{
  PL_DECLARE_POD_TYPE();

  plPathState()
  {
    m_iReachedThroughNode = 0;
    m_fCostToNode = 0.0f;
    m_fEstimatedCostToTarget = 0.0f;
  }

  /// Initialized by the path searcher. Back-pointer to the node from which this node was reached.
  plInt64 m_iReachedThroughNode;

  /// Each plPathStateGenerator needs to update the costs by taking the nodes from the predecessor state and adding a non-zero cost value to
  /// it.
  float m_fCostToNode;

  /// To get directed path searches (A*) this estimation needs to be filled out. It must be the sum of m_fCostToNode and an estimation
  /// of how much the remaining path will cost (at least).
  /// To always get the 'optimal path' (the shortest) the estimation should be 'optimistic', ie. it mustn't be possible to reach the
  /// destination with less costs than what was estimated.
  /// However, the estimation can also be 'pessimistic', ie. the final path will actually cost less than what was estimated. In this case
  /// path searches can be a lot faster, but they will also produce paths that are longer than necessary and might be overly winding.
  float m_fEstimatedCostToTarget;
};

template <typename PathStateType>
class plPathSearch;

/// \brief The base class for all path state generates.
///
/// A path state generator is a class that takes one 'path state' (typically a node in a graph) and generates all the adjacent nodes
/// that can be reached from there. It carries state, which allows to expand nodes only in certain directions, depending on what actions
/// are possible at some graph node.
///
/// This can be used to do path searches where the capabilities of a unit are taken into account, such as limited turning speeds.
/// The 'state' that is then carried is the current orientation of the unit at that point along the path, which determines into which
/// directions the path search can be expanded.
///
/// PathStateType needs to be derived from plPathState.
template <typename PathStateType>
class plPathStateGenerator
{
public:
  /// \brief Called by a plPathSearch object to generate the adjacent states from graph node iNodeIndex.
  ///
  /// On a 2D grid the iNodeIndex would just be the grid cell index (GridHeight * Cell.y + Cell.x). This function would then 'expand'
  /// the 4 or 8 direct neighbor cells by creating a new PathStateType object for each and then passing that to the plPathSearch object
  /// pPathSearch by calling plPathSearch::AddPathNode.
  virtual void GenerateAdjacentStates(plInt64 iNodeIndex, const PathStateType& StartState, plPathSearch<PathStateType>* pPathSearch) = 0;

  /// \brief Automatically called by plPathSearch objects when a new path search is about to start (plPathSearch::FindClosest).
  /// Allows the generator to do some initial setup.
  virtual void StartSearchForClosest(plInt64 iStartNodeIndex, const PathStateType* pStartState) {}

  /// \brief Automatically called by plPathSearch objects when a new path search is about to start (plPathSearch::FindPath).
  /// Allows the generator to do some initial setup.
  virtual void StartSearch(plInt64 iStartNodeIndex, const PathStateType* pStartState, plInt64 iTargetNodeIndex) {}

  /// \brief Automatically called by plPathSearch objects when a path search was finished.
  /// Allows the generator to do some cleanup.
  virtual void SearchFinished(plResult res) {}
};
