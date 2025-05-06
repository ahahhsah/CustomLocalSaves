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

		void attempt_save();
		bool load();

	private:
		bool deep_compare(nlohmann::json& current_settings, const nlohmann::json& default_settings, bool compare_value = false);
		bool save();
		bool write_default_config();

	private:
		bool m_running;

		file m_save_file;

		nlohmann::json m_default_options;
		nlohmann::json m_options;

	public:
		bool save_unmodified_stats = false;
		bool use_human_readable_stat_names = false;

		NLOHMANN_DEFINE_TYPE_INTRUSIVE(menu_settings, save_unmodified_stats, use_human_readable_stat_names)
	};

	inline auto g = menu_settings();
}
