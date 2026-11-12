#ifndef AVR_REGFILE_HPP_
#define AVR_REGFILE_HPP_

#include <array>

#include "core/avr_config.hpp"

namespace avrpp {

/* The special 16-bit pairs */
enum class PtrReg : uint8_t { None = 0, X = 26, Y = 28, Z = 30 };

/* Combinatorial access for register file reads */
struct RegFileReadPort {
  // IN in VHDL Equivalent
  uint8_t rd_addr_i{0};   // 0..31 -> R0 .. R31
  uint8_t rr_addr_i{0};   // 0..31 -> R0 .. R31
  uint8_t pair_addr_i{0}; // 0..15 -> R1:R0 .. R31:R30
  PtrReg ptr_sel_i{PtrReg::None};

  // OUT in VHDL Equivalent
  uint8_t rd_data_o{0};
  uint8_t rr_data_o{0};
  uint16_t pair_data_o{0};
  uint16_t ptr_val_o{0};
};

/* Clocked access for writes onto the register file */
struct RegFileWritePort {
  bool we_i{false};
  uint8_t waddr_i{0};
  uint8_t wdata_i{0};

  bool we_pair_i{false};
  uint8_t waddr_pair_i{0};
  uint16_t wval_pair_i{0};

  bool ptr_we_i{false};
  PtrReg ptr_sel_i{PtrReg::None};
  uint16_t ptr_wval_i{0};
};

template <CoreConfig Config>
class RegisterFile {
public:
  static constexpr std::size_t kNumRegs = Config.num_gprs();
  
  static constexpr std::size_t physical_index(uint8_t architecture_reg) noexcept {
    if constexpr (Config.is_avrrc) {
        return static_cast<std::size_t>(architecture_reg & 0x0FU); // Got only 16 regs but this emulator maps R16..R31 to 0..15
    } else {
        return static_cast<std::size_t>(architecture_reg & 0x1FU); // Maps 1:1 to the register, R0..R31 -> 0..31
    }
  }

  constexpr void eval_read_comb(RegFileReadPort& port) const noexcept {
    port.rd_data_o = regs_[physical_index(port.rd_addr_i)];
    port.rr_data_o = regs_[physical_index(port.rr_addr_i)];

    const std::size_t pair_index = physical_index(static_cast<uint8_t>((port.pair_addr_i & 0x0FU) << 1));
    port.pair_data_o = read_pair(pair_index);

    if (port.ptr_sel_i != PtrReg::None) {
      port.ptr_val_o = read_pair(static_cast<uint8_t>(port.ptr_sel_i));
    }
  }

  constexpr void tick_write_seq(const RegFileWritePort& port) noexcept {
    if (port.ptr_we_i && port.ptr_sel_i != PtrReg::None) {
      write_pair(static_cast<uint8_t>(port.ptr_sel_i), port.ptr_wval_i);
    }

    if (port.we_pair_i) {
      const uint8_t pair_index = static_cast<uint8_t>((port.waddr_pair_i & 0x0FU) << 1);
      write_pair(pair_index, port.wval_pair_i);
    }

    if (port.we_i) {
      regs_[physical_index(port.waddr_i)] = port.wdata_i;
    }
  }

  constexpr uint16_t read_pair(uint8_t reg_idx_even) const noexcept {
    const std::size_t sanitized_idx = physical_index(reg_idx_even);
    return static_cast<uint16_t>((static_cast<uint16_t>(regs_[sanitized_idx + 1]) << 8) | regs_[sanitized_idx]);
  }

  constexpr void write_pair(uint8_t reg_idx_even, uint16_t value) noexcept {
    const std::size_t sanitized_idx = physical_index(reg_idx_even);
    regs_[sanitized_idx]     = static_cast<uint8_t>(value & 0xFFU);
    regs_[sanitized_idx + 1] = static_cast<uint8_t>(value >> 8);
  }

private:
  std::array<uint8_t, kNumRegs> regs_{};
};

}

#endif  // AVR_REGFILE_HPP_