#include "gta/stat.hpp"
#include "hooking/hooking.hpp"
#include "logger/logger.hpp"
#include "pointers.hpp"
#include "services/stats/stats_service.hpp"

namespace big
{
	bool hooks::stat_ctor(sStatData* _this, const char* name, void* p2)
	{
		bool ret = g_hooking->get_original<hooks::stat_ctor>()(_this, name, p2);
		g_stats_service->register_stat(_this, name);
		return ret;
	}
	bool hooks::stat_dtor(sStatData* _this, uint32_t p2)
	{
		LOG(VERBOSE) << "Stat destroyed!";
		g_stats_service->delete_stat(_this);
		return g_hooking->get_original<hooks::stat_dtor>()(_this, p2);
	}

	bool hooks::mp_stats_save(void* _this, uint32_t p2, uint32_t p3, uint32_t p4, uint32_t p5, uint32_t p6)
	{
		bool ret = g_hooking->get_original<hooks::mp_stats_save>()(_this, p2, p3, p4, p5, p6);

		LOGF(VERBOSE, "mp_stats_save({}, {}, {}, {}, {}, {}) == {}", _this, p2, p3, p4, p5, p6, ret);

		g_stats_service->save_stats();

		return ret;
	}
	bool hooks::mp_save_download(void* _this)
	{
		if(*(int*)((size_t)_this+0x14) == 3)
		{
			int char_slot = *(int*)((size_t)_this+0x10);
			LOG(VERBOSE) << "Loading custom stats, Index: " << char_slot;
			if(g_stats_service->load_stats() && !g.always_load_into_character_creator)
			{
				// Set the load as successful
				LOG(VERBOSE) << "Setting load as successful";
				*g_pointers->m_mp_save_download_error = 0;
				*(bool*)((size_t)_this+0x2E0) = true;
				*(bool*)((size_t)_this+0x2E1) = false;
			}
		}
		bool ret = g_hooking->get_original<hooks::mp_save_download>()(_this);
		return ret;
	}
}