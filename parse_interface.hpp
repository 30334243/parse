#ifndef PARSE_SCRIPT_INTERFACE_HPP
#define PARSE_SCRIPT_INTERFACE_HPP

#include <string>
#include <vector>
#include <functional>
#include <cstdint>

namespace Parse {
	// CONSTANTS
    static std::string const kExec{"exec("};
	static std::string const kExecEnd{");"};
	static std::string const kFilterMask{",["};
    static std::string const kHex{"0x"};
	static std::string const kBin{"0b"};
	static std::string const kInsert{"insert{"};
	static size_t const kMaxChar{(size_t)0xFF};
	static size_t const kMaxShort{(size_t)0xFFFF};
	static size_t const kMaxInt{(size_t)0xFFFFFFFF};
	static size_t const kMaxLong{(size_t)0xFFFFFFFFFFFFFFFF};
	static uint8_t const kHexBase{16};
	static uint8_t const kBinBase{2};
	static uint8_t const kDecBase{10};
	std::vector<std::string> const kNameFuncs{
		"crop{","cropb{",
			"shr{","shrb{",
			"shl{","shlb{",
			"and{","!and{",
			"split{","!split{",
			"filter{","!filter{",
			"eq{[","!eq{[",
			"insert{[",
			"replace{[",
	};
	std::vector<std::string> const kEditableFuncs{
		"crop{","cropb{",
			"shrb{",
			"shlb{",
	};
	// USING
	using ItStr = std::string::iterator;
	using Func = std::function<void(std::string const&,ItStr&,ItStr&,uint8_t&)>;
};
#endif 
