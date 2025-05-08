#pragma once
#include "file_manager.hpp"

#include <bitset>
#include <cstdint>
#include <string>

#ifdef ENABLE_GUI
#define IMGUI_DEFINE_MATH_OPERATORS
#include <imgui.h>
#endif

class CNetGamePlayer;

namespace big
{
	class menu_settings
	{
	public:
		void destroy();
		void init(const file& save_file);

		bool load();

	private:
		bool write_default_config();
		bool deep_compare(nlohmann::json& current_settings, const nlohmann::json& default_settings, bool compare_value = false);
		bool save();

	private:
		bool m_running;

		file m_save_file;

		nlohmann::json m_default_options;
		nlohmann::json m_options;

	public:
		bool save_unmodified_stats = false;
		bool use_human_readable_stat_names = false;
		bool always_load_into_character_creator = false;
		bool load_fsl_files = false;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(menu_settings, save_unmodified_stats, use_human_readable_stat_names, always_load_into_character_creator, load_fsl_files)
	};

	inline auto g = menu_settings();
}
