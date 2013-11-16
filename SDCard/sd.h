#ifndef MMC_H
#define MMC_H

#define SD_DDR DDRB
#define SD_PORT PORTB
#define SD_PIN PINB

#define SD_CD (1 << 1)
#define SD_WP (1 << 0)

#include <avr/io.h>
#include "spi.h"

class cidReg
{
public:
	inline cidReg(void) {}
	cidReg(uint8_t dat[16]);

	uint8_t MID;		// Manufacturer ID
	uint16_t OID;		// OEM/Application ID
	char PNM[5];		// Product name
	uint8_t PRV;		// Product revision
	uint32_t PSN;		// Product serial number
	uint16_t MDT:12;	// Manufacturing date
	uint8_t CRC:7;		// CRC7 checksum
};

class csdReg
{
public:
	inline csdReg(void) {}
	csdReg(uint8_t dat[16]);

	uint8_t CSD_STRUCTURE:2;
	uint8_t TAAC;
	uint8_t NSAC;
	uint8_t TRAN_SPEED;
	uint16_t CCC:12;
	uint8_t READ_BL_LEN:4;
	uint8_t READ_BL_PARTIAL:1;
	uint8_t WRITE_BLK_MISALIGN:1;
	uint8_t READ_BLK_MISALIGN:1;
	uint8_t DSR_IMP:1;
	uint32_t C_SIZE:22;	// 12 for ver1, 22 for ver2
	uint8_t VDD_R_CURR_MIN:3;
	uint8_t VDD_R_CURR_MAX:3;
	uint8_t VDD_W_CURR_MIN:3;
	uint8_t VDD_W_CURR_MAX:3;
	uint8_t C_SIZE_MULT:3;
	uint8_t ERASE_BLK_EN:1;
	uint8_t SECTOR_SIZE:7;
	uint8_t WP_GRP_SIZE:7;
	uint8_t WP_GRP_ENABLE:1;
	uint8_t R2W_FACTOR:3;
	uint8_t WRITE_BL_LEN:4;
	uint8_t WRITE_BL_PARTIAL:1;
	uint8_t FILE_FORMAT_GRP:1;
	uint8_t COPY:1;
	uint8_t PERM_WRITE_PROTECT:1;
	uint8_t TMP_WRITE_PROTECT:1;
	uint8_t FILE_FORMAT:2;
	uint8_t CRC:7;
};

class sdhw: public spi
{
public:
	inline sdhw(void);
	uint8_t init(void);
	bool detect(void);
	inline bool writeProtected(void);
	inline uint8_t version(void) const {return _ver;}
	//inline struct cidReg& cid(void) {return _cid;}
	inline struct csdReg& csd(void) {return _csd;}
	inline uint32_t size(void) const;
	inline uint8_t readBlock(uint32_t addr, uint8_t buff[512]);

protected:
	inline void cmd(uint8_t index, uint32_t arg, uint8_t crc);
	inline void acmd(uint8_t index, uint32_t arg, uint8_t crc);
	inline uint8_t recv(void);
	inline uint8_t recv(uint8_t n);
	inline void wait(void);

	//class cidReg _cid;
	class csdReg _csd;
	uint8_t _res[5], _ver;
};

extern class sdhw sd;

// Defined as inline to execute faster

inline sdhw::sdhw(void)
{
	spi::init();
	SD_DDR &= ~(SD_CD | SD_WP);
	SD_PORT |= SD_CD | SD_WP;
	_ver = 0;
}

inline uint8_t sdhw::recv(uint8_t n)
{
	n = n > 5 ? 5 : n;
	for (uint8_t r = 0; r < n; r++) {
		uint8_t i = 100;
		do {
			spi::send(0xFF);
			i--;
		} while (((_res[r] = spi::recv()) == 0xFF) && (i != 0));
	}
	return _res[0];
}

inline uint8_t sdhw::recv(void)
{
	for (uint8_t r = 0; r < 5; r++) {
		uint8_t i = 100;
		do {
			spi::send(0xFF);
			i--;
		} while ((_res[r] = spi::recv()) == 0xFF && i != 0);
	}
	return _res[0];
}

inline void sdhw::acmd(uint8_t index, uint32_t arg, uint8_t crc)
{
	cmd(55, 0, 0xFF);			// Lead cmd(55)
	cmd(index, arg, crc);
}

inline void sdhw::cmd(uint8_t index, uint32_t arg, uint8_t crc)
{
	wait();
	wait();
	spi::send(index | (1 << 6));
	spi::send(arg >> 24);
	spi::send(arg >> 16);
	spi::send(arg >> 8);
	spi::send(arg);
	spi::send((crc << 1) | 1);
}

inline void sdhw::wait(void)
{
	do
		spi::send(0xFF);
	while (spi::recv() != 0xFF);
}

inline bool sdhw::writeProtected(void)
{
	return SD_PIN & SD_WP;
}

inline uint32_t sdhw::size(void) const
{
	if (!_csd.CSD_STRUCTURE)
		return (uint32_t)(_csd.C_SIZE + 1) * \
			(uint32_t)((uint32_t)1 << (_csd.C_SIZE_MULT + 2)) * \
			(uint32_t)((uint32_t)1 << _csd.READ_BL_LEN) / 1024;
	else
		return (uint32_t)(_csd.C_SIZE + 1) * (uint32_t)512;
}

inline uint8_t sdhw::readBlock(uint32_t addr, uint8_t buff[512])
{
	cmd(17, addr, 0xFF);			// Read block
	uint8_t res = recv(1);
	if (res)
		return res;
	do
		spi::send(0xFF);
	while (spi::recv() != 0xFE);
	for (uint16_t i = 0; i < 512; i++) {
		spi::send(0xFF);
		buff[i] = spi::recv();
	}
	wait();
	return 0;
}

#endif
