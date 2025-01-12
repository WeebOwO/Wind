#pragma once

namespace wind
{
    class RenderGraph;

    class IRenderPass
    {
    protected:
        virtual void Record(RenderGraph& renderGraph);
        
    };
} // namespace wind