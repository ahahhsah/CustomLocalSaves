#include "gta/stat.hpp"
#include "hooking/hooking.hpp"
#include "logger/logger.hpp"
#include "pointers.hpp"
#include "util/decrypt_save.hpp"
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

	std::string pso_buffer;
	char* og_buffer_ptr;
	uint64_t og_buffer_size;
	bool hooks::mp_save_download(intptr_t _this)
	{
		int download_state = *(int*)(_this+0x14);
		int char_slot = *(int*)(_this+0x10);
		if(download_state == 1 && g.load_fsl_files)
		{
			auto size = g_stats_service->get_pso_file_size(char_slot);
			LOG(VERBOSE) << "Buffer set size: " << size;
			pso_buffer.resize(size, '\0');
			g_stats_service->read_pso_file(char_slot, pso_buffer.data(), size);

			og_buffer_ptr = *(char**)(_this+0x30);
			og_buffer_size = *(uint32_t*)(_this+0x3C);
			*(char**)(_this+0x30) = pso_buffer.data();
			*(uint32_t*)(_this+0x3C) = size;
			*(int*)(_this+0x20) = 3;
			decrypt_save_patch::apply();
		}

		if(download_state == 3)
		{
			LOG(VERBOSE) << "Loading custom stats, Index: " << char_slot;
			if(g.load_fsl_files)
			{
				pso_buffer.clear();
				*(char**)(_this+0x30) = og_buffer_ptr;
				*(uint32_t*)(_this+0x3C) = og_buffer_size;
				decrypt_save_patch::restore();
			}

			if(g_stats_service->load_stats() && !g.always_load_into_character_creator)
			{
				// Set the load as successful
				LOG(VERBOSE) << "Setting load as successful";
				*g_pointers->m_mp_save_download_error = 0;
				*(bool*)(_this+0x2E0) = true;
				*(bool*)(_this+0x2E1) = false;
			}
		}
		bool ret = g_hooking->get_original<hooks::mp_save_download>()(_this);
		return ret;
	}
}