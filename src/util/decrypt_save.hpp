#pragma once
#include "memory/byte_patch.hpp"

namespace big
{
	struct decrypt_save_patch
	{
		inline static memory::byte_patch* m_check_enc_param;

		inline static void apply()
		{
			decrypt_save_patch::m_check_enc_param->apply();
		}

		inline static void restore()
		{
			decrypt_save_patch::m_check_enc_param->restore();
		}
	};
}