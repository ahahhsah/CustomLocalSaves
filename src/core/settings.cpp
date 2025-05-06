#include "settings.hpp"

#include "thread_pool.hpp"

namespace big
{
	void menu_settings::destroy()
	{
		m_running = false;
	}

	void menu_settings::init(const file& save_file)
	{
		m_running   = true;
		m_save_file = save_file;
		load();

	}
	bool menu_settings::load()
	{
		m_default_options = *this;

		std::ifstream file(m_save_file.get_path());

		if (!m_save_file.exists())
		{
			write_default_config();

			file.open(m_save_file.get_path());
		}

		try
		{
			file >> m_options;

			file.close();
		}
		catch (const std::exception&)
		{
			file.close();

			LOG(WARNING) << "Detected corrupt settings, writing default config...";

			write_default_config();

			return load();
		}

		try
		{
			from_json(m_options, *this);
		}
		catch (const nlohmann::detail::type_error& e)
		{
			LOG(WARNING) << e.what();

			return false;
		}

		return true;
	}

	bool menu_settings::write_default_config()
	{
		std::ofstream file(m_save_file.get_path(), std::ios::out | std::ios::trunc);
		file << m_default_options.dump(4);
		file.close();

		return true;
	}
}