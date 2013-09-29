#pragma once

#include "btree.h"
#include "types.h"
#include "noncopyable.h"

namespace Utils
{

class TreeWithRms : Utils::NonCopyable
{
public:
    TreeWithRms();

    //! Tries to add new value to the tree. If succeeded - calculate new RMS, otherwise return old RMS
    Networking::Types::ResultType AddNewValueAndCalculateRms(Networking::Types::InputType new_value);

    //! Get current RMS
    Networking::Types::ResultType GetRms() const;

private:
    Networking::Types::ResultType _rms;
    typedef Utils::BinaryTree<unsigned> BinaryTreeOfUnsigned;
    BinaryTreeOfUnsigned _tree;
};

} // namespace Utils