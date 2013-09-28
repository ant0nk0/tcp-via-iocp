#pragma once

#include "btree.h"
#include "types.h"

class TreeWithRms
{
public:
    TreeWithRms() : _rms(0) {}

    Networking::Types::ResultType AddNewValueAndCalculateRms(Networking::Types::InputType new_value)
    {
        if (!_tree.Add(new_value))
            return _rms;

        // new value was added. need to re-calculate RMS
        Networking::Types::InputType squares_summ = 0;
        std::size_t walked_elements_cnt = 0;

        _tree.Walk
        (
            [&] (const BinaryTreeOfUnsigned::ValueType& value)
            {
                ++walked_elements_cnt;
                squares_summ += value * value;
            }
        );

        _rms = sqrtf(static_cast<Networking::Types::ResultType>(squares_summ) / walked_elements_cnt);
        return _rms;
    }

    Networking::Types::ResultType GetRms() const
    {
        return _rms; 
    }

private:
    Networking::Types::ResultType _rms;
    typedef Utils::BinaryTree<unsigned> BinaryTreeOfUnsigned;
    BinaryTreeOfUnsigned _tree;
};
