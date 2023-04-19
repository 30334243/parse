#ifndef PARSE_HPP
#define PARSE_HPP

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>
#include <iterator>
#include <list>
#include <cmath>
#include <algorithm>
#include <iostream>
#include "../shit_msvc13/shit_msvc13.hpp"

// PARSE
namespace Parse {
	// CONSTANTS
	static std::string const kExec{"exec("};
	static std::string const kExecEnd{");"};
	static std::string const kFilterMask{",["};
	static std::string const kIsBits{"bits"};
	static std::string const kHex{"0x"};
	static std::string const kBin{"0b"};
	static size_t const kMaxChar{(size_t)(int8_t)-1};
	static size_t const kMaxShort{(size_t)(int16_t)-1};
	static size_t const kMaxInt{(size_t)(int32_t)-1};
	static size_t const kMaxLong{(size_t)(uint64_t)-1};
	static uint8_t const kHexBase{16};
	static uint8_t const kBinBase{2};
	static uint8_t const kDecBase{10};
	static uint64_t lid{};
	static uint8_t offset{};
	std::vector<std::string> const kNameFuncs{
		"crop{","shr{","shl{","mask{","filter{","eq{[","!mask{","!filter{","!eq{["
	};
	// MEMBERS
	static std::vector<std::function<bool(uint8_t**,uint8_t**)>> funcs{};
	// USING
	using ItStr = std::string::iterator;
	using Poly = std::tuple<uint8_t,uint16_t,uint32_t,uint64_t>;
	using PolyVec = std::tuple<std::vector<uint8_t>,std::vector<uint16_t>,std::vector<uint32_t>,std::vector<uint64_t>>;
	using ItPair = std::pair<ItStr,ItStr>;
	// GET BASE
	static uint8_t GetBase(ItStr& beg, ItStr& end) {
		uint8_t ret{};
		if (std::search(beg, beg+2, kHex.cbegin(), kHex.cend()) != beg+2) {
			ret = kHexBase;
		} else if (std::search(beg, beg+2, kBin.cbegin(), kBin.cend()) != beg+2) {
			ret = kBinBase;
		} else {
			ret = kDecBase;
		}
		return ret;
	}
	// CONVERT
	static uint64_t Convert(ItStr& beg, ItStr& end) {
		uint8_t const base{GetBase(beg, end)};
		beg += base == kDecBase ? 0 : 2;
		uint64_t const sz{std::stoul(std::string(beg,end), nullptr, base)};
		return sz;
	}
	// SPLIT
	static std::vector<uint64_t> Split(ItStr& beg, ItStr& end, char const term) {
		std::vector<uint64_t> ret{};
		auto it = std::find(beg, end, term);
		while (it < end) {
			ret.emplace_back(Convert(beg, it));
			beg = ++it;
			it = std::find(it, end, term);
		}
		ret.emplace_back(Convert(beg, it));
		return ret;
	}
	// SET MASK
	static void SetMask(uint64_t const sz, std::list<Shit::Func>& funcs) {
		funcs.emplace_back(Shit::Check::OutOfRange());
		if (sz < kMaxChar) {
			funcs.emplace_back(Shit::Mask((uint8_t)sz,
													Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < kMaxShort) {
			funcs.emplace_back(Shit::Mask((uint16_t)sz,
													Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < kMaxInt) {
			funcs.emplace_back(Shit::Mask((uint32_t)sz,
													Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < kMaxLong) {
			funcs.emplace_back(Shit::Mask((uint64_t)sz,
													Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// SET MASK NOT
	static void SetMaskNot(uint64_t const sz, std::list<Shit::Func>& funcs) {
		funcs.emplace_back(Shit::Check::OutOfRange());
		if (sz < kMaxChar) {
			funcs.emplace_back(Shit::MaskNot((uint8_t)sz,
														Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < kMaxShort) {
			funcs.emplace_back(Shit::MaskNot((uint16_t)sz,
														Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < kMaxInt) {
			funcs.emplace_back(Shit::MaskNot((uint32_t)sz,
														Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < kMaxLong) {
			funcs.emplace_back(Shit::MaskNot((uint64_t)sz,
														Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// SET FILTER
	static void SetFilter(uint64_t const sz, std::vector<uint64_t> const& args, std::list<Shit::Func>& funcs) {
		funcs.emplace_back(Shit::Check::OutOfRange());
		if (sz < kMaxChar) {
			funcs.emplace_back(Shit::Filter<uint8_t>(sz,
																  args,
																  Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < kMaxShort) {
			funcs.emplace_back(Shit::Filter<uint16_t>(sz,
																	args,
																	Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < kMaxInt) {
			funcs.emplace_back(Shit::Filter<uint32_t>(sz,
																	args,
																	Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < kMaxLong) {
			funcs.emplace_back(Shit::Filter<uint64_t>(sz,
																	args,
																	Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// SET FILTER NOT
	static void SetFilterNot(uint64_t const sz, std::vector<uint64_t> const& args, std::list<Shit::Func>& funcs) {
		funcs.emplace_back(Shit::Check::OutOfRange());
		if (sz < kMaxChar) {
			funcs.emplace_back(Shit::FilterNot<uint8_t>(sz,
																	  args,
																	  Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < kMaxShort) {
			funcs.emplace_back(Shit::FilterNot<uint16_t>(sz,
																		args,
																		Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < kMaxInt) {
			funcs.emplace_back(Shit::FilterNot<uint32_t>(sz,
																		args,
																		Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < kMaxLong) {
			funcs.emplace_back(Shit::FilterNot<uint64_t>(sz,
																		args,
																		Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// SET EQUAL
	static void SetEq(std::vector<uint64_t> const& args, std::list<Shit::Func>& funcs) {
		funcs.emplace_back(Shit::Check::OutOfRange());
		size_t const sz{args[0]};
		if (sz < kMaxChar) {
			funcs.emplace_back(Shit::Eq<uint8_t>(args,
															 Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < kMaxShort) {
			funcs.emplace_back(Shit::Eq<uint16_t>(args,
															  Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < kMaxInt) {
			funcs.emplace_back(Shit::Eq<uint32_t>(args,
															  Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < kMaxLong) {
			funcs.emplace_back(Shit::Eq<uint64_t>(args,
															  Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// SET EQUAL NOT
	static void SetEqNot(std::vector<uint64_t> const& args, std::list<Shit::Func>& funcs) {
		funcs.emplace_back(Shit::Check::OutOfRange());
		size_t const sz{args[0]};
		if (sz < kMaxChar) {
			funcs.emplace_back(Shit::EqNot<uint8_t>(args,
																 Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < kMaxShort) {
			funcs.emplace_back(Shit::EqNot<uint16_t>(args,
																  Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < kMaxInt) {
			funcs.emplace_back(Shit::EqNot<uint32_t>(args,
																  Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < kMaxLong) {
			funcs.emplace_back(Shit::EqNot<uint64_t>(args,
																  Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// MASK
	static auto Mask(std::list<Shit::Func>& funcs) -> std::function<void(std::string const&,ItStr&,ItStr&)> {
		return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
			if (name == "mask{" && beg < end) {
				if (beg != end) {
					if (std::count(beg, end, ',') == 0) {
						uint64_t const sz{Convert(beg, end)};
						SetMask(sz, funcs);
						beg += std::distance(beg, end);
					} else {
						std::cout << "command mask invalid: " + std::string(beg, end) << std::endl;
					}
				}
			}
		};
	}
	// MASK NOT
	static auto MaskNot(std::list<Shit::Func>& funcs) -> std::function<void(std::string const&,ItStr&,ItStr&)> {
		return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
			if (name == "!mask{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					uint64_t const sz{Convert(beg, end)};
					SetMaskNot(sz, funcs);
					beg += std::distance(beg, end);
				} else {
					std::cout << "command MaskNot invalid: " + std::string(beg, end) << std::endl;
				}
			}
		};
	}
	// GET FILTER MASK
	static uint64_t GetFilterMask(ItStr& beg, ItStr& end) {
		uint64_t ret{};
		auto it = std::search(beg, end, kFilterMask.begin(), kFilterMask.end());
		if (it < end) {
			ret = Convert(beg, it);
			beg = it + kFilterMask.size();
		} else {
			std::cout << "filter arg mask invalid" << std::string(beg,end) << std::endl;
			beg = end;
		}
		return ret;
	}
	// FILTER ARGUMENT
	static std::vector<uint64_t> FilterArgs(ItStr& beg, ItStr& end) {
		std::vector<uint64_t> ret{};
		auto it = std::find(beg, end, ']');
		if (it < end && beg != it && it[1] == '}') {
			ret = Split(beg, it, ',');
			beg = it+1;
		} else {
			std::cout << "filter args invalid: " << std::string(beg,end) << std::endl;
			beg = end;
		}
		return ret;
	}
	// FILTER
	static void Filter(ItStr& beg, ItStr& end, std::list<Shit::Func>& funcs) {
		uint64_t const mask{GetFilterMask(beg, end)};
		auto vec = FilterArgs(beg,end);
		SetFilter(mask, vec, funcs);
	}
	// FILTER
	static auto Filter(std::list<Shit::Func>& funcs) ->
		std::function<void(std::string const&,ItStr&,ItStr&)> {
			return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
				if (name == "filter{" && beg < end) {
					Filter(beg, end, funcs);
				}
			};
		}
	// FILTER NOT
	static void FilterNot(ItStr& beg, ItStr& end, std::list<Shit::Func>& funcs) {
		uint64_t const mask{GetFilterMask(beg, end)};
		auto vec = FilterArgs(beg,end);
		SetFilterNot(mask, vec, funcs);
	}
	// FILTER NOT
	static auto FilterNot(std::list<Shit::Func>& funcs) ->
		std::function<void(std::string const&,ItStr&,ItStr&)> {
			return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
				if (name == "!filter{" && beg < end) {
					FilterNot(beg, end, funcs);
				}
			};
		}
	// EQUAL ARGUMENT
	static std::vector<uint64_t> EqArgs(ItStr& beg, ItStr& end) {
		std::vector<uint64_t> ret{};
		auto it = std::find(beg, end, ']');
		if (it < end && beg != it && it[1] == '}') {
			ret = Split(beg, it, ',');
			beg = it+1;
		} else {
			std::cout << "filter args invalid: " << std::string(beg,end) << std::endl;
			beg = end;
		}
		return ret;
	}
	// EQUAL
	static std::list<Shit::Func> Eq(ItStr& beg, ItStr& end, std::list<Shit::Func>& funcs) {
		std::list<Shit::Func> ret{};
		auto vec = EqArgs(beg, end);
		SetEq(vec, funcs);
		return ret;
	}
	// EQUAL
	static auto Eq(std::list<Shit::Func>& funcs) ->
		std::function<void(std::string const&,ItStr&,ItStr&)> {
			return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
				if (name == "eq{[" && beg < end) {
					funcs.splice(funcs.cend(), Eq(beg, end, funcs));
					beg += std::distance(beg, end);
				}
			};
		}
	// EQUAL NOT
	static std::list<Shit::Func> EqNot(ItStr& beg, ItStr& end, std::list<Shit::Func>& funcs) {
		std::list<Shit::Func> ret{};
		auto vec = EqArgs(beg, end);
		SetEqNot(vec, funcs);
		return ret;
	}
	// EQUAL NOT
	static auto EqNot(std::list<Shit::Func>& funcs) ->
		std::function<void(std::string const&,ItStr&,ItStr&)> {
			return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
				if (name == "!eq{[" && beg < end) {
					funcs.splice(funcs.cend(), EqNot(beg, end, funcs));
					beg += std::distance(beg, end);
				}
			};
		}
	// SHIFT LEFT IN BITS
	static std::list<Shit::Func> ShlInBits(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		size_t const arg{Convert(beg, end)};
		ret.emplace_back(Shit::Check::OutOfRangeLeft(arg));
		ret.emplace_back(Shit::ShlInBits(arg));
		return ret;
	}
	// SHIFT LEFT
	static std::list<Shit::Func> Shl(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		size_t const arg{Convert(beg, end)};
		ret.emplace_back(Shit::Check::OutOfRangeLeft(arg));
		ret.emplace_back(Shit::Shl(arg));
		return ret;
	}
	// SHIFT LEFT
	static auto Shl(std::list<Shit::Func>& funcs) ->
		std::function<void(std::string const&,ItStr&,ItStr&)> {
			return [&funcs] (std::string const& name,ItStr& beg, ItStr& end) {
				if (name == "shl{" && beg < end) {
					auto is_bits = std::search(beg, end, kIsBits.begin(), kIsBits.end());
					if (std::count(beg, end, ',') == 0) {
						funcs.splice(funcs.cend(), Shl(beg, end));
						beg += std::distance(beg, end);
					} else if (is_bits != end) {
						funcs.splice(funcs.cend(), ShlInBits(beg, is_bits));
						beg += std::distance(beg, end);
					} else {
						std::cout << "command \"shl\" invalid: " << std::string(beg,end) << std::endl;
					}
				} 
			};
		}
	// SHIFT RIGHT IN BITS
	static std::list<Shit::Func> ShrInBits(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		size_t const arg{Convert(beg, end)};
		ret.emplace_back(Shit::Check::OutOfRangeRight(arg));
		ret.emplace_back(Shit::ShrInBits(arg));
		return ret;
	}
	// SHIFT RIGHT
	static std::list<Shit::Func> Shr(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		size_t const arg{Convert(beg, end)};
		ret.emplace_back(Shit::Check::OutOfRangeRight(arg));
		ret.emplace_back(Shit::Shr(arg));
		return ret;
	}
	// SHIFT RIGHT
	static auto Shr(std::list<Shit::Func>& funcs) ->
		std::function<void(std::string const&,ItStr&,ItStr&)> {
			return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
				if (name == "shr{" && beg < end) {
					auto is_bits = std::search(beg, end, kIsBits.begin(), kIsBits.end());
					if (std::count(beg, end, ',') == 0 && is_bits == end) {
						funcs.splice(funcs.cend(), Shr(beg, end));
						beg += std::distance(beg, end);
					} else if (is_bits != end) {
						funcs.splice(funcs.cend(), ShrInBits(beg, is_bits));
						beg += std::distance(beg, end);
					} else {
						std::cout << "command \"shr\" invalid: " << std::string(beg,end) << std::endl;
					}
				}
			};
		}
	// CROP ARGUMENTS
	static std::tuple<size_t,size_t> CropArgs(ItStr& beg, ItStr& end) {
		std::tuple<size_t,size_t> ret{};
		auto first = std::find(beg, end, ',');
		size_t const arg_beg{Convert(beg, first)};
		++first;
		size_t const arg_end{Convert(first, end)};
		ret = std::make_tuple(arg_beg, arg_end);
		return ret;	
	}
	// CROP
	static std::list<Shit::Func> Crop(ItStr& beg, ItStr& end) {
		std::list<Shit::Func> ret{};
		auto args = CropArgs(beg, end);
		ret.emplace_back(Shit::Check::OutOfRange(std::get<0>(args), std::get<1>(args)));
		ret.emplace_back(Shit::Crop(std::get<0>(args), std::get<1>(args)));
		return ret;
	}
	// CROP
	static auto Crop(std::list<Shit::Func>& funcs) ->
		std::function<void(std::string const&,ItStr&,ItStr&)> {
			return [&funcs] (std::string const& name, ItStr& beg, ItStr& end) {
				if (name == "crop{" && beg < end) {
					if (std::count(beg, end, ',') == 1) {
						funcs.splice(funcs.cend(), Crop(beg, end));
						beg += std::distance(beg, end);
					} else {
						std::cout << "command \"crop\" invalid: " << std::string(beg,end) << std::endl;
					}
				}
			};
		}
	// GET COMMAND
	std::string GetCommand(std::string const& name, ItStr& beg, ItStr& end) {
		std::string ret{};
		auto it = std::search(beg, end,
									 name.begin(), name.end());
		if (it < end && it == beg) {
			beg = it;
			ret = std::string(beg, beg + name.size());
		}
		return ret;
	}
	// FOR TUPLE
	template <size_t I = 0, typename... Ts>
		typename std::enable_if<I == sizeof...(Ts), void>::type
		for_tuple(std::tuple<Ts...>& tup, std::string const& name, ItStr& beg, ItStr& end) {
			return;
		}

	template <size_t I = 0, typename... Ts>
		typename std::enable_if<(I < sizeof...(Ts)), void>::type
		for_tuple(std::tuple<Ts...>& tup, std::string const& name, ItStr& beg, ItStr& end) {
			std::get<I>(tup)(name, beg, end);
			for_tuple<I + 1>(tup, name, beg, end);
		}
	// COMMANDS
	template<class... ArgsTuple>
		static auto Commands(ArgsTuple... args_tuple) ->
		std::function<void(ItStr&,ItStr&)> {
			return [args_tuple...] (ItStr& beg, ItStr& end) {
				std::tuple<ArgsTuple...> ts{args_tuple...};
				while (beg < end) {
					std::string command{};
					for (auto const& name : kNameFuncs) {
						command = GetCommand(name, beg, end);
						if (!command.empty()) {
							beg += name.size();
							auto end_arg = std::find(beg, end, '}');
							for_tuple(ts, command, beg, end_arg);
							beg += 2;
							break;
						}
					}
					if (command.empty()) {
						std::cout << "unknown command: " << std::string(beg,end) << std::endl;
						beg = end;
					}
				}
			};
		}
	// REMOVE UNUSED CHARACTERS
	void RemoveUnusedChars(std::string& str) {
		auto rem = [] (char const c) {
			return c==' '||c=='\n'||c=='\r'||c=='\t'||c=='_'||c=='\'';
		};
		str.erase(std::remove_if(str.begin(), str.end(), rem), str.end());
	}
	// EXECUTION
	std::list<Shit::Func> Exec(std::string str) {
		std::list<Shit::Func> ret{};
		RemoveUnusedChars(str);
		auto commands = Commands(Crop(ret),
										 Shr(ret),
										 Shl(ret),
										 Eq(ret),
										 EqNot(ret),
										 Filter(ret),
										 FilterNot(ret),
										 Mask(ret),
										 MaskNot(ret));
		auto exec = std::search(str.begin(), str.end(),
										kExec.begin(), kExec.end());
		if (exec != str.end()) {
			auto beg = exec + kExec.size();
			auto end = std::search(beg, str.end(),
										  kExecEnd.begin(), kExecEnd.end());
			while (beg < end) {
				commands(beg, end);
			}
		}
		return ret;
	}
}

#endif
