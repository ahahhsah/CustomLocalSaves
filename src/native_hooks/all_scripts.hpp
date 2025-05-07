/**
 * @file all_scripts.hpp
 * 
 * @copyright GNU General Public License Version 2.
 * This file is part of YimMenu.
 * YimMenu is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 2 of the License, or (at your option) any later version.
 * YimMenu is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License along with YimMenu. If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once
#include "fiber_pool.hpp"
#include "gta/enums.hpp"
#include "gta/stat.hpp"
#include "hooking/hooking.hpp"
#include "native_hooks.hpp"
#include "natives.hpp"
#include "services/stats/stats_service.hpp"
#include "util/scripts.hpp"

namespace big
{
	namespace all_scripts
	{
		inline void UGC_SET_USING_OFFLINE_CONTENT(rage::scrNativeCallContext* src)
		{
			NETWORK::UGC_SET_USING_OFFLINE_CONTENT(TRUE);
		}
		inline void UGC_SET_QUERY_DATA_FROM_OFFLINE(rage::scrNativeCallContext* src)
		{
			NETWORK::UGC_SET_USING_OFFLINE_CONTENT(TRUE);
		}

		inline void NET_GAMESERVER_RETRIEVE_INIT_SESSION_STATUS(rage::scrNativeCallContext* src)
		{
			*src->get_arg<int*>(0) = 3;
			src->set_return_value<BOOL>(TRUE);
		}


		inline void WITHDRAW_VC(rage::scrNativeCallContext* src)
		{
			int amount = src->get_arg<int>(0);
			int last_character = g_stats_service->get_stat_by_hash(RAGE_JOAAT("MPPLY_LAST_MP_CHAR"))->m_stat->GetIntData();
			sStatData* BANK_BALANCE = g_stats_service->get_stat_by_hash(RAGE_JOAAT("BANK_BALANCE"))->m_stat;
			uint64_t current_bank_balance = BANK_BALANCE->GetInt64Data();
			sStatData* WALLET_BALANCE = g_stats_service->get_stat_by_hash(rage::joaat(std::format("MP{}_WALLET_BALANCE", last_character)))->m_stat;
			uint64_t current_wallet_balance = WALLET_BALANCE->GetInt64Data();
			if(WALLET_BALANCE == nullptr)
			{
				LOG(FATAL) << "WITHDRAW_VC: Failed to find WALLET_BALANCE stat";
				src->set_return_value<int>(0);
				return;
			}

			if(current_bank_balance < amount || amount < 0)
			{
				src->set_return_value<int>(0);
				return;
			}

			BANK_BALANCE->SetInt64Data(current_bank_balance - amount);
			WALLET_BALANCE->SetInt64Data(current_wallet_balance + amount);
			src->set_return_value(amount);
		}

		inline void DEPOSIT_VC(rage::scrNativeCallContext* src)
		{
			int amount = src->get_arg<int>(0);
			int last_character = g_stats_service->get_stat_by_hash(RAGE_JOAAT("MPPLY_LAST_MP_CHAR"))->m_stat->GetIntData();
			sStatData* BANK_BALANCE = g_stats_service->get_stat_by_hash(RAGE_JOAAT("BANK_BALANCE"))->m_stat;
			uint64_t current_bank_balance = BANK_BALANCE->GetInt64Data();
			sStatData* WALLET_BALANCE = g_stats_service->get_stat_by_hash(rage::joaat(std::format("MP{}_WALLET_BALANCE", last_character)))->m_stat;
			uint64_t current_wallet_balance = WALLET_BALANCE->GetInt64Data();
			if(WALLET_BALANCE == nullptr)
			{
				LOG(FATAL) << "DEPOSIT_VC: Failed to find WALLET_BALANCE stat";
				src->set_return_value<BOOL>(FALSE);
				return;
			}

			if(current_bank_balance < amount || amount < 0)
			{
				src->set_return_value<BOOL>(FALSE);
				return;
			}

			BANK_BALANCE->SetInt64Data(current_bank_balance + amount);
			WALLET_BALANCE->SetInt64Data(current_wallet_balance - amount);
			src->set_return_value<BOOL>(TRUE);
		}

		void RETURN_TRUE(rage::scrNativeCallContext* src)
		{
			src->set_return_value<BOOL>(TRUE);
		}

		void RETURN_FALSE(rage::scrNativeCallContext* src)
		{
			src->set_return_value<BOOL>(FALSE);
		}

		void DO_NOTHING(rage::scrNativeCallContext* src)
		{
		}
	}
}