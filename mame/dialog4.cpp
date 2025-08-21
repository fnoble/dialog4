#include "emu.h"
#include "cpu/m68000/m68000.h"
#include "video/mc6845.h"
#include "machine/68230pit.h"
#include "machine/mc68681.h"
#include "bus/rs232/rs232.h"
#include "screen.h"

class dialog4_state : public driver_device
{
public:
    dialog4_state(machine_config const &mconfig, device_type type, char const *tag)
        : driver_device(mconfig, type, tag),
          m_maincpu(*this, "maincpu"),
          m_crtc(*this, "crtc"),
          m_pit0(*this, "pit0"),
          m_pit1(*this, "pit1"),
          m_duart(*this, "duart"),
          m_vram(*this, "vram"),
          m_fontrom(*this, "fontrom"),
          m_mode(*this, "MODE")
    { }

    void dialog4(machine_config &config);
    INPUT_CHANGED_MEMBER(inject_a);
    INPUT_CHANGED_MEMBER(mode_changed);

private:
    virtual void machine_start() override;
    virtual void machine_reset() override;

    void prg_map(address_map &map);

    required_device<m68000_base_device> m_maincpu;
    required_device<mc6845_device>      m_crtc;
	required_device<pit68230_device>    m_pit0;
	required_device<pit68230_device>    m_pit1;
	required_device<scn2681_device>     m_duart;
    required_shared_ptr<u16>            m_vram;
    required_region_ptr<u8>             m_fontrom;
    required_ioport                     m_mode;
    
    MC6845_UPDATE_ROW(crtc_update_row);
    void duart_out_w(u8 data);

    void pit1_pb_out(u8 data);
    u8 pit1_pa_in();
    // tiny FIFO for injected MC->CPU bytes
    std::deque<u8> m_zp_to_cpu;
    u16 pit1_r(offs_t offset, u16 mem_mask = 0xffff);
    void pit1_w(offs_t offset, u16 data, u16 mem_mask = 0xffff);
    void pit1_check_data_available();

    void cpu_int(address_map &map);

};

/* ----------------  memory map  ---------------- */
void dialog4_state::prg_map(address_map &map)
{
    map(0x000000, 0x000007).rom().region("npp55rom", 0);    // Boot ROM (NPP55)
    map(0x000008, 0x00ffff).ram();                          // On-board RAM (NPP55)
    map(0x0a00000,0x0a9ffff).rom().region("nep52rom", 0);   // Main ROM (NEP52)
    map(0x038000, 0x038fff).ram().share("vram");            // Video RAM (NCR53)
    map(0x080000, 0x09ffff).ram();                          // RAM
    map(0x0D0000, 0x0Dffff).ram();                          // Main RAM (NSI56)

    /* MC6845: odd‑byte address= $39001 (addr) , $39003 (data) */
    map(0x0039000, 0x0039001).w("crtc", FUNC(mc6845_device::address_w));
    map(0x0039002, 0x0039003).w("crtc", FUNC(mc6845_device::register_w));

    // three PIAs on a 1 kB boundary, 16-bit bus, 2-byte register spacing
    map(0x012000, 0x01203F).mirror(0x0003E0).rw("pit0", FUNC(pit68230_device::read), FUNC(pit68230_device::write));
    //map(0x012400, 0x01241F).mirror(0x0003E0).rw("pit1", FUNC(pit68230_device::read), FUNC(pit68230_device::write));
    map(0x012400, 0x01243F).mirror(0x0003E0).rw(FUNC(dialog4_state::pit1_r), FUNC(dialog4_state::pit1_w));
    //map(0x012800, 0x01281F).mirror(0x0003E0).rw("pia2", FUNC(pit68230_device::read), FUNC(pit68230_device::write));

    // DUART, word-wide, register stride 2
    map(0xE00000, 0xE0001F).rw("duart", FUNC(scn2681_device::read), FUNC(scn2681_device::write));
}

