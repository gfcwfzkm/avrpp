// Unit tests for the register file (include/core/avr_regfile.hpp).
//
// The register file is tested only through its ports, the way the CPU (and later the VHDL) uses it:
// set the inputs of a RegFileReadPort and call eval_read_comb() to read, fill a RegFileWritePort and
// call tick_write_seq() for one clock edge to write.
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include <cstdint>

#include "core/avr_regfile.hpp"

namespace {

using avrpp::PtrReg;
using avrpp::RegFileReadPort;
using avrpp::RegFileWritePort;
using avrpp::RegisterFile;

// A full core (R0..R31) and the reduced core of the ATtiny10 (R16..R31 only).
using FullRegs = RegisterFile<avrpp::Presets::AVR1>;
using RcRegs = RegisterFile<avrpp::Presets::AVRRC>;

// One clock edge that writes one byte register.
template <typename Regs>
void WriteReg(Regs& regs, uint8_t reg, uint8_t value) {
  RegFileWritePort w{};
  w.we_i = true;
  w.waddr_i = reg;
  w.wdata_i = value;
  regs.tick_write_seq(w);
}

// Reads one register through the Rd read port.
template <typename Regs>
uint8_t ReadReg(const Regs& regs, uint8_t reg) {
  RegFileReadPort r{};
  r.rd_addr_i = reg;
  regs.eval_read_comb(r);
  return r.rd_data_o;
}

// Reads a pointer register (X, Y or Z) through the pointer read port.
template <typename Regs>
uint16_t ReadPtr(const Regs& regs, PtrReg ptr) {
  RegFileReadPort r{};
  r.ptr_sel_i = ptr;
  regs.eval_read_comb(r);
  return r.ptr_val_o;
}

}  // namespace

TEST_CASE("Register file: sizes per core") {
  CHECK(FullRegs::kNumRegs == 32);
  CHECK(RcRegs::kNumRegs == 16);
}

TEST_CASE("Register file: all registers are 0 after construction") {
  const FullRegs regs;
  for (uint8_t i = 0; i < 32; ++i) {
    CAPTURE(i);  // shown in the failure message, so you know which register failed
    CHECK(ReadReg(regs, i) == 0);
  }
}

TEST_CASE("Register file: every register keeps its own value") {
  FullRegs regs;
  // Different values everywhere: a write that lands in the wrong register is caught.
  for (uint8_t i = 0; i < 32; ++i) {
    WriteReg(regs, i, static_cast<uint8_t>(0xA0U + i));
  }
  for (uint8_t i = 0; i < 32; ++i) {
    CAPTURE(i);
    CHECK(ReadReg(regs, i) == 0xA0U + i);
  }
}

TEST_CASE("Register file: Rd and Rr read two registers in the same evaluation") {
  FullRegs regs;
  WriteReg(regs, 3, 0x11);
  WriteReg(regs, 17, 0x22);

  RegFileReadPort r{};
  r.rd_addr_i = 3;
  r.rr_addr_i = 17;
  regs.eval_read_comb(r);
  CHECK(r.rd_data_o == 0x11);
  CHECK(r.rr_data_o == 0x22);

  r.rd_addr_i = 17;  // Rd and Rr may name the same register (ADD R17, R17)
  regs.eval_read_comb(r);
  CHECK(r.rd_data_o == 0x22);
  CHECK(r.rr_data_o == 0x22);
}

TEST_CASE("Register file: a write needs its enable") {
  FullRegs regs;
  RegFileWritePort w{};
  w.waddr_i = 5;
  w.wdata_i = 0x55;
  w.waddr_pair_i = 12;
  w.wval_pair_i = 0x1234;
  w.ptr_sel_i = PtrReg::Z;
  w.ptr_wval_i = 0xBEEF;
  regs.tick_write_seq(w);  // all enables false
  for (uint8_t i = 0; i < 32; ++i) {
    CAPTURE(i);
    CHECK(ReadReg(regs, i) == 0);
  }
}

TEST_CASE("Register file: the read port shows the old value until the clock edge") {
  // Like a flip-flop: eval_read_comb() sees what was written by the *previous* tick_write_seq().
  FullRegs regs;
  WriteReg(regs, 8, 0x01);
  RegFileWritePort w{};
  w.we_i = true;
  w.waddr_i = 8;
  w.wdata_i = 0x02;
  CHECK(ReadReg(regs, 8) == 0x01);  // the write port is set up, but no edge yet
  regs.tick_write_seq(w);
  CHECK(ReadReg(regs, 8) == 0x02);
}

TEST_CASE("Register file: register pairs are little endian (MOVW, ADIW)") {
  FullRegs regs;
  RegFileWritePort w{};
  w.we_pair_i = true;
  w.waddr_pair_i = 12;  // pair 12 = R25:R24
  w.wval_pair_i = 0x1234;
  regs.tick_write_seq(w);
  CHECK(ReadReg(regs, 24) == 0x34);  // low byte in the even register
  CHECK(ReadReg(regs, 25) == 0x12);

  RegFileReadPort r{};
  r.pair_addr_i = 12;
  regs.eval_read_comb(r);
  CHECK(r.pair_data_o == 0x1234);
  CHECK(r.pair_addr_i == 12);  // an input: reading must not change it
}

