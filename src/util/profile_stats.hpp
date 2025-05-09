#pragma once
#include "memory/byte_patch.hpp"

namespace big
{
	struct skip_profile_stats_patch
	{
		inline static memory::byte_patch* m_skip;

		inline static void apply()
		{
			skip_profile_stats_patch::m_skip->apply();
		}

		inline static void restore()
		{
			skip_profile_stats_patch::m_skip->restore();
		}
	};
}