void dialog4_state::cpu_int(address_map &map)
{
	map(0xff'fff3, 0xff'fff3).lr8(NAME([]() { return m68000_base_device::autovector(1); }));
	map(0xff'fff5, 0xff'fff5).lr8(NAME([]() { return m68000_base_device::autovector(2); }));
	map(0xff'fff7, 0xff'fff7).lr8(NAME([]() { return m68000_base_device::autovector(3); }));
	map(0xff'fff9, 0xff'fff9).lr8(NAME([]() { return m68000_base_device::autovector(4); }));
	map(0xff'fffb, 0xff'fffb).lr8(NAME([]() { return 0x40; }));
	map(0xff'fffd, 0xff'fffd).lr8(NAME([]() { return m68000_base_device::autovector(6); }));
	map(0xff'ffff, 0xff'ffff).lr8(NAME([]() { return m68000_base_device::autovector(7); }));
}

/* ----------------  machine config ------------- */
void dialog4_state::dialog4(machine_config &config)
{
    M68000(config, m_maincpu, 8_MHz_XTAL);           // CPU
    m_maincpu->set_addrmap(AS_PROGRAM, &dialog4_state::prg_map);
    m_maincpu->set_addrmap(m68000_base_device::AS_CPU_SPACE, &dialog4_state::cpu_int);

    MC6845(config, m_crtc, 8_MHz_XTAL/8);            // char‑clock
    screen_device &screen(SCREEN(config, "screen", SCREEN_TYPE_RASTER));
    /* pixel clock = 8 MHz, 512 px/line, 312 lines/frame */
    /* visible      = 0 … 343 px          0 … 272 lines  */
    screen.set_raw(8_MHz_XTAL,  // 8 MHz pixel clock
               512,             // H total (pixels)
               344, 0,          // hbend, hbstart  (visible 0-343)
               312,             // V total (scan-lines)
               273, 0);         // vbend, vbstart (visible 0-272)
	screen.set_screen_update("crtc", FUNC(mc6845_device::screen_update));

	m_crtc->set_screen("screen");
	m_crtc->set_show_border_area(false);
	m_crtc->set_char_width(8);
	m_crtc->set_update_row_callback(FUNC(dialog4_state::crtc_update_row));

	/* ---------- two MC68230s ---------- */
	PIT68230(config, m_pit0, 8_MHz_XTAL);
	PIT68230(config, m_pit1, 8_MHz_XTAL);

    //m_pit0->port_irq_callback().set_inputline("maincpu", 2);   // Port-A/B/C
    //m_pit0->timer_irq_callback().set_inputline("maincpu", 2);  // 16-bit timer

    //m_pit1->port_irq_callback().set(FUNC(dialog4_state::pit1_port_irq));
    //m_pit1->timer_irq_callback().set_inputline("maincpu", 3);
    m_pit1->pb_out_callback().set(FUNC(dialog4_state::pit1_pb_out));  // CPU -> MC bytes
    m_pit1->pa_in_callback().set(FUNC(dialog4_state::pit1_pa_in));    // MC -> CPU bytes on demand


	/* ---------- MC68681 / SCN2681 DUART ---------- */
	SCN2681(config, m_duart, 3.6864_MHz_XTAL);

	/* combined interrupt → IPL3 */
    //m_duart->irq_cb().set_inputline("maincpu", 3);

	/* Channel A console */
	rs232_port_device &rs232a(RS232_PORT(config, "rs232a", default_rs232_devices, "terminal"));
	rs232a.rxd_handler().set(m_duart, FUNC(scn2681_device::rx_a_w));
	m_duart->a_tx_cb().set(rs232a, FUNC(rs232_port_device::write_txd));

	/* Channel B spare */
	rs232_port_device &rs232b(RS232_PORT(config, "rs232b", default_rs232_devices, "null_modem"));
	rs232b.rxd_handler().set(m_duart, FUNC(scn2681_device::rx_b_w));
	m_duart->b_tx_cb().set(rs232b, FUNC(rs232_port_device::write_txd));

	/* DUART output-port (heartbeat LED, DTRs, etc.) */
	m_duart->outport_cb().set(FUNC(dialog4_state::duart_out_w));

}

