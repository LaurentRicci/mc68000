namespace mc68000
{
	bool isValidAddressingMode(unsigned short ea, unsigned short acceptable)
	{
		static const unsigned masks[] = {
			1 << 11, 1 << 10, 1 << 9, 1 << 8, 1 << 7, 1 << 6, 1 << 5, // mode 000 to 110
			1 << 4, 1 << 3, 1 << 2, 1 << 1, 1                         // mode 111 
		};
		unsigned mode = ea >> 3;
		if (mode != 7)
		{
			return (acceptable & masks[mode]) == masks[mode];
		}

		static const unsigned offsets[] = { 7, 8, 10, 11, 9 }; // 000, 001, 100, 010, 011
		mode = ea & 7;
		if (mode > 4)
		{
			return false;
		}
		auto offset = offsets[mode];
		return (acceptable & masks[offset]) == masks[offset];
	}

}