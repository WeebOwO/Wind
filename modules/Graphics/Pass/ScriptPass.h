#pragma once

namespace wind
{
    class RenderGraph;

    class ScriptPass
    {
    protected:
        virtual void Record(RenderGraph& renderGraph);
        
    };
} // namespace wind