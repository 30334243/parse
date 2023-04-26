#ifndef PARSE_SCRIPT_HPP
#define PARSE_SCRIPT_HPP

#include <algorithm>
#include <list>
#include <cctype>

#include "parse_interface.hpp"
#include "../shit_msvc13/shit_msvc13.hpp"

namespace Parse {
	namespace Utils {
		// CHECK TYPE
		static bool CheckType(ItStr& beg, ItStr& end, uint8_t const base) {
			bool ret{true};
			if (base == kBinBase) {
				if(std::find_if(beg, end,
									 [] (char const c) {
										 return (c == '1' || c == '0') ? false : true;
									 }) != end) {
					beg -= 2;
					ret = false;
				}
			} else if (base == kDecBase) {
				if(std::find_if(beg, end,
									 [] (char const c) {
										 return (c >= '0' && c <= '9') ? false : true;
									 }) != end) {
					ret = false;
				}
			} else if (base == kHexBase) {
				if(std::find_if(beg, end,
									 [] (char const c) {
										 return (c >= '0' && c <= 'f') ? false : true;
									 }) != end) {
					beg -= 2;
					ret = false;
				}
			} else {
				ret = false;
			}
			return ret;
		}
		// REMOVE UNUSED CHARACTERS
		static void RemoveUnusedChars(std::string& str) {
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
		static bool Convert(ItStr& beg, ItStr& end, uint64_t& out) {
			bool ret{};
			uint8_t const base{GetBase(beg, end)};
			beg += base == kDecBase ? 0 : 2;
			if (Parse::Utils::CheckType(beg, end, base)) {
				out = std::stoul(std::string(beg, end), nullptr, base);
				ret = true;
			}
			return ret;
		}
		// FOR EACH TO ARGUMENTS
		template<class Func>
			bool ForEachToArgs(ItStr& beg,
									 ItStr& end,
									 char const term,
									 Func func) {
				bool ret{true};
				auto it = find(beg, end, term);
				uint64_t num{};
				while (it < end) {
					if (!Convert(beg, it, num)) {
						ret = false;
						break;
					}
					if (!func(num)) {
						ret = false;
						break;
					}
					beg = ++it;
					it = std::find(it, end, term);
				}
				if (it == end) {
					if (Convert(beg, it, num) && (!func(num))) {
						ret = false;
					}
				}
				return ret;
			}
		// GET LIST ARGUMENT
		template<class Func>
			static bool OpForArgsList(ItStr& beg,
											  ItStr& end,
											  char const term,
											  Func func,
											  std::string& msg) {
				bool ret{};
				ItStr it = std::find(beg, end, ']');
				if (it < end && beg != it && it[1] == '}') {
					if (Parse::Utils::ForEachToArgs(beg, it, term, func)) {
						beg = it+1;
						ret = true;
					} else {
						msg = "Argument invalid: " + std::string(beg,end);
					}
				} else {
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
	std::string msg{};
	uint64_t insert_sz{};
	uint64_t lid{};
	size_t offset{};
	// AND
	void And(uint64_t const sz) {
		this->funcs.back().emplace_back(Shit::Check::Lid(offset, msg));
		if (sz <= Parse::kMaxChar) {
			this->funcs.back().emplace_back(
				Shit::And((uint8_t)sz,
							 Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz <= Parse::kMaxShort) {
			this->funcs.back().emplace_back(
				Shit::And((uint16_t)sz,
							 Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz <= Parse::kMaxInt) {
			this->funcs.back().emplace_back(
				Shit::And((uint32_t)sz,
							 Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz <= Parse::kMaxLong) {
			this->funcs.back().emplace_back(
				Shit::And((uint64_t)sz,
							 Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			msg = "unknown base: " + std::to_string(sz);
		}
	}
	// AND NOT
	void AndNot(uint64_t const sz) {
		this->funcs.back().emplace_back(Shit::Check::Lid(offset, msg));
		if (sz <= Parse::kMaxChar) {
			this->funcs.back().emplace_back(
				Shit::AndNot((uint8_t)sz,
								 Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz <= Parse::kMaxShort) {
			this->funcs.back().emplace_back(
				Shit::AndNot((uint16_t)sz,
								 Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz <= Parse::kMaxInt) {
			this->funcs.back().emplace_back(
				Shit::AndNot((uint32_t)sz,
								 Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz <= Parse::kMaxLong) {
			this->funcs.back().emplace_back(
				Shit::AndNot((uint64_t)sz,
								 Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			msg = "unknown base" + std::to_string(sz);
		}
	}
	// SPLIT
	void Split(uint64_t const sz) {
		this->funcs.back().emplace_back(Shit::Check::Lid(offset, msg));
		if (sz <= Parse::kMaxChar) {
			this->funcs.back().emplace_back(
				Shit::Split((uint8_t)sz,
								Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz <= Parse::kMaxShort) {
			this->funcs.back().emplace_back(
				Shit::Split((uint16_t)sz,
								Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz <= Parse::kMaxInt) {
			this->funcs.back().emplace_back(
				Shit::Split((uint32_t)sz,
								Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz <= Parse::kMaxLong) {
			this->funcs.back().emplace_back(
				Shit::Split((uint64_t)sz,
								Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			msg = "unknown base: " + std::to_string(sz);
		}
	}
	// SPLIT NOT
	void SplitNot(uint64_t const sz) {
		this->funcs.back().emplace_back(Shit::Check::Lid(offset, msg));
		if (sz <= Parse::kMaxChar) {
			this->funcs.back().emplace_back(
				Shit::SplitNot((uint8_t)sz,
									Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz <= Parse::kMaxShort) {
			this->funcs.back().emplace_back(
				Shit::SplitNot((uint16_t)sz,
									Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz <= Parse::kMaxInt) {
			this->funcs.back().emplace_back(
				Shit::SplitNot((uint32_t)sz,
									Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz <= Parse::kMaxLong) {
			this->funcs.back().emplace_back(
				Shit::SplitNot((uint64_t)sz,
									Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			msg = "unknown base: " + std::to_string(sz);
		}
	}
	// INSERT
	void Insert(std::vector<uint8_t> const& args) {
		this->insert_sz += args.size();
		this->funcs.back().emplace_back(Shit::Check::OutOfRange());
		this->funcs.back().emplace_back(Shit::Insert(args.size(), args));
	}
	// REPLACE
	void Replace(std::vector<uint8_t> const& args) {
		this->funcs.back().emplace_back(Shit::Check::OutOfRangeRight(args.size(), msg));
		this->funcs.back().emplace_back(Shit::Replace(args));
	}
	// FILTER
	void Filter(uint64_t const sz, std::vector<uint64_t> const& args) {
		this->funcs.back().emplace_back(Shit::Check::OutOfRange());
		if (sz <= Parse::kMaxChar) {
			this->funcs.back().emplace_back(
				Shit::Filter<uint8_t>(sz,
											 args,
											 Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz <= Parse::kMaxShort) {
			this->funcs.back().emplace_back(
				Shit::Filter<uint16_t>(sz,
											  args,
											  Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz <= Parse::kMaxInt) {
			this->funcs.back().emplace_back(
				Shit::Filter<uint32_t>(sz,
											  args,
											  Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz <= Parse::kMaxLong) {
			this->funcs.back().emplace_back(
				Shit::Filter<uint64_t>(sz,
											  args,
											  Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			msg = "unknown base: " + std::to_string(sz);
		}
	}
	// FILTER NOT
	void FilterNot(uint64_t const sz, std::vector<uint64_t> const& args) {
		this->funcs.back().emplace_back(Shit::Check::OutOfRange());
		if (sz <= Parse::kMaxChar) {
			this->funcs.back().emplace_back(Shit::FilterNot<uint8_t>(sz,
																						args,
																						Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz <= Parse::kMaxShort) {
			this->funcs.back().emplace_back(Shit::FilterNot<uint16_t>(sz,
																						 args,
																						 Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz <= Parse::kMaxInt) {
			this->funcs.back().emplace_back(Shit::FilterNot<uint32_t>(sz,
																						 args,
																						 Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz <= Parse::kMaxLong) {
			this->funcs.back().emplace_back(Shit::FilterNot<uint64_t>(sz,
																						 args,
																						 Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			msg = "unknown base: " + std::to_string(sz);
		}
	}
	// EQUAL
	void Eq(std::vector<uint64_t> const& args) {
		this->funcs.back().emplace_back(Shit::Check::Lid(offset, msg));
		size_t const sz{args[0]};
		if (sz <= Parse::kMaxChar) {
			this->funcs.back().emplace_back(Shit::Eq<uint8_t>(args,
																			  Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz <= Parse::kMaxShort) {
			this->funcs.back().emplace_back(Shit::Eq<uint16_t>(args,
																				Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz <= Parse::kMaxInt) {
			this->funcs.back().emplace_back(Shit::Eq<uint32_t>(args,
																				Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz <= Parse::kMaxLong) {
			this->funcs.back().emplace_back(Shit::Eq<uint64_t>(args,
																				Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			msg = "unknown base: " + std::to_string(sz);
		}
	}
	// EQUAL NOT
	void EqNot(std::vector<uint64_t> const& args) {
		this->funcs.back().emplace_back(Shit::Check::Lid(offset, msg));
		size_t const sz{args[0]};
		if (sz <= Parse::kMaxChar) {
			this->funcs.back().emplace_back(
				Shit::EqNot<uint8_t>(args,
											Shit::SaveToLid<uint8_t>(lid, offset)));
		} else if (sz <= Parse::kMaxShort) {
			this->funcs.back().emplace_back(
				Shit::EqNot<uint16_t>(args,
											 Shit::SaveToLid<uint16_t>(lid, offset)));
		} else if (sz <= Parse::kMaxInt) {
			this->funcs.back().emplace_back(
				Shit::EqNot<uint32_t>(args,
											 Shit::SaveToLid<uint32_t>(lid, offset)));
		} else if (sz <= Parse::kMaxLong) {
			this->funcs.back().emplace_back(
				Shit::EqNot<uint64_t>(args,
											 Shit::SaveToLid<uint64_t>(lid, offset)));
		} else {
			msg = "unknown base: " + std::to_string(sz);
		}
	}
	// AND
	auto And() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "and{" && beg < end) {
				if (beg != end) {
					if (std::count(beg, end, ',') == 0) {
						uint64_t sz{};
						if (Parse::Utils::Convert(beg, end, sz)) {
							And(sz);
							beg += std::distance(beg, end);
						} else {
							err = 1;
						}
					} else {
						msg = "Command \"And\" invalid: " + std::string(beg, end);
						err = 1;
					}
				}
			}
		};
	}
	// AND NOT
	auto AndNot() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "!and{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					uint64_t sz{};
					if (Parse::Utils::Convert(beg, end, sz)) {
						AndNot(sz);
						beg += std::distance(beg, end);
					} else {
						err = 1;
					}
				} else {
					msg = "Command \"AndNot\" invalid: " + std::string(beg, end);
					err = 1;
				}
			}
		};
	}
	// SPLIT
	auto Split() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "split{" && beg < end) {
				if (beg != end) {
					if (std::count(beg, end, ',') == 0) {
						uint64_t sz{};
						if (Parse::Utils::Convert(beg, end, sz)) {
							Split(sz);
							beg += std::distance(beg, end);
						} else {
							err = 1;
						}
					} else {
						msg = "Command \"Split\" invalid: " + std::string(beg, end);
						err = 1;
					}
				}
			}
		};
	}
	// SPLIT NOT
	auto SplitNot() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "!split{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					uint64_t sz{};
					if (Parse::Utils::Convert(beg, end, sz)) {
						SplitNot(sz);
						beg += std::distance(beg, end);
					} else {
						err = 1;
					}
				} else {
					msg = "Command \"SplitNot\" invalid: " + std::string(beg, end);
					err = 1;
				}
			}
		};
	}
	// GET FILTER AND
	bool GetFilterMask(ItStr& beg, ItStr& end, uint64_t& out) {
		bool ret{};
		auto it = std::search(beg, end,
									 Parse::kFilterMask.begin(), Parse::kFilterMask.end());
		if (it < end) {
			if (Parse::Utils::Convert(beg, it, out)) {
				beg = it + Parse::kFilterMask.size();
				ret = true;
			}
		} else {
			msg = "Function \"GetFilterMask\" invalid: " + std::string(beg, end);
			beg = end;
		}
		return ret;
	}
	// INSERT
	auto Insert() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "insert{[" && beg < end) {
				auto tmp = beg;
				bool const res = Parse::Utils::OpForArgsList(
					tmp, end, ',',
					[this] (uint64_t const arg) {
						return arg <= Parse::kMaxChar;
					},msg);
				if (res) {
					std::vector<uint8_t> vec{};
					Parse::Utils::OpForArgsList(
						beg, end, ',',
						[&vec] (uint8_t const arg) {
							vec.emplace_back(arg);
							return true;
						},msg);
					Insert(vec);
				} else {
					err = 1;
				}
			}
		};
	}
	// REPLACE
	auto Replace() -> Func {
		return [this] (std::string const& name,
							ItStr& beg,
							ItStr& end,
							uint8_t& err) {
			if (name == "replace{[") {
				auto tmp = beg;
				bool const res = Parse::Utils::OpForArgsList(
					tmp, end, ',',
					[this] (uint64_t const arg) {
						return arg <= Parse::kMaxChar;
					},msg);
				if (res) {
					std::vector<uint8_t> vec{};
					Parse::Utils::OpForArgsList(
						beg, end, ',',
						[&vec] (uint8_t const arg) {
							vec.emplace_back(arg);
							return true;
						},msg);
					Replace(vec);
				} else {
					err = 1;
				}
			}
		};
	}
	// FILTER
	auto Filter() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "filter{" && beg < end) {
				uint64_t mask{};
				GetFilterMask(beg, end, mask);
				std::vector<uint64_t> vec{};
				Parse::Utils::OpForArgsList(
					beg, end, ',',
					[&vec] (uint64_t const arg) {
						vec.emplace_back(arg);
						return true;
					},msg);
				Filter(mask, vec);
			}
		};
	}
	// FILTER NOT
	auto FilterNot() -> Func {
		return [this] (std::string const& name,
							ItStr& beg,
							ItStr& end,
							uint8_t& err) {
			if (name == "!filter{" && beg < end) {
				uint64_t mask{};
				GetFilterMask(beg, end, mask);
				auto tmp = beg;
				bool const res = Parse::Utils::OpForArgsList(
					tmp, end, ',',
					[this] (uint64_t const arg) {
						return arg <= Parse::kMaxChar;
					},msg);
				std::vector<uint8_t> vec{};
				if (res) {
					std::vector<uint64_t> vec{};
					Parse::Utils::OpForArgsList(
						beg, end, ',',
						[&vec] (uint64_t const arg) {
							vec.emplace_back(arg);
							return true;
						},msg);
					FilterNot(mask, vec);
				} else {
					err = 1;
				}
			}
		};
	}
	// EQUAL
	auto Eq() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "eq{[" && beg < end) {
				auto tmp = beg;
				bool const res = Parse::Utils::OpForArgsList(
					tmp, end, ',',
					[this] (uint64_t const arg) {
						return arg <= Parse::kMaxChar;
					},msg);
				if (res) {
					std::vector<uint64_t> vec{};
					Parse::Utils::OpForArgsList(
						beg, end, ',',
						[&vec] (uint8_t const arg) {
							vec.emplace_back(arg);
							return true;
						},msg);
					Eq(vec);
				} else {
					err = 1;
				}
			}
		};
	}
	// EQUAL NOT
	auto EqNot() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "!eq{[" && beg < end) {
				auto tmp = beg;
				bool const res = Parse::Utils::OpForArgsList(
					tmp, end, ',',
					[this] (uint64_t const arg) {
						return arg <= Parse::kMaxChar;
					},msg);
				if (res) {
					std::vector<uint64_t> vec{};
					Parse::Utils::OpForArgsList(
						beg, end, ',',
						[&vec] (uint8_t const arg) {
							vec.emplace_back(arg);
							return true;
						},msg);
					EqNot(vec);
				} else {
					err = 1;
				}
			}
		};
	}
	// SHIFT LEFT BITS
	auto ShlInBits() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "shlb{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					uint64_t arg{};
					if (Parse::Utils::Convert(beg, end, arg) && arg < 8) {
						++this->insert_sz;
						this->funcs.back().emplace_back(Shit::ShlInBits(arg));
					} else {
						msg = "arg shlb must be < 8: " + std::to_string(arg);
						err = 1;
					}
					beg += std::distance(beg, end);
				} else {
					msg = "command \"shlb\" invalid: " + std::string(beg,end);
				}
			} 
		};
	}
	// SHIFT LEFT
	auto Shl() -> Func {
		return [this] (std::string const& name,ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "shl{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					uint64_t arg{};
					if (Parse::Utils::Convert(beg, end, arg)) {
						this->funcs.back().emplace_back(Shit::Check::OutOfRangeLeft(arg, msg));
						this->funcs.back().emplace_back(Shit::Shl(arg));
						beg += std::distance(beg, end);
					} else {
						err = 1;
					}
				} else {
					msg = "command \"shl\" invalid: " + std::string(beg,end);
					err = 1;
				}
			} 
		};
	}
	// SHIFT RIGHT BITS
	auto ShrInBits() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "shrb{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					uint64_t arg{};
					if (Parse::Utils::Convert(beg, end, arg) && arg < 8) {
						++this->insert_sz;
						this->funcs.back().emplace_back(Shit::ShrInBits(arg));
					} else {
						msg = "arg \"shrb\" must be < 8: " + std::to_string(arg);
						err = 1;
					}
					beg += std::distance(beg, end);
				} else {
					msg = "command \"shrb\" invalid: " + std::string(beg,end);
					err = 1;
				}
			} 
		};
	}
	// SHIFT RIGHT
	auto Shr() -> Func {
		return [this] (std::string const& name, ItStr& beg, ItStr& end, uint8_t& err) {
			if (name == "shr{" && beg < end) {
				if (std::count(beg, end, ',') == 0) {
					uint64_t arg{};
					if (Parse::Utils::Convert(beg, end, arg)) {
						this->funcs.back().emplace_back(Shit::Check::OutOfRangeRight(arg, msg));
						this->funcs.back().emplace_back(Shit::Shr(arg));
						beg += std::distance(beg, end);
					} else {
						err = 1;
					}
				} else {
					msg = "command \"shr\" invalid: " + std::string(beg,end);
					err = 1;
				}
			}
		};
	}
	// CROP ARGUMENTS
	bool CropArgs(ItStr& beg, ItStr& end, std::tuple<size_t,size_t>& out) {
		bool ret{};
		auto first = std::find(beg, end, ',');
		uint64_t arg_beg{};
		uint64_t arg_end{};
		if (first < end &&
			 Parse::Utils::Convert(beg, first, arg_beg)) {
			++first;
			if (Parse::Utils::Convert(first, end, arg_end)) {
				out = std::make_tuple(arg_beg, arg_end);
				ret = true;
			}
		}
		return ret;	
	}
	// CROP
	auto Crop(size_t& counter) -> Func {
		return [this, &counter] (std::string const& name,
										 ItStr& beg,
										 ItStr& end,
										 uint8_t& err) {
			if (name == "crop{" && beg < end) {
				if (std::count(beg, end, ',') == 1) {
					std::tuple<size_t,size_t> args{};
					auto tmp_beg = beg;
					auto tmp_end = end;
					if (CropArgs(beg, end, args)) {
						this->funcs.back().emplace_back(
							Shit::Check::OutOfRange(std::get<0>(args),
															std::get<1>(args),
															counter,
															msg));
						this->funcs.back().emplace_back(
							Shit::Crop(std::get<0>(args), std::get<1>(args)));
						beg += std::distance(beg, end);
					} else {
						err = 1;
					}
				} else {
					msg = "command \"crop\" invalid: " + std::string(beg,end);
					err = 1;
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
						beg = end;
						err = 1;
						return;
					}
				}
			};
		}
	// CONSTRUCTOR
	explicit Parsed(std::string str,
						 /* std::ofstream& dst, */
						 std::vector<uint8_t>& vec,
						 uint8_t& err,
						 size_t& num_pck) {
		std::transform(str.begin(), str.end(),
							str.begin(),
							[] (char c) {return std::tolower(c);});
		Parse::Utils::RemoveUnusedChars(str);
		auto exec = std::search(str.begin(), str.end(),
										Parse::kExec.begin(), Parse::kExec.end());
		while (exec != str.end()) {
			this->funcs.emplace_back(std::list<Shit::Func>{});
			auto commands = Commands(Crop(num_pck),
											 Shr(),ShrInBits(),
											 Shl(),ShlInBits(),
											 Eq(),EqNot(),
											 Filter(),FilterNot(),
											 And(),AndNot(),
											 Split(),SplitNot(),
											 Insert(),
											 Replace());
			auto beg = exec + Parse::kExec.size();
			auto end = std::search(beg, str.end(),
										  Parse::kExecEnd.begin(), Parse::kExecEnd.end());
			if (end == str.end()) {
				msg = "Missing \")\"";
				exec = end;
			} else {
				while (beg < end) {
					commands(beg, end, err);
					if (err == 1) {
						break;
					}
				}
				exec = std::search(end, str.end(),
										 Parse::kExec.begin(), Parse::kExec.end());
				if (err == 0) {
					/* this->funcs.back().emplace_back(Shit::Write<Shit::kSig>(dst)); */
					this->funcs.back().emplace_back(Shit::Test(vec));
				}
			}
		}
	}
	// RUN EDIT
	bool RunEdit(uint8_t* pbeg, size_t const sz) {
		bool ret{true};
		lid = 0;
		for (auto& execs : funcs) {
			std::vector<uint8_t> buf(sz + 2);
			std::copy(pbeg, pbeg + sz, buf.begin() + 1);
			uint8_t* pbeg{buf.data()+1};
			uint8_t* pend{pbeg + buf.size() - 2};
			Shit::Init(pbeg, pend);
			for (auto& func : execs) {
				bool const state{func(&pbeg, &pend)};
				if (!state) {
					ret = false;
					break;
				}
			}
			offset = 0;
		}
		return ret;
	}
	// RUN INSERT
	bool RunInsert(uint8_t* pbeg, size_t const sz) {
		bool ret{true};
		lid = 0;
		for (auto& execs : funcs) {
			std::vector<uint8_t> buf(insert_sz + sz);
			std::copy(pbeg, pbeg + sz, buf.begin());
			uint8_t* pbeg{buf.data()};
			uint8_t* pend(pbeg + sz);
			Shit::Init(pbeg, pend + insert_sz);
			for (auto& func : execs) {
				bool const state{func(&pbeg, &pend)};
				if (!state) {
					ret = false;
					break;
				}
			}
			offset = 0;
		}
		return ret;
	}
	// RUN
	bool Run(uint8_t* pbeg, size_t const sz) {
		bool ret{true};
		lid = 0;
		for (auto& execs : funcs) {
			uint8_t* pend{pbeg + sz};
			Shit::Init(pbeg, pend);
			for (auto& func : execs) {
				bool const state{func(&pbeg, &pend)};
				if (!state) {
					ret = false;
					break;
				}
			}
			offset = 0;
		}
		return ret;
	}
};

#endif
