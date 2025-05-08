#include "hooking/hooking.hpp"

namespace big
{
	bool hooks::construct_basket(void* _this, int* transaction_id, int category_type, int action_type, int target)
	{
		LOGF(WARNING, "Tried to create basket ({}, {}, {}, {}, {}); This should never happen.", _this, *transaction_id, category_type, action_type, target);
		return false;
	}
}
