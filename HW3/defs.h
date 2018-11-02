#pragma once

//definistions of necessary macros

#define OFFSET_LENGTH	12
#define TABLE_LENGTH	10
#define DIR_LENGTH		10

#define OFFSET_MASK		((1 << OFFSET_LENGTH) - 1)
#define TABLE_MASK		(((1 << TABLE_LENGTH) - 1) << OFFSET_LENGTH)
#define DIR_MASK		(((1 << DIR_LENGTH) - 1) << (TABLE_LENGTH + OFFSET_LENGTH))
#define VPN_MASK		(((1 << (DIR_LENGTH + TABLE_LENGTH)) - 1) << (OFFSET_LENGTH))

#define OFFSET(x)		(x & OFFSET_MASK)
#define VPN(x)			((x & VPN_MASK) >> OFFSET_LENGTH)