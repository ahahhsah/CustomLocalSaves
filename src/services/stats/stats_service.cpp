#include "stats_service.hpp"

#include "file_manager.hpp"
#include "gta/joaat.hpp"
#include "gta/stat.hpp"
#include "security/ObfVar.hpp"

#include <cstddef>

namespace big
{
	stats_service::stats_service()
	{
		g_stats_service     = this;
		m_save_file_default = g_file_manager.get_project_file("./save_default0000.json");
		m_save_file_char1   = g_file_manager.get_project_file("./save_char0001.json");
		m_save_file_char2   = g_file_manager.get_project_file("./save_char0002.json");
		m_save_overwrite    = g_file_manager.get_project_file("./save_overwrite.json");
	}
	stats_service::~stats_service()
	{
		g_stats_service = nullptr;
	}

	void stats_service::register_stat(sStatData* data, const char* name)
	{
		bool is_char      = false;
		uint8_t character = 0;
		bool is_online    = data->m_flags & (1 << 3);
		if (name[0] == 'M' && name[1] == 'P' && name[3] == '_')
		{
			is_char   = true;
			character = std::atoi(&name[2]);
		}
		m_all_stats[rage::joaat(name)] = {data, is_char, character, is_online};
		if (g.use_human_readable_stat_names)
		{
			m_stat_hash_to_string[rage::joaat(name)] = name;
		}
	}
	void stats_service::delete_stat(sStatData* data)
	{
		for (auto stat : m_all_stats)
		{
			if (stat.second.m_stat == data)
			{
				m_all_stats.erase(stat.first);
				return;
			}
		}
	}

	sCustomStat* stats_service::get_stat_by_hash(Hash stat_to_find)
	{
		if (m_all_stats.contains(stat_to_find))
		{
			return &m_all_stats[stat_to_find];
		}
		return nullptr;
	}

	template<typename T>
	void stats_service::save_stat_map_to_json(nlohmann::json& json, T& map, bool use_stat_names, uint8_t char_index)
	{
		auto stats = nlohmann::json::array();
		for (auto stat : map)
		{
			bool char_stat = char_index > 0;
			if (get_stat_by_hash(stat.first)->m_is_character != char_stat)
				continue;
			if (get_stat_by_hash(stat.first)->m_character_index != char_index-1 && char_stat)
				continue;

			if (use_stat_names)
			{
				stats.push_back({m_stat_hash_to_string[stat.first], stat.second});
			}
			else
			{
				stats.push_back({stat.first, stat.second});
			}
		}
		json = stats;
	}
	template<typename T>
	void stats_service::load_stat_map_from_json(const nlohmann::json& json, T& map, bool use_stat_names)
	{
		if (use_stat_names)
		{
			for (auto stat : json)
			{
				map[rage::joaat(stat[0])] = stat[1];
			}
		}
		else
		{
			map = json;
		}
	}

	void stats_service::save_internal_stats_to_json(uint8_t char_index)
	{
		nlohmann::json json;

		bool string_names                      = g.use_human_readable_stat_names;
		json["uses_human_readable_stat_names"] = string_names;

		try
		{
			save_stat_map_to_json(json["INT"], m_int_stats, string_names, char_index);
			save_stat_map_to_json(json["FLOAT"], m_float_stats, string_names, char_index);
			save_stat_map_to_json(json["STRING"], m_string_stats, string_names, char_index);
			save_stat_map_to_json(json["BOOL"], m_bool_stats, string_names, char_index);
			save_stat_map_to_json(json["UINT8"], m_uint8_stats, string_names, char_index);
			save_stat_map_to_json(json["UINT16"], m_uint16_stats, string_names, char_index);
			save_stat_map_to_json(json["UINT32"], m_uint32_stats, string_names, char_index);
			save_stat_map_to_json(json["UINT64"], m_uint64_stats, string_names, char_index);
			save_stat_map_to_json(json["INT64"], m_int64_stats, string_names, char_index);
			save_stat_map_to_json(json["DATE"], m_date_stats, string_names, char_index);
			save_stat_map_to_json(json["POS"], m_pos_stats, string_names, char_index);
			save_stat_map_to_json(json["TEXTLABEL"], m_textlabel_stats, string_names, char_index);
			save_stat_map_to_json(json["PACKED"], m_packed_stats, string_names, char_index);
			save_stat_map_to_json(json["USERID"], m_userid_stats, string_names, char_index);
		}
		catch (const std::exception& ex)
		{
			LOG(FATAL) << "Stats failed to save: " << ex.what();
			return;
		}
		std::ofstream file;
		if (char_index == 0)
		{
			file.open(m_save_file_default.get_path(), std::ios::out | std::ios::trunc);
		}
		else if (char_index == 1)
		{
			file.open(m_save_file_char1.get_path(), std::ios::out | std::ios::trunc);
		}
		else if (char_index == 2)
		{
			file.open(m_save_file_char2.get_path(), std::ios::out | std::ios::trunc);
		}

		file << json.dump(1, '	');
		file.close();
	}

