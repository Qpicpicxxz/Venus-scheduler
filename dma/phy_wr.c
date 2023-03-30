#include "dma.h"

uint64_t read_burst_64(uint32_t addr, uint32_t offset) {
  return *(volatile uint64_t*)(addr + offset);
}

void write_burst_64(uint32_t addr, uint32_t offset, uint64_t value) {
  volatile uint64_t* localaddr = (volatile uint64_t*)(addr + offset);
  *localaddr = value;
}

uint32_t read_burst_32(uint32_t addr, uint32_t offset) {
  return *(volatile uint32_t*)(addr + offset);
}

void write_burst_32(uint32_t addr, uint32_t offset, uint32_t value) {
  volatile uint32_t* localaddr = (volatile uint32_t*)(addr + offset);
  *localaddr = value;
}
