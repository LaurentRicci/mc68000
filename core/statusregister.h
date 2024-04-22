#pragma once
#include <cassert>
#include <cstdint>
namespace mc68000
{
	struct StatusRegister
	{
		uint16_t t : 1;
		uint16_t s : 1;
		uint16_t i : 3;
		uint8_t x : 1;
		uint8_t n : 1;
		uint8_t z : 1;
		uint8_t v : 1;
		uint8_t c : 1;
		StatusRegister()
		{
			t = s = i = 0;
			c = v = z = n = x = 0;
		}
		StatusRegister& operator=(int8_t ccr)
		{
			c = (ccr & 0x01);
			v = (ccr & 0x02) >> 1;
			z = (ccr & 0x04) >> 2;
			n = (ccr & 0x08) >> 3;
			x = (ccr & 0x10) >> 4;
			return *this;
		}
		StatusRegister& operator=(int16_t sr)
		{
			c = sr & 0x1;
			v = (sr & 0x0002) >> 1;
			z = (sr & 0x0004) >> 2;
			n = (sr & 0x0008) >> 3;
			x = (sr & 0x0010) >> 4;
			i = (sr & 0x0700) >> 8;
			s = (sr & 0x2000) >> 13;
			t = (sr & 0x8000) >> 15;
			return *this;
		}

		operator uint8_t() const
		{
			uint8_t ccr = (x << 4) | (n << 3) | (z << 2) | (v << 1) | c;
			return ccr;
		}

		operator uint16_t() const
		{
			uint16_t sr = (t << 15) | (s << 13) | (i << 8) | (x << 4) | (n << 3) | (z << 2) | (v << 1) | c;
			return sr;
		}

		// For reference: https://en.wikibooks.org/wiki/68000_Assembly/Conditional_Tests
		bool cc() const { return !c; }
		bool cs() const { return c; }
		bool eq() const { return z; }
		bool ne() const { return !z; }
		bool ge() const { return (n && v) || (!n && !v); }
		bool gt() const { return (n & v & !z) | (!n & !v & !z); }
		bool hi() const { return !c && !z; }
		bool le() const { return (z | n & !v | !n & v); }
		bool ls() const { return c | z; }
		bool lt() const { return (n & v) | (!n & v); }
		bool mi() const { return n; }
		bool pl() const { return !n; }
		bool vc() const { return !v; }
		bool vs() const { return v; }

		bool condition(uint16_t conditionCode) const
		{
			switch (conditionCode)
			{
				case 0: return true;
				case 1: return false;
				case 4: return cc();
				case 5: return cs();
				case 7: return eq();
				case 6: return ne();
				case 12: return ge();
				case 14: return gt();
				case 2: return hi();
				case 15: return le();
				case 3: return ls();
				case 13: return lt();
				case 11: return mi();
				case 10: return pl();
				case 8: return  vc();
				case 9: return vs();
				default: assert(!"invalid condition");
			}
			return false;
		}
	};
}