	bool stats_service::load_internal_stats_from_json(uint8_t char_index)
	{
		std::ifstream file;
		if (char_index == 0)
		{
			if (!m_save_file_default.exists())
				return false;

			LOG(VERBOSE) << "Loading save_default0000.json";
			file.open(m_save_file_default.get_path());
		}
		else if (char_index == 1)
		{
			if (!m_save_file_char1.exists())
				return false;

			LOG(VERBOSE) << "Loading save_char0001.json";
			file.open(m_save_file_char1.get_path());
		}
		else if (char_index == 2)
		{
			if (!m_save_file_char2.exists())
				return false;

			LOG(VERBOSE) << "Loading save_char0002.json";
			file.open(m_save_file_char2.get_path());
		}
		else if (char_index == SAVE_OVERWRITE_INDEX)
		{
			if (!m_save_overwrite.exists())
				return false;

			LOG(VERBOSE) << "Loading save_overwrite.json";
			file.open(m_save_overwrite.get_path());
		}
		try
		{
			// Ignore comments for save_overwrite.json
			const nlohmann::json& json = nlohmann::json::parse(file, nullptr, true, /*ignore_comments*/ true);
			bool uses_human_readable_stat_names = json["uses_human_readable_stat_names"];
			load_stat_map_from_json(json["INT"], m_int_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["FLOAT"], m_float_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["STRING"], m_string_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["BOOL"], m_bool_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["UINT8"], m_uint8_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["UINT16"], m_uint16_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["UINT32"], m_uint32_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["UINT64"], m_uint64_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["INT64"], m_int64_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["DATE"], m_date_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["POS"], m_pos_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["TEXTLABEL"], m_textlabel_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["PACKED"], m_packed_stats, uses_human_readable_stat_names);
			load_stat_map_from_json(json["USERID"], m_userid_stats, uses_human_readable_stat_names);

			return true;
		}
		catch (const std::exception& ex)
		{
			LOG(WARNING) << "Detected corrupt save file: " << ex.what();
		}

		return false;
	}

	void stats_service::save_stats()
	{
		for (auto stat : m_all_stats)
		{
			if (!g.save_unmodified_stats && stat.second.m_stat->IsZero())
				continue;

			// Skip stats not marked "online"
			if (!stat.second.m_is_online)
				continue;

			switch (stat.second.m_stat->GetTypeId())
			{
			case eStatType::INT:
			{
				m_int_stats[stat.first] = stat.second.m_stat->GetIntData();
				break;
			}
			case eStatType::FLOAT:
			{
				m_float_stats[stat.first] = stat.second.m_stat->GetFloatData();
				break;
			}
			case eStatType::STRING:
			{
				m_string_stats[stat.first] = std::string(stat.second.m_stat->GetStringData());
				break;
			}
			case eStatType::BOOL_:
			{
				m_bool_stats[stat.first] = stat.second.m_stat->GetBoolData();
				break;
			}
			case eStatType::UINT8:
			{
				m_uint8_stats[stat.first] = stat.second.m_stat->GetUint8Data();
				break;
			}
			case eStatType::UINT16:
			{
				m_uint16_stats[stat.first] = stat.second.m_stat->GetUint16Data();
				break;
			}
			case eStatType::UINT32:
			{
				m_uint32_stats[stat.first] = stat.second.m_stat->GetUint32Data();
				break;
			}
			case eStatType::UINT64:
			{
				m_uint64_stats[stat.first] = stat.second.m_stat->GetUint64Data();
				break;
			}
			case eStatType::INT64:
			{
				m_int64_stats[stat.first] = stat.second.m_stat->GetInt64Data();
				break;
			}
			case eStatType::DATE:
			{
				m_date_stats[stat.first] = stat.second.m_stat->GetUint64Data();
				break;
			}
			case eStatType::POS:
			{
				m_pos_stats[stat.first] = stat.second.m_stat->GetUint64Data();
				break;
			}
			case eStatType::TEXTLABEL:
			{
				m_textlabel_stats[stat.first] = stat.second.m_stat->GetIntData();
				break;
			}
			case eStatType::PACKED:
			{
				m_packed_stats[stat.first] = stat.second.m_stat->GetUint64Data();
				break;
			}
			case eStatType::USERID:
			{
				m_userid_stats[stat.first] = stat.second.m_stat->GetUint64Data();
				break;
			}
			case eStatType::PROFILE_SETTING: break;
			default:
			{
				LOG(VERBOSE) << "Unknown stat type: " << (int)stat.second.m_stat->GetTypeId() << "In stat: " << stat.first;
				break;
			}
			}
		}

		int last_character = g_stats_service->get_stat_by_hash(RAGE_JOAAT("MPPLY_LAST_MP_CHAR"))->m_stat->GetIntData();
		save_internal_stats_to_json(0);
		save_internal_stats_to_json(last_character + 1);
	}

