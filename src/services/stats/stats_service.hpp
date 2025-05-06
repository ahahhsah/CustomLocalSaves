#pragma once

#include "gta/stat.hpp"
namespace big
{
	class stats_service
	{
	public:
		stats_service();
		~stats_service();

		void save_stats();
		bool load_stats();

		void register_stat(sStatData* data, const char* name);
		void delete_stat(sStatData* data);

		sStatData* get_stat_by_hash(Hash stat);

	private:
		std::unordered_map<sStatData*, Hash> m_all_stats;

		std::unordered_map<Hash, int> m_int_stats;
		std::unordered_map<Hash, float> m_float_stats;
		std::unordered_map<Hash, std::string> m_string_stats;
		std::unordered_map<Hash, bool> m_bool_stats;
		std::unordered_map<Hash, uint8_t> m_uint8_stats;
		std::unordered_map<Hash, uint16_t> m_uint16_stats;
		std::unordered_map<Hash, uint32_t> m_uint32_stats;
		std::unordered_map<Hash, uint64_t> m_uint64_stats;
		std::unordered_map<Hash, int64_t> m_int64_stats;
		std::unordered_map<Hash, uint64_t> m_date_stats;
		std::unordered_map<Hash, uint64_t> m_pos_stats;
		std::unordered_map<Hash, int> m_textlabel_stats;
		std::unordered_map<Hash, int64_t> m_packed_stats;
		std::unordered_map<Hash, int64_t> m_userid_stats;

		std::unordered_map<Hash, std::string> m_stat_hash_to_string;

		file m_save_file;
	};

	inline stats_service* g_stats_service;
}