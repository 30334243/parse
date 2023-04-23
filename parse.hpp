#ifndef PARSE_SCRIPT_HPP
#define PARSE_SCRIPT_HPP

#include <algorithm>
#include <list>

#include "parse_script_interface.hpp"
#include "../shit_msvc13/shit_msvc13.hpp"

namespace Parse {
	namespace Utils {
		// REMOVE UNUSED CHARACTERS
		void RemoveUnusedChars(std::string& str) {
			auto rem = [] (char const c) {
				return c==' '||c=='\n'||c=='\r'||c=='\t'||c=='_'||c=='\'';
			};
			str.erase(std::remove_if(str.begin(), str.end(), rem), str.end());
		}
		// FIND INSERT FUNC
		static bool FindInsertFunc(std::string const& script) {
			return std::search(script.cbegin(), script.cend(),
									 kInsert.cbegin(), kInsert.cend()) != script.cend();
		}
		// FIND EDITABLE FUNCTIONS
		static bool FindEditableFuncs(std::string const& script) {
			bool ret{};
			for (auto& elm : kEditableFuncs) {
				auto it = std::search(script.begin(), script.end(),
											 elm.begin(), elm.end());
				if (it != script.end()) {
					ret = true;
					break;
				}
			}
			return ret;
		}
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
		template<class T>
		static std::vector<T> Split(ItStr& beg, ItStr& end, char const term) {
			std::vector<T> ret{};
			auto it = std::find(beg, end, term);
			while (it < end) {
				ret.emplace_back(Convert(beg, it));
				beg = ++it;
				it = std::find(it, end, term);
			}
			ret.emplace_back(Convert(beg, it));
			return ret;
		}
		// GET LIST ARGUMENT
		template<class T>
		static std::vector<T> GetListArgs(ItStr& beg, ItStr& end) {
			std::vector<T> ret{};
			auto it = std::find(beg, end, ']');
			if (it < end && beg != it && it[1] == '}') {
				ret = Parse::Utils::Split<T>(beg, it, ',');
				beg = it+1;
			} else {
				std::cout << "filter args invalid: " << std::string(beg,end) << std::endl;
				beg = end;
			}
			return ret;
		}
	}
}
// PARSE
struct Parsed {
	// USING
	using ItStr = std::string::iterator;
	using Func = std::function<void(std::string const&,ItStr&,ItStr&,uint8_t&)>;
	// MEMBERS
	std::vector<std::list<Shit::Func>> funcs;
	uint64_t insert_sz{};
	uint64_t lid{};
	uint8_t offset{};
	// MASK
	void Mask(uint64_t const sz) {
		this->funcs.back().emplace_back(Shit::Check::OutOfRange());
		if (sz < Parse::kMaxChar) {
			this->funcs.back().emplace_back(Shit::Mask((uint8_t)sz,
																	 Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < Parse::kMaxShort) {
			this->funcs.back().emplace_back(Shit::Mask((uint16_t)sz,
																	 Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < Parse::kMaxInt) {
			this->funcs.back().emplace_back(Shit::Mask((uint32_t)sz,
																	 Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < Parse::kMaxLong) {
			this->funcs.back().emplace_back(
				Shit::Mask((uint64_t)sz,
							  Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// MASK NOT
	void MaskNot(uint64_t const sz) {
		this->funcs.back().emplace_back(Shit::Check::OutOfRange());
		if (sz < Parse::kMaxChar) {
			this->funcs.back().emplace_back(
				Shit::MaskNot((uint8_t)sz,
								  Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < Parse::kMaxShort) {
			this->funcs.back().emplace_back(
				Shit::MaskNot((uint16_t)sz,
								  Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < Parse::kMaxInt) {
			this->funcs.back().emplace_back(
				Shit::MaskNot((uint32_t)sz,
								  Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < Parse::kMaxLong) {
			this->funcs.back().emplace_back(
				Shit::MaskNot((uint64_t)sz,
								  Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// INSERT
	void Insert(std::vector<uint8_t> const& args) {
		this->insert_sz += args.size();
		this->funcs.back().emplace_back(Shit::Check::OutOfRange());
		this->funcs.back().emplace_back(Shit::Insert(args.size(), args));
	}
	// FILTER
	void Filter(uint64_t const sz, std::vector<uint64_t> const& args) {
		this->funcs.back().emplace_back(Shit::Check::OutOfRange());
		if (sz < Parse::kMaxChar) {
			this->funcs.back().emplace_back(
				Shit::Filter<uint8_t>(sz,
											 args,
											 Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < Parse::kMaxShort) {
			this->funcs.back().emplace_back(
				Shit::Filter<uint16_t>(sz,
											  args,
											  Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < Parse::kMaxInt) {
			this->funcs.back().emplace_back(
				Shit::Filter<uint32_t>(sz,
											  args,
											  Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < Parse::kMaxLong) {
			this->funcs.back().emplace_back(
				Shit::Filter<uint64_t>(sz,
											  args,
											  Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// FILTER NOT
	void FilterNot(uint64_t const sz, std::vector<uint64_t> const& args) {
		this->funcs.back().emplace_back(Shit::Check::OutOfRange());
		if (sz < Parse::kMaxChar) {
			this->funcs.back().emplace_back(Shit::FilterNot<uint8_t>(sz,
																						args,
																						Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < Parse::kMaxShort) {
			this->funcs.back().emplace_back(Shit::FilterNot<uint16_t>(sz,
																						 args,
																						 Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < Parse::kMaxInt) {
			this->funcs.back().emplace_back(Shit::FilterNot<uint32_t>(sz,
																						 args,
																						 Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < Parse::kMaxLong) {
			this->funcs.back().emplace_back(Shit::FilterNot<uint64_t>(sz,
																						 args,
																						 Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// EQUAL
	void Eq(std::vector<uint64_t> const& args) {
		this->funcs.back().emplace_back(Shit::Check::OutOfRange());
		size_t const sz{args[0]};
		if (sz < Parse::kMaxChar) {
			this->funcs.back().emplace_back(Shit::Eq<uint8_t>(args,
																			  Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < Parse::kMaxShort) {
			this->funcs.back().emplace_back(Shit::Eq<uint16_t>(args,
																				Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < Parse::kMaxInt) {
			this->funcs.back().emplace_back(Shit::Eq<uint32_t>(args,
																				Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < Parse::kMaxLong) {
			this->funcs.back().emplace_back(Shit::Eq<uint64_t>(args,
																				Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// EQUAL NOT
	void EqNot(std::vector<uint64_t> const& args) {
		this->funcs.back().emplace_back(Shit::Check::OutOfRange());
		size_t const sz{args[0]};
		if (sz < Parse::kMaxChar) {
			this->funcs.back().emplace_back(
				Shit::EqNot<uint8_t>(args,
											Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz < Parse::kMaxShort) {
			this->funcs.back().emplace_back(
				Shit::EqNot<uint16_t>(args,
											 Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz < Parse::kMaxInt) {
			this->funcs.back().emplace_back(
				Shit::EqNot<uint32_t>(args,
											 Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz < Parse::kMaxLong) {
			this->funcs.back().emplace_back(
				Shit::EqNot<uint64_t>(args,
											 Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			std::cout << "unknown base: " << std::endl;
		}
	}
	// MASK
	auto Mask() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "mask{" && beg < end) {
				if (beg != end) {
					if (std::count(beg, end, ',') == 0) {
						uint64_t const sz{Parse::Utils::Convert(beg, end)};
						Mask(sz);
						beg += std::distance(beg, end);
					} else {
						std::cout << "command mask invalid: " + std::string(beg, end) << std::endl;
					}
				}
			}
			return 0;
		};
	}
	// MASK NOT
	auto MaskNot() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "!mask{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					uint64_t const sz{Parse::Utils::Convert(beg, end)};
					MaskNot(sz);
					beg += std::distance(beg, end);
				} else {
					std::cout << "command MaskNot invalid: " + std::string(beg, end) << std::endl;
				}
			}
			return 0;
		};
	}
	// GET FILTER MASK
	static uint64_t GetFilterMask(ItStr& beg, ItStr& end) {
		uint64_t ret{};
		auto it = std::search(beg, end, Parse::kFilterMask.begin(), Parse::kFilterMask.end());
		if (it < end) {
			ret = Parse::Utils::Convert(beg, it);
			beg = it + Parse::kFilterMask.size();
		} else {
			std::cout << "filter arg mask invalid" << std::string(beg,end) << std::endl;
			beg = end;
		}
		return ret;
	}
	// INSERT
	auto Insert() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "insert{[" && beg < end) {
				auto vec = Parse::Utils::GetListArgs<uint8_t>(beg,end);
				Insert(vec);
			}
		};
	}
	// FILTER
	auto Filter() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "filter{" && beg < end) {
				uint64_t const mask{GetFilterMask(beg, end)};
				auto vec = Parse::Utils::GetListArgs<uint64_t>(beg,end);
				Filter(mask, vec);
			}
			return 0;
		};
	}
	// FILTER NOT
	auto FilterNot() -> Func {
		return [this] (std::string const& name,
							ItStr& beg,
							ItStr& end,
							uint8_t& err) {
			if (name == "!filter{" && beg < end) {
				uint64_t const mask{GetFilterMask(beg, end)};
				auto vec = Parse::Utils::GetListArgs<uint64_t>(beg,end);
				FilterNot(mask, vec);
			}
			return 0;
		};
	}
	// EQUAL ARGUMENT
	std::vector<uint64_t> EqArgs(ItStr& beg, ItStr& end) {
		std::vector<uint64_t> ret{};
		auto it = std::find(beg, end, ']');
		if (it < end && beg != it && it[1] == '}') {
			ret = Parse::Utils::Split<uint64_t>(beg, it, ',');
			beg = it+1;
		} else {
			std::cout << "filter args invalid: " << std::string(beg,end) << std::endl;
			beg = end;
		}
		return ret;
	}
	// EQUAL
	auto Eq() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "eq{[" && beg < end) {
				auto vec = EqArgs(beg, end);
				Eq(vec);
				beg += std::distance(beg, end);
			}
			return 0;
		};
	}
	// EQUAL NOT
	auto EqNot() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "!eq{[" && beg < end) {
				auto vec = EqArgs(beg, end);
				EqNot(vec);
				beg += std::distance(beg, end);
			}
			return 0;
		};
	}
	// SHIFT LEFT IN BITS
	std::list<Shit::Func> ShlInBits(ItStr& beg, ItStr& end, uint8_t& err) {
		std::list<Shit::Func> ret{};
		size_t const arg{Parse::Utils::Convert(beg, end)};
		if (arg < 8) {
			ret.emplace_back(Shit::ShlInBits(arg));
		} else {
			std::cout << "arg shlb must be < 8: " << std::to_string(arg) << std::endl;
			err = 1;
		}
		return ret;
	}
	// SHIFT LEFT BITS
	auto ShlInBits() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "shlb{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					size_t const arg{Parse::Utils::Convert(beg, end)};
					if (arg < 8) {
						this->funcs.back().emplace_back(Shit::ShlInBits(arg));
					} else {
						std::cout << "arg shlb must be < 8: " << std::to_string(arg) << std::endl;
						err = 1;
					}
					beg += std::distance(beg, end);
				} else {
					std::cout << "command \"shlb\" invalid: " << std::string(beg,end) << std::endl;
				}
			} 
		};
	}
	// SHIFT LEFT
	auto Shl() -> Func {
		return [this] (std::string const& name,ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "shl{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					size_t const arg{Parse::Utils::Convert(beg, end)};
					this->funcs.back().emplace_back(Shit::Check::OutOfRangeLeft(arg));
					this->funcs.back().emplace_back(Shit::Shl(arg));
					beg += std::distance(beg, end);
				} else {
					std::cout << "command \"shl\" invalid: " << std::string(beg,end) << std::endl;
				}
			} 
			return 0;
		};
	}
	// SHIFT RIGHT IN BITS
	std::list<Shit::Func> ShrInBits(ItStr& beg, ItStr& end, uint8_t& err) {
		std::list<Shit::Func> ret{};
		size_t const arg{Parse::Utils::Convert(beg, end)};
		if (arg < 8) {
			ret.emplace_back(Shit::ShrInBits(arg));
		} else {
			std::cout << "arg shrb must be < 8: " << std::to_string(arg) << std::endl;
			err = 1;
		}
		return ret;
	}
	// SHIFT RIGHT BITS
	auto ShrInBits() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "shrb{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					size_t const arg{Parse::Utils::Convert(beg, end)};
					if (arg < 8) {
						this->funcs.back().emplace_back(Shit::ShrInBits(arg));
					} else {
						std::cout << "arg shrb must be < 8: " << std::to_string(arg) << std::endl;
						err = 1;
					}
					beg += std::distance(beg, end);
				} else {
					std::cout << "command \"shrb\" invalid: " << std::string(beg,end) << std::endl;
				}
			} 
		};
	}
	// SHIFT RIGHT
	auto Shr() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "shr{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					size_t const arg{Parse::Utils::Convert(beg, end)};
					this->funcs.back().emplace_back(Shit::Check::OutOfRangeRight(arg));
					this->funcs.back().emplace_back(Shit::Shr(arg));
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
		size_t const arg_beg{Parse::Utils::Convert(beg, first)};
		++first;
		size_t const arg_end{Parse::Utils::Convert(first, end)};
		ret = std::make_tuple(arg_beg, arg_end);
		return ret;	
	}
	// CROP
	void Crop(ItStr& beg, ItStr& end, size_t& counter) {
	}
	// CROP
	auto Crop(size_t& counter) -> Func {
		return [this, &counter] (std::string const& name,
										 ItStr& beg,
										 ItStr& end,
										 uint8_t& err) {
			if (name == "crop{" && beg < end) {
				if (std::count(beg, end, ',') == 1) {
					auto args = CropArgs(beg, end);
					this->funcs.back().emplace_back(
						Shit::Check::OutOfRange(std::get<0>(args),
														std::get<1>(args),
														counter));
					this->funcs.back().emplace_back(
						Shit::Crop(std::get<0>(args), std::get<1>(args)));
					beg += std::distance(beg, end);
				} else {
					std::cout << "command \"crop\" invalid: " << std::string(beg,end) << std::endl;
				}
			}
			return 0;
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
	// FOR TUPLE EMPTY
	template <size_t I = 0, typename... Ts>
		typename std::enable_if<I == sizeof...(Ts), void>::type
		for_tuple(std::tuple<Ts...>& tup, std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			return;
		}
	// FOR TUPLE
	template <size_t I = 0, typename... Ts>
		typename std::enable_if<(I < sizeof...(Ts)), void>::type
		for_tuple(std::tuple<Ts...>& tup, std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			std::get<I>(tup)(name, beg, end, err);
			if (err == 0) {
				for_tuple<I + 1>(tup, name, beg, end, err);
			}
		}
	// COMMANDS
	template<class... ArgsTuple>
		auto Commands(ArgsTuple... args_tuple) ->
		std::function<void(ItStr&,ItStr&,uint8_t&)> {
			return [this, args_tuple...] (ItStr& beg, ItStr& end, uint8_t& err) {
				std::tuple<ArgsTuple...> ts{args_tuple...};
				while (beg < end) {
					std::string command{};
					for (auto const& name : Parse::kNameFuncs) {
						command = GetCommand(name, beg, end);
						if (!command.empty()) {
							beg += name.size();
							auto end_arg = std::find(beg, end, '}');
							for_tuple(ts, command, beg, end_arg, err);
							if (err == 1) {
								return;
							}
							beg += 2;
							break;
						}
					}
					if (command.empty()) {
						std::cout << "unknown command: " << std::string(beg,end) << std::endl;
						beg = end;
						err = 1;
						return;
					}
				}
			};
		}
	// CONSTRUCTOR
	explicit Parsed(std::string str,
						 std::ofstream& dst,
						 uint8_t& err,
						 size_t& num_pck) {
		Parse::Utils::RemoveUnusedChars(str);
		auto exec = std::search(str.begin(), str.end(),
										Parse::kExec.begin(), Parse::kExec.end());
		while (exec != str.end()) {
			this->funcs.emplace_back(std::list<Shit::Func>{});
			auto commands = Commands(Crop(num_pck),
											 Shr(),ShrInBits(),
											 Shl(),ShlInBits(),
											 Insert(),
											 Eq(),EqNot(),
											 Filter(),FilterNot(),
											 Mask(),MaskNot());
			auto beg = exec + Parse::kExec.size();
			auto end = std::search(beg, str.end(),
										  Parse::kExecEnd.begin(), Parse::kExecEnd.end());
			while (beg < end) {
				commands(beg, end, err);
				if (err == 1) {
					break;
				}
			}
			exec = std::search(end, str.end(),
									 Parse::kExec.begin(), Parse::kExec.end());
			if (err == 0) {
				this->funcs.back().emplace_back(Shit::Write<Shit::kSig>(dst));
			}
		}
	}
	// RUN EDIT
	void RunEdit(uint8_t* pbeg, size_t const sz) {
		insert_sz = 0;
		for (auto& execs : funcs) {
			std::vector<uint8_t> buf(sz + 2);
			std::copy(pbeg, pbeg + sz, buf.begin() + 1);
			uint8_t* pbeg{buf.data()+1};
			uint8_t* pend{pbeg + buf.size() - 2};
			Shit::Init(pbeg, pend);
			for (auto& func : execs) {
				bool const state{func(&pbeg, &pend)};
				if (!state) {
					Shit::CleanOffset(Parse::offset);
					break;
				}
			}
			Shit::CleanOffset(Parse::offset);
		}
	}
	// RUN INSERT
	void RunInsert(uint8_t* pbeg, size_t const sz) {
		for (auto& execs : funcs) {
			std::vector<uint8_t> buf(insert_sz + sz);
			std::copy(pbeg, pbeg + sz, buf.begin() + this->insert_sz);
			uint8_t* pbeg{buf.data() + insert_sz};
			uint8_t* pend(pbeg + buf.size());
			Shit::Init(pbeg - insert_sz, pend);
			for (auto& func : execs) {
				bool const state{func(&pbeg, &pend)};
				if (!state) {
					Shit::CleanOffset(Parse::offset);
					break;
				}
			}
		}
		Shit::CleanOffset(Parse::offset);
	}
	// RUN
	void Run(uint8_t* pbeg, size_t const sz) {
		for (auto& execs : funcs) {
			uint8_t* pend{pbeg + sz - 1};
			Shit::Init(pbeg, pend);
			for (auto& func : execs) {
				bool const state{func(&pbeg, &pend)};
				if (!state) {
					Shit::CleanOffset(Parse::offset);
					break;
				}
			}
			Shit::CleanOffset(Parse::offset);
		}
	}
};

#endif
