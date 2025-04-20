#include "Service.h"

#include "Core/Application.h"

namespace wind 
{
    Service::Service(const std::string& name) :
        m_Application(nullptr), m_Name(name), m_Active(true)
    {
    }

    void Service::ProduceSignal(Signal signal)
    {
        if (m_Application != nullptr) 
        {
            m_Application->PushFrameSignal(signal);
        }
    }
}