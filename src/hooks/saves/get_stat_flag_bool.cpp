#include "hooking/hooking.hpp"

namespace big
{
	bool hooks::get_stat_flag_bool(void* p0, const char* flag, bool default_, bool p4)
	{
		if(strcmp(flag, "ServerAuthoritative") == 0
		|| strcmp(flag, "profile") == 0)
			return false;

		return g_hooking->get_original<hooks::get_stat_flag_bool>()(p0, flag, default_, p4);
	}
}