TEST_CASE("Register file: the pair address uses only 4 bits") {
  FullRegs regs;
  RegFileWritePort w{};
  w.we_pair_i = true;
  w.waddr_pair_i = 0x10 | 3;  // bit 4 is ignored -> pair 3 = R7:R6
  w.wval_pair_i = 0xCAFE;
  regs.tick_write_seq(w);
  CHECK(ReadReg(regs, 6) == 0xFE);
  CHECK(ReadReg(regs, 7) == 0xCA);
}

TEST_CASE("Register file: X, Y and Z are R27:R26, R29:R28 and R31:R30") {
  FullRegs regs;
  WriteReg(regs, 26, 0x01);
  WriteReg(regs, 27, 0x02);
  WriteReg(regs, 28, 0x03);
  WriteReg(regs, 29, 0x04);
  WriteReg(regs, 30, 0x05);
  WriteReg(regs, 31, 0x06);
  CHECK(ReadPtr(regs, PtrReg::X) == 0x0201);
  CHECK(ReadPtr(regs, PtrReg::Y) == 0x0403);
  CHECK(ReadPtr(regs, PtrReg::Z) == 0x0605);
}

TEST_CASE("Register file: a pointer write changes both bytes (LD R0, X+)") {
  FullRegs regs;
  RegFileWritePort w{};
  w.ptr_we_i = true;
  w.ptr_sel_i = PtrReg::Y;
  w.ptr_wval_i = 0x08FF;
  regs.tick_write_seq(w);
  CHECK(ReadReg(regs, 28) == 0xFF);
  CHECK(ReadReg(regs, 29) == 0x08);
  CHECK(ReadPtr(regs, PtrReg::Y) == 0x08FF);
  CHECK(ReadReg(regs, 26) == 0);  // X and Z untouched
  CHECK(ReadReg(regs, 30) == 0);
}

TEST_CASE("Register file: PtrReg::None reads 0 and writes nothing") {
  FullRegs regs;
  RegFileWritePort w{};
  w.ptr_we_i = true;
  w.ptr_sel_i = PtrReg::None;
  w.ptr_wval_i = 0xFFFF;
  regs.tick_write_seq(w);
  for (uint8_t i = 0; i < 32; ++i) {
    CAPTURE(i);
    CHECK(ReadReg(regs, i) == 0);
  }
  CHECK(ReadPtr(regs, PtrReg::None) == 0);
}

TEST_CASE("Register file: byte, pair and pointer writes in the same clock") {
  // Different registers: all three land.
  FullRegs regs;
  RegFileWritePort w{};
  w.we_i = true;
  w.waddr_i = 0;
  w.wdata_i = 0x11;
  w.we_pair_i = true;
  w.waddr_pair_i = 1;  // R3:R2
  w.wval_pair_i = 0x3322;
  w.ptr_we_i = true;
  w.ptr_sel_i = PtrReg::Z;
  w.ptr_wval_i = 0x5544;
  regs.tick_write_seq(w);
  CHECK(ReadReg(regs, 0) == 0x11);
  CHECK(ReadReg(regs, 2) == 0x22);
  CHECK(ReadReg(regs, 3) == 0x33);
  CHECK(ReadReg(regs, 30) == 0x44);
  CHECK(ReadReg(regs, 31) == 0x55);

  // The same register: the byte write wins. This pins down the current order in tick_write_seq()
  // (pointer, then pair, then byte). If you change the order on purpose, change this test.
  w = {};
  w.ptr_we_i = true;
  w.ptr_sel_i = PtrReg::X;
  w.ptr_wval_i = 0x0000;
  w.we_i = true;
  w.waddr_i = 26;
  w.wdata_i = 0x77;
  regs.tick_write_seq(w);
  CHECK(ReadReg(regs, 26) == 0x77);
}

TEST_CASE("Register file, AVRrc: R16..R31 hold their values") {
  RcRegs regs;
  for (uint8_t i = 16; i < 32; ++i) {
    WriteReg(regs, i, static_cast<uint8_t>(0x40U + i));
  }
  for (uint8_t i = 16; i < 32; ++i) {
    CAPTURE(i);
    CHECK(ReadReg(regs, i) == 0x40U + i);
  }
}

TEST_CASE("Register file, AVRrc: R0..R15 don't exist and alias R16..R31") {
  RcRegs regs;
  WriteReg(regs, 21, 0x5A);
  CHECK(ReadReg(regs, 5) == 0x5A);
}

TEST_CASE("Register file, AVRrc: X, Y and Z still work") {
  RcRegs regs;
  WriteReg(regs, 30, 0x34);
  WriteReg(regs, 31, 0x12);
  CHECK(ReadPtr(regs, PtrReg::Z) == 0x1234);

  RegFileWritePort w{};
  w.ptr_we_i = true;
  w.ptr_sel_i = PtrReg::X;
  w.ptr_wval_i = 0x4321;
  regs.tick_write_seq(w);
  CHECK(ReadReg(regs, 26) == 0x21);
  CHECK(ReadReg(regs, 27) == 0x43);
  CHECK(ReadPtr(regs, PtrReg::X) == 0x4321);
}

static_assert([] {
  FullRegs regs;
  RegFileWritePort w{};
  w.ptr_we_i = true;
  w.ptr_sel_i = PtrReg::Z;
  w.ptr_wval_i = 0xABCD;
  regs.tick_write_seq(w);
  RegFileReadPort r{};
  r.ptr_sel_i = PtrReg::Z;
  regs.eval_read_comb(r);
  return r.ptr_val_o == 0xABCD;
}());