	bool stats_service::load_stats()
	{
		if (!load_internal_stats_from_json(0))
		{
			return false;
		}

		// Load character stats if they exist.
		load_internal_stats_from_json(1);
		load_internal_stats_from_json(2);

		// Load stat overrides last.
		load_internal_stats_from_json(SAVE_OVERWRITE_INDEX);

		for (auto stat : m_all_stats)
		{
			switch (stat.second.m_stat->GetTypeId())
			{
			case eStatType::INT:
			{
				if (m_int_stats.contains(stat.first))
					stat.second.m_stat->SetIntData(m_int_stats[stat.first]);
				break;
			}
			case eStatType::FLOAT:
			{
				if (m_float_stats.contains(stat.first))
					stat.second.m_stat->SetFloatData(m_float_stats[stat.first]);
				break;
			}
			case eStatType::STRING:
			{
				if (m_string_stats.contains(stat.first))
					strncpy(((sSubStatData<char[32]>*)stat.second.m_stat)->m_data, m_string_stats[stat.first].data(), 32);
				break;
			}
			case eStatType::BOOL_:
			{
				if (m_bool_stats.contains(stat.first))
					stat.second.m_stat->SetBoolData(m_bool_stats[stat.first]);
				break;
			}
			case eStatType::UINT8:
			{
				if (m_uint8_stats.contains(stat.first))
					stat.second.m_stat->SetUint8Data(m_uint8_stats[stat.first]);
				break;
			}
			case eStatType::UINT16:
			{
				if (m_uint16_stats.contains(stat.first))
					stat.second.m_stat->SetUint16Data(m_uint16_stats[stat.first]);
				break;
			}
			case eStatType::UINT32:
			{
				if (m_uint32_stats.contains(stat.first))
					stat.second.m_stat->SetUint32Data(m_uint32_stats[stat.first]);
				break;
			}
			case eStatType::UINT64:
			{
				if (m_uint64_stats.contains(stat.first))
					stat.second.m_stat->SetUint64Data(m_uint64_stats[stat.first]);
				break;
			}
			case eStatType::INT64:
			{
				if (m_int64_stats.contains(stat.first))
					stat.second.m_stat->SetInt64Data(m_int64_stats[stat.first]);
				break;
			}
			case eStatType::DATE:
			{
				if (m_date_stats.contains(stat.first))
					stat.second.m_stat->SetUint64Data(m_date_stats[stat.first]);
				break;
			}
			case eStatType::POS:
			{
				if (m_pos_stats.contains(stat.first))
					stat.second.m_stat->SetUint64Data(m_pos_stats[stat.first]);
				break;
			}
			case eStatType::TEXTLABEL:
			{
				if (m_textlabel_stats.contains(stat.first))
					stat.second.m_stat->SetIntData(m_textlabel_stats[stat.first]);
				break;
			}
			case eStatType::PACKED:
			{
				if (m_packed_stats.contains(stat.first))
					stat.second.m_stat->SetUint64Data(m_packed_stats[stat.first]);
				break;
			}
			case eStatType::USERID:
			{
				if (m_userid_stats.contains(stat.first))
					stat.second.m_stat->SetUint64Data(m_userid_stats[stat.first]);
				break;
			}
			case eStatType::PROFILE_SETTING: break;
			default:
			{
				LOGF(WARNING, "Unknown stat type: {} In stat: {}", (int)stat.second.m_stat->GetTypeId(), stat.first);
				break;
			}
			}

			//  Hack to fix the character creator always activating.
			if (stat.first == RAGE_JOAAT("MP0_CHAR_IS_NGPC_VERSION") || stat.first == RAGE_JOAAT("MP1_CHAR_IS_NGPC_VERSION"))
			{
				stat.second.m_stat->SetIntData(1);
			}
		}
		return true;
	}
}