void dialog4_state::machine_start()
{
}

void dialog4_state::machine_reset()
{
}

// crude half-offset to register name (even addresses only; odd are unused on your board)
static const char* regname(int half)
{
	switch (half) {
	case 0x00: return "PGCR";   // +0x00
	case 0x01: return "PSRR";   // +0x02
	case 0x02: return "PADDR";  // +0x04
	case 0x03: return "PBDDR";  // +0x06
	case 0x04: return "RCDDR";     // +0x08 (unused here)
	case 0x05: return "PIVR";   // +0x0A
	case 0x06: return "PACR";   // +0x0C
	case 0x07: return "PBCR";   // +0x0E
	case 0x08: return "PADR";   // +0x10
	case 0x09: return "PBDR";   // +0x12
	case 0x0A: return "PAAR";   // +0x12
	case 0x0B: return "PBAR";   // +0x12
	case 0x0C: return "PCDR";   // +0x14
	case 0x0D: return "PSR";    // +0x16  (your firmware polls bit 2 here)
	// timer block starts at +0x20 (half-index 0x10)
	default:   return "?";
	}
}

// helpers for 68000 big-endian byte lanes
static inline bool hi_byte(u16 mem_mask) { return (mem_mask & 0xFF00) == 0xFF00; }
static inline bool lo_byte(u16 mem_mask) { return (mem_mask & 0x00FF) == 0x00FF; }
static inline u8   hi(u16 w)             { return u8(w >> 8); }
static inline u8   lo(u16 w)             { return u8(w & 0xFF); }

u16 dialog4_state::pit1_r(offs_t offset, u16 mem_mask)
{
	u16 data = m_pit1->read(offset);

	const u32 byte_addr = 0x012400 + (offset << 1);

	if (hi_byte(mem_mask))
		logerror("PIT1 R %02X %s @%06X.even -> %02X (mask=%04X)\n", offset, regname(offset), byte_addr, hi(data), mem_mask);
	if (lo_byte(mem_mask))
		logerror("PIT1 R %02X %s?@%06X.odd  -> %02X (mask=%04X)\n", offset, regname(offset), byte_addr+1, lo(data), mem_mask);

	return data;
}

void dialog4_state::pit1_w(offs_t offset, u16 data, u16 mem_mask)
{
	const u32 byte_addr = 0x012400 + (offset << 1);

	if (hi_byte(mem_mask)) {
		logerror("PIT1 W %02X %s @%06X.even <= %02X (mask=%04X)\n", offset, regname(offset), byte_addr, hi(data), mem_mask);
	    m_pit1->write(offset, hi(data));
    }
	if (lo_byte(mem_mask)) {
		logerror("PIT1 W %02X %s?@%06X.odd  <= %02X (mask=%04X)\n", offset, regname(offset), byte_addr+1, lo(data), mem_mask);
	    m_pit1->write(offset, lo(data));
    }

}

void dialog4_state::pit1_pb_out(u8 data)
{
    logerror("PIT1 PB TX (CPU->MC): %02X\n", data);
}

u8 dialog4_state::pit1_pa_in()
{
    u8 v = 0xFF;
    if (!m_zp_to_cpu.empty()) {
        v = m_zp_to_cpu.front();
        m_zp_to_cpu.pop_front();
    }
    
    m_pit1->h1_w(1);
    m_maincpu->set_input_line(M68K_IRQ_5, CLEAR_LINE);

    logerror("PIT1 PA RX (MC->CPU): %02X  (queue=%zu)\n", v, m_zp_to_cpu.size());

    pit1_check_data_available();
    return v;
}

void dialog4_state::duart_out_w(u8 data)
{
	/* OP7 is the 7 Hz heartbeat, OP4/6 are the DTR lines.
	   For now just log them so you can see they’re changing.          */
	logerror("DUART outport = %02X\n", data);
}

MC6845_UPDATE_ROW( dialog4_state::crtc_update_row )
{
    //  ma = start of row in VRAM, ra = raster line (0-15)

    for (int column = 0; column < x_count; column++)
    {
        rgb_t fg = rgb_t::green();
        rgb_t bg = rgb_t::black();

        u16 code = m_vram[(ma + column) & 0x7ff];
        u16 glyph = code & 0x3FF;
        u8 inv = BIT(code, 15);
        u8  dots = m_fontrom[glyph * 16 + ra];

        if (inv) {
			std::swap(fg, bg);
        }

        for (int bit = 0; bit < 8; bit++)
        {
            bitmap.pix(y, (column*8) + bit) = (dots & 0x80) ? bg : fg;
            dots <<= 1;
        }
    }
}

static INPUT_PORTS_START( dialog4 )
    PORT_START("INJECT")
    PORT_BIT(0x01, IP_ACTIVE_HIGH, IPT_OTHER)
        PORT_NAME("Inject byte 'A'")
        PORT_CODE(KEYCODE_A)
        PORT_IMPULSE(1)
        PORT_CHANGED_MEMBER(DEVICE_SELF, FUNC(dialog4_state::inject_a), 0)
    PORT_START("MODE")
    PORT_CONFNAME(0x1F, 0x00, "Mode")
        PORT_CONFSETTING(0x00, "None")
        PORT_CONFSETTING(0x01, "Manual")
        PORT_CONFSETTING(0x02, "Zero")
        PORT_CONFSETTING(0x10, "Setup")
        PORT_CHANGED_MEMBER(DEVICE_SELF, FUNC(dialog4_state::mode_changed), 0)
INPUT_PORTS_END

void dialog4_state::pit1_check_data_available()
{
    if (!m_zp_to_cpu.empty()) {
        logerror("Kick\n");
        m_pit1->h1_w(0);
        m_maincpu->set_input_line(M68K_IRQ_5, HOLD_LINE);
    }
}

INPUT_CHANGED_MEMBER(dialog4_state::inject_a)
{
    if (!newval) return;

    m_zp_to_cpu.push_back(0xcc);
    m_zp_to_cpu.push_back(0x10);
    pit1_check_data_available();
}

INPUT_CHANGED_MEMBER(dialog4_state::mode_changed)
{
    if (!newval) return;

    m_zp_to_cpu.push_back(0xcc);
    m_zp_to_cpu.push_back(u8(m_mode->read()));
    pit1_check_data_available();
}

/* ----------------  ROM definitions ------------- */
ROM_START(dialog4)
    ROM_REGION16_BE(0x20000, "npp55rom", 0)
    ROM_LOAD("353.21_NPP55_2.14.bin", 0x0000, 0x20000, CRC(ff48390b) SHA1(0)) // fill in CRC later

    ROM_REGION16_BE(0xa0000, "nep52rom", 0)
    ROM_LOAD("353.32_NEP52_2.16.bin", 0x00000, 0xa0000, CRC(d91e2d75) SHA1(0))

    ROM_REGION(0x8000, "fontrom", 0)
    ROM_LOAD("44206-352.00 NCR 53 IC 02 1.04.bin", 0x0000, 0x8000, CRC(6c4346be) SHA1(0))
ROM_END

//    YEAR  NAME     PARENT  COMPAT  MACHINE  INPUT    CLASS          INIT        COMPANY   FULLNAME     FLAGS
CONS( 1987, dialog4, 0,      0,      dialog4, dialog4, dialog4_state, empty_init, "Deckel", "Dialog 4",  MACHINE_NOT_WORKING | MACHINE_NO_SOUND )
