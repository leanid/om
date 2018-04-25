#pragma once

#include "om/engine.hxx"

namespace om
{

struct engine_impl final : engine
{
    engine_impl(int argc, char** argv);

    void initialize(params) final;
};

} // end namespace om
