#pragma once
#include "fiber_pool.hpp"
#include "gta/enums.hpp"
#include "hooking/hooking.hpp"
#include "native_hooks.hpp"
#include "natives.hpp"
#include "util/scripts.hpp"

namespace big
{
	namespace fm_maintain_cloud_header_data
	{
		// Too lazy to fix UGC
		inline void UGC_WAS_QUERY_FORCE_CANCELLED(rage::scrNativeCallContext* src)
		{
			rage::scrThread* thread = rage::tlsContext::get()->m_script_thread;
			if(thread)
			{
				thread->m_context.m_state = rage::eThreadState::paused;
			}
		}
	}
}