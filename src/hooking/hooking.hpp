#pragma once
#include "common.hpp"
#include "detour_hook.hpp"
#include "gta/fwddec.hpp"
#include "vmt_hook.hpp"

#include <MinHook.h>

namespace big
{
	struct hooks
	{
		static bool run_script_threads(std::uint32_t ops_to_execute);
		static bool init_native_tables(rage::scrProgram* program);

		static uint32_t network_can_access_multiplayer(uint32_t a1, uint64_t* a2);
		static bool get_stat_flag_bool(void* p0, const char* flag, bool default_, bool p4);
	};

	class minhook_keepalive
	{
	public:
		minhook_keepalive()
		{
			MH_Initialize();
		}
		~minhook_keepalive()
		{
			MH_Uninitialize();
		}
	};

	class hooking
	{
		friend hooks;

	public:
		explicit hooking();
		~hooking();

		void enable();
		void disable();

		class detour_hook_helper
		{
			friend hooking;

			using ret_ptr_fn = std::function<void*()>;

			ret_ptr_fn m_on_hooking_available = nullptr;

			detour_hook* m_detour_hook = nullptr;

			~detour_hook_helper();

			void enable_hook_if_hooking_is_already_running();

			template<auto detour_function>
			struct hook_to_detour_hook_helper
			{
				static inline detour_hook* m_detour_hook;
			};

			template<auto detour_function>
			static detour_hook_helper* add_internal(detour_hook* dh)
			{
				auto d           = new detour_hook_helper();
				d->m_detour_hook = dh;

				m_detour_hook_helpers.push_back(d);
				hook_to_detour_hook_helper<detour_function>::m_detour_hook = dh;

				return d;
			}

		public:
			template<auto detour_function>
			static void add(const std::string& name, void* target)
			{
				auto d = add_internal<detour_function>(new detour_hook(name, target, (void*)detour_function));

				d->enable_hook_if_hooking_is_already_running();
			}

			template<auto detour_function>
			static void* add_lazy(const std::string& name, detour_hook_helper::ret_ptr_fn on_hooking_available)
			{
				auto d                    = add_internal<detour_function>(new detour_hook(name, detour_function));
				d->m_on_hooking_available = on_hooking_available;

				d->enable_hook_if_hooking_is_already_running();

				return nullptr;
			}
		};

		template<auto detour_function>
		static auto get_original()
		{
			return detour_hook_helper::hook_to_detour_hook_helper<detour_function>::m_detour_hook->template get_original<decltype(detour_function)>();
		}

	private:
		bool m_enabled{};
		minhook_keepalive m_minhook_keepalive;

#ifdef ENABLE_GUI
		vmt_hook m_swapchain_hook;

		WNDPROC m_og_wndproc = nullptr;
#endif

		static inline std::vector<detour_hook_helper*> m_detour_hook_helpers;
	};

	inline hooking* g_hooking{};
}