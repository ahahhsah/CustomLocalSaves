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
		g_stats_service = this;
		m_save_file     = g_file_manager.get_project_file("./gamesave.json");
	}
	stats_service::~stats_service()
	{
		g_stats_service = nullptr;
	}

	void stats_service::register_stat(sStatData* data, const char* name)
	{
		m_all_stats[data] = rage::joaat(name);
		if (g.use_human_readable_stat_names)
		{
			m_stat_hash_to_string[rage::joaat(name)] = name;
		}
	}
	void stats_service::delete_stat(sStatData* data)
	{
		m_all_stats.erase(data);
	}

	sStatData* stats_service::get_stat_by_hash(Hash stat_to_find)
	{
		for(auto stat : m_all_stats)
		{
			if(stat.second == stat_to_find)
			{
				return stat.first;
			}
		}
		return nullptr;
	}

	void stats_service::save_stats()
	{
		nlohmann::json json;

		for (auto stat : m_all_stats)
		{
			if (!g.save_unmodified_stats && stat.first->IsZero())
				continue;

			// Skip stats not marked "online"
			if ((stat.first->m_flags & (1 << 3)) == 0)
				continue;

			switch (stat.first->GetTypeId())
			{
			case eStatType::INT:
			{
				m_int_stats[stat.second] = stat.first->GetIntData();
				break;
			}
			case eStatType::FLOAT:
			{
				m_float_stats[stat.second] = stat.first->GetFloatData();
				break;
			}
			case eStatType::STRING:
			{
				m_string_stats[stat.second] = std::string(stat.first->GetStringData());
				break;
			}
			case eStatType::BOOL_:
			{
				m_bool_stats[stat.second] = stat.first->GetBoolData();
				break;
			}
			case eStatType::UINT8:
			{
				m_uint8_stats[stat.second] = stat.first->GetUint8Data();
				break;
			}
			case eStatType::UINT16:
			{
				m_uint16_stats[stat.second] = stat.first->GetUint16Data();
				break;
			}
			case eStatType::UINT32:
			{
				m_uint32_stats[stat.second] = stat.first->GetUint32Data();
				break;
			}
			case eStatType::UINT64:
			{
				m_uint64_stats[stat.second] = stat.first->GetUint64Data();
				break;
			}
			case eStatType::INT64:
			{
				m_int64_stats[stat.second] = stat.first->GetInt64Data();
				break;
			}
			case eStatType::DATE:
			{
				m_date_stats[stat.second] = stat.first->GetUint64Data();
				break;
			}
			case eStatType::POS:
			{
				m_pos_stats[stat.second] = stat.first->GetUint64Data();
				break;
			}
			case eStatType::TEXTLABEL:
			{
				m_textlabel_stats[stat.second] = stat.first->GetIntData();
				break;
			}
			case eStatType::PACKED:
			{
				m_packed_stats[stat.second] = stat.first->GetUint64Data();
				break;
			}
			case eStatType::USERID:
			{
				m_userid_stats[stat.second] = stat.first->GetUint64Data();
				break;
			}
			case eStatType::PROFILE_SETTING: break;
			default:
			{
				LOG(VERBOSE) << "Unknown stat type: " << (int)stat.first->GetTypeId() << "In stat: " << stat.second;
				break;
			}
			}
		}

		json["INT"]       = m_int_stats;
		json["FLOAT"]     = m_float_stats;
		json["STRING"]    = m_string_stats;
		json["BOOL"]      = m_bool_stats;
		json["UINT8"]     = m_uint8_stats;
		json["UINT16"]    = m_uint16_stats;
		json["UINT32"]    = m_uint32_stats;
		json["UINT64"]    = m_uint64_stats;
		json["INT64"]     = m_int64_stats;
		json["DATE"]      = m_date_stats;
		json["POS"]       = m_pos_stats;
		json["TEXTLABEL"] = m_textlabel_stats;
		json["PACKED"]    = m_packed_stats;
		json["USERID"]    = m_userid_stats;
		std::ofstream file(m_save_file.get_path(), std::ios::out | std::ios::trunc);
		file << json.dump(1, '	');
		file.close();
	}

	bool stats_service::load_stats()
	{
		std::ifstream file(m_save_file.get_path());
		bool result = false;

		if (!m_save_file.exists())
		{
			file.close();
			return result;
		}

		nlohmann::json json;
		try
		{
			file >> json;
			m_int_stats       = json["INT"];
			m_float_stats     = json["FLOAT"];
			m_string_stats    = json["STRING"];
			m_bool_stats      = json["BOOL"];
			m_uint8_stats     = json["UINT8"];
			m_uint16_stats    = json["UINT16"];
			m_uint32_stats    = json["UINT32"];
			m_uint64_stats    = json["UINT64"];
			m_int64_stats     = json["INT64"];
			m_date_stats      = json["DATE"];
			m_pos_stats       = json["POS"];
			m_textlabel_stats = json["TEXTLABEL"];
			m_packed_stats    = json["PACKED"];
			m_userid_stats    = json["USERID"];

			for (auto stat : m_all_stats)
			{
				switch (stat.first->GetTypeId())
				{
				case eStatType::INT:
				{
					if (m_int_stats.contains(stat.second))
						stat.first->SetIntData(m_int_stats[stat.second]);
					break;
				}
				case eStatType::FLOAT:
				{
					if (m_float_stats.contains(stat.second))
						stat.first->SetFloatData(m_float_stats[stat.second]);
					break;
				}
				case eStatType::STRING:
				{
					if (m_string_stats.contains(stat.second))
						strncpy(((sSubStatData<char[32]>*)stat.first)->m_data, m_string_stats[stat.second].data(), 32);
					break;
				}
				case eStatType::BOOL_:
				{
					if (m_bool_stats.contains(stat.second))
						stat.first->SetBoolData(m_bool_stats[stat.second]);
					break;
				}
				case eStatType::UINT8:
				{
					if (m_uint8_stats.contains(stat.second))
						stat.first->SetUint8Data(m_uint8_stats[stat.second]);
					break;
				}
				case eStatType::UINT16:
				{
					if (m_uint16_stats.contains(stat.second))
						stat.first->SetUint16Data(m_uint16_stats[stat.second]);
					break;
				}
				case eStatType::UINT32:
				{
					if (m_uint32_stats.contains(stat.second))
						stat.first->SetUint32Data(m_uint32_stats[stat.second]);
					break;
				}
				case eStatType::UINT64:
				{
					if (m_uint64_stats.contains(stat.second))
						stat.first->SetUint64Data(m_uint64_stats[stat.second]);
					break;
				}
				case eStatType::INT64:
				{
					if (m_int64_stats.contains(stat.second))
						stat.first->SetInt64Data(m_int64_stats[stat.second]);
					break;
				}
				case eStatType::DATE:
				{
					if (m_date_stats.contains(stat.second))
						stat.first->SetUint64Data(m_date_stats[stat.second]);
					break;
				}
				case eStatType::POS:
				{
					if (m_pos_stats.contains(stat.second))
						stat.first->SetUint64Data(m_pos_stats[stat.second]);
					break;
				}
				case eStatType::TEXTLABEL:
				{
					if (m_textlabel_stats.contains(stat.second))
						stat.first->SetIntData(m_textlabel_stats[stat.second]);
					break;
				}
				case eStatType::PACKED:
				{
					if (m_packed_stats.contains(stat.second))
						stat.first->SetUint64Data(m_packed_stats[stat.second]);
					break;
				}
				case eStatType::USERID:
				{
					if (m_userid_stats.contains(stat.second))
						stat.first->SetUint64Data(m_userid_stats[stat.second]);
					break;
				}
				case eStatType::PROFILE_SETTING: break;
				default:
				{
					LOGF(WARNING, "Unknown stat type: {} In stat: {}", (int)stat.first->GetTypeId(), stat.second);
					break;
				}
				}

				// Fixes the character creator always activating.
				if(stat.second == RAGE_JOAAT("MP0_CHAR_IS_NGPC_VERSION")
					|| stat.second == RAGE_JOAAT("MP1_CHAR_IS_NGPC_VERSION"))
				{
					stat.first->SetIntData(1);
				}
			}
			result = true;
		}
		catch (const std::exception& ex)
		{
			LOG(WARNING) << "Detected corrupt save file: " << ex.what();
		}
		file.close();

		return result;
	}
}