#ifndef AVR_CONFIG_HPP_
#define AVR_CONFIG_HPP_

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>

namespace avrpp {

/** All the different AVR variants */
enum class SubVariant : uint8_t {
  AVR1,         // 1995 Minimal Core (AT90S1200: 3-level HW stack, no SRAM)
  AVR2,         // 1995 Classic Core (AT90S8515: LPM R0 only, 16-bit PC)
  AVR25,        // AVRe <= 8kB (ATtiny85: +MOVW, +LPM Rd,Z+, no JMP/CALL)
  AVR31,        // AVR 128kB (ATmega103: +JMP/CALL, +ELPM R0, no MOVW/MUL)
  AVR4,         // AVRe+ <=8kB (ATmega88: +MUL/FMUL, no JMP/CALL)
  AVR5,         // AVRe+ <=64kB (ATmega328P: +JMP/CALL, +MUL/FMUL)
  AVR51,        // AVRe+ 128kB (ATmega1280: :ELPM Rd,Z+, +RAMPZ)
  AVR6,         // AVRe+ 256kB (ATmega2560: 22-bit 3-byte PC, +EIND)
  AVRXMEGA2,    // AVRxm <= 64kB (ATxmega32D4: early silicon, no RMW)
  AVRXMEGA6_AU, // AVRxm >128kB (ATxmega256A3U: 22-bit PC, +RMW, +DES)
  AVRXMEGA7,    // AVRxm + EBI (ATxmega128A1U: 22-bit PC, 24-bit RMPX/Y/Z/D)
  AVRXT,        // AVRxt / avrxmega4 (AVR128DA: 1-cyc STD, +RAMPZ/ELPM)
  AVRRC,        // AVRrc (ATtiny10: 16 GPRs R16-R31, 16-bit LDS/STS)
  AVRXT3        // AVRxt / avrxmega3 (ATmega4809: <=64kB, no RAMPZ/ELPM)
};

struct CoreConfig {
  SubVariant id;
  bool       is_avrrc;  // True: Only R16..R31 exist

  [[nodiscard]] constexpr std::size_t num_gprs() const noexcept {
    return is_avrrc ? 16 : 32;
  }
};

namespace Presets {
  inline constexpr CoreConfig AVR1  {SubVariant::AVR1,  false};
  inline constexpr CoreConfig AVRRC {SubVariant::AVRRC, true};
};

[[nodiscard]] constexpr std::string_view variant_name(SubVariant v) noexcept {
  switch (v) {
    case SubVariant::AVR1:          return "AVR1 (AVR, 3-level HW stack, no SRAM)";
    case SubVariant::AVR2:          return "AVR2 (AVR, <=8 kB)";
    case SubVariant::AVR25:         return "AVR25 (AVRe, <=8 kB, MOVW/LPMX)";
    case SubVariant::AVR31:         return "AVR31 (AVR, 128 kB, ELPM R0)";
    case SubVariant::AVR4:          return "AVR4 (AVRe+, <=8 kB, MUL)";
    case SubVariant::AVR5:          return "AVR5 (AVRe+, <=64 kB)";
    case SubVariant::AVR51:         return "AVR51 (AVRe+, 128 kB, ELPM)";
    case SubVariant::AVR6:          return "AVR6 (AVRe+, 22-bit PC, EIND)";
    case SubVariant::AVRXMEGA2:     return "AVRXMEGA2 (AVRxm, <=64 kB)";
    case SubVariant::AVRXMEGA6_AU:  return "AVRXMEGA6 (AVRxm, >128 kB, RMW)";
    case SubVariant::AVRXMEGA7:     return "AVRXMEGA7 (AVRxm, RAMPX/Y/Z/D)";
    case SubVariant::AVRXT:         return "AVRXMEGA4 (AVRxt, RAMPZ/ELPM)";
    case SubVariant::AVRRC:         return "AVRTINY (AVRrc, 16 registers)";
    case SubVariant::AVRXT3:        return "AVRXMEGA3 (AVRxt, <=64 kB)";
  }
  return "unknown";
}

}  // namespace avrpp
#endif // AVR_CONFIG_